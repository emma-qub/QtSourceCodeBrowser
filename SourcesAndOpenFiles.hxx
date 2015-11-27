#ifndef SOURCESANDOPENFILES_HXX
#define SOURCESANDOPENFILES_HXX

#include <QWidget>
#include <QStackedWidget>
#include <QListView>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QTreeView>
#include <QLineEdit>

#include "ui_SourcesAndOpenFiles.h"

#include "SourceFileSystemModel.hxx"
#include "SourceFileSystemProxyModel.hxx"
#include "OpenDocumentsModel.hxx"

class SourcesAndOpenFiles: public QWidget, protected Ui::SourcesAndOpenFiles {
  Q_OBJECT

public:
  explicit SourcesAndOpenFiles(QWidget* p_parent = nullptr);

  void setSearchLineEditText(QString const& p_text) { m_searchLineEdit->setText(p_text); }
  QModelIndex getCurrentIndex() const;
  void setCurrentIndex(QString const& p_fileName, QString const& p_absoluteFilePath);
  QString getCurrentOpenDocumentAbsolutePath() const;
  void insertDocument(QString const& p_fileName, QString const& p_absoluteFilePath);
  void removeOpenDocument(QString const& p_absoluteFilePath);
  void clearOpenDocument();

public slots:
  void addOrRemoveStarToOpenDocument(QString const& p_fileName, QString const& p_absoluteFilePath, bool p_add);
  void openSourceCodeFromFileName(QString const& p_fileName);

protected slots:
  //void openPreviousSource(QModelIndex const& p_currentIndex);
  void searchFiles(QString const& p_fileName);
  void sortOpenDocuments(QModelIndex, int, int);
  void destroyContextualMenu(QObject* p_object);
  void openSourceCodeFromMenu();
  void expandTreeView(QModelIndex const& p_index);

signals:
  void openSourceCodeFromTreeViewRequested(QModelIndex);
  void openSourceCodeFromSearchRequested(QModelIndex);
  void openSourceCodeFromOpenDocumentsRequested(QModelIndex);
  void openSourceCodeFromContextualMenuRequested(QModelIndex);
  //void openPreviousSourceRequested(QModelIndex);
  //void clearSourceCodeRequested();
  //void clearAllSourceCodeRequested();

private:
  void fillSourceModelFromSettingsDirectory(QString const& p_directoryName);

  SourceFileSystemModel* m_sourceModel;

  OpenDocumentsModel* m_sourceSearchModel;
  SourceFileSystemProxyModel* m_proxyModel;

  QModelIndex m_sourceRootIndex;
  QModelIndex m_proxyRootIndex;

  QString m_rootDirectoryName;

  OpenDocumentsModel* m_openDocumentsModel;

  QMap<QAction*, QModelIndex> m_actionSourcesMap;

  QString m_previousAbsolutePath;
};

#endif // SOURCESANDOPENFILES_HXX
