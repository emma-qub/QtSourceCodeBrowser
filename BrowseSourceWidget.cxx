#include "BrowseSourceWidget.hxx"

#include <QSettings>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTextStream>
#include <QMessageBox>
#include <QInputDialog>
#include <QAction>
#include <QMenu>
#include <QDebug>

#include "CodeEditor.hxx"

BrowseSourceWidget::BrowseSourceWidget(QWidget* p_parent):
  QWidget(p_parent),
  m_notesExist(false),
  m_notesFileInfo(),
  m_notesFilePath(),
  m_currentOpenDocumentIndex() {

  // Sources and open files
  m_sourcesAndOpenFiles = new SourcesAndOpenFiles(this);

  // Source editor
  m_sourcesEditor = new SourceCodeEditor(this);

  // Note Text Edit
  m_notesTextEdit = new NoteRichTextEdit(this);
  connect(m_notesTextEdit, SIGNAL(openSourceRequested(QString)), m_sourcesAndOpenFiles, SLOT(openSourceCodeFromFileName(QString)));
  connect(m_notesTextEdit, SIGNAL(saveNotesRequested()), this, SLOT(saveNotesFromSource()));
  connect(m_notesTextEdit, SIGNAL(modificationsNotSaved(bool)), m_sourcesAndOpenFiles, SLOT(addOrRemoveStarToOpenDocument(bool)));
  connect(m_notesTextEdit, SIGNAL(notesEditOff(bool)), m_sourcesAndOpenFiles, SLOT(addOrRemoveStarToOpenDocument(bool)));

  // Edit Notes ON horizontal
  QAction* editNotesOnHorizontalAction = new QAction(this);
  editNotesOnHorizontalAction->setShortcut(QKeySequence("CTRL+E,3"));
  addAction(editNotesOnHorizontalAction);
  connect(editNotesOnHorizontalAction, SIGNAL(triggered()), this, SLOT(showHorizontal()));

  // Edit Notes ON vertical
  QAction* editNotesOnVerticalAction = new QAction(this);
  editNotesOnVerticalAction->setShortcut(QKeySequence("CTRL+E,2"));
  addAction(editNotesOnVerticalAction);
  connect(editNotesOnVerticalAction, SIGNAL(triggered()), this, SLOT(showVertical()));

  // Edit Notes OFF
  QAction* editNotesOffAction = new QAction(this);
  editNotesOffAction->setShortcut(QKeySequence("CTRL+E,1"));
  addAction(editNotesOffAction);
  connect(editNotesOffAction, SIGNAL(triggered()), m_notesTextEdit, SLOT(hide()));

  // Sources and Notes Splitter
  m_sourcesNotesSplitter = new QSplitter;
  m_sourcesNotesSplitter->addWidget(m_sourcesEditor);
  m_sourcesNotesSplitter->addWidget(m_notesTextEdit);
  m_sourcesNotesSplitter->setStretchFactor(0, 1);
  m_sourcesNotesSplitter->setStretchFactor(1, 2);
  m_notesTextEdit->hide();

  connect(m_sourcesAndOpenFiles, SIGNAL(openSourceCodeRequested(QModelIndex)), this, SLOT(openSourceCode(QModelIndex)));
  connect(m_sourcesAndOpenFiles, SIGNAL(openSourceCodeFromOpenDocumentsRequested(QModelIndex)), this, SLOT(openSourceCodeFromOpenDocuments(QModelIndex)));

  // Main part
  QSplitter* hsplitter = new QSplitter;
  hsplitter->addWidget(m_sourcesAndOpenFiles);
  hsplitter->addWidget(m_sourcesNotesSplitter);
  hsplitter->setStretchFactor(0, 0);
  hsplitter->setStretchFactor(1, 1);

  // Main layout
  QHBoxLayout* mainLayout = new QHBoxLayout;
  mainLayout->addWidget(hsplitter);

  setLayout(mainLayout);

  setContextMenuPolicy(Qt::CustomContextMenu);

  layout()->setContentsMargins(0, 0, 0, 0);
}

void BrowseSourceWidget::keyReleaseEvent(QKeyEvent* p_event) {
  QString currentSourceOpen = m_sourcesAndOpenFiles->getCurrentOpenDocumentAbsolutePath();
  if (currentSourceOpen.isEmpty())
    return;

  switch (p_event->key()) {
    case Qt::Key_F4: {
      QString currentExtension = currentSourceOpen.split('.').last();
      QString newExtension = (currentExtension == "h") ? "cpp" : "h";
      QStringList filePathSplit = currentSourceOpen.split(QDir::separator());
      QString fileName = filePathSplit.last();
      QStringList fileNameSplit = fileName.split(".");
      fileNameSplit.replace(fileNameSplit.size()-1, newExtension);
      if (fileNameSplit.at(fileNameSplit.size()-2).endsWith("_p"))
        fileNameSplit.replace(fileNameSplit.size()-2, fileNameSplit.at(fileNameSplit.size()-2).mid(0, fileNameSplit.at(fileNameSplit.size()-2).size()-2));
      fileName = fileNameSplit.join(".");
      filePathSplit.replace(filePathSplit.size()-1, fileName);
      currentSourceOpen = filePathSplit.join(QDir::separator());
      openDocumentInEditor(fileName, currentSourceOpen);
      break;
    } case Qt::Key_F5: {
      QStringList fileAndExtension = currentSourceOpen.split('.');
      if (fileAndExtension.last() == "cpp")
      {
        fileAndExtension.replace(fileAndExtension.size()-1, "h");
        currentSourceOpen = fileAndExtension.join(".");
      }
      QStringList filePathSplit = currentSourceOpen.split(QDir::separator());
      QString fileName = filePathSplit.last();
      QStringList fileNameSplit = fileName.split(".");
      if (fileNameSplit.at(fileNameSplit.size()-2).endsWith("_p"))
        fileNameSplit.replace(fileNameSplit.size()-2, fileNameSplit.at(fileNameSplit.size()-2).mid(0, fileNameSplit.at(fileNameSplit.size()-2).size()-2));
      else
        fileNameSplit.replace(fileNameSplit.size()-2, fileNameSplit.at(fileNameSplit.size()-2)+"_p");
      fileName = fileNameSplit.join(".");
      filePathSplit.replace(filePathSplit.size()-1, fileName);
      currentSourceOpen = filePathSplit.join(QDir::separator());
      openDocumentInEditor(fileName, currentSourceOpen);
      break;
    }
  }
}

void BrowseSourceWidget::openSourceCode(QModelIndex const& p_index) {
  QString fileName(p_index.data().toString());
  if (fileName.endsWith(".cpp") || fileName.endsWith(".h")) {
    QString absoluteFilePath(fileName);
    QModelIndex currentIndex = p_index;
    QModelIndex parentIndex;
    while ((parentIndex = currentIndex.parent()) != QModelIndex()) {
      QString parentDirectoryName = parentIndex.data().toString();
      if (parentDirectoryName == QString(QDir::separator())) {
        absoluteFilePath = QDir::separator() + absoluteFilePath;
        break;
      }

      absoluteFilePath = parentDirectoryName + QDir::separator() + absoluteFilePath;
      currentIndex = parentIndex;
    }

    openDocumentInEditor(fileName, absoluteFilePath);
  }
}

void BrowseSourceWidget::showHorizontal() {
  m_sourcesNotesSplitter->setOrientation(Qt::Horizontal);
  m_notesTextEdit->show();
}

void BrowseSourceWidget::showVertical() {
  m_sourcesNotesSplitter->setOrientation(Qt::Vertical);
  m_notesTextEdit->show();
}

void BrowseSourceWidget::openSourceCodeFromOpenDocuments(QModelIndex const& p_index) {
  QString fileName = p_index.data(Qt::DisplayRole).toString();
  QString absoluteFilePath = p_index.data(Qt::ToolTipRole).toString();
  openDocumentInEditor(fileName, absoluteFilePath);
}

void BrowseSourceWidget::openDocumentInEditor(QString const& p_fileName, QString const& p_absoluteFilePath) {
  if (m_notesTextEdit->hasModificationsNotSaved()) {
    QMessageBox msgBox;
    msgBox.setText("The document has been modified.");
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Save: {
      saveNotesFromSource();
      break;
    }
    case QMessageBox::Cancel: {
      m_sourcesAndOpenFiles->setCurrentIndex(m_currentOpenDocumentIndex);
      return;
    }
    case QMessageBox::Discard:
    default: {
      break;
    }
    }
  }

  m_notesTextEdit->turnEditOff();

  QFile sourceFile(p_absoluteFilePath);
  if (!sourceFile.exists()) {
    qDebug() << "404 Not Found" << "The file\n"+p_absoluteFilePath+"\ndoes not exist on this computer.";
    return;
  }

  CodeEditor::FileType fileType;
  if (p_fileName.endsWith("_p.h")) {
    fileType = CodeEditor::ePrivateH;
  } else if (p_fileName.endsWith(".h")) {
    fileType = CodeEditor::eH;
  } else if (p_fileName.endsWith(".cpp")) {
    fileType = CodeEditor::eCpp;
  } else {
    fileType = CodeEditor::eOtherFile;
  }

  m_sourcesAndOpenFiles->insertDocument(p_fileName, p_absoluteFilePath);
  m_sourcesEditor->openSourceCode(getSourceContent(p_absoluteFilePath), fileType);
  QModelIndex openIndex = m_sourcesAndOpenFiles->getCurrentOpenDocumentIndex(p_fileName, p_absoluteFilePath);
  m_currentOpenDocumentIndex = openIndex;
  m_sourcesAndOpenFiles->setCurrentIndex(openIndex);

  openNotesFromSource(p_fileName);
}

QString BrowseSourceWidget::getSourceContent(QString const& p_absoluteFilePath) {
  QFile sourceFile(p_absoluteFilePath);
  if (!sourceFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(this, "Opening issue", sourceFile.errorString());
  }

  QTextStream in(&sourceFile);

  QString content;
  while(!in.atEnd()) {
    content.append(in.readLine());
    content.append("\n");
  }

  sourceFile.close();

  return content;
}

void BrowseSourceWidget::saveNotesFromSource()
{
  if (m_notesFilePath.isEmpty()) {
    return;
  }

  QFile noteFile(m_notesFilePath);
  if (!noteFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::warning(this, "Writting issue", noteFile.errorString());
  }

  QTextStream in(&noteFile);

  in << m_notesTextEdit->toHtml();

  noteFile.close();

  m_sourcesAndOpenFiles->addOrRemoveStarToOpenDocument(false);
}

void BrowseSourceWidget::getNotesFileInfo(QString const& p_fileName) {
  QString notesFileName = p_fileName;
  notesFileName.replace(QRegExp("(\\.cpp$|\\.h$|_p\\.h$)"), ".txt");

  m_notesFileInfo = QFileInfo("../QtSourceCodeBrowser/notes/"+notesFileName);
  m_notesFilePath = m_notesFileInfo.absoluteFilePath();
}

void BrowseSourceWidget::openNotesFromSource(QString const& p_fileName) {
  QFileInfo previousFileInfo = m_notesFileInfo;
  getNotesFileInfo(p_fileName);

  if (previousFileInfo == m_notesFileInfo) {
    return;
  }

  m_notesTextEdit->setText("");

  m_notesExist = m_notesFileInfo.exists();
  if (m_notesExist) {
    m_notesTextEdit->setText(getSourceContent(m_notesFilePath));
  }
}
