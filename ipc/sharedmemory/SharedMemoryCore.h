#ifndef SHARED_MEMORY_CORE_H
#define SHARED_MEMORY_CORE_H

#include "../IIpcCore.h"
#include "Protocol.h"

#include <string>
#include <vector>
#include <windows.h>

class SharedMemoryCore : public IIpcCore
{
public:
    SharedMemoryCore(bool host);
    ~SharedMemoryCore();

public:
    bool AddIdea(const std::string &text, int worker_id) override;
    std::vector<Idea> GetAllIdeas() override;
    void Vote(const std::string &uuid) override;

public:
    void SetStopped(bool stop) override;
    bool IsStopped() override;
    bool IsConnected() override;

private:
    HANDLE m_hMapFile;
    HANDLE m_hMutex;
    BoardLayout *m_pBoard;
    bool m_isHost;

    void Lock();
    void Unlock();

    // Generate real UUID
    void GenerateUUID(char *buffer);
};

#endif // SHARED_MEMORY_CORE_H
