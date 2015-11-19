#include "BrowseSourceWidget.hxx"

#include <QSettings>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTextStream>
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>

#include "CodeEditor.hxx"

BrowseSourceWidget::BrowseSourceWidget(QWidget* p_parent):
  QWidget(p_parent),
  m_highlighters() {

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

  // Settings
  QSettings settings("ValentinMicheletINC", "QtSourceBrowser");
  /// Uncomment below to reinit source directory
  //settings.setValue("SourceDirectory", "");

  // Root directory name
  QString sourceDirectory = settings.value("SourceDirectory").toString();
  while (sourceDirectory.isEmpty())
  {
    sourceDirectory = QInputDialog::getText(this, "Source Directory", "Provide the source directory");
    settings.setValue("SourceDirectory", sourceDirectory);
  }
  m_rootDirectoryName = settings.value("SourceDirectory").toString();

  // Search line edit
  m_searchLineEdit = new QLineEdit;
  connect(m_searchLineEdit, SIGNAL(textChanged(QString)), this, SLOT(searchFiles(QString)));

  // File System Model
  m_sourceModel = new SourceFileSystemModel(this);
  connect(m_sourceModel, SIGNAL(rootPathChanged(QString)), this, SLOT(setRootPathToModel(QString)));
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
  hsplitter->addWidget(m_sourcesEditor);
  hsplitter->setStretchFactor(0, 0);
  hsplitter->setStretchFactor(1, 1);

  // Main layout
  QHBoxLayout* mainLayout = new QHBoxLayout;
  mainLayout->addWidget(hsplitter);
  mainLayout->setStretch(0, 0);
  mainLayout->setStretch(1, 1);

  setLayout(mainLayout);
}

void BrowseSourceWidget::expandAll()
{
  m_sourcesTreeView->expandAll();
}

void BrowseSourceWidget::keyReleaseEvent(QKeyEvent* p_event)
{
  QString currentSourceOpen = m_openDocumentsView->currentIndex().data(Qt::ToolTipRole).toString();
  if (currentSourceOpen.isEmpty())
    return;

  switch (p_event->key())
  {
  case Qt::Key_F4:
  {
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
  }
  case Qt::Key_F5:
  {
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

void BrowseSourceWidget::searchFiles(QString const& p_fileName)
{
  if (p_fileName.isEmpty() && m_sourcesStackedWidget->currentWidget() != m_sourcesTreeView)
    m_sourcesStackedWidget->setCurrentWidget(m_sourcesTreeView);
  else if (!p_fileName.isEmpty() && m_sourcesStackedWidget->currentWidget() != m_sourceSearchView)
    m_sourcesStackedWidget->setCurrentWidget(m_sourceSearchView);

  m_proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
  m_proxyModel->setFilterRegExp(p_fileName);
}

void BrowseSourceWidget::setRootPathToModel(const QString& p_rootPath)
{
  qDebug() << p_rootPath;
}

void BrowseSourceWidget::openSourceCode(QModelIndex const& p_index)
{
  QString fileName(p_index.data().toString());
  if (fileName.endsWith(".cpp") || fileName.endsWith(".h"))
  {
    QString absoluteFilePath(fileName);
    QModelIndex currentIndex = p_index;
    QModelIndex parentIndex;
    while ((parentIndex = currentIndex.parent()) != QModelIndex())
    {
      QString parentDirectoryName = parentIndex.data().toString();
      if (parentDirectoryName == QString(QDir::separator()))
      {
        absoluteFilePath = QDir::separator() + absoluteFilePath;
        break;
      }

      absoluteFilePath = parentDirectoryName + QDir::separator() + absoluteFilePath;
      currentIndex = parentIndex;
    }

    openDocumentInEditor(fileName, absoluteFilePath);
  }
}

void BrowseSourceWidget::sortOpenDocuments(QModelIndex, int, int)
{
  m_openDocumentsProxyModel->sort(0);
}

void BrowseSourceWidget::openSourceCodeFromOpenDocuments(QModelIndex const& p_index)
{
  QString fileName = p_index.data(Qt::DisplayRole).toString();
  QString absoluteFilePath = p_index.data(Qt::ToolTipRole).toString();
  openDocumentInEditor(fileName, absoluteFilePath);
}

void BrowseSourceWidget::openDocumentInEditor(QString const& fileName, QString const& absoluteFilePath)
{
  QFile sourceFile(absoluteFilePath);
  if (!sourceFile.exists())
  {
    qDebug() << "404 Not Found" << "The file\n"+absoluteFilePath+"\ndoes not exist on this computer.";
    return;
  }
  m_openDocumentsModel->insertDocument(fileName, absoluteFilePath);
  m_sourcesEditor->setPlainText(getSourceContent(absoluteFilePath));
  QModelIndex openIndex = m_openDocumentsProxyModel->mapFromSource(m_openDocumentsModel->indexFromFile(fileName, absoluteFilePath));
  m_openDocumentsView->setCurrentIndex(openIndex);
}

QString BrowseSourceWidget::getSourceContent(QString const& absoluteFilePath)
{
  QFile sourceFile(absoluteFilePath);
  if (!sourceFile.open(QIODevice::ReadOnly | QIODevice::Text))
    QMessageBox::warning(this, "Opening issue", sourceFile.errorString());

  QTextStream in(&sourceFile);

  QString content;
  while(!in.atEnd())
  {
    content.append(in.readLine());
    content.append("\n");
  }

  sourceFile.close();

  return content;
}

void BrowseSourceWidget::initSourceSearchModel()
{
  QSettings settings("ValentinMicheletINC", "QtSourceBrowser");
  fillSourceModelFromDirectory(settings.value("SourceDirectory").toString());
}

void BrowseSourceWidget::fillSourceModelFromDirectory(QString const& p_directoryName)
{
  QDir directory(p_directoryName);
  QStringList subDirectories = directory.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
  QStringList sourceCodeFiles = directory.entryList(QStringList() << "*.cpp" << "*.h", QDir::Files);

  for (QString const& sourceCodeFile: sourceCodeFiles)
  {
    m_sourceSearchModel->insertDocument(sourceCodeFile, p_directoryName+QDir::separator()+sourceCodeFile);
  }

  for (QString const& subDirectory: subDirectories)
  {
    fillSourceModelFromDirectory(p_directoryName+QDir::separator()+subDirectory);
  }
}
