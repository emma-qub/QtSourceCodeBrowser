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

  void connectTextChanged();
  void disconnectTextChanged();

protected:
  bool canInsertFromMimeData(QMimeData const* p_source) const override;
  void insertFromMimeData(QMimeData const* p_source) override;
  QMimeData* createMimeDataFromSelection() const override;
  void mouseMoveEvent(QMouseEvent* p_event) override;
  void mousePressEvent(QMouseEvent* p_event) override;
  void keyReleaseEvent(QKeyEvent* p_event) override;
  void mouseDoubleClickEvent(QMouseEvent* p_event) override;

protected slots:
  void hasModificationsNotSaved();

signals:
  void transformToLinkRequested(QTextCursor);
  void transformLinkBackRequested();
  void contextMenuRequested(QTextCursor);
  void modificationsNotSaved(bool);
  void editNotesRequested();

private:
  QTextCursor m_currentCursor;
};

#endif
