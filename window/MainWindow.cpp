#include "MainWindow.h"
#include "./ui_MainWindow.h"
#include "ipc/VirtualBoard.h"
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_ui(std::make_unique<Ui::MainWindow>())
{
    m_ui->setupUi(this);
}

// Use default - cause we had unique_ptr in class
MainWindow::~MainWindow() = default;

// Get window
Ui::MainWindow *MainWindow::getWindow()
{
    return m_ui.get();
}

void MainWindow::SetBoard(VirtualBoard *board)
{
    m_board = board;

    // Start Timer for 3 mins (180s)
    m_timeLeft = 180;
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::UpdateUI);
    m_timer->start(1000); // 1 sec
}

void MainWindow::UpdateUI()
{
    if (!m_board)
        return;

    // Time
    m_timeLeft--;
    QString timeStr = QString("Time Remaining: %1:%2")
                          .arg(m_timeLeft / 60, 2, 10, QChar('0'))
                          .arg(m_timeLeft % 60, 2, 10, QChar('0'));

    // TODO: Update specific label in UI if exists.
    // m_ui->lblStatus->setText(timeStr);

    // Refresh Ideas
    m_ui->listIdeas->clear();
    auto ideas = m_board->FetchAllIdeas();
    for (const auto &idea : ideas)
    {
        m_ui->listIdeas->addItem(QString::fromStdString(idea.text) +
                                 " [Votes: " + QString::number(idea.votes) + "]");
    }

    if (m_timeLeft <= 0)
    {
        EndSession();
    }
}

void MainWindow::EndSession()
{
    if (m_timer)
        m_timer->stop();
    if (m_board)
    {
        m_board->StopSession();
        m_board->SaveReport("report.txt");
    }
}
