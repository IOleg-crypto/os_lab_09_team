#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <QProcess>
#include <memory>
#include <vector>

class ProcessManager
{
public:
    ProcessManager();
    ~ProcessManager();

    // Spawns 'count' worker processes using the executable at 'executablePath'.
    // Passes the --worker and ID arguments, and --ipc argument.
    void StartWorkers(int count, const QString &executablePath, const QString &ipcMode);

    // Terminates all running worker processes. First tries soft terminate, then force kill.
    void StopAllWorkers();

    // Returns true if there are any workers in the list.
    bool AreWorkersRunning() const;

private:
    // List of process handles. We own them.
    std::vector<std::unique_ptr<QProcess>> m_workers;
};

#endif // PROCESS_MANAGER_H
