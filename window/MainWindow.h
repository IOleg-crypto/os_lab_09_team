#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <memory>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
public:
  Ui::MainWindow* getWindow();

private:
  // Use unique_ptr instead raw pointer
  std::unique_ptr<Ui::MainWindow> m_ui;
};
#endif // MAINWINDOW_H
