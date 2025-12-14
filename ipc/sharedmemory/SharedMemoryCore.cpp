#include <rpc.h> // Library for UUID
// Include class
#include "SharedMemoryCore.h"

// Link (for MSVC)
#pragma comment(lib, "Rpcrt4.lib")

SharedMemoryCore::SharedMemoryCore(bool host) : m_isHost(host), m_hMapFile(NULL), m_pBoard(nullptr)
{
    m_hMutex = CreateMutexA(NULL, FALSE, MUTEX_NAME);

    if (m_isHost)
    {
        m_hMapFile = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0,
                                        sizeof(BoardLayout), SHM_NAME);
    }
    else
    {
        m_hMapFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, SHM_NAME);
    }

    if (m_hMapFile)
    {
        m_pBoard = (BoardLayout *)MapViewOfFile(m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0,
                                                sizeof(BoardLayout));
        if (m_isHost && m_pBoard)
        {
            Lock();
            memset(m_pBoard, 0, sizeof(BoardLayout));
            Unlock();
        }
    }
}

SharedMemoryCore::~SharedMemoryCore()
{
    if (m_pBoard)
    {
        UnmapViewOfFile(m_pBoard);
    }
    if (m_hMapFile)
    {
        CloseHandle(m_hMapFile);
    }
    if (m_hMutex)
    {
        CloseHandle(m_hMutex);
    }
}

void SharedMemoryCore::Lock()
{
    WaitForSingleObject(m_hMutex, INFINITE);
}
void SharedMemoryCore::Unlock()
{
    ReleaseMutex(m_hMutex);
}

// --- РЕАЛІЗАЦІЯ СПРАВЖНЬОГО UUID ---
void SharedMemoryCore::GenerateUUID(char *buffer)
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

bool SharedMemoryCore::AddIdea(const std::string &text, int worker_id)
{
    if (!m_pBoard)
    {
        return false;
    }
    bool success = false;

    Lock();
    if (!m_pBoard->is_stopped && m_pBoard->count < MAX_IDEAS)
    {
        Idea &idea = m_pBoard->ideas[m_pBoard->count];
        strncpy_s(idea.text, text.c_str(), TEXT_SIZE);
        GenerateUUID(idea.uuid);
        idea.worker_id = worker_id;
        idea.votes = 0;
        m_pBoard->count++;
        success = true;
    }
    Unlock();
    return success;
}

std::vector<Idea> SharedMemoryCore::GetAllIdeas()
{
    std::vector<Idea> list;
    if (!m_pBoard)
    {
        return list;
    }

    Lock();
    for (int i = 0; i < m_pBoard->count; ++i)
    {
        list.push_back(m_pBoard->ideas[i]);
    }
    Unlock();
    return list;
}

void SharedMemoryCore::Vote(const std::string &uuid)
{
    if (!m_pBoard)
    {
        return;
    }
    Lock();
    for (int i = 0; i < m_pBoard->count; ++i)
    {
        if (strcmp(m_pBoard->ideas[i].uuid, uuid.c_str()) == 0)
        {
            m_pBoard->ideas[i].votes++;
            break;
        }
    }
    Unlock();
}

void SharedMemoryCore::SetStopped(bool stop)
{
    if (m_pBoard)
    {
        Lock();
        m_pBoard->is_stopped = stop;
        Unlock();
    }
}

bool SharedMemoryCore::IsStopped()
{
    if (!m_pBoard)
    {
        return true;
    }
    Lock();
    bool s = m_pBoard->is_stopped;
    Unlock();
    return s;
}
