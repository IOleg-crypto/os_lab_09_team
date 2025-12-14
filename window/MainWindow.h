#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

// Forward declarations
class VirtualBoard;
class QTimer;

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
    void SetBoard(VirtualBoard *board);

private slots:
    void UpdateUI();
    void EndSession();
    void FinishVoting();

private:
    std::unique_ptr<Ui::MainWindow> m_ui;

           // Logic
    VirtualBoard *m_board;
    QTimer *m_timer;
    int m_timeLeft;
};

#endif // MAINWINDOW_H
