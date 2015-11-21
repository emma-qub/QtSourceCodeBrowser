#include "MainWindow.hxx"

#include "BrowseSourceWidget.hxx"

#include <QAction>
#include <QMenuBar>
#include <QDebug>

MainWindow::MainWindow(QWidget* p_parent):
  QMainWindow(p_parent) {

  m_centralWidget = new BrowseSourceWidget;
  setCentralWidget(m_centralWidget);

  setWindowState(Qt::WindowMaximized);

  setWindowIcon(QIcon("../QtSourceCodeBrowser/icons/appIcon.png"));

  QAction* explandAll = menuBar()->addAction("Exapnd All");

  connect(explandAll, SIGNAL(triggered()), m_centralWidget, SLOT(expandAll()));
}
