#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <VirtualBoard.h>

#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class QTimer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    Ui::MainWindow *getWindow();
    // Also pass the IPC mode string for display
    void SetBoard(std::unique_ptr<VirtualBoard> board, const QString &ipcMode);

private slots:
    void UpdateUI();
    void UpdateIPCData();
    void UpdateVotingUI();
    void EndSession();
    void FinishVoting();

private:
    std::unique_ptr<Ui::MainWindow> m_ui;

    // Logic
    std::unique_ptr<VirtualBoard> m_board;
    std::unique_ptr<QTimer> m_timer;
    std::unique_ptr<QTimer> m_ipcTimer;
    int m_timeLeft;
    int m_votingTimeLeft;
};

#endif // MAINWINDOW_H
