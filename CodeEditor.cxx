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

#include "CodeEditor.hxx"

#include <QPainter>
#include <QTextBlock>

#include <QDebug>

CodeEditor::CodeEditor(QWidget* p_parent):
  QPlainTextEdit(p_parent),
  m_lineNumberArea(new LineNumberArea(this)),
  m_methodsPerLineMap() {

  connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
  connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
  connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

  updateLineNumberAreaWidth(0);
  highlightCurrentLine();
}

int CodeEditor::lineNumberAreaWidth() {
  int digits = 1;
  int max = qMax(1, blockCount());
  while (max >= 10) {
    max /= 10;
    ++digits;
  }

  int space = 3 + fontMetrics().width(QLatin1Char('9'))*  digits;

  return space;
}

void CodeEditor::updateLineNumberAreaWidth(int p_newBlockCount) {
  Q_UNUSED(p_newBlockCount);
  setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(QRect const& p_rect, int p_dy) {
  if (p_dy) {
    m_lineNumberArea->scroll(0, p_dy);
  } else {
    m_lineNumberArea->update(0, p_rect.y(), m_lineNumberArea->width(), p_rect.height());
  }

  if (p_rect.contains(viewport()->rect())) {
    updateLineNumberAreaWidth(0);
  }
}

void CodeEditor::setPlainText(const QString& p_text)
{
   QPlainTextEdit::setPlainText(p_text);
}

void CodeEditor::openSourceCode(const QString& p_className, const QString& p_content) {
  setPlainText(p_content);
  m_methodsPerLineMap.clear();

  QString content = toPlainText();
  QRegExp methodName("(([A-Za-z_&:]+\\s+\\**\\s*)?)?"+p_className+"[A-Za-z_]*::[A-Za-z_~]+\\([A-Za-z\\s_*&:,]*\\)(\\s+const)?");
  methodName.setCaseSensitivity(Qt::CaseInsensitive);

  int i = -1;
  while ((i = methodName.indexIn(content, i+1)) >= 0) {
    m_methodsPerLineMap.insert(i, methodName.cap(0).split("\n").last());
    i += methodName.cap(0).length();
  }

  emit methodListReady(m_methodsPerLineMap);
}

void CodeEditor::resizeEvent(QResizeEvent* p_event) {
  QPlainTextEdit::resizeEvent(p_event);

  QRect cr = contentsRect();
  m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine() {
  QList<QTextEdit::ExtraSelection> extraSelections;

  if ((textInteractionFlags() & Qt::TextSelectableByKeyboard) == Qt::TextSelectableByKeyboard) {
    QTextEdit::ExtraSelection selection;

    QColor lineColor("#E0EEF6");

    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);
  }

  setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent* p_event) {
  QPainter painter(m_lineNumberArea);
  painter.fillRect(p_event->rect(), QColor("#EFEBE7"));

  QTextBlock block = firstVisibleBlock();
  int blockNumber = block.blockNumber();
  int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
  int bottom = top + (int) blockBoundingRect(block).height();

  while (block.isValid() && top <= p_event->rect().bottom()) {
    if (block.isVisible() && bottom >= p_event->rect().top()) {
      QString number = QString::number(blockNumber + 1);
      painter.setPen(Qt::black);
      painter.drawText(0, top, m_lineNumberArea->width(), fontMetrics().height(), Qt::AlignRight, number);
    }

    block = block.next();
    top = bottom;
    bottom = top + (int) blockBoundingRect(block).height();
    ++blockNumber;
  }
}
