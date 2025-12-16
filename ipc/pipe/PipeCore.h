#ifndef PIPE_CORE_H
#define PIPE_CORE_H

#include "../IIpcCore.h"
#include "Protocol.h"

#include <mutex>
#include <string>
#include <vector>
#include <windows.h>

class PipeCore : public IIpcCore
{
public:
    explicit PipeCore(bool host);
    ~PipeCore() override;

    // ---- IIpcCore implementation ----
    bool AddIdea(const std::string &text, int worker_id) override;
    std::vector<Idea> GetAllIdeas() override;
    void Vote(const std::string &uuid) override;

    void SetStopped(bool stop) override;
    bool IsStopped() override;
    bool IsConnected() override;

    // ВАЖЛИВО:
    // Викликається ТІЛЬКИ якщо m_isHost == true
    // Запускати в окремому потоці
    void ServerRun();

private:
    // ---- Client internals ----
    void ClientAddIdea(const std::string &text, int worker_id);
    std::vector<Idea> ClientGetAllIdeas();
    bool ClientIsStopped();
    void ClientVote(const std::string &uuid);

    bool SendRequest(PipeCmd cmd, const void *dataIn, int sizeIn, void *dataOut, int sizeOut,
                     int *bytesRead);

    // ---- Utils ----
    void GenerateUUID(char *buffer);

private:
    bool m_isHost{false};
    bool m_server_stopped_flag{false};

    HANDLE m_hPipe{INVALID_HANDLE_VALUE};

    std::vector<Idea> m_serverIdeas;
    std::mutex m_ideasMutex;
};

#endif // PIPE_CORE_H
