#include "MainWindow.h"
#include "./ui_MainWindow.h"
#include "ipc/VirtualBoard.h"

#include <QMessageBox>
#include <QTimer>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_ui(std::make_unique<Ui::MainWindow>()), m_board(nullptr),
      m_timer(nullptr), m_timeLeft(0)
{
    m_ui->setupUi(this);
}

MainWindow::~MainWindow() = default;

Ui::MainWindow *MainWindow::getWindow()
{
    return m_ui.get();
}

void MainWindow::SetBoard(std::unique_ptr<VirtualBoard> &board)
{
    if (board)
    {
        m_board = std::move(board);
    }

    // Start Timer for 3 mins (180s)
    // Start Timer for 3 mins (180s)
    m_timeLeft = 180;
    m_timer = std::make_unique<QTimer>(); // No parent, we own it
    connect(m_timer.get(), &QTimer::timeout, this, &MainWindow::UpdateUI);
    m_timer->start(1000); // 1 sec

    // Initial UI update
    UpdateUI();
}

void MainWindow::UpdateUI()
{
    if (!m_board)
        return;

    // Update Timer Display
    m_timeLeft--;
    int minutes = m_timeLeft / 60;
    int seconds = m_timeLeft % 60;

    QString timeStr = QString("Generating Ideas... (%1:%2 remaining)")
                          .arg(minutes, 2, 10, QChar('0'))
                          .arg(seconds, 2, 10, QChar('0'));

    m_ui->lblStatus->setText(timeStr);

    // Refresh Ideas List
    auto ideas = m_board->FetchAllIdeas();

    for (size_t i = 0; i < ideas.size(); ++i)
    {
        const auto &idea = ideas[i];
        QString itemText = QString("#%1: %2 [Worker %3, Votes: %4]")
                               .arg(i + 1)
                               .arg(QString::fromStdString(idea.text))
                               .arg(idea.worker_id)
                               .arg(idea.votes);

        if (i < m_ui->listIdeas->count())
        {
            // Update existing item
            m_ui->listIdeas->item(i)->setText(itemText);
        }
        else
        {
            // Add new item
            m_ui->listIdeas->addItem(itemText);
        }
    }

    // Check if time is up
    if (m_timeLeft <= 0)
    {
        EndSession();
    }
}

void MainWindow::EndSession()
{
    if (m_timer)
    {
        m_timer->stop();
        m_timer.reset();
    }

    if (m_board)
    {
        // Stop idea generation
        m_board->StopSession();

        m_ui->lblStatus->setText("Idea Generation Complete. Starting Voting Phase...");

        // Wait for workers to vote (give them 30 seconds)
        QTimer::singleShot(30000, this, &MainWindow::FinishVoting);
    }
}

void MainWindow::FinishVoting()
{
    if (!m_board)
        return;

    // Save report with top 3 ideas
    m_board->SaveReport("report.txt");

    m_ui->lblStatus->setText("Session Complete. Check report.txt for results.");

    // Show final results
    auto ideas = m_board->FetchAllIdeas();
    m_ui->listIdeas->clear();

    // Sort by votes
    std::sort(ideas.begin(), ideas.end(),
              [](const Idea &a, const Idea &b) { return a.votes > b.votes; });

    // Display top 3
    m_ui->listIdeas->addItem("TOP 3 IDEAS");
    for (int i = 0; i < std::min(3, (int)ideas.size()); ++i)
    {
        QString itemText = QString("#%1: %2 [%3 votes]")
                               .arg(i + 1)
                               .arg(QString::fromStdString(ideas[i].text))
                               .arg(ideas[i].votes);
        m_ui->listIdeas->addItem(itemText);
    }

    QMessageBox::information(this, "Session Complete",
                             "session finished\nResults saved to report.txt");
}
