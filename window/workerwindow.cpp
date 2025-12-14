#include "workerwindow.h"
#include "ui_workerwindow.h"
#include <QDateTime>

WorkerWindow::WorkerWindow(int workerId, VirtualBoard* board, QWidget *parent)
    : QMainWindow(parent),
      ui(std::make_unique<Ui::WorkerWindow>()),
      m_board(board),
      m_workerId(workerId)
{
    ui->setupUi(this);
    setWindowTitle(QString("Worker #%1").arg(workerId));

    connect(ui->btnSend, &QPushButton::clicked, this, &WorkerWindow::onSendClicked);

    // Підключаємо нову кнопку голосування
    connect(ui->btnVote, &QPushButton::clicked, this, &WorkerWindow::onVoteClicked);
}

WorkerWindow::~WorkerWindow() = default;

void WorkerWindow::onSendClicked()
{
    QString text = ui->lineEditIdea->text();
    if (text.isEmpty()) return;

    if (m_board) {
        // Відправляємо ідею
        m_board->SubmitIdea(text.toStdString(), m_workerId);
    }

    ui->listMyIdeas->addItem(text);
    ui->lineEditIdea->clear();
}

void WorkerWindow::onVoteClicked()
{
    if (!m_board) return;

           // 1. Отримуємо номер ID з поля вводу
    int targetId = ui->spinBoxVoteID->value(); // Це число (1, 2, 3...)

           // 2. Отримуємо всі ідеї з сервера, щоб знайти потрібну
    auto allIdeas = m_board->FetchAllIdeas();

           // 3. Перевіряємо, чи існує така ідея
           // (targetId - 1, бо люди рахують з 1, а програмісти з 0)
    if (targetId > 0 && targetId <= (int)allIdeas.size()) {

        // Знаходимо UUID (унікальний код) ідеї за її номером у списку
        const auto& idea = allIdeas[targetId - 1];

        // Голосуємо!
        m_board->VoteForIdea(idea.uuid);

        QMessageBox::information(this, "Voted", QString("Voted for idea #%1").arg(targetId));
    } else {
        QMessageBox::warning(this, "Error", "Invalid Idea ID!");
    }
}
