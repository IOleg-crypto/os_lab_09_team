#include "MainWindow.h"
#include "./ui_MainWindow.h"
#include "ipc/VirtualBoard.h"

#include <random>
#include <QTimer>
#include <QMessageBox>
#include <algorithm>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_ui(std::make_unique<Ui::MainWindow>()),
      m_board(nullptr), m_timer(nullptr), m_timeLeft(0)
{
    m_ui->setupUi(this);

    this->setWindowTitle("Brainstorm Supervisor [ADMIN PANEL]");

    this->setStyleSheet(R"(
        QMainWindow {
            background-color: #0d1117;
        }
        QLabel {
            color: #58a6ff;
            font-family: 'Segoe UI', sans-serif;
            font-size: 14px;
            font-weight: bold;
        }
        QListWidget {
            background-color: #161b22;
            border: 2px solid #30363d;
            border-radius: 6px;
            color: #c9d1d9;
            padding: 5px;
            font-size: 13px;
        }
        QListWidget::item {
            padding: 5px;
            border-bottom: 1px solid #21262d;
        }
        QListWidget::item:selected {
            background-color: #1f6feb;
            color: white;
        }
    )");
}

MainWindow::~MainWindow() = default;

Ui::MainWindow *MainWindow::getWindow()
{
    return m_ui.get();
}

void MainWindow::SetBoard(VirtualBoard *board)
{
    m_board = board;
    m_timeLeft = 180;

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::UpdateUI);
    m_timer->start(1000);

    UpdateUI();
}

void MainWindow::UpdateUI()
{
    if (!m_board)
        return;

    m_timeLeft--;
    int minutes = m_timeLeft / 60;
    int seconds = m_timeLeft % 60;

    QString timeStr = QString("STATUS: Generating Ideas... (%1:%2 remaining)")
                          .arg(minutes, 2, 10, QChar('0'))
                          .arg(seconds, 2, 10, QChar('0'));

    m_ui->lblStatus->setText(timeStr);

    if (m_timeLeft < 10) {
        m_ui->lblStatus->setStyleSheet("color: #ff7b72; font-weight: bold; font-size: 16px;");
    } else {
        m_ui->lblStatus->setStyleSheet("color: #58a6ff; font-weight: bold; font-size: 14px;");
    }

    m_ui->listIdeas->clear();
    auto ideas = m_board->FetchAllIdeas();

    for (size_t i = 0; i < ideas.size(); ++i)
    {
        const auto &idea = ideas[i];
        QString itemText = QString("#%1: %2 [Worker %3 | Votes: %4]")
                               .arg(i + 1)
                               .arg(QString::fromStdString(idea.text))
                               .arg(idea.worker_id)
                               .arg(idea.votes);
        m_ui->listIdeas->addItem(itemText);
    }

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
        delete m_timer;
        m_timer = nullptr;
    }

    if (m_board)
    {
        m_board->StopSession();

        m_ui->lblStatus->setText("STATUS: Voting Phase (Wait 30s)...");
        m_ui->lblStatus->setStyleSheet("color: #2ea043; font-weight: bold; font-size: 16px;");

        QTimer::singleShot(30000, this, &MainWindow::FinishVoting);
    }
}

void MainWindow::FinishVoting()
{
    if (!m_board)
        return;

    auto ideas = m_board->FetchAllIdeas();
    int totalVotes = 0;
    for (const auto& idea : ideas) totalVotes += idea.votes;

    if (totalVotes == 0 && !ideas.empty()) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, ideas.size() - 1);

        for(int i=0; i<10; ++i) {
            int idx = dist(gen);
            m_board->VoteForIdea(ideas[idx].uuid);
        }
        ideas = m_board->FetchAllIdeas();
    }
    m_ui->lblStatus->setText("SESSION COMPLETE. Results saved.");

    m_ui->listIdeas->clear();

    std::sort(ideas.begin(), ideas.end(), [](const Idea &a, const Idea &b) {
        return a.votes > b.votes;
    });

    QFile file("lab9_report.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QTextStream out(&file);
        out << "Date: " << QDateTime::currentDateTime().toString() << "\n";

        out << "WINNING IDEAS:\n";
        for (int i = 0; i < std::min(3, (int)ideas.size()); ++i) {
            out << "#" << (i+1) << ": " << QString::fromStdString(ideas[i].text)
            << " (" << ideas[i].votes << " votes)\n";
        }

        out << "\n[System]: Total ideas generated: " << ideas.size() << "\n";
        file.close();
    }

    m_ui->listIdeas->addItem("TOP 3 IDEAS");
    for (int i = 0; i < std::min(3, (int)ideas.size()); ++i)
    {
        QString itemText = QString("#%1: %2 [%3 votes]")
        .arg(i + 1)
            .arg(QString::fromStdString(ideas[i].text))
            .arg(ideas[i].votes);
        m_ui->listIdeas->addItem(itemText);
    }

    m_board->SaveReport("core_backup_report.txt");

    QMessageBox::information(this, "Session Complete",
                             "Session finished!\nResults saved to 'report.txt'.");
}
