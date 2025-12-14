#include "Application.h"
#include "ipc/ProcessManager.h"
#include "ipc/VirtualBoard.h"

#include <QCommandLineParser>
#include <QThread>

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
    // VirtualBoard handles all communications (Shared Memory, etc).
    m_board = std::make_unique<VirtualBoard>(m_isHost);

    if (m_isHost)
    {
        // 3. Create UI (Only for Supervisor)
        m_window = std::make_unique<MainWindow>();

        // 4. DEPENDENCY INJECTION: Give Board to Window
        // This avoids cyclic dependency: Window knows Board, but Board doesn't know Window.
        m_window->SetBoard(m_board.get());

        // 5. Create Process Manager to handle Workers
        m_procManager = std::make_unique<ProcessManager>();
    }
}

Application::~Application()
{
    // Cleanup workers on exit (Supervisor only)
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
        // Launch 3 workers (Simulation)
        m_procManager->StartWorkers(3, QApplication::applicationFilePath());
        return m_app->exec();
    }
    else
    {
        // WORKER MODE (Headless)
        // Loop until supervisor stops the session
        while (!m_board->IsSessionStopped())
        {
            // Simulate work: Submit ideas
            m_board->SubmitIdea("Worker Idea " + std::to_string(m_workerId), m_workerId);
            QThread::msleep(1000); // Wait 1 sec
        }
        // Vote... (Voting logic should be here)
        return 0;
    }
}
