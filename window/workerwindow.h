#ifndef WORKERWINDOW_H
#define WORKERWINDOW_H

#include <QMainWindow>
#include <memory>
// Додаємо ці інклуди, щоб Qt знав типи об'єктів
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QVBoxLayout>

// Твій клас для зв'язку (перевір шлях, якщо він в іншій папці)
#include "../ipc/VirtualBoard.h"

namespace Ui {
class WorkerWindow;
}

class WorkerWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Конструктор приймає ID воркера і вказівник на дошку
    explicit WorkerWindow(int workerId, VirtualBoard* board, QWidget *parent = nullptr);
    ~WorkerWindow();

private slots:
    void onSendClicked(); // Слот для натискання кнопки

private:
    std::unique_ptr<Ui::WorkerWindow> ui;
    VirtualBoard* m_board;
    int m_workerId;
};

#endif // WORKERWINDOW_H
