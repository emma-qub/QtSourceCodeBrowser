#ifndef BROWSESOURCEWIDGET_HXX
#define BROWSESOURCEWIDGET_HXX

#include <QWidget>
#include <QSplitter>
#include <QFileInfo>
#include <QMessageBox>

#include "SourcesAndOpenFiles.hxx"
#include "SourceCodeEditor.hxx"
#include "NoteRichTextEdit.hxx"

class BrowseSourceWidget: public QWidget {
  Q_OBJECT

  class BrowseFileInfo {
  public:
    bool isNotesSaved(QString const& p_absoluteFilePath, QString const& p_fileName = "") const {
      QString fileName = p_fileName;
      if (fileName.isEmpty()) {
        fileName = getFileNameFromOpenDocumentAbsolutePath(p_absoluteFilePath);
      }
      return m_noteSaveStates.value(QPair<QString, QString>(p_absoluteFilePath, fileName), true);
    }
    bool isCurrentNotesSaved() const {
      QString currentAbsoluteFilePath = getCurrentNotesAbsoluteFilePath();
      return isNotesSaved(currentAbsoluteFilePath, getCurrentOpenDocumentFileName(currentAbsoluteFilePath));
    }
    void setNotesSaveState(bool p_saved, QString p_absoluteFilePath = "") {
      QString absoluteFilePath = p_absoluteFilePath;
      if (absoluteFilePath.isEmpty()) {
        absoluteFilePath = getCurrentOpenDocumentAbsoluteFilePath();
      }
      QFileInfo currentFileInfo(absoluteFilePath);
      setNoteSaveState(absoluteFilePath, currentFileInfo.fileName(), p_saved);
    }
    QList<QPair<QString, QString>> getNotSavedNotes() const {
      QList<QPair<QString, QString>> notSavedNotesList;
      for (QPair<QString, QString> currentNote: m_noteSaveStates.keys()) {
        if (m_noteSaveStates.value(currentNote) == false) {
          notSavedNotesList << currentNote;
        }
      }
      return notSavedNotesList;
    }
    bool hasNoteNotSaved() const {
      for (QPair<QString, QString> currentNote: m_noteSaveStates.keys()) {
        if (m_noteSaveStates.value(currentNote) == false) {
          return true;
        }
      }
      return false;
    }

    QString getCurrentNotesAbsoluteFilePath() const {
      return m_currentNoteAndSourceAbsoluteFilePath.first;
    }
    QString getCurrentOpenDocumentAbsoluteFilePath() const {
      return m_currentNoteAndSourceAbsoluteFilePath.second;
    }
    QString getCurrentOpenDocumentFileName(QString const& p_absoluteFilePath) const {
      for (QPair<QString, QString> fileNameAndAbsoluteFilePath: m_noteSaveStates.keys()) {
        if (fileNameAndAbsoluteFilePath.first == p_absoluteFilePath) {
          return fileNameAndAbsoluteFilePath.second;
        }
      }
      return "";
    }

    void appendNotesAndOpenDocument(QString const& p_absoluteFilePath, QString const& p_fileName) {
      if (addNotesIfNotExists(p_absoluteFilePath, p_fileName)) {
        QString notesAbsoluteFilePath = getNotesAbsolutePathFromOpenDocumentAbsolutePath(p_absoluteFilePath);
        setCurrentNotesAndSourceAbsoluteFilePath(notesAbsoluteFilePath, p_absoluteFilePath);
      }
    }

    void setCurrentNotesAndSourceAbsoluteFilePath(QString const& p_absoluteFilePath) {
      QString notesAbsoluteFilePath = getNotesAbsolutePathFromOpenDocumentAbsolutePath(p_absoluteFilePath);
      setCurrentNotesAndSourceAbsoluteFilePath(notesAbsoluteFilePath, p_absoluteFilePath);
    }

    int getNotesPositionInStack(QString const& p_notesAbsoluteFilePath) {
      for (int k = 0; k < m_notesAbsoluteFilePaths.size(); ++k) {
        if (m_notesAbsoluteFilePaths.at(k) == p_notesAbsoluteFilePath) {
          return k;
        }
      }
      return -1;
    }

    void insertNotesAbsoluteFilePath(int position, QString const& p_notesAbsoluteFilePath) {
      m_notesAbsoluteFilePaths.insert(position, p_notesAbsoluteFilePath);
    }

    QList<QPair<QString, QString>> getNotSavedNotesList() const {
      QList<QPair<QString, QString>> notSavedNotesList;
      for (auto currentNotSavedNotes: getNotSavedNotes()) {
        notSavedNotesList << currentNotSavedNotes;
      }
      return notSavedNotesList;
    }

    void removeNotesAt(int p_currentNotesPosition) {
      m_notesAbsoluteFilePaths.removeAt(p_currentNotesPosition);
    }

    void clearNotes() {
      m_notesAbsoluteFilePaths.clear();
    }

    bool removeSourceFromAbsoluteFilePath(QString const& p_absoluteFilePath) {
      for (auto absoluteFilePathAndFileName: m_noteSaveStates.keys()) {
        if (absoluteFilePathAndFileName.first == p_absoluteFilePath) {
          m_noteSaveStates.remove(absoluteFilePathAndFileName);
          return true;
        }
      }
      return false;
    }

    void clearSourceFromAbsoluteFilePath() {
      m_noteSaveStates.clear();
    }

    void clearCurrentNoteAndSourceAbsoluteFilePath() {
      m_currentNoteAndSourceAbsoluteFilePath = QPair<QString, QString>("", "");
    }

    QString getNotesAbsolutePathFromOpenDocumentAbsolutePath(QString const& p_absoluteFilePath) const {
      QFileInfo fileInfo(p_absoluteFilePath);
      QString baseName = fileInfo.baseName();
      if (baseName.endsWith("_p")) {
        baseName.remove(baseName.size()-2, 2);
      }
      QFileInfo notesPathFileInfo("../QtSourceCodeBrowser/notes/");
      return notesPathFileInfo.absolutePath()+QDir::separator()+baseName+".txt";
    }

    QString getFileNameFromOpenDocumentAbsolutePath(QString const& p_absoluteFilePath) const {
      QFileInfo fileInfo(p_absoluteFilePath);
      return fileInfo.fileName();
    }

  protected:
    void setNoteSaveState(QString const& p_absoluteFilePath, QString const& p_fileName, bool p_saved) {
      auto key = QPair<QString, QString>(p_absoluteFilePath, p_fileName);
      Q_ASSERT(m_noteSaveStates.contains(key));
      m_noteSaveStates[key] = p_saved;
    }
    bool addNotesIfNotExists(QString const& p_absoluteFilePath, QString const& p_fileName) {
      QPair<QString, QString> pathAndfile(p_absoluteFilePath, p_fileName);
      if (m_noteSaveStates.contains(pathAndfile) == false) {
        m_noteSaveStates.insert(pathAndfile, true);
        return true;
      }
      return false;
    }
    void setCurrentNotesAndSourceAbsoluteFilePath(QString const& p_notesAbsoluteFilePath, QString const& p_absoluteFilePath) {
      m_currentNoteAndSourceAbsoluteFilePath = QPair<QString, QString>(p_notesAbsoluteFilePath, p_absoluteFilePath);
    }

  private:
    QMap<QPair<QString, QString>, bool> m_noteSaveStates;
    QPair<QString, QString> m_currentNoteAndSourceAbsoluteFilePath;
    QVector<QString> m_notesAbsoluteFilePaths;
  };


public:
  explicit BrowseSourceWidget(QWidget* p_parent = nullptr);

  bool hasModificationsNotSaved() const { return m_browseFileInfo.hasNoteNotSaved(); }
  QList<QPair<QString, QString>> getNotSavedNotes() const;
  int askToSave(QStringList const& fileNamesList) const;
  void getNotesListToSaveAndFileNamesList(QStringList& p_absoluteFilePathList, QStringList& p_fileNamesList) const;

protected:
  void keyReleaseEvent(QKeyEvent* p_event) override;
  void addOrRemoveStarToOpenDocument(bool p_value, const QString& p_absoluteFilePath = "");
  NoteRichTextEdit* getNotesTextEditFromPosition(int p_position = -1) const;

public slots:
  void saveNotesFromSourceAndCloseEditor();
  void saveAllNotes();
  void saveNotesFromSource(QStringList const& p_absoluteFilePathListToSave = QStringList());
  void saveNotesFromSource(QString const& p_absoluteFilePath);
  void showHorizontal();
  void showVertical();
  void hideNotesTextEdit();
  void closeNotesAndSource(QString const& p_absoluteFilePath = "");
  void closeAllNotesAndSource();

protected slots:
  void openSourceCodeFromTreeView(QModelIndex const& p_index);
  void openSourceCodeFromSearch(QModelIndex const& p_index);
  void openSourceCodeFromOpenDocuments(QModelIndex const& p_index);
  void openSourceCodeFromContextualMenu(QModelIndex const& p_index);
  void updateSaveStateToNotes(bool p_value, QString const& p_absoluteFilePath = "");

signals:
  void enableSplitRequested();
  void disableSplitRequested();

private:
  QString getFileContent(QString const& p_absoluteFilePath);
  void openDocumentInEditor(QString const& p_fileName, QString const& p_absoluteFilePath);
  void openNotes(QString const& p_notesAbsoluteFilePath);

  BrowseFileInfo m_browseFileInfo;

  SourcesAndOpenFiles* m_sourcesAndOpenFilesWidget;
  SourceCodeEditor* m_sourceCodeEditorWidget;
  QStackedWidget* m_noteRichTextEditStackWidget;
  QSplitter* m_sourcesNotesSplitter;
};

#endif // BROWSESOURCEWIDGET_HXX
