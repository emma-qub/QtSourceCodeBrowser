#include "MainWindow.hxx"

#include "BrowseSourceWidget.hxx"

#include <QAction>
#include <QMenuBar>
#include <QMessageBox>
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

void MainWindow::closeEvent(QCloseEvent* p_event)
{
  if (m_centralWidget->hasModificationsNotSaved()) {
    QMessageBox msgBox;
    msgBox.setText("The document has been modified.");
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Save: {
      m_centralWidget->saveNotesFromSource();
      break;
    }
    case QMessageBox::Cancel: {
      p_event->ignore();
      return;
    }
    case QMessageBox::Discard:
    default: {
      break;
    }
    }
  }

  p_event->accept();
}

