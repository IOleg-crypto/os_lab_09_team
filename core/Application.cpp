#include "Application.h"
#include "../window/MainWindow.h"
#include "../window/workerwindow.h"
#include "ipc/ProcessManager.h"
#include "ipc/VirtualBoard.h"

#include <QString>
#include <QStringList>
#include <QThread>
#include <vector>


Application::Application(int &argc, char **argv)
    : m_app(std::make_unique<QApplication>(argc, argv)), m_isHost(true), m_workerId(0)
{
    // 1. Parse Args
    QStringList args = m_app->arguments();
    for (int i = 0; i < args.count(); ++i)
    {
        if (args[i] == "--worker" || args[i] == "-w")
        {
            if (i + 1 < args.count())
            {
                m_isHost = false;
                m_workerId = args[i + 1].toInt();
                break;
            }
        }
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
    // WorkerWindow *workerwindow = new WorkerWindow(m_workerId, std::move(m_board));
    std::unique_ptr<WorkerWindow> worker =
        std::make_unique<WorkerWindow>(m_workerId, std::move(m_board));

    // 2. Показуємо вікно
    worker->show();

    // 3. Запускаємо головний цикл подій Qt
    // Це дозволяє вікну реагувати на кліки, ввід тексту і не зависати.
    return m_app->exec();
}
