#ifndef WORKERWINDOW_H
#define WORKERWINDOW_H

#include <QMainWindow>
#include <memory>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QSpinBox> // Додали для лічильника
#include <QLabel>
#include <QVBoxLayout>
#include <QMessageBox> // Додали для повідомлень

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
    void onVoteClicked(); // Новий слот

private:
    std::unique_ptr<Ui::WorkerWindow> ui;
    VirtualBoard* m_board;
    int m_workerId;
};

#endif // WORKERWINDOW_H
