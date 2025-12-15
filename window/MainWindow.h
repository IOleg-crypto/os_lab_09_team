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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    Ui::MainWindow *getWindow();
    void SetBoard(std::unique_ptr<VirtualBoard>board);

private slots:
    void UpdateUI();
    void EndSession();
    void FinishVoting();

private:
    std::unique_ptr<Ui::MainWindow> m_ui;

    // Logic
    std::unique_ptr<VirtualBoard>m_board;
    std::unique_ptr<QTimer> m_timer;
    int m_timeLeft;
};

#endif // MAINWINDOW_H
