#ifndef I_IPC_CORE_H
#define I_IPC_CORE_H

#include "Protocol.h"
#include <string>
#include <vector>

// Abstract Strategy Interface for IPC
class IIpcCore
{
public:
    virtual ~IIpcCore() = default;

    // Core Logic
    virtual bool AddIdea(const std::string &text, int worker_id) = 0;
    virtual std::vector<Idea> GetAllIdeas() = 0;
    virtual void Vote(const std::string &uuid) = 0;

    // Control
    virtual void SetStopped(bool stop) = 0;
    virtual bool IsStopped() = 0;
    virtual bool IsConnected() = 0;
};

#endif // I_IPC_CORE_H
