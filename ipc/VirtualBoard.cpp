#include "VirtualBoard.h"
#include <algorithm>
#include <fstream>
#include <iostream>

// Constructor: Initializes the low-level Shared Memory Core.
// If isHost is true, it creates the memory mapping. If false, it opens existing one.
// Constructor: Initializes the selected IPC Core.
VirtualBoard::VirtualBoard(bool isHost, IpcType type) : m_isHost(isHost)
{
    if (type == IpcType::Pipes)
    {
        m_memoryCore = std::make_unique<PipeCore>(isHost);
    }
    else
    {
        m_memoryCore = std::make_unique<SharedMemoryCore>(isHost);
    }
}

VirtualBoard::~VirtualBoard()
{
}

// Adds an idea to the board. Thread-safe due to internal Mutex usage.
bool VirtualBoard::SubmitIdea(const std::string &text, int workerId)
{
    if (!m_memoryCore)
        return false;
    if (!m_memoryCore)
        return false;
    // Delegate to shared memory
    return m_memoryCore->AddIdea(text, workerId);
}

std::vector<Idea> VirtualBoard::FetchAllIdeas()
{
    if (!m_memoryCore)
        return {};
    if (!m_memoryCore)
        return {};
    // Retrieve collection of ideas
    return m_memoryCore->GetAllIdeas();
}

void VirtualBoard::VoteForIdea(const std::string &uuid)
{
    if (!m_memoryCore)
        return;
    m_memoryCore->Vote(uuid);
}

void VirtualBoard::StopSession()
{
    if (!m_memoryCore)
        return;
    m_memoryCore->SetStopped(true);
}

bool VirtualBoard::IsSessionStopped()
{
    if (!m_memoryCore)
        return true;
    return m_memoryCore->IsStopped();
}

bool VirtualBoard::IsConnected()
{
    if (!m_memoryCore)
        return false;
    return m_memoryCore->IsConnected();
}

// Generates a report file.
// 1. Fetches all ideas.
// 2. Sorts them by votes (Descending).
// 3. Writes top 3 to the file.
void VirtualBoard::SaveReport(const std::string &filename)
{
    auto ideas = FetchAllIdeas();

    // Sort ideas by votes
    std::sort(ideas.begin(), ideas.end(),
              [](const Idea &a, const Idea &b) { return a.votes > b.votes; });

    std::ofstream file(filename);
    if (file.is_open())
    {
        file << "Top Ideas:\n";
        int top = 0;
        for (const auto &idea : ideas)
        {
            top++;
            if (top > 3)
                break;
            file << "#" << top << " " << idea.text << " (" << idea.votes << ")\n";
        }
        file.close();
    }
}
