#include "WorkerWindow.h"
#include "ui_workerwindow.h"
#include <QDateTime>
#include <QMessageBox>
#include <QTimer>

WorkerWindow::WorkerWindow(int workerId, std::unique_ptr<VirtualBoard> board, QWidget *parent)
    : QMainWindow(parent), ui(std::make_unique<Ui::WorkerWindow>()), m_board(std::move(board)),
      m_workerId(workerId)
{
    ui->setupUi(this);
    setWindowTitle(QString("Worker #%1").arg(workerId));

    connect(ui->btnSend, &QPushButton::clicked, this, &WorkerWindow::onSendClicked);
    connect(ui->btnVote, &QPushButton::clicked, this, &WorkerWindow::onVoteClicked);

    ui->btnVote->setEnabled(true);
    // User wants to be able to change ID even before voting starts
    ui->spinBoxVoteID->setEnabled(true);

    // Timer to check connection status and phase
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this,
            [this]()
            {
                if (!m_board || !m_board->IsConnected())
                {
                    ui->label_2->setText("ERROR: NOT CONNECTED (RESTART APP)");
                    ui->label_2->setStyleSheet("color: red; font-weight: bold;");
                    ui->label_2->setVisible(true);
                    ui->btnVote->setEnabled(false);
                    ui->btnSend->setEnabled(false);
                    return;
                }

                // Check if supervisor stopped session (Voting Phase)
                if (m_board && m_board->IsSessionStopped())
                {
                    ui->lineEditIdea->setEnabled(false);
                    ui->btnSend->setEnabled(false);

                    ui->btnVote->setEnabled(true);
                    // ui->spinBoxVoteID is already enabled
                    ui->label_2->setText("PHASE 2: VOTING ACTIVE!");
                    ui->label_2->setStyleSheet("color: #28a745; font-weight: bold;");
                }
            });
    timer->start(1000);
}

WorkerWindow::~WorkerWindow() = default;

void WorkerWindow::onSendClicked()
{
    QString text = ui->lineEditIdea->text();
    if (text.isEmpty())
        return;

    if (m_board)
    {
        // Send idea to shared memory
        m_board->SubmitIdea(text.toStdString(), m_workerId);
    }

    ui->listMyIdeas->addItem(text);
    ui->lineEditIdea->clear();
}

void WorkerWindow::onVoteClicked()
{
    if (!m_board)
        return;

    int targetId = ui->spinBoxVoteID->value();

    if (m_votedIds.count(targetId))
    {
        QMessageBox::warning(this, "Error", "You have already voted for this idea!");
        return;
    }

    auto allIdeas = m_board->FetchAllIdeas();
    if (targetId > 0 && targetId <= (int)allIdeas.size())
    {
        // Send vote to shared memory
        const auto &idea = allIdeas[targetId - 1];
        m_board->VoteForIdea(idea.uuid);

        m_votedIds.insert(targetId);

        QMessageBox::information(this, "Success", QString("Vote cast for idea #%1").arg(targetId));
    }
    else
    {
        QMessageBox::warning(this, "Error", "Invalid Idea ID!");
    }
}
