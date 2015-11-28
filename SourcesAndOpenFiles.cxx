#include "SourcesAndOpenFiles.hxx"

#include <QSettings>
#include <QInputDialog>
#include <QMenu>
#include <QDebug>

SourcesAndOpenFiles::SourcesAndOpenFiles(QWidget* p_parent):
  QWidget(p_parent) {

  setupUi(this);

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

  // Search Line Edit
  connect(m_searchLineEdit, SIGNAL(textChanged(QString)), this, SLOT(searchFiles(QString)));

  // File System Model
  m_sourceModel = new SourceFileSystemModel(this);
  m_sourceRootIndex = m_sourceModel->setRootPath(m_rootDirectoryName);

  // Source Tree View
  m_sourcesTreeView->setModel(m_sourceModel);
  m_sourcesTreeView->setRootIndex(m_sourceRootIndex);
  m_sourcesTreeView->setHeaderHidden(true);
  connect(m_sourcesTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SIGNAL(openSourceCodeFromTreeViewRequested(QModelIndex)));
  connect(m_sourcesTreeView, SIGNAL(activated(QModelIndex)), this, SIGNAL(openSourceCodeFromTreeViewRequested(QModelIndex)));

  // Source Search Model
  m_sourceSearchModel = new OpenDocumentsModel(this);
  fillSourceModelFromSettingsDirectory(m_rootDirectoryName);

  // Source Search Proxy model
  m_sourceFileSystemProxyModel = new SourceFileSystemProxyModel(QModelIndex());
  m_sourceFileSystemProxyModel->setSourceModel(m_sourceSearchModel);

  // Source Search List View
  m_sourceSearchView->setModel(m_sourceFileSystemProxyModel);
  connect(m_sourceSearchView, SIGNAL(clicked(QModelIndex)), this, SIGNAL(openSourceCodeFromSearchRequested(QModelIndex)));
  connect(m_sourceSearchView, SIGNAL(clicked(QModelIndex)), this, SLOT(expandTreeView(QModelIndex)));
  connect(m_openDocumentsView, SIGNAL(clicked(QModelIndex)), this, SLOT(expandTreeView(QModelIndex)));

  // Show only first column
  for (int k = 1; k < m_sourceModel->columnCount(); ++k)
    m_sourcesTreeView->hideColumn(k);

  //Open documents model
  m_openDocumentsModel = new OpenDocumentsModel;
  m_openDocumentsSortFilterProxyModel = new QSortFilterProxyModel;
  m_openDocumentsSortFilterProxyModel->setSourceModel(m_openDocumentsModel);
  connect(m_openDocumentsSortFilterProxyModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(sortOpenDocuments(QModelIndex,int,int)));

  // Open documents view
  m_openDocumentsView->setModel(m_openDocumentsSortFilterProxyModel);
  m_openDocumentsView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  connect(m_openDocumentsView, SIGNAL(clicked(QModelIndex)), this, SIGNAL(openSourceCodeFromOpenDocumentsRequested(QModelIndex)));

  // Splitter
  m_splitter->setStretchFactor(0, 2);
  m_splitter->setStretchFactor(1, 1);
}

/// Public

void SourcesAndOpenFiles::setSearchLineEditText(const QString& p_text) {
  m_searchLineEdit->setText(p_text);
}

void SourcesAndOpenFiles::setFocusToSearchLineEdit() {
  m_searchLineEdit->setFocus();
}

QModelIndex SourcesAndOpenFiles::getCurrentIndex() const {
  return m_openDocumentsView->currentIndex();
}

void SourcesAndOpenFiles::setCurrentIndex(const QString& p_fileName, const QString& p_absoluteFilePath) {
  for (int k = 0; k < m_openDocumentsSortFilterProxyModel->rowCount(); ++k) {
    QModelIndex currentIndex = m_openDocumentsSortFilterProxyModel->index(k, 0);
    if ((currentIndex.data() == p_fileName || currentIndex.data() == p_fileName+"*")
      && currentIndex.data(Qt::ToolTipRole) == p_absoluteFilePath) {
      m_openDocumentsView->setCurrentIndex(currentIndex);
      return;
    }
  }
}

QString SourcesAndOpenFiles::getCurrentOpenDocumentAbsolutePath() const {
  return m_openDocumentsView->currentIndex().data(Qt::ToolTipRole).toString();
}

void SourcesAndOpenFiles::insertDocument(const QString& p_fileName, const QString& p_absoluteFilePath) {
  m_openDocumentsModel->insertDocument(p_fileName, p_absoluteFilePath);
}

void SourcesAndOpenFiles::removeOpenDocument(QString const& p_absoluteFilePath) {
  m_openDocumentsModel->closeOpenDocument(p_absoluteFilePath);
}

void SourcesAndOpenFiles::clearOpenDocument() {
  m_openDocumentsModel->closeAllOpenDocument();
}


/// Public slots

void SourcesAndOpenFiles::addOrRemoveStarToOpenDocument(QString const& p_fileName, QString const& p_absoluteFilePath, bool p_add) {
  QModelIndex currentIndex = m_openDocumentsModel->getIndexFromFileNameAndAbsolutePath(p_fileName, p_absoluteFilePath);
  if (currentIndex.isValid() == false) {
    return;
  }

  QString newCurrentOpenDocumentName = p_fileName;
  if (p_add && !newCurrentOpenDocumentName.endsWith("*")) {
    newCurrentOpenDocumentName += "*";
  } else if (!p_add && newCurrentOpenDocumentName.endsWith("*")) {
    newCurrentOpenDocumentName.remove(newCurrentOpenDocumentName.size()-1, 1);
  }
  m_openDocumentsModel->setData(currentIndex, newCurrentOpenDocumentName);
}

void SourcesAndOpenFiles::openSourceCodeFromFileName(const QString& p_fileName) {
  m_searchLineEdit->setText("^"+p_fileName.toLower()+"(_p)?\\.");
  QMenu contextMenu(tr("Context menu"), this);
  contextMenu.setStyleSheet("QMenu { menu-scrollable: 1; }");

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


/// Protected slots

void SourcesAndOpenFiles::searchFiles(QString const& p_fileName) {
  if (p_fileName.isEmpty() && m_sourcesStackedWidget->currentWidget() != m_sourcesTreeView->parentWidget()) {
    m_sourcesStackedWidget->setCurrentWidget(m_sourcesTreeView->parentWidget());
  } else if (!p_fileName.isEmpty() && m_sourcesStackedWidget->currentWidget() != m_sourceSearchView->parentWidget()) {
    m_sourcesStackedWidget->setCurrentWidget(m_sourceSearchView->parentWidget());
  }

  m_sourceFileSystemProxyModel->setFilterRegExp(p_fileName);
}

void SourcesAndOpenFiles::sortOpenDocuments(QModelIndex, int, int) {
  m_openDocumentsSortFilterProxyModel->sort(0);
}

void SourcesAndOpenFiles::destroyContextualMenu(QObject* p_object) {
  Q_UNUSED(p_object)
  qDeleteAll(m_actionSourcesMap.keys());
  m_actionSourcesMap.clear();
  m_searchLineEdit->clear();
}

void SourcesAndOpenFiles::openSourceCodeFromMenu() {
  QAction* actionSender = dynamic_cast<QAction*>(sender());
  Q_ASSERT(actionSender != nullptr);
  emit openSourceCodeFromContextualMenuRequested(m_actionSourcesMap.value(actionSender));
}

void SourcesAndOpenFiles::expandTreeView(const QModelIndex& p_index) {
  m_searchLineEdit->clear();
  m_sourcesTreeView->collapseAll();
  QString absolutePath = p_index.data(Qt::ToolTipRole).toString();
  m_sourcesTreeView->setCurrentIndex(m_sourceModel->index(absolutePath));
}


/// Private

void SourcesAndOpenFiles::fillSourceModelFromSettingsDirectory(QString const& p_directoryName) {
  QDir directory(p_directoryName);
  QStringList subDirectories = directory.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
  QStringList sourceCodeFiles = directory.entryList(QStringList() << "*.cpp" << "*.h", QDir::Files);

  for (QString const& sourceCodeFile: sourceCodeFiles) {
    m_sourceSearchModel->insertDocument(sourceCodeFile, p_directoryName+QDir::separator()+sourceCodeFile);
  }

  for (QString const& subDirectory: subDirectories) {
    fillSourceModelFromSettingsDirectory(p_directoryName+QDir::separator()+subDirectory);
  }
}
