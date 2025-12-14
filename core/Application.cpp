#include "Application.h"
#include "ipc/ProcessManager.h"
#include "ipc/VirtualBoard.h"

#include <QCommandLineParser>
#include <QThread>
#include <random>
#include <vector>

Application::Application(int &argc, char **argv)
    : m_app(std::make_unique<QApplication>(argc, argv)), m_isHost(true), m_workerId(0)
{
    // 1. Parse Args
    QCommandLineParser parser;
    QCommandLineOption workerOption("worker", "Run as worker", "id");
    parser.addOption(workerOption);
    parser.process(*m_app);

    if (parser.isSet(workerOption))
    {
        m_isHost = false;
        m_workerId = parser.value(workerOption).toInt();
    }

    // 2. Create Logic (IPC)
    m_board = std::make_unique<VirtualBoard>(m_isHost);

    if (m_isHost)
    {
        // 3. Create UI (Only for Supervisor)
        m_window = std::make_unique<MainWindow>();

        // 4. DEPENDENCY INJECTION
        m_window->SetBoard(m_board);

        // 5. Create Process Manager
        m_procManager = std::make_unique<ProcessManager>();
    }
}

Application::~Application()
{
    if (m_isHost && m_procManager)
    {
        m_procManager->StopAllWorkers();
    }
}

int Application::Run()
{
    if (m_isHost)
    {
        // SUPERVISOR MODE
        m_window->show();
        // Launch 3 workers
        m_procManager->StartWorkers(3, QApplication::applicationFilePath());
        return m_app->exec(); // <-- RETURN тут є!
    }
    else
    {
        // WORKER MODE
        return RunWorker(); // <-- RETURN тут є!
    }
}

int Application::RunWorker()
{
    // Random generator for idea diversity
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> ideaDelay(5, 15); // 5-15 seconds between ideas
    std::uniform_int_distribution<> ideaType(0, 9);

    // Pool of idea templates
    std::vector<std::string> ideaTemplates = {
        "Implement AI-powered code review",  "Add real-time collaboration features",
        "Create mobile app version",         "Integrate blockchain for security",
        "Develop dark mode theme",           "Add voice control interface",
        "Implement automated testing suite", "Create plugin marketplace",
        "Add multilingual support",          "Develop VR interface"};

    // PHASE 1: Generate Ideas (until stopped)
    int ideasSubmitted = 0;
    while (!m_board->IsSessionStopped() && ideasSubmitted < 10) // Max 10 ideas per worker
    {
        // Generate unique idea
        std::string idea = ideaTemplates[ideaType(gen)] + " (Worker " + std::to_string(m_workerId) +
                           " v" + std::to_string(ideasSubmitted + 1) + ")";

        if (m_board->SubmitIdea(idea, m_workerId))
        {
            ideasSubmitted++;
        }

        // Random delay between ideas
        int delay = ideaDelay(gen) * 1000;
        QThread::msleep(delay);
    }

    // PHASE 2: Wait for voting phase (Wait for Supervisor to stop session)
    while (!m_board->IsSessionStopped())
    {
        QThread::msleep(100);
    }

    QThread::sleep(1); // Brief pause before voting

    // PHASE 3: Vote for top ideas
    auto allIdeas = m_board->FetchAllIdeas();

    if (!allIdeas.empty())
    {
        // Vote randomly for 3 different ideas (excluding own ideas)
        std::vector<int> votableIndices;
        for (size_t i = 0; i < allIdeas.size(); ++i)
        {
            if (allIdeas[i].worker_id != m_workerId)
            {
                votableIndices.push_back(i);
            }
        }

        // Shuffle and pick top 3
        std::shuffle(votableIndices.begin(), votableIndices.end(), gen);
        int votesToCast = std::min(3, (int)votableIndices.size());

        for (int i = 0; i < votesToCast; ++i)
        {
            const auto &idea = allIdeas[votableIndices[i]];
            m_board->VoteForIdea(idea.uuid);
            QThread::msleep(500);
        }
    }

    return 0; // <-- EXIT success
}
