#ifndef APPLICATION_H
#define APPLICATION_H

#include "window/MainWindow.h"

#include <QApplication>
#include <memory>

class Application {
private:
  std::unique_ptr<QApplication> m_app;
  std::unique_ptr<MainWindow> m_window;
public:
  Application(int &argc, char **argv);
  ~Application();
public:
  // Init and run application
  int Run();
};

#endif // APPLICATION_H
