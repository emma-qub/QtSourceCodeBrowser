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
  QWidget(p_parent) {

  // Sources and open files
  m_sourcesAndOpenFilesWidget = new SourcesAndOpenFiles(this);

  // Source editor
  m_sourceCodeEditorWidget = new SourceCodeEditor(this);

  // Note Text Edit
  m_noteRichTextEditStackWidget = new QStackedWidget(this);

  // Sources and Notes Splitter
  m_sourcesNotesSplitter = new QSplitter;
  m_sourcesNotesSplitter->addWidget(m_sourceCodeEditorWidget);
  m_sourcesNotesSplitter->addWidget(m_noteRichTextEditStackWidget);
  m_sourcesNotesSplitter->setStretchFactor(0, 1);
  m_sourcesNotesSplitter->setStretchFactor(1, 2);
  m_noteRichTextEditStackWidget->hide();

  connect(m_sourcesAndOpenFilesWidget, SIGNAL(openSourceCodeFromTreeViewRequested(QModelIndex)), this, SLOT(openSourceCodeFromTreeView(QModelIndex)));
  connect(m_sourcesAndOpenFilesWidget, SIGNAL(openSourceCodeFromSearchRequested(QModelIndex)), this, SLOT(openSourceCodeFromSearch(QModelIndex)));
  connect(m_sourcesAndOpenFilesWidget, SIGNAL(openSourceCodeFromOpenDocumentsRequested(QModelIndex)), this, SLOT(openSourceCodeFromOpenDocuments(QModelIndex)));
  connect(m_sourcesAndOpenFilesWidget, SIGNAL(openSourceCodeFromContextualMenuRequested(QModelIndex)), this, SLOT(openSourceCodeFromContextualMenu(QModelIndex)));

  // Main part
  QSplitter* hsplitter = new QSplitter;
  hsplitter->addWidget(m_sourcesAndOpenFilesWidget);
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

QList<QPair<QString, QString>> BrowseSourceWidget::getNotSavedNotes() const {
  return m_browseFileInfo.getNotSavedNotesList();
}

int BrowseSourceWidget::askToSave(QStringList const& fileNamesList) const {
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

  return msgBox.exec();
}

void BrowseSourceWidget::getNotesListToSaveAndFileNamesList(QStringList& p_absoluteFilePathList, QStringList& p_fileNamesList) const {
  QList<QPair<QString, QString>> notesListToSave = m_browseFileInfo.getNotSavedNotesList();
  for (auto currentNotesAndFileNames: notesListToSave) {
    p_absoluteFilePathList << currentNotesAndFileNames.first;
    p_fileNamesList << currentNotesAndFileNames.second;
  }
}


/// PROTECTED

void BrowseSourceWidget::keyReleaseEvent(QKeyEvent* p_event) {
  QString currentSourceOpen = m_sourcesAndOpenFilesWidget->getCurrentOpenDocumentAbsolutePath();
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

void BrowseSourceWidget::addOrRemoveStarToOpenDocument(bool p_value, QString const& p_absoluteFilePath) {
  QString absoluteFilePath = p_absoluteFilePath;
  QString fileName;

  if (p_absoluteFilePath.isEmpty()) {
    absoluteFilePath = m_browseFileInfo.getCurrentOpenDocumentAbsoluteFilePath();
    fileName = m_browseFileInfo.getCurrentOpenDocumentFileName(absoluteFilePath);
  } else {
    fileName = m_browseFileInfo.getFileNameFromOpenDocumentAbsolutePath(p_absoluteFilePath);
  }

  m_sourcesAndOpenFilesWidget->addOrRemoveStarToOpenDocument(fileName, absoluteFilePath, p_value);

  bool allSave = m_browseFileInfo.hasNoteNotSaved();

  emit enableSaveActionRequested(p_value, allSave);
}

NoteRichTextEdit* BrowseSourceWidget::getNotesTextEditFromPosition(int p_position) const {
  int position = p_position;
  if (position == -1) {
    position = m_noteRichTextEditStackWidget->currentIndex();
  }

  NoteRichTextEdit* currentNotesTextEdit = dynamic_cast<NoteRichTextEdit*>(m_noteRichTextEditStackWidget->widget(position));
  Q_ASSERT(currentNotesTextEdit != nullptr);
  return currentNotesTextEdit;
}


/// PUBLIC SLOTS

void BrowseSourceWidget::saveNotesFromSourceAndCloseEditor() {
  saveNotesFromSource();
  NoteRichTextEdit* currentNoteRichTextEdit = getNotesTextEditFromPosition();
  currentNoteRichTextEdit->editOff();
}

void BrowseSourceWidget::saveAllNotes() {
  QStringList absoluteFilePathList;
  QStringList fileNamesList;
  getNotesListToSaveAndFileNamesList(absoluteFilePathList, fileNamesList);
  saveNotesFromSource(absoluteFilePathList);
}

void BrowseSourceWidget::saveNotesFromSource(QStringList const& p_absoluteFilePathListToSave) {
  if (p_absoluteFilePathListToSave.isEmpty()) {
    saveNotesFromSource(m_browseFileInfo.getCurrentOpenDocumentAbsoluteFilePath());
  } else {
    for (auto absoluteFilePath: p_absoluteFilePathListToSave) {
      saveNotesFromSource(absoluteFilePath);
    }
  }
}

void BrowseSourceWidget::saveNotesFromSource(QString const& p_absoluteFilePath) {
  QString notesAbsoluteFilePath = m_browseFileInfo.getNotesAbsolutePathFromOpenDocumentAbsolutePath(p_absoluteFilePath);
  QFile noteFile(notesAbsoluteFilePath);
  if (!noteFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::warning(this, "Writting issue", noteFile.errorString());
  }

  QTextStream in(&noteFile);

  int position = m_browseFileInfo.getNotesPositionInStack(notesAbsoluteFilePath);
  in << getNotesTextEditFromPosition(position)->toHtml();

  noteFile.close();

  updateSaveStateToNotes(false, p_absoluteFilePath);
}

void BrowseSourceWidget::showHorizontal() {
  m_sourcesNotesSplitter->setOrientation(Qt::Horizontal);
  m_noteRichTextEditStackWidget->show();
}

void BrowseSourceWidget::showVertical() {
  m_sourcesNotesSplitter->setOrientation(Qt::Vertical);
  m_noteRichTextEditStackWidget->show();
}

void BrowseSourceWidget::hideNotesTextEdit() {
  m_noteRichTextEditStackWidget->hide();
}

void BrowseSourceWidget::closeNotesAndSource(QString const& p_absoluteFilePath) {
  QString absoluteFilePath = p_absoluteFilePath;
  if (absoluteFilePath.isEmpty()) {
    absoluteFilePath = m_browseFileInfo.getCurrentOpenDocumentAbsoluteFilePath();
  }
  QString notesAbsoluteFilePath = m_browseFileInfo.getNotesAbsolutePathFromOpenDocumentAbsolutePath(absoluteFilePath);

  if (m_browseFileInfo.isNotesSaved(absoluteFilePath) == false) {
    int confirm = askToSave(QStringList() << m_browseFileInfo.getCurrentOpenDocumentFileName(absoluteFilePath));

    switch (confirm) {
    case QMessageBox::Save: {
      saveNotesFromSource(absoluteFilePath);
      break;
    }
    case QMessageBox::Cancel: {
      return;
    }
    case QMessageBox::Discard:
    default: {
      break;
    }
    }
  }

  // Remove Notes
  int currentNotesPosition = m_browseFileInfo.getNotesPositionInStack(notesAbsoluteFilePath);
  m_noteRichTextEditStackWidget->removeWidget(m_noteRichTextEditStackWidget->widget(currentNotesPosition));
  m_browseFileInfo.removeNotesAt(currentNotesPosition);

  // Remove Source
  m_sourceCodeEditorWidget->clear();
  m_browseFileInfo.removeSourceFromAbsoluteFilePath(absoluteFilePath);

  // Remove Open Document
  m_sourcesAndOpenFilesWidget->removeOpenDocument(absoluteFilePath);

  // Open current Document
  QModelIndex currentIndex = m_sourcesAndOpenFilesWidget->getCurrentIndex();
  if (currentIndex.isValid()) {
    openSourceCodeFromOpenDocuments(currentIndex);
  } else {
    // Disable split
    emit disableSplitRequested();
    // Disable close and save actions
    emit enableCloseActionRequested(false);
  }
}

void BrowseSourceWidget::closeAllNotesAndSource() {
  QStringList absoluteFilePathList;
  QStringList fileNamesList;
  getNotesListToSaveAndFileNamesList(absoluteFilePathList, fileNamesList);
  if (fileNamesList.size() > 0) {
    int confirm = askToSave(fileNamesList);

    switch (confirm) {
    case QMessageBox::Save:
    case QMessageBox::SaveAll: {
      saveNotesFromSource(absoluteFilePathList);
      break;
    }
    case QMessageBox::Cancel: {
      return;
    }
    case QMessageBox::Discard:
    default: {
      break;
    }
    }
  }

  // Remove Notes
  QWidget* currentWidget = nullptr;
  while ((currentWidget = m_noteRichTextEditStackWidget->widget(0)) != nullptr) {
    m_noteRichTextEditStackWidget->removeWidget(currentWidget);
    m_browseFileInfo.clearNotes();
  }

  // Remove Source
  m_sourceCodeEditorWidget->clear();
  m_browseFileInfo.clearSourceFromAbsoluteFilePath();

  // Remove Open Document
  m_sourcesAndOpenFilesWidget->clearOpenDocument();

  // Disable Split
  emit disableSplitRequested();

  // Disable close and save actions
  emit enableCloseActionRequested(false);
}


/// PROTECTED SLOTS

void BrowseSourceWidget::openSourceCodeFromTreeView(QModelIndex const& p_index) {
  QFileSystemModel const* model = dynamic_cast<QFileSystemModel const*>(p_index.model());
  Q_ASSERT(model != nullptr);

  QString fileName = model->fileName(p_index);
  QString absoluteFilePath = model->filePath(p_index);

  QFileInfo fileInfo(absoluteFilePath);
  if (fileInfo.isDir()) {
    return;
  }

  m_browseFileInfo.appendNotesAndOpenDocument(absoluteFilePath, fileName);

  openDocumentInEditor(fileName, absoluteFilePath);

  m_sourcesAndOpenFilesWidget->setCurrentIndex(fileName, absoluteFilePath);

  requestUpdateFileAction();
}

void BrowseSourceWidget::openSourceCodeFromSearch(QModelIndex const& p_index) {
  QString fileName = p_index.data(Qt::DisplayRole).toString();
  QString absoluteFilePath = p_index.data(Qt::ToolTipRole).toString();

  m_browseFileInfo.appendNotesAndOpenDocument(absoluteFilePath, fileName);

  openDocumentInEditor(fileName, absoluteFilePath);

  m_sourcesAndOpenFilesWidget->setCurrentIndex(fileName, absoluteFilePath);

  requestUpdateFileAction();
}

void BrowseSourceWidget::openSourceCodeFromOpenDocuments(QModelIndex const& p_index) {
  QString fileName = p_index.data(Qt::DisplayRole).toString();
  QString absoluteFilePath = p_index.data(Qt::ToolTipRole).toString();

  m_browseFileInfo.setCurrentNotesAndSourceAbsoluteFilePath(absoluteFilePath);

  openDocumentInEditor(fileName, absoluteFilePath);

  requestUpdateFileAction();
}

void BrowseSourceWidget::openSourceCodeFromContextualMenu(const QModelIndex& p_index) {
  QString fileName = p_index.data(Qt::DisplayRole).toString();
  QString absoluteFilePath = p_index.data(Qt::ToolTipRole).toString();

  m_browseFileInfo.appendNotesAndOpenDocument(absoluteFilePath, fileName);

  openDocumentInEditor(fileName, absoluteFilePath);

  m_sourcesAndOpenFilesWidget->setCurrentIndex(fileName, absoluteFilePath);

  requestUpdateFileAction();
}

void BrowseSourceWidget::updateSaveStateToNotes(bool p_value, QString const& p_absoluteFilePath) {
  QString absoluteFilePath = p_absoluteFilePath;
  QString notesAbsoluteFilePath;

  if (absoluteFilePath.isEmpty() == false) {
    notesAbsoluteFilePath = m_browseFileInfo.getNotesAbsolutePathFromOpenDocumentAbsolutePath(absoluteFilePath);
  }

  m_browseFileInfo.setNotesSaveState(!p_value, absoluteFilePath);
  addOrRemoveStarToOpenDocument(p_value, absoluteFilePath);
}

void BrowseSourceWidget::requestUpdateFileAction() {
  bool saved = !m_browseFileInfo.isCurrentNotesSaved();
  bool allSaved = m_browseFileInfo.hasNoteNotSaved();

  emit enableSaveActionRequested(saved, allSaved);

  QString currentFileName = m_browseFileInfo.getCurrentOpenDocumentFileName(m_browseFileInfo.getCurrentOpenDocumentAbsoluteFilePath());
  if (currentFileName.endsWith("*")) {
    currentFileName.remove(currentFileName.size()-1, 1);
  }
  emit updateFileMenuRequested(currentFileName);

  emit enableCloseActionRequested(true);
}


/// PRIVATE

QString BrowseSourceWidget::getFileContent(QString const& p_absoluteFilePath) {
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

void BrowseSourceWidget::openDocumentInEditor(QString const& p_fileName, QString const& p_absoluteFilePath) {
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

  QString notesAbsoluteFilePath = m_browseFileInfo.getCurrentNotesAbsoluteFilePath();

  m_sourcesAndOpenFilesWidget->insertDocument(p_fileName, p_absoluteFilePath);
  m_sourceCodeEditorWidget->openSourceCode(getFileContent(p_absoluteFilePath), fileType);

  int position = m_browseFileInfo.getNotesPositionInStack(notesAbsoluteFilePath);
  if (position > -1) {
    m_noteRichTextEditStackWidget->setCurrentIndex(position);
  } else {
    openNotes(notesAbsoluteFilePath);
  }

  m_sourceCodeEditorWidget->setFocusToSourceEditor();

  emit enableSplitRequested();
}

void BrowseSourceWidget::openNotes(QString const& p_notesAbsoluteFilePath) {
  NoteRichTextEdit* notesTextEdit = new NoteRichTextEdit;
  notesTextEdit->openNotes(getFileContent(p_notesAbsoluteFilePath));

  connect(notesTextEdit, SIGNAL(contextMenuRequested(QString)), m_sourcesAndOpenFilesWidget, SLOT(openSourceCodeFromFileName(QString)));
  connect(notesTextEdit, SIGNAL(saveNotesRequested()), this, SLOT(saveNotesFromSource()));
  connect(notesTextEdit, SIGNAL(modificationsNotSaved(bool)), this, SLOT(updateSaveStateToNotes(bool)));

  int position = m_noteRichTextEditStackWidget->addWidget(notesTextEdit);
  m_noteRichTextEditStackWidget->setCurrentIndex(position);
  m_browseFileInfo.insertNotesAbsoluteFilePath(position, p_notesAbsoluteFilePath);
}
