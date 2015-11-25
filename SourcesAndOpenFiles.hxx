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
  void setCurrentIndex(QModelIndex const& p_index);
  QString getCurrentOpenDocumentAbsolutePath() const;
  void insertDocument(QString const& p_fileName, QString const& p_absoluteFilePath);
  QModelIndex getCurrentOpenDocumentIndex(QString const& p_fileName, QString const& p_absoluteFilePath);

public slots:
  void addOrRemoveStarToOpenDocument(bool p_add);
  void openSourceCodeFromFileName(const QString& p_fileName);

protected slots:
  void searchFiles(QString const& p_fileName);
  void sortOpenDocuments(QModelIndex, int, int);
  void destroyContextualMenu(QObject* p_object);
  void openSourceCodeFromMenu();
  void expandTreeView(QModelIndex const& p_index);

signals:
  void openSourceCodeRequested(QModelIndex);
  void openSourceCodeFromOpenDocumentsRequested(QModelIndex);

private:
  void fillSourceModelFromSettingsDirectory(QString const& p_directoryName);

  SourceFileSystemModel* m_sourceModel;

  OpenDocumentsModel* m_sourceSearchModel;
  SourceFileSystemProxyModel* m_proxyModel;

  QModelIndex m_sourceRootIndex;
  QModelIndex m_proxyRootIndex;

  QString m_rootDirectoryName;

  OpenDocumentsModel* m_openDocumentsModel;
  QSortFilterProxyModel* m_openDocumentsProxyModel;

  QMap<QAction*, QModelIndex> m_actionSourcesMap;
};

#endif // SOURCESANDOPENFILES_HXX
