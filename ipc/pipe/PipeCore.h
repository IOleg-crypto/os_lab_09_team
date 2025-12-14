#ifndef PIPE_CORE_H
#define PIPE_CORE_H

#include "Protocol.h"

#include <windows.h>
#include <vector>
#include <string>

class PipeCore {
public:
    PipeCore(bool host);
    ~PipeCore();
public:
    // Server
    void ServerProcessPendingConnection();
    void ServerSetStop(bool stop);
    std::vector<Idea> ServerGetIdeasLocal();
public:
    // Client
    void ClientAddIdea(const std::string& text, int worker_id);
    std::vector<Idea> ClientGetAllIdeas();
    bool ClientIsStopped();
    void ClientVote(const std::string& uuid);

private:
    bool m_isHost;
    bool m_server_stopped_flag;
    std::vector<Idea> m_serverIdeas;

    bool SendRequest(PipeCmd cmd, const void* dataIn, int sizeIn, void* dataOut, int sizeOut, int* bytesRead);
private:
    // UUID generation
    void GenerateUUID(char* buffer);
};

#endif // PIPE_CORE_H
