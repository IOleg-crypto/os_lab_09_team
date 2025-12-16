#include "ProcessManager.h"
#include <QDebug>

ProcessManager::ProcessManager()
{
}

ProcessManager::~ProcessManager()
{
    StopAllWorkers();
}

void ProcessManager::StartWorkers(int count, const QString &executablePath, const QString &ipcMode)
{
    for (int i = 0; i < count; ++i)
    {
        // Create new QProcess for each worker
        auto process = std::make_unique<QProcess>();
        QStringList args;
        // Pass worker flag and unique ID
        args << "--worker" << QString::number(i + 1);
        // Pass IPC mode
        args << "--ipc" << ipcMode;

        process->start(executablePath, args);
        process->waitForStarted(); // Ensure it started

        m_workers.push_back(std::move(process));
    }
}

void ProcessManager::StopAllWorkers()
{
    for (const auto &p : m_workers)
    {
        if (p->state() == QProcess::Running)
        {
            p->terminate();
            if (!p->waitForFinished(1000))
                p->kill();
        }
    }
    m_workers.clear();
}

bool ProcessManager::AreWorkersRunning() const
{
    return !m_workers.empty();
}
