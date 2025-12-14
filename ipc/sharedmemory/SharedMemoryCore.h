#ifndef SHARED_MEMORY_CORE_H
#define SHARED_MEMORY_CORE_H

#include "Protocol.h"

#include <windows.h>
#include <vector>
#include <string>


class SharedMemoryCore {
public:
    SharedMemoryCore(bool host);
    ~SharedMemoryCore();
public:
    bool AddIdea(const std::string& text, int worker_id);
    std::vector<Idea> GetAllIdeas();
    void Vote(const std::string& uuid);
public:
    void SetStopped(bool stop);
    bool IsStopped();

private:
    HANDLE m_hMapFile;
    HANDLE m_hMutex;
    BoardLayout* m_pBoard;
    bool m_isHost;

    void Lock();
    void Unlock();
    
    // Generate real UUID
    void GenerateUUID(char* buffer);
};

#endif // SHARED_MEMORY_CORE_H
