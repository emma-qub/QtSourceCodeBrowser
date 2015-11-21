#ifndef NOTETEXTEDIT_H
#define NOTETEXTEDIT_H

#include <QTextEdit>
#include <QMimeData>
#include <QImage>

class NoteTextEdit: public QTextEdit {
  Q_OBJECT

public:
  NoteTextEdit(QWidget *parent);
  void dropImage(const QImage& image, const QString& format);

protected:
  bool canInsertFromMimeData(const QMimeData *source) const override;
  void insertFromMimeData(const QMimeData *source) override;
  QMimeData* createMimeDataFromSelection() const override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;

signals:
  void transformToLinkRequested(QTextCursor);
  void transformLinkBackRequested();
  void openSourceRequested(QTextCursor);

private:
  QTextCursor m_currentCursor;
};

#endif
