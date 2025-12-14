#ifndef SHARED_MEMORY_CORE_H
#define SHARED_MEMORY_CORE_H

#include <windows.h>
#include <vector>
#include <string>
#include "Protocol.h"

class SharedMemoryCore {
public:
    SharedMemoryCore(bool host);
    ~SharedMemoryCore();

    bool addIdea(const std::string& text, int worker_id);
    std::vector<Idea> getAllIdeas();
    void vote(const std::string& uuid);

    void setStopped(bool stop);
    bool isStopped();

private:
    HANDLE hMapFile;
    HANDLE hMutex;
    BoardLayout* pBoard;
    bool is_host;

    void lock();
    void unlock();
    
    // Генерація справжнього UUID
    void generateUUID(char* buffer);
};

#endif // SHARED_MEMORY_CORE_H