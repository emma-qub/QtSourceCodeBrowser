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
  bool canInsertFromMimeData(const QMimeData *source) const;
  void insertFromMimeData(const QMimeData *source);
  QMimeData* createMimeDataFromSelection() const;
};

#endif
