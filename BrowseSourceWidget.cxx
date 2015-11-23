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
  m_highlighters(),
  m_notesExist(false),
  m_notesFileInfo(),
  m_notesFilePath(),
  m_currentOpenDocumentIndex() {

  // Methods ComboBox
  m_methodsComboBox = new QComboBox;
  connect(m_methodsComboBox, SIGNAL(activated(int)), this, SLOT(goToLine(int)));

  // Source editor
  m_sourcesEditor = new CodeEditor(this);

  QFont font;
  font.setFamily("Mono");
  font.setFixedPitch(true);
  font.setPointSize(10);

  m_sourcesEditor->setFont(font);
  m_sourcesEditor->setReadOnly(true);
  m_sourcesEditor->setTextInteractionFlags(m_sourcesEditor->textInteractionFlags() | Qt::TextSelectableByKeyboard);
  m_highlighters.append(new Highlighter(m_sourcesEditor->document()));
  connect(m_sourcesEditor, SIGNAL(methodListReady(QMap<int,QString>)), this, SLOT(fillMethodsComboBox(QMap<int,QString>)));

  // Methods and Sources Splitter
  m_sourcesMethodsSplitter = new QSplitter(Qt::Vertical);
  m_sourcesMethodsSplitter->addWidget(m_methodsComboBox);
  m_sourcesMethodsSplitter->addWidget(m_sourcesEditor);
  m_sourcesMethodsSplitter->setStretchFactor(0, 0);
  m_sourcesMethodsSplitter->setStretchFactor(1, 1);

  // Note Text Edit
  m_notesTextEdit = new NoteRichTextEdit(this);
  connect(m_notesTextEdit, SIGNAL(openSourceRequested(QString)), this, SLOT(openSourceCodeFromFileName(QString)));
  connect(m_notesTextEdit, SIGNAL(saveNotesRequested()), this, SLOT(saveNotesFromSource()));
  connect(m_notesTextEdit, SIGNAL(modificationsNotSaved(bool)), this, SLOT(addOrRemoveStarToOpenDocument(bool)));
  connect(m_notesTextEdit, SIGNAL(notesEditOff(bool)), this, SLOT(addOrRemoveStarToOpenDocument(bool)));

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
  m_sourcesNotesSplitter->addWidget(m_sourcesMethodsSplitter);
  m_sourcesNotesSplitter->addWidget(m_notesTextEdit);
  m_sourcesNotesSplitter->setStretchFactor(0, 1);
  m_sourcesNotesSplitter->setStretchFactor(1, 0);
  m_notesTextEdit->hide();

  // Settings
  QSettings settings("ValentinMicheletINC", "QtSourceBrowser");
  /// Uncomment below to reinit source directory
  //settings.setValue("SourceDirectory", "");

  // Root directory name
  QString sourceDirectory = settings.value("SourceDirectory").toString();
  while (sourceDirectory.isEmpty()) {
    sourceDirectory = QInputDialog::getText(this, "Source Directory", "Provide the source directory");
    if (sourceDirectory.endsWith("/")) {
      sourceDirectory.remove(sourceDirectory.size()-1, 1);
    }
    settings.setValue("SourceDirectory", sourceDirectory);
  }
  m_rootDirectoryName = settings.value("SourceDirectory").toString();

  // Search line edit
  m_searchLineEdit = new QLineEdit;
  m_searchLineEdit->setPlaceholderText("Source file");
  connect(m_searchLineEdit, SIGNAL(textChanged(QString)), this, SLOT(searchFiles(QString)));

  // File System Model
  m_sourceModel = new SourceFileSystemModel(this);
  m_sourceRootIndex = m_sourceModel->setRootPath(m_rootDirectoryName);

  // Source Tree View
  m_sourcesTreeView = new QTreeView;
  m_sourcesTreeView->setModel(m_sourceModel);
  m_sourcesTreeView->setRootIndex(m_sourceRootIndex);
  m_sourcesTreeView->resizeColumnToContents(0);
  connect(m_sourcesTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(openSourceCode(QModelIndex)));

  // Source Search Model
  m_sourceSearchModel = new OpenDocumentsModel(this);
  initSourceSearchModel();

  // Source Search Proxy model
  m_proxyModel = new SourceFileSystemProxyModel(QModelIndex());
  m_proxyModel->setSourceModel(m_sourceSearchModel);

  // Source Search List View
  m_sourceSearchView = new QListView;
  m_sourceSearchView->setModel(m_proxyModel);
  connect(m_sourceSearchView, SIGNAL(clicked(QModelIndex)), this, SLOT(openSourceCodeFromOpenDocuments(QModelIndex)));

  // Source Search Stacked Widget
  m_sourcesStackedWidget = new QStackedWidget;
  m_sourcesStackedWidget->addWidget(m_sourcesTreeView);
  m_sourcesStackedWidget->addWidget(m_sourceSearchView);

  // Show only first column
  for (int k = 1; k < m_sourceModel->columnCount(); ++k)
    m_sourcesTreeView->hideColumn(k);

  //Open documents model
  m_openDocumentsModel = new OpenDocumentsModel;
  m_openDocumentsProxyModel = new QSortFilterProxyModel;
  m_openDocumentsProxyModel->setSourceModel(m_openDocumentsModel);
  connect(m_openDocumentsModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(sortOpenDocuments(QModelIndex,int,int)));

  // Open documents view
  m_openDocumentsView = new QListView;
  m_openDocumentsView->setModel(m_openDocumentsProxyModel);
  connect(m_openDocumentsView, SIGNAL(clicked(QModelIndex)), this, SLOT(openSourceCodeFromOpenDocuments(QModelIndex)));

  // Left part
  QSplitter* vsplitter = new QSplitter(Qt::Vertical);
  vsplitter->addWidget(m_searchLineEdit);
  vsplitter->addWidget(m_sourcesStackedWidget);
  vsplitter->addWidget(m_openDocumentsView);
  vsplitter->setStretchFactor(0, 0);
  vsplitter->setStretchFactor(1, 2);
  vsplitter->setStretchFactor(2, 1);

  // Main part
  QSplitter* hsplitter = new QSplitter;
  hsplitter->addWidget(vsplitter);
  hsplitter->addWidget(m_sourcesNotesSplitter);
  hsplitter->setStretchFactor(0, 0);
  hsplitter->setStretchFactor(1, 1);

  // Main layout
  QHBoxLayout* mainLayout = new QHBoxLayout;
  mainLayout->addWidget(hsplitter);

  setLayout(mainLayout);

  setContextMenuPolicy(Qt::CustomContextMenu);

  /////////////////////////////
  openSourceCodeFromFileName("qmap.cpp");
  openNotesFromSource("qmap.cpp");
  m_notesTextEdit->show();
}

void BrowseSourceWidget::keyReleaseEvent(QKeyEvent* p_event) {
  QString currentSourceOpen = m_openDocumentsView->currentIndex().data(Qt::ToolTipRole).toString();
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

void BrowseSourceWidget::searchFiles(QString const& p_fileName) {
  if (p_fileName.isEmpty() && m_sourcesStackedWidget->currentWidget() != m_sourcesTreeView) {
    m_sourcesStackedWidget->setCurrentWidget(m_sourcesTreeView);
  } else if (!p_fileName.isEmpty() && m_sourcesStackedWidget->currentWidget() != m_sourceSearchView) {
    m_sourcesStackedWidget->setCurrentWidget(m_sourceSearchView);
  }

  m_proxyModel->setFilterRegExp(p_fileName);
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

void BrowseSourceWidget::sortOpenDocuments(QModelIndex, int, int) {
  m_openDocumentsProxyModel->sort(0);
}

void BrowseSourceWidget::showHorizontal() {
  m_sourcesNotesSplitter->setOrientation(Qt::Horizontal);
  m_notesTextEdit->show();
}

void BrowseSourceWidget::showVertical() {
  m_sourcesNotesSplitter->setOrientation(Qt::Vertical);
  m_notesTextEdit->show();
}

void BrowseSourceWidget::openSourceCodeFromFileName(const QString& p_fileName) {
  m_searchLineEdit->setText("^"+p_fileName.toLower());
  QMenu contextMenu(tr("Context menu"), this);

  for (int k = 0; k < m_sourceSearchView->model()->rowCount(); ++k) {
    QString sourceFileName = m_sourceSearchView->model()->index(k, 0).data().toString();
    QAction* action = new QAction(sourceFileName, this);
    connect(action, SIGNAL(triggered()), this, SLOT(openSourceCodeFromMenu()));
    contextMenu.addAction(action);
    if (sourceFileName.endsWith(".cpp")) {
      action->setIcon(QIcon("../QtSourceCodeBrowser/icons/cppFile.png"));
    } else if (sourceFileName.endsWith(".h")) {
      action->setIcon(QIcon("../QtSourceCodeBrowser/icons/hFile.png"));
    }
    m_actionSourcesMap.insert(action, m_sourceSearchView->model()->index(k, 0));
  }
  contextMenu.exec(cursor().pos());
  connect(&contextMenu, SIGNAL(destroyed(QObject*)), this, SLOT(destroyContextualMenu(QObject*)));
}

void BrowseSourceWidget::fillMethodsComboBox(QMap<int, QString> const& p_methodsAndIndex)
{
  m_methodsComboBox->clear();
  for (int index: p_methodsAndIndex.keys())
  {
    m_methodsComboBox->addItem(p_methodsAndIndex.value(index), QVariant::fromValue(index));
  }
}

void BrowseSourceWidget::goToLine(int p_index)
{
  QTextCursor cursor = m_sourcesEditor->textCursor();
  m_sourcesEditor->moveCursor(QTextCursor::End);

  int middleLine = m_sourcesEditor->viewport()->height() / (2 * m_sourcesEditor->fontMetrics().height());

  cursor.setPosition(m_methodsComboBox->itemData(p_index).toInt());
  m_sourcesEditor->setTextCursor(cursor);

  for (int k = 0; k < middleLine; ++k)
    m_sourcesEditor->moveCursor(QTextCursor::Up);
  for (int k = 0; k < middleLine; ++k)
    m_sourcesEditor->moveCursor(QTextCursor::Down);
}

void BrowseSourceWidget::addOrRemoveStarToOpenDocument(bool p_add)
{
  QModelIndex currentOpenDocumentIndex = m_openDocumentsProxyModel->mapToSource(m_openDocumentsView->currentIndex());
  QString newCurrentOpenDocumentName = m_openDocumentsModel->data(currentOpenDocumentIndex).toString();
  if (p_add && !newCurrentOpenDocumentName.endsWith("*")) {
    newCurrentOpenDocumentName += "*";
  } else if (!p_add && newCurrentOpenDocumentName.endsWith("*")) {
    newCurrentOpenDocumentName.remove(newCurrentOpenDocumentName.size()-1, 1);
  }
  m_openDocumentsModel->setData(currentOpenDocumentIndex, newCurrentOpenDocumentName);
}

void BrowseSourceWidget::destroyContextualMenu(QObject* p_object) {
  Q_UNUSED(p_object)
  qDeleteAll(m_actionSourcesMap.keys());
  m_actionSourcesMap.clear();
  m_searchLineEdit->clear();
}

void BrowseSourceWidget::openSourceCodeFromMenu() {
  QAction* actionSender = dynamic_cast<QAction*>(sender());
  Q_ASSERT(actionSender);
  openSourceCodeFromOpenDocuments(m_actionSourcesMap.value(actionSender));
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
      m_openDocumentsView->setCurrentIndex(m_currentOpenDocumentIndex);
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

  m_openDocumentsModel->insertDocument(p_fileName, p_absoluteFilePath);
  m_sourcesEditor->openSourceCode(p_fileName.split(".").first(), getSourceContent(p_absoluteFilePath), fileType);
  QModelIndex openIndex = m_openDocumentsProxyModel->mapFromSource(m_openDocumentsModel->indexFromFile(p_fileName, p_absoluteFilePath));
  m_currentOpenDocumentIndex = openIndex;
  m_openDocumentsView->setCurrentIndex(openIndex);

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

void BrowseSourceWidget::initSourceSearchModel() {
  QSettings settings("ValentinMicheletINC", "QtSourceBrowser");
  fillSourceModelFromDirectory(settings.value("SourceDirectory").toString());
}

void BrowseSourceWidget::fillSourceModelFromDirectory(QString const& p_directoryName)
{
  QDir directory(p_directoryName);
  QStringList subDirectories = directory.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
  QStringList sourceCodeFiles = directory.entryList(QStringList() << "*.cpp" << "*.h", QDir::Files);

  for (QString const& sourceCodeFile: sourceCodeFiles) {
    m_sourceSearchModel->insertDocument(sourceCodeFile, p_directoryName+QDir::separator()+sourceCodeFile);
  }

  for (QString const& subDirectory: subDirectories) {
    fillSourceModelFromDirectory(p_directoryName+QDir::separator()+subDirectory);
  }
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

  addOrRemoveStarToOpenDocument(false);
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
