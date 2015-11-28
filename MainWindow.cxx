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

  // File QMenu
  QMenu* fileMenu = menuBar()->addMenu("File");

  // Save current notes
  m_saveAction = new QAction("Save", this);
  m_saveAction->setShortcut(QKeySequence::Save);
  m_saveAction->setEnabled(false);
  connect(m_saveAction, SIGNAL(triggered()), m_centralWidget, SLOT(saveNotesFromSource()));
  fileMenu->addAction(m_saveAction);

  // Save current notes and close editor
  m_saveAndCloseEditAction = new QAction("Save and close editor", this);
  m_saveAndCloseEditAction->setShortcut(QKeySequence(Qt::CTRL+Qt::ALT+Qt::Key_S));
  m_saveAndCloseEditAction->setEnabled(false);
  connect(m_saveAndCloseEditAction, SIGNAL(triggered()), m_centralWidget, SLOT(saveNotesFromSourceAndCloseEditor()));
  fileMenu->addAction(m_saveAndCloseEditAction);

  // Save all notes
  m_saveAllAction = new QAction("Save all", this);
  m_saveAllAction->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_S));
  m_saveAllAction->setEnabled(false);
  connect(m_saveAllAction, SIGNAL(triggered()), m_centralWidget, SLOT(saveAllNotes()));
  fileMenu->addAction(m_saveAllAction);
  fileMenu->addSeparator();

  // Close current source
  m_closeAction = new QAction("Close", this);
  m_closeAction->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_W));
  m_closeAction->setEnabled(false);
  connect(m_closeAction, SIGNAL(triggered()), m_centralWidget, SLOT(closeNotesAndSource()));
  fileMenu->addAction(m_closeAction);

  // Close all sources
  m_closeAllAction = new QAction("Close all", this);
  m_closeAllAction->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_W));
  m_closeAllAction->setEnabled(false);
  connect(m_closeAllAction, SIGNAL(triggered()), m_centralWidget, SLOT(closeAllNotesAndSource()));
  fileMenu->addAction(m_closeAllAction);

  // Quit action
  QAction* quitAction = new QAction("Quit", this);
  quitAction->setShortcut(QKeySequence::Quit);
  connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
  fileMenu->addSeparator();
  fileMenu->addAction(quitAction);

  // Window QMenu
  QMenu* windowMenu = menuBar()->addMenu("Window");

  // Edit Notes ON vertical
  m_editNotesOnVerticalAction = new QAction("Split", this);
  m_editNotesOnVerticalAction->setShortcut(QKeySequence("CTRL+E,2"));
  windowMenu->addAction(m_editNotesOnVerticalAction);
  connect(m_editNotesOnVerticalAction, SIGNAL(triggered()), this, SLOT(showVertical()));
  m_editNotesOnVerticalAction->setEnabled(false);

  // Edit Notes ON horizontal
  m_editNotesOnHorizontalAction = new QAction("Split side by side", this);
  m_editNotesOnHorizontalAction->setShortcut(QKeySequence("CTRL+E,3"));
  windowMenu->addAction(m_editNotesOnHorizontalAction);
  connect(m_editNotesOnHorizontalAction, SIGNAL(triggered()), this, SLOT(showHorizontal()));
  m_editNotesOnHorizontalAction->setEnabled(false);

  // Edit Notes OFF
  m_editNotesOffAction = new QAction("Remove split", this);
  m_editNotesOffAction->setShortcut(QKeySequence("CTRL+E,1"));
  windowMenu->addAction(m_editNotesOffAction);
  connect(m_editNotesOffAction, SIGNAL(triggered()), this, SLOT(hideNotesTextEdit()));
  m_editNotesOffAction->setEnabled(false);

  connect(m_centralWidget, SIGNAL(enableSplitRequested()), this, SLOT(enableSplit()));
  connect(m_centralWidget, SIGNAL(disableSplitRequested()), this, SLOT(disableSplit()));
  connect(m_centralWidget, SIGNAL(updateFileMenuRequested(QString)), this, SLOT(updateFileMenu(QString)));
  connect(m_centralWidget, SIGNAL(enableSaveActionRequested(bool, bool)), this, SLOT(enableSaveAction(bool, bool)));
  connect(m_centralWidget, SIGNAL(enableCloseActionRequested(bool)), this, SLOT(enableCloseAction(bool)));

  // Show maximized
  setWindowState(Qt::WindowMaximized);

  // Set app icon
  setWindowIcon(QIcon("../QtSourceCodeBrowser/icons/appIcon.png"));

  QFile file("../QtSourceCodeBrowser/css/style.css");
  file.open(QFile::ReadOnly);
  QString styleSheet = QLatin1String(file.readAll());
  setStyleSheet(styleSheet);
}

void MainWindow::closeEvent(QCloseEvent* p_event) {
  QStringList absoluteFilePathList;
  QStringList fileNamesList;
  m_centralWidget->getNotesListToSaveAndFileNamesList(absoluteFilePathList, fileNamesList);
  if (fileNamesList.size() > 0) {
    int confirm = m_centralWidget->askToSave(fileNamesList);

    switch (confirm) {
    case QMessageBox::Save:
    case QMessageBox::SaveAll: {
      m_centralWidget->saveNotesFromSource(absoluteFilePathList);
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

void MainWindow::showHorizontal() {
  m_centralWidget->showHorizontal();
  m_editNotesOffAction->setEnabled(true);
  m_editNotesOnHorizontalAction->setEnabled(false);
  m_editNotesOnVerticalAction->setEnabled(true);
}

void MainWindow::showVertical() {
  m_centralWidget->showVertical();
  m_editNotesOffAction->setEnabled(true);
  m_editNotesOnHorizontalAction->setEnabled(true);
  m_editNotesOnVerticalAction->setEnabled(false);
}

void MainWindow::hideNotesTextEdit() {
  m_centralWidget->hideNotesTextEdit();
  m_editNotesOffAction->setEnabled(false);
  m_editNotesOnHorizontalAction->setEnabled(true);
  m_editNotesOnVerticalAction->setEnabled(true);
}

void MainWindow::enableSplit() {
  if (m_editNotesOnHorizontalAction->isEnabled() == false
   && m_editNotesOnVerticalAction->isEnabled() == false) {
    m_editNotesOnHorizontalAction->setEnabled(true);
    m_editNotesOnVerticalAction->setEnabled(true);
  }
}

void MainWindow::disableSplit() {
  m_centralWidget->hideNotesTextEdit();
  m_editNotesOffAction->setEnabled(false);
  m_editNotesOnHorizontalAction->setEnabled(false);
  m_editNotesOnVerticalAction->setEnabled(false);
}

void MainWindow::updateFileMenu(QString const& p_fileName) {
  m_saveAction->setText("Save "+p_fileName);
  m_closeAction->setText("Close "+p_fileName);
}

void MainWindow::enableSaveAction(bool p_value, bool p_allSave) {
  m_saveAction->setEnabled(p_value);
  m_saveAndCloseEditAction->setEnabled(p_value);

  m_saveAllAction->setEnabled(p_allSave);
}

void MainWindow::enableCloseAction(bool p_value) {
  m_closeAction->setEnabled(p_value);
  m_closeAllAction->setEnabled(p_value);

  if (p_value == false) {
    enableSaveAction(p_value, p_value);
    updateFileMenu("");
  }
}
