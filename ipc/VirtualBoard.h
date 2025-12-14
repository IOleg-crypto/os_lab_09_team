#ifndef VIRTUAL_BOARD_H
#define VIRTUAL_BOARD_H

#include "sharedmemory/SharedMemoryCore.h"
#include <Protocol.h>
#include <memory>
#include <string>
#include <vector>

class VirtualBoard
{
public:
    // Host mode: Creates SHM. Client mode: Opens SHM.
    VirtualBoard(bool isHost);
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

    // Reporting
    // Sorts ideas by votes and saves them to a text file.
    void SaveReport(const std::string &filename);

private:
    // IPC Core component (Composition)
    std::unique_ptr<SharedMemoryCore> m_memoryCore;
    bool m_isHost;
};

#endif // VIRTUAL_BOARD_H
