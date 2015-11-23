#ifndef NOTETEXTEDIT_H
#define NOTETEXTEDIT_H

#include <QTextBrowser>
#include <QMimeData>
#include <QImage>

class NoteTextEdit: public QTextBrowser {
  Q_OBJECT

public:
  NoteTextEdit(QWidget* p_parent = nullptr);
  void dropImage(QImage const& p_image, QString const& p_format);
  bool hasModificationsNotSaved() const { return m_hasModificationsNotSaved; }
  void hasNoModifications() { m_hasModificationsNotSaved = false; document()->clearUndoRedoStacks(); }

public slots:
  void setModificationsNotSaved(bool p_value);

protected:
  bool canInsertFromMimeData(QMimeData const* p_source) const override;
  void insertFromMimeData(QMimeData const* p_source) override;
  QMimeData* createMimeDataFromSelection() const override;
  void mouseMoveEvent(QMouseEvent* p_event) override;
  void mousePressEvent(QMouseEvent* p_event) override;
  void keyReleaseEvent(QKeyEvent* p_event) override;
  void mouseDoubleClickEvent(QMouseEvent* p_event) override;

signals:
  void transformToLinkRequested(QTextCursor);
  void transformLinkBackRequested();
  void openSourceRequested(QTextCursor);
  void modificationsNotSaved(bool);
  void editNotesRequested();

private:
  QTextCursor m_currentCursor;
  bool m_hasModificationsNotSaved;
};

#endif
