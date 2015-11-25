#ifndef BROWSESOURCEWIDGET_HXX
#define BROWSESOURCEWIDGET_HXX

#include <QWidget>
#include <QSplitter>
#include <QFileInfo>

#include "SourcesAndOpenFiles.hxx"
#include "SourceCodeEditor.hxx"
#include "NoteRichTextEdit.hxx"

class BrowseSourceWidget: public QWidget {
  Q_OBJECT

public:
  explicit BrowseSourceWidget(QWidget* p_parent = nullptr);

  bool hasModificationsNotSaved() const { return m_notesTextEdit->hasModificationsNotSaved(); }

protected:
  void keyReleaseEvent(QKeyEvent* p_event) override;
  void getNotesFileInfo(const QString& p_fileName);

public slots:
  void saveNotesFromSource();

protected slots:
  void openSourceCode(QModelIndex const& p_index);
  void openSourceCodeFromOpenDocuments(QModelIndex const& p_index);
  void showHorizontal();
  void showVertical();
  void hideNotesTextEdit();

private:
  QString getSourceContent(QString const& p_absoluteFilePath);
  void openDocumentInEditor(QString const& p_fileName, QString const& p_absoluteFilePath);
  void openNotesFromSource(QString const& p_fileName);

  SourcesAndOpenFiles* m_sourcesAndOpenFiles;
  SourceCodeEditor* m_sourcesEditor;
  NoteRichTextEdit* m_notesTextEdit;
  QSplitter* m_sourcesNotesSplitter;

  bool m_notesExist;
  QFileInfo m_notesFileInfo;
  QString m_notesFilePath;

  QModelIndex m_currentOpenDocumentIndex;

  QAction* m_editNotesOnHorizontalAction;
  QAction* m_editNotesOnVerticalAction;
  QAction* m_editNotesOffAction;
};

#endif // BROWSESOURCEWIDGET_HXX
