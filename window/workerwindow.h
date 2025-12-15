#ifndef WORKERWINDOW_H
#define WORKERWINDOW_H

#include "ipc/VirtualBoard.h"
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <memory>
#include <set>

namespace Ui
{
class WorkerWindow;
}

class WorkerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit WorkerWindow(int workerId, std::unique_ptr<VirtualBoard> board,
                          QWidget *parent = nullptr);
    ~WorkerWindow();

private slots:
    void onSendClicked();
    void onVoteClicked();

private:
    std::unique_ptr<Ui::WorkerWindow> ui;
    std::unique_ptr<VirtualBoard> m_board;
    int m_workerId;
    std::set<int> m_votedIds;
    void checkPhase();
};

#endif // WORKERWINDOW_H
