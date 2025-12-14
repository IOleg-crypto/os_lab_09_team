#ifndef PIPE_CORE_H
#define PIPE_CORE_H

#include <windows.h>
#include <vector>
#include <string>
#include "Protocol.h"

class PipeCore {
public:
    PipeCore(bool host);
    ~PipeCore();

    // Сервер
    void serverProcessPendingConnection();
    void serverSetStop(bool stop);
    std::vector<Idea> serverGetIdeasLocal();

    // Клієнт
    void clientAddIdea(const std::string& text, int worker_id);
    std::vector<Idea> clientGetAllIdeas();
    bool clientIsStopped();
    void clientVote(const std::string& uuid);

private:
    bool is_host;
    bool server_stopped_flag;
    std::vector<Idea> server_ideas;

    bool sendRequest(PipeCmd cmd, const void* dataIn, int sizeIn, void* dataOut, int sizeOut, int* bytesRead);
    
    // Генерація UUID
    void generateUUID(char* buffer);
};

#endif // PIPE_CORE_H