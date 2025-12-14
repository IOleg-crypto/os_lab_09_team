#include <rpc.h>
// Include class
#include "PipeCore.h"

#pragma comment(lib, "Rpcrt4.lib")

PipeCore::PipeCore(bool host) : m_isHost(host), m_server_stopped_flag(false) {}
PipeCore::~PipeCore() {}

// --- Generation UUID ---
void PipeCore::GenerateUUID(char* buffer) {
    UUID uuid;
    UuidCreate(&uuid);
    unsigned char* str;
    UuidToStringA(&uuid, &str);
    if (str != NULL) {
        strncpy_s(buffer, UUID_SIZE, (char*)str, UUID_SIZE - 1);
        RpcStringFreeA(&str);
    }
}

// --- Server ---
void PipeCore::ServerProcessPendingConnection() {
    if (!m_isHost) {
        return;
    }

    HANDLE hPipe = CreateNamedPipeA(
        PIPE_NAME, PIPE_ACCESS_DUPLEX, 
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_NOWAIT,
        PIPE_UNLIMITED_INSTANCES, 4096, 4096, 0, NULL);

    if (hPipe == INVALID_HANDLE_VALUE)
    {
        return;
    }

    bool connected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

    if (connected) {
        PipeMessageHeader header;
        DWORD bytesRead;
        
        if (ReadFile(hPipe, &header, sizeof(header), &bytesRead, NULL)) {
            if (header.command == CMD_ADD_IDEA) {
                Idea newIdea;
                if (ReadFile(hPipe, &newIdea, sizeof(Idea), &bytesRead, NULL)) {
                    if (!m_server_stopped_flag)
                    {
                        m_serverIdeas.push_back(newIdea);
                    }
                }
            } 
            else if (header.command == CMD_GET_ALL) {
                int count = m_serverIdeas.size();
                DWORD written;
                WriteFile(hPipe, &count, sizeof(int), &written, NULL);
                if (count > 0) WriteFile(hPipe, m_serverIdeas.data(), count * sizeof(Idea), &written, NULL);
            }
            else if (header.command == CMD_CHECK_STOP) {
                DWORD written;
                WriteFile(hPipe, &m_server_stopped_flag, sizeof(bool), &written, NULL);
            }
            else if (header.command == CMD_VOTE) {
                char uuid[UUID_SIZE];
                ReadFile(hPipe, uuid, UUID_SIZE, &bytesRead, NULL);
                for (auto& idea : m_serverIdeas) {
                    if (strcmp(idea.uuid, uuid) == 0) {
                        idea.votes++;
                        break;
                    }
                }
            }
        }
        DisconnectNamedPipe(hPipe);
    }
    CloseHandle(hPipe);
}

void PipeCore::ServerSetStop(bool stop){
    m_server_stopped_flag = stop;
}

std::vector<Idea> PipeCore::ServerGetIdeasLocal() {
    return m_serverIdeas;
}

// --- КЛІЄНТ ---
bool PipeCore::SendRequest(PipeCmd cmd, const void* dataIn, int sizeIn, void* dataOut, int sizeOut, int* bytesRead) {
    if (!WaitNamedPipeA(PIPE_NAME, NMPWAIT_WAIT_FOREVER))
    {
        return false;
    }
    HANDLE hPipe = CreateFileA(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hPipe == INVALID_HANDLE_VALUE) return false;

    PipeMessageHeader header = { (int)cmd, sizeIn };
    DWORD written;
    WriteFile(hPipe, &header, sizeof(header), &written, NULL);

    if (sizeIn > 0 && dataIn)
    {
        WriteFile(hPipe, dataIn, sizeIn, &written, NULL);
    }

    bool result = true;
    if (sizeOut > 0 && dataOut) {
        DWORD read;
        if (ReadFile(hPipe, dataOut, sizeOut, &read, NULL)) {
            if (bytesRead) *bytesRead = read;
        } else { result = false; }
    }
    CloseHandle(hPipe);
    return result;
}

void PipeCore::ClientAddIdea(const std::string& text, int worker_id) {
    Idea idea;
    strncpy_s(idea.text, text.c_str(), TEXT_SIZE);
    idea.worker_id = worker_id;
    idea.votes = 0;
    GenerateUUID(idea.uuid);
    SendRequest(CMD_ADD_IDEA, &idea, sizeof(Idea), NULL, 0, NULL);
}

bool PipeCore::ClientIsStopped() {
    bool stopped = true;
    SendRequest(CMD_CHECK_STOP, NULL, 0, &stopped, sizeof(bool), NULL);
    return stopped;
}

void PipeCore::ClientVote(const std::string& uuid) {
    SendRequest(CMD_VOTE, uuid.c_str(), UUID_SIZE, NULL, 0, NULL);
}

std::vector<Idea> PipeCore::ClientGetAllIdeas() {
    std::vector<Idea> list;
    if (!WaitNamedPipeA(PIPE_NAME, NMPWAIT_WAIT_FOREVER)) return list;
    HANDLE hPipe = CreateFileA(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hPipe == INVALID_HANDLE_VALUE) return list;

    PipeMessageHeader header = {
        (int)CMD_GET_ALL, 0
    };
    DWORD written, read;
    WriteFile(hPipe, &header, sizeof(header), &written, NULL);

    int count = 0;
    if (ReadFile(hPipe, &count, sizeof(int), &read, NULL) && count > 0) {
        list.resize(count);
        ReadFile(hPipe, list.data(), count * sizeof(Idea), &read, NULL);
    }
    CloseHandle(hPipe);
    return list;
}
