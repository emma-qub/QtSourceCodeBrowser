#ifndef BROWSESOURCEWIDGET_HXX
#define BROWSESOURCEWIDGET_HXX

#include <QWidget>
#include <QTabWidget>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QTreeView>
#include <QLineEdit>
#include <QListView>
#include <QStackedWidget>
#include <QSplitter>

#include "Highlighter.hxx"
#include "SourceFileSystemModel.hxx"
#include "SourceFileSystemProxyModel.hxx"
#include "OpenDocumentsModel.hxx"
#include "CodeEditor.hxx"
#include "NoteRichTextEdit.hxx"

class BrowseSourceWidget: public QWidget {
  Q_OBJECT

public:
  explicit BrowseSourceWidget(QWidget *parent = nullptr);

public slots:
  void expandAll();

protected:
  void keyReleaseEvent(QKeyEvent* p_event) override;
  void getNotesFileInfo(const QString& fileName);
  void saveNotesFromSource();

protected slots:
  void searchFiles(QString const& p_fileName);
  void openSourceCode(QModelIndex const& p_index);
  void openSourceCodeFromOpenDocuments(QModelIndex const& p_index);
  void sortOpenDocuments(QModelIndex, int, int);

private:
  QString getSourceContent(QString const& absoluteFilePath);
  void openDocumentInEditor(QString const& fileName, QString const& absoluteFilePath);
  void initSourceSearchModel();
  void fillSourceModelFromDirectory(QString const& p_directoryName);
  void openNotesFromSource(QString const& fileName);

  CodeEditor* m_sourcesEditor;
  NoteRichTextEdit* m_notesTextEdit;
  QSplitter* m_sourcesNotesSplitter;

  SourceFileSystemModel* m_sourceModel;
  QTreeView* m_sourcesTreeView;

  OpenDocumentsModel* m_sourceSearchModel;
  SourceFileSystemProxyModel* m_proxyModel;
  QListView* m_sourceSearchView;

  QStackedWidget* m_sourcesStackedWidget;

  QLineEdit* m_searchLineEdit;
  QVector<Highlighter*> m_highlighters;

  QModelIndex m_sourceRootIndex;
  QModelIndex m_proxyRootIndex;

  QString m_rootDirectoryName;

  OpenDocumentsModel* m_openDocumentsModel;
  QListView* m_openDocumentsView;
  QSortFilterProxyModel* m_openDocumentsProxyModel;

  bool m_notesExist;
  QFileInfo m_notesFileInfo;
  QString m_notesFilePath;
};

#endif // BROWSESOURCEWIDGET_HXX
