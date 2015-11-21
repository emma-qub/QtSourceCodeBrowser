#ifndef NOTETEXTEDIT_H
#define NOTETEXTEDIT_H

#include <QTextEdit>
#include <QMimeData>
#include <QImage>

class NoteTextEdit: public QTextEdit {
  Q_OBJECT

public:
  NoteTextEdit(QWidget* p_parent = nullptr);
  void dropImage(QImage const& p_image, QString const& p_format);

protected:
  bool canInsertFromMimeData(QMimeData const* p_source) const override;
  void insertFromMimeData(QMimeData const* p_source) override;
  QMimeData* createMimeDataFromSelection() const override;
  void mouseMoveEvent(QMouseEvent* p_event) override;
  void mousePressEvent(QMouseEvent* p_event) override;
  void keyReleaseEvent(QKeyEvent* p_event) override;

signals:
  void transformToLinkRequested(QTextCursor);
  void transformLinkBackRequested();
  void openSourceRequested(QTextCursor);

private:
  QTextCursor m_currentCursor;
};

#endif
