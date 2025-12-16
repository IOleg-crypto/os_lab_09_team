#ifndef APPLICATION_H
#define APPLICATION_H

#include "ipc/ProcessManager.h"
#include "ipc/VirtualBoard.h"
#include "window/MainWindow.h"

#include "../ipc/VirtualBoard.h" // Needed for IpcType enum
#include <QApplication>
#include <QObject> // Added for QObject
#include <memory>


class Application
{
private:
    std::unique_ptr<QApplication> m_app;
    std::unique_ptr<MainWindow> m_window;

private:
    // Logic (From IPC Lib)
    std::unique_ptr<VirtualBoard> m_board;
    std::unique_ptr<ProcessManager> m_procManager;

private:
    bool m_isHost;
    int m_workerId;
    VirtualBoard::IpcType m_ipcType; // Store chosen type

public:
    Application(int &argc, char **argv);
    ~Application();

    // Init and run application
    int Run();

private:
    // Worker-specific logic
    int RunWorker();
};

#endif // APPLICATION_H
