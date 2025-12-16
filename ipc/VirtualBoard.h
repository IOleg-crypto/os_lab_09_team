#ifndef VIRTUAL_BOARD_H
#define VIRTUAL_BOARD_H

#include "IIpcCore.h"
#include <memory>
#include <string>
#include <vector>

// Forward declarations to avoid heavy includes if possible, or just include
#include "pipe/PipeCore.h"
#include "sharedmemory/SharedMemoryCore.h"

class VirtualBoard
{
public:
    enum class IpcType
    {
        SharedMemory,
        Pipes
    };
    // Host mode: Creates SHM/Pipe. Client mode: Opens SHM/Pipe.
    VirtualBoard(bool isHost, IpcType type = IpcType::SharedMemory);
    ~VirtualBoard();

    // Core Logic
    // Submits a new idea to the shared board. Returns true if successful.
    bool SubmitIdea(const std::string &text, int workerId);

    // Retrieves a list of all current ideas from the board.
    std::vector<Idea> FetchAllIdeas();

    // Increments vote count for a specific idea by UUID.
    void VoteForIdea(const std::string &uuid);

    // Control
    // Stops the brainstorming session (sets flag in SHM).
    void StopSession();

    // Checks if the session has been stopped by the supervisor.
    bool IsSessionStopped();
    bool IsConnected();

    // Reporting
    // Sorts ideas by votes and saves them to a text file.
    void SaveReport(const std::string &filename);

private:
    // IPC Core component (Polymorphic)
    std::unique_ptr<IIpcCore> m_memoryCore;
    bool m_isHost;
};

#endif // VIRTUAL_BOARD_H
