#include "workerwindow.h"
// ВАЖЛИВО: Оскільки файл називається workerwindow.ui (маленькими),
// то інклуд має бути саме таким:
#include "ui_workerwindow.h"

#include <QDateTime>

WorkerWindow::WorkerWindow(int workerId, VirtualBoard* board, QWidget *parent)
    : QMainWindow(parent),
      ui(std::make_unique<Ui::WorkerWindow>()),
      m_board(board),
      m_workerId(workerId)
{
    ui->setupUi(this);

    // Встановимо заголовок
    setWindowTitle(QString("Worker #%1").arg(workerId));

           // Підключимо кнопку
    connect(ui->btnSend, &QPushButton::clicked, this, &WorkerWindow::onSendClicked);
}

WorkerWindow::~WorkerWindow() = default;

void WorkerWindow::onSendClicked()
{
    QString text = ui->lineEditIdea->text();
    if (text.isEmpty()) return;

    if (m_board) {
        // ТУТ БУЛА ПОМИЛКА з типами.
        // Я поміняв аргументи місцями (текст, потім ID).
        // Якщо знову буде помилка - поміняй назад: (m_workerId, text.toStdString())
        m_board->SubmitIdea(text.toStdString(), m_workerId);
    }

    ui->listMyIdeas->addItem(text);
    ui->lineEditIdea->clear();
}
