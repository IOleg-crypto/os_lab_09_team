#include "Application.h"
#include "ipc/ProcessManager.h"
#include "ipc/VirtualBoard.h"
#include "../window/MainWindow.h"
#include "../window/workerwindow.h"

#include <QCommandLineParser>
#include <QThread>
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
           // Створюємо дошку (для воркера isHost = false)
    m_board = std::make_unique<VirtualBoard>(m_isHost);

    if (m_isHost)
    {
        // 3. Create UI (Only for Supervisor)
        m_window = std::make_unique<MainWindow>();
        m_window->SetBoard(std::move(m_board));

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
        // Запускаємо 3 воркерів
        m_procManager->StartWorkers(3, QApplication::applicationFilePath());

        return m_app->exec();
    }
    else
    {
        // WORKER MODE
        return RunWorker();
    }
}

int Application::RunWorker()
{
    // --- ТУТ БУВ КОД СИМУЛЯЦІЇ (БОТА) ---
    // Ми його видаляємо, бо тепер у нас є графічний інтерфейс,
    // і ти сам будеш вводити ідеї руками.

           // 1. Створюємо твоє вікно виконавця
           // Передаємо ID і вказівник на дошку, щоб вікно могло слати дані
           // Використовуємо 'new', щоб об'єкт жив поки працює програма
    WorkerWindow *w = new WorkerWindow(m_workerId, m_board.get());

    // 2. Показуємо вікно
    w->show();

           // 3. Запускаємо головний цикл подій Qt
           // Це дозволяє вікну реагувати на кліки, ввід тексту і не зависати.
    return m_app->exec();
}
