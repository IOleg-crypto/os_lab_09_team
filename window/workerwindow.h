#ifndef WORKERWINDOW_H
#define WORKERWINDOW_H

#include <QMainWindow>
#include <memory>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QSpinBox> // лічильник
#include <QLabel>
#include <QVBoxLayout>
#include <QMessageBox> // повідомлень
#include <set>
#include "../ipc/VirtualBoard.h"

namespace Ui {
class WorkerWindow;
}

class WorkerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit WorkerWindow(int workerId, VirtualBoard* board, QWidget *parent = nullptr);
    ~WorkerWindow();

private slots:
    void onSendClicked();
    void onVoteClicked();

private:
    std::unique_ptr<Ui::WorkerWindow> ui;
    VirtualBoard* m_board;
    int m_workerId;
    std::set<int> m_votedIds;
    void checkPhase();
};

#endif // WORKERWINDOW_H
