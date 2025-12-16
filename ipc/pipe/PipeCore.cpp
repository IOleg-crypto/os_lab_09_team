#include "PipeCore.h"
#include <iostream>
#include <mutex>
#include <rpc.h>
#include <thread>

#pragma comment(lib, "Rpcrt4.lib")

// Constructor
// If this instance is the Host (Supervisor), we immediately spawn the background
// server thread. This thread runs independently (detached) to handle incoming
// pipe connections without blocking the main application UI.
PipeCore::PipeCore(bool host) : m_isHost(host)
{
    if (m_isHost)
    {
        std::thread([this]() { this->ServerRun(); }).detach();
    }
}

// Destructor
// Sets the stop flag to signal the server loop to exit.
// We also close the pipe handle immediately to force the blocking ConnectNamedPipe
// or ReadFile calls to wake up (fail) and allow the thread to terminate.
PipeCore::~PipeCore()
{
    m_server_stopped_flag = true;
    if (m_hPipe != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hPipe);
        m_hPipe = INVALID_HANDLE_VALUE;
    }
}

// Helper: Generates a standard UUID string for unique idea identification.
void PipeCore::GenerateUUID(char *buffer)
{
    UUID uuid;
    UuidCreate(&uuid);
    unsigned char *str;
    UuidToStringA(&uuid, &str);
    if (str != NULL)
    {
        strncpy_s(buffer, UUID_SIZE, (char *)str, UUID_SIZE - 1);
        RpcStringFreeA(&str);
    }
}

// Main Server Loop (Runs in background thread)
// This function implements a blocking Named Pipe server.
// It creates a single pipe instance and continuously loops to accept connections.
//
// Workflow:
// 1. Create Named Pipe in BLOCKING mode (PIPE_WAIT).
// 2. Wait for a client to connect (ConnectNamedPipe).
// 3. Read the command header.
// 4. Process the command (Add, Get, Vote, etc.).
// 5. Disconnect and loop back to wait for the next client.
void PipeCore::ServerRun()
{
    // Create the Named Pipe instance.
    // We use PIPE_WAIT to ensure operations block until completion, which is
    // more CPU-efficient than polling with PIPE_NOWAIT.
    if (m_hPipe == INVALID_HANDLE_VALUE)
    {
        m_hPipe =
            CreateNamedPipeA(PIPE_NAME, PIPE_ACCESS_DUPLEX,
                             PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, // BLOCKING MODE
                             PIPE_UNLIMITED_INSTANCES, 4096, 4096, 0, NULL);
        if (m_hPipe == INVALID_HANDLE_VALUE)
        {
            // Failed to create pipe. This is a fatal error for IPC.
            return;
        }
    }

    // Main request processing loop
    while (!m_server_stopped_flag)
    {
        // specific connection handling:
        // ConnectNamedPipe returns TRUE if a client connects.
        // If it returns FALSE with ERROR_PIPE_CONNECTED, it means a client connected
        // between the CreatePipe call and this Connect call (which is also a success).
        bool connected =
            ConnectNamedPipe(m_hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

        if (connected)
        {
            PipeMessageHeader header;
            DWORD bytesRead;

            // Read the command header first to determine payload type
            if (ReadFile(m_hPipe, &header, sizeof(header), &bytesRead, NULL))
            {
                // -- CMD_ADD_IDEA: Client submitting a new idea --
                if (header.command == CMD_ADD_IDEA)
                {
                    Idea newIdea;
                    if (ReadFile(m_hPipe, &newIdea, sizeof(Idea), &bytesRead, NULL))
                    {
                        // Critical Section: Modifying shared data
                        std::lock_guard<std::mutex> lock(m_ideasMutex);
                        if (!m_server_stopped_flag)
                        {
                            m_serverIdeas.push_back(newIdea);
                        }
                    }
                }
                // -- CMD_GET_ALL: Client requesting full list of ideas --
                else if (header.command == CMD_GET_ALL)
                {
                    std::lock_guard<std::mutex> lock(m_ideasMutex);
                    int count = (int)m_serverIdeas.size();
                    DWORD written;

                    // Send count first, then the array of ideas
                    WriteFile(m_hPipe, &count, sizeof(int), &written, NULL);
                    if (count > 0)
                    {
                        WriteFile(m_hPipe, m_serverIdeas.data(), count * sizeof(Idea), &written,
                                  NULL);
                    }
                }
                // -- CMD_CHECK_STOP: Client checking if voting phase started --
                else if (header.command == CMD_CHECK_STOP)
                {
                    DWORD written;
                    bool stopped = m_server_stopped_flag;
                    WriteFile(m_hPipe, &stopped, sizeof(bool), &written, NULL);
                }
                // -- CMD_VOTE: Client voting for an idea --
                else if (header.command == CMD_VOTE)
                {
                    char uuid[UUID_SIZE];
                    if (ReadFile(m_hPipe, uuid, UUID_SIZE, &bytesRead, NULL))
                    {
                        std::lock_guard<std::mutex> lock(m_ideasMutex);
                        for (auto &idea : m_serverIdeas)
                        {
                            if (strcmp(idea.uuid, uuid) == 0)
                            {
                                idea.votes++;
                                break;
                            }
                        }
                    }
                }
            }

            // Flush the buffer to ensure all data is written to the client
            FlushFileBuffers(m_hPipe);

            // Disconnect the pipe instance to free it for the next client connection.
            // Note: We do NOT close the handle here, just disconnect.
            DisconnectNamedPipe(m_hPipe);
        }
        else
        {
            // If connection failed unexpectedly, wait briefly to prevent tight failure loops
            Sleep(10);
        }
    }
}

// --- IIpcCore Interface Implementation ---

// Adds an idea.
// If Host: Adds directly to the local vector (thread-safe).
// If Client: Sends an IPC request to the server.
bool PipeCore::AddIdea(const std::string &text, int worker_id)
{
    if (m_isHost)
    {
        std::lock_guard<std::mutex> lock(m_ideasMutex);
        Idea idea;
        strncpy_s(idea.text, text.c_str(), TEXT_SIZE);
        idea.worker_id = worker_id;
        idea.votes = 0;
        GenerateUUID(idea.uuid);
        m_serverIdeas.push_back(idea);
        return true;
    }
    else
    {
        ClientAddIdea(text, worker_id);
        return true;
    }
}

// Retrieves all ideas.
// If Host: Returns local copy (thread-safe).
// If Client: Fetches from server.
std::vector<Idea> PipeCore::GetAllIdeas()
{
    if (m_isHost)
    {
        std::lock_guard<std::mutex> lock(m_ideasMutex);
        return m_serverIdeas;
    }
    else
    {
        return ClientGetAllIdeas();
    }
}

// Casts a vote.
// If Host: Updates local count (thread-safe).
// If Client: Sends vote command to server.
void PipeCore::Vote(const std::string &uuid)
{
    if (m_isHost)
    {
        std::lock_guard<std::mutex> lock(m_ideasMutex);
        for (auto &idea : m_serverIdeas)
        {
            if (strcmp(idea.uuid, uuid.c_str()) == 0)
            {
                idea.votes++;
                break;
            }
        }
    }
    else
    {
        ClientVote(uuid);
    }
}

// Stops the session (Voting phase transition).
void PipeCore::SetStopped(bool stop)
{
    if (m_isHost)
        m_server_stopped_flag = stop;
}

// Checks if session is stopped.
bool PipeCore::IsStopped()
{
    return m_isHost ? m_server_stopped_flag : ClientIsStopped();
}

// Checks connection status.
// For pipes, we verify if the server pipe is available/waitable.
bool PipeCore::IsConnected()
{
    return WaitNamedPipeA(PIPE_NAME, 0);
}

// --- Client Implementation Details ---

// Generic request sender for Clients.
// Handles connecting to the Named Pipe, sending the command/data,
// reading the response, and cleaning up.
bool PipeCore::SendRequest(PipeCmd cmd, const void *dataIn, int sizeIn, void *dataOut, int sizeOut,
                           int *bytesRead)
{
    // Wait for the server pipe to become available.
    // 1000ms timeout provides robustness against momentary server busy states.
    if (!WaitNamedPipeA(PIPE_NAME, 1000))
    {
        return false;
    }

    // Connect to the pipe
    HANDLE hPipe =
        CreateFileA(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hPipe == INVALID_HANDLE_VALUE)
        return false;

    // Set pipe to blocking message mode for consistent read/write behavior
    DWORD mode = PIPE_READMODE_MESSAGE | PIPE_WAIT;
    SetNamedPipeHandleState(hPipe, &mode, NULL, NULL);

    // Write Command Header
    PipeMessageHeader header = {(int)cmd, sizeIn};
    DWORD written;
    if (!WriteFile(hPipe, &header, sizeof(header), &written, NULL))
    {
        CloseHandle(hPipe);
        return false;
    }

    // Write input data (if any)
    if (sizeIn > 0 && dataIn)
    {
        if (!WriteFile(hPipe, dataIn, sizeIn, &written, NULL))
        {
            CloseHandle(hPipe);
            return false;
        }
    }

    // Read response data (if expected)
    bool result = true;
    if (sizeOut > 0 && dataOut)
    {
        DWORD read;
        if (ReadFile(hPipe, dataOut, sizeOut, &read, NULL))
        {
            if (bytesRead)
                *bytesRead = read;
        }
        else
        {
            result = false;
        }
    }

    CloseHandle(hPipe);
    return result;
}

void PipeCore::ClientAddIdea(const std::string &text, int worker_id)
{
    Idea idea;
    strncpy_s(idea.text, text.c_str(), TEXT_SIZE);
    idea.worker_id = worker_id;
    idea.votes = 0;
    GenerateUUID(idea.uuid);
    SendRequest(CMD_ADD_IDEA, &idea, sizeof(Idea), NULL, 0, NULL);
}

bool PipeCore::ClientIsStopped()
{
    bool stopped = false;
    // Note: If connection fails, valid decision is to assume session is NOT stopped
    // to keep the Worker UI active and retry later.
    if (!SendRequest(CMD_CHECK_STOP, NULL, 0, &stopped, sizeof(bool), NULL))
    {
        return false;
    }
    return stopped;
}

void PipeCore::ClientVote(const std::string &uuid)
{
    char buf[UUID_SIZE];
    strncpy_s(buf, uuid.c_str(), UUID_SIZE - 1);
    SendRequest(CMD_VOTE, buf, UUID_SIZE, NULL, 0, NULL);
}

std::vector<Idea> PipeCore::ClientGetAllIdeas()
{
    std::vector<Idea> list;

    // Quick check to quit early if server is down, avoiding long timeouts in UI loop
    if (!WaitNamedPipeA(PIPE_NAME, 10))
        return list;

    // We can't use generic SendRequest comfortably here because the array size
    // is dynamic (only known after reading the first integer).

    HANDLE hPipe =
        CreateFileA(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hPipe == INVALID_HANDLE_VALUE)
        return list;

    DWORD mode = PIPE_READMODE_MESSAGE | PIPE_WAIT;
    SetNamedPipeHandleState(hPipe, &mode, NULL, NULL);

    PipeMessageHeader header = {(int)CMD_GET_ALL, 0};
    DWORD written;
    WriteFile(hPipe, &header, sizeof(header), &written, NULL);

    // Read number of ideas first
    int count = 0;
    DWORD read;
    if (ReadFile(hPipe, &count, sizeof(int), &read, NULL) && count > 0)
    {
        list.resize(count);
        // Direct read into vector's contiguous memory
        ReadFile(hPipe, list.data(), count * sizeof(Idea), &read, NULL);
    }

    CloseHandle(hPipe);
    return list;
}
