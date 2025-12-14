#include "SharedMemoryCore.h"
#include <iostream>
#include <rpc.h> // Бібліотека для UUID

// Автоматичне підключення лінкера (для MSVC)
#pragma comment(lib, "Rpcrt4.lib")

SharedMemoryCore::SharedMemoryCore(bool host) : is_host(host), hMapFile(NULL), pBoard(NULL) {
    hMutex = CreateMutexA(NULL, FALSE, MUTEX_NAME);

    if (is_host) {
        hMapFile = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 
                                      0, sizeof(BoardLayout), SHM_NAME);
    } else {
        hMapFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, SHM_NAME);
    }

    if (hMapFile) {
        pBoard = (BoardLayout*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(BoardLayout));
        if (is_host && pBoard) {
            lock();
            memset(pBoard, 0, sizeof(BoardLayout));
            unlock();
        }
    }
}

SharedMemoryCore::~SharedMemoryCore() {
    if (pBoard) UnmapViewOfFile(pBoard);
    if (hMapFile) CloseHandle(hMapFile);
    if (hMutex) CloseHandle(hMutex);
}

void SharedMemoryCore::lock() { WaitForSingleObject(hMutex, INFINITE); }
void SharedMemoryCore::unlock() { ReleaseMutex(hMutex); }

// --- РЕАЛІЗАЦІЯ СПРАВЖНЬОГО UUID ---
void SharedMemoryCore::generateUUID(char* buffer) {
    UUID uuid;
    UuidCreate(&uuid); 
    
    unsigned char* str;
    UuidToStringA(&uuid, &str); 
    
    if (str != NULL) {
        strncpy_s(buffer, UUID_SIZE, (char*)str, UUID_SIZE - 1);
        RpcStringFreeA(&str); 
    }
}

bool SharedMemoryCore::addIdea(const std::string& text, int worker_id) {
    if (!pBoard) return false;
    bool success = false;

    lock();
    if (!pBoard->is_stopped && pBoard->count < MAX_IDEAS) {
        Idea& idea = pBoard->ideas[pBoard->count];
        strncpy_s(idea.text, text.c_str(), TEXT_SIZE);
        generateUUID(idea.uuid);
        idea.worker_id = worker_id;
        idea.votes = 0;
        pBoard->count++;
        success = true;
    }
    unlock();
    return success;
}

std::vector<Idea> SharedMemoryCore::getAllIdeas() {
    std::vector<Idea> list;
    if (!pBoard) return list;

    lock();
    for (int i = 0; i < pBoard->count; ++i) {
        list.push_back(pBoard->ideas[i]);
    }
    unlock();
    return list;
}

void SharedMemoryCore::vote(const std::string& uuid) {
    if (!pBoard) return;
    lock();
    for (int i = 0; i < pBoard->count; ++i) {
        if (strcmp(pBoard->ideas[i].uuid, uuid.c_str()) == 0) {
            pBoard->ideas[i].votes++;
            break;
        }
    }
    unlock();
}

void SharedMemoryCore::setStopped(bool stop) {
    if (pBoard) {
        lock();
        pBoard->is_stopped = stop;
        unlock();
    }
}

bool SharedMemoryCore::isStopped() {
    if (!pBoard) return true;
    lock();
    bool s = pBoard->is_stopped;
    unlock();
    return s;
}