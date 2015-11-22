/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;

class LineNumberArea;


class CodeEditor: public QPlainTextEdit {
  Q_OBJECT

public:
  enum FileType {
    eCpp,
    eH,
    ePrivateH,
    eOtherFile
  };

  CodeEditor(QWidget* p_parent = nullptr);

  void lineNumberAreaPaintEvent(QPaintEvent* p_event);
  int lineNumberAreaWidth();
  void openSourceCode(QString const& p_className, QString const& p_content, FileType p_fileType);

protected:
  void resizeEvent(QResizeEvent* p_event) override;

private slots:
  void updateLineNumberAreaWidth(int p_newBlockCount);
  void highlightCurrentLine();
  void updateLineNumberArea(QRect const& p_rect, int p_dy);

signals:
  void methodListReady(QMap<int, QString>);

private:
  void setPlainText(const QString& p_text);
  bool methodIsInComment(int p_methodStartIndex) const;

  QWidget* m_lineNumberArea;
  QMap<int, QString> m_methodsPerLineMap;
  QVector<QPair<int, int>> m_commentsVector;
};


class LineNumberArea: public QWidget {

public:
  LineNumberArea(CodeEditor* p_editor):
    QWidget(p_editor) {
    m_codeEditor = p_editor;
  }

  QSize sizeHint() const override {
    return QSize(m_codeEditor->lineNumberAreaWidth(), 0);
  }

protected:
  void paintEvent(QPaintEvent* p_event) override {
    m_codeEditor->lineNumberAreaPaintEvent(p_event);
  }

private:
    CodeEditor* m_codeEditor;
};


#endif
