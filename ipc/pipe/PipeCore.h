#ifndef PIPE_CORE_H
#define PIPE_CORE_H

#include "../IIpcCore.h"
#include "Protocol.h"

#include <string>
#include <vector>
#include <windows.h>


class PipeCore : public IIpcCore
{
public:
    PipeCore(bool host);
    ~PipeCore();
    // IIpcCore Implementation
    bool AddIdea(const std::string &text, int worker_id) override;
    std::vector<Idea> GetAllIdeas() override;
    void Vote(const std::string &uuid) override;

    void SetStopped(bool stop) override;
    bool IsStopped() override;
    bool IsConnected() override;

private:
    // Server internals
    void ServerProcessPendingConnection();
    std::vector<Idea> ServerGetIdeasLocal();

    // Client internals
    void ClientAddIdea(const std::string &text, int worker_id);
    std::vector<Idea> ClientGetAllIdeas();
    bool ClientIsStopped();
    void ClientVote(const std::string &uuid);

private:
    bool m_isHost;
    bool m_server_stopped_flag;
    std::vector<Idea> m_serverIdeas;

    bool SendRequest(PipeCmd cmd, const void *dataIn, int sizeIn, void *dataOut, int sizeOut,
                     int *bytesRead);

private:
    // UUID generation
    void GenerateUUID(char *buffer);
};

#endif // PIPE_CORE_H
