/*
** Copyright (C) 2013 Jiří Procházka (Hobrasoft)
** Contact: http://www.hobrasoft.cz/
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file is under the terms of the GNU Lesser General Public License
** version 2.1 as published by the Free Software Foundation and appearing
** in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the
** GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
*/

#ifndef NOTERICHTEXTEDIT_H
#define NOTERICHTEXTEDIT_H

#include <QPointer>

#include "ui_NoteRichTextEdit.h"

class NoteRichTextEdit: public QWidget, protected Ui::NoteRichTextEdit {
  Q_OBJECT

public:
  NoteRichTextEdit(QWidget* p_parent = nullptr);

  QString toPlainText() const;
  QString toHtml() const;
  QTextDocument* document() const;
  QTextCursor textCursor() const;
  void setTextCursor(const QTextCursor& p_cursor);
  void turnEditOff();
  bool hasModificationsNotSaved() const { return f_textedit->hasModificationsNotSaved(); }

public slots:
  void setText(const QString& p_text);

protected slots:
  void setPlainText(const QString& p_text);
  void setHtml(const QString& p_text);
  void textRemoveFormat();
  void textRemoveAllFormat();
  void textBold();
  void textUnderline();
  void textStrikeout();
  void textItalic();
  void textSize(const QString& p_point);
  void textLink(bool p_checked);
  void textStyle(int p_index);
  void textFgColor();
  void textBgColor();
  void listBullet(bool p_checked);
  void listOrdered(bool p_checked);
  void slotCurrentCharFormatChanged(const QTextCharFormat& p_format);
  void slotCursorPositionChanged();
  void increaseIndentation();
  void decreaseIndentation();
  void insertImage();
  void textSource();
  void insertCode(bool checked);
  void transformTextToInternalLink(QTextCursor const& p_cursor);
  void transformInternalLinkBack();
  void openSourceFromPosition(const QTextCursor& p_cursor);
  void editOn();
  void editOff(bool p_requestSave = true);
  void setModificationsNotSaved(bool p_value) { f_textedit->setModificationsNotSaved(p_value); }
  void notesHaveBeenSaved() { setModificationsNotSaved(false); }

signals:
  void openSourceRequested(QString);
  void saveNotesRequested();
  void modificationsNotSaved(bool);
  void notesEditOff(bool);

protected:
  void mergeFormatOnWordOrSelection(const QTextCharFormat& p_format);
  void fontChanged(const QFont& p_font);
  void fgColorChanged(const QColor& p_color);
  void bgColorChanged(const QColor& p_color);
  void list(bool p_checked, QTextListFormat::Style p_style);
  void indent(int p_delta);
  void focusInEvent(QFocusEvent* p_event) override;
  void mouseMoveEvent(QMouseEvent* p_event) override;

  QStringList m_paragraphItems;
  int m_fontsize_h1;
  int m_fontsize_h2;
  int m_fontsize_h3;
  int m_fontsize_h4;

  enum ParagraphItems {
    ParagraphStandard = 0,
    ParagraphHeading1,
    ParagraphHeading2,
    ParagraphHeading3,
    ParagraphHeading4,
    ParagraphMonospace
  };

  QPointer<QTextList> m_lastBlockList;

private:
  QTextCursor m_previousCursor;
  QString m_previousHtmlSelection;
};

#endif
