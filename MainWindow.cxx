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

  // Close current source
  QAction* closeCurrentSource = new QAction("Close", this);
  closeCurrentSource->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_W));
  connect(closeCurrentSource, SIGNAL(triggered()), m_centralWidget, SLOT(closeNotesAndSource()));
  fileMenu->addAction(closeCurrentSource);

  // Close current source
  QAction* closeAllSource = new QAction("Close all", this);
  closeAllSource->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_W));
  connect(closeAllSource, SIGNAL(triggered()), m_centralWidget, SLOT(closeAllNotesAndSource()));
  fileMenu->addAction(closeAllSource);

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

  // Show maximized
  setWindowState(Qt::WindowMaximized);

  // Set app icon
  setWindowIcon(QIcon("../QtSourceCodeBrowser/icons/appIcon.png"));

  QFile file("../QtSourceCodeBrowser/css/style.css");
  file.open(QFile::ReadOnly);
  QString styleSheet = QLatin1String(file.readAll());
  setStyleSheet(styleSheet);
}

void MainWindow::closeEvent(QCloseEvent* p_event)
{
  QList<QPair<QString, QString>> notesListToSave = m_centralWidget->getNotSavedNotes();
  if (notesListToSave.size() > 0) {
    QStringList notesListAbsoluteFilePath;
    QStringList fileNamesList;
    for (auto currentNotesAndFileNames: notesListToSave) {
      notesListAbsoluteFilePath << currentNotesAndFileNames.first;
      fileNamesList << currentNotesAndFileNames.second;
    }

    QMessageBox msgBox;

    QString text;
    if (fileNamesList.size() == 1) {
      text = "The document "+fileNamesList.first()+" has been modified.";
      msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
      msgBox.setDefaultButton(QMessageBox::Save);
    } else {
      text += "These documents have been modified:\n";
      for (auto currentFileName: fileNamesList) {
        text += currentFileName+"\n";
      }
      msgBox.setStandardButtons(QMessageBox::SaveAll | QMessageBox::Discard | QMessageBox::Cancel);
      msgBox.setDefaultButton(QMessageBox::SaveAll);
    }

    msgBox.setText(text);
    msgBox.setInformativeText("Do you want to save your changes?");

    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Save:
    case QMessageBox::SaveAll: {
      m_centralWidget->saveNotesFromSource(notesListAbsoluteFilePath);
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
