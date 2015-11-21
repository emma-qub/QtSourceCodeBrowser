#include "MainWindow.hxx"

#include "BrowseSourceWidget.hxx"

#include <QAction>
#include <QMenuBar>
#include <QDebug>

MainWindow::MainWindow(QWidget* p_parent):
  QMainWindow(p_parent) {

  // Set central widget
  m_centralWidget = new BrowseSourceWidget;
  setCentralWidget(m_centralWidget);

  // Show maximized
  setWindowState(Qt::WindowMaximized);

  // Set app icon
  setWindowIcon(QIcon("../QtSourceCodeBrowser/icons/appIcon.png"));
}
