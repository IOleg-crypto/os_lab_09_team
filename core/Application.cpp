#include "Application.h"

Application::Application(int &argc, char **argv)
    : m_app(std::make_unique<QApplication>(argc, argv)),
      m_window(std::make_unique<MainWindow>()) {}

Application::~Application() = default;

int Application::Run() {
  m_window->show();
  return m_app->exec();
}
