#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <memory>

#include <ipc/VirtualBoard.h>
#include <ipc/ProcessManager.h>

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
public:
    Ui::MainWindow *getWindow();
    void SetBoard(VirtualBoard *board);

private:
    std::unique_ptr<Ui::MainWindow> m_ui;

    // Logic
    VirtualBoard *m_board = nullptr;
    QTimer *m_timer = nullptr;
    int m_timeLeft;

    void UpdateUI();
    void EndSession();
};
#endif // MAINWINDOW_H
