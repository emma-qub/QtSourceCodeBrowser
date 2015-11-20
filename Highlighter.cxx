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

#include "Highlighter.hxx"

#include <QApplication>

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    qmacroFormat.setForeground(Qt::darkBlue);
    rule.pattern = QRegExp("\\bQ[A-Z0-9_]+\\b");
    rule.format = qmacroFormat;
    highlightingRules.append(rule);

    labelFormat.setForeground(Qt::darkRed);
    rule.pattern = QRegExp("[A-Za-z_]+:");
    rule.format = labelFormat;
    highlightingRules.append(rule);

    noLabelFormat.setForeground(QColor("#444"));
    rule.pattern = QRegExp("[A-Za-z_]+::");
    rule.format = noLabelFormat;
    highlightingRules.append(rule);

    signalSlotMacroFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("(public|protected|private)\\s+(Q_SIGNALS|Q_SLOTS)");
    rule.format = signalSlotMacroFormat;
    highlightingRules.append(rule);

    keywordFormat.setForeground(Qt::darkYellow);
    QStringList keywordPatterns;
    keywordPatterns << "\\balignas\\b" << "\\balignof\\b" << "\\basm\\b"
                    << "\\bauto\\b" << "\\bbool\\b" << "\\bbreak\\b"
                    << "\\bcase\\b" << "\\bcatch\\b" << "\\bchar\\b"
                    << "\\bchar16_t\\b" << "\\bchar32_t\\b" << "\\bclass\\b"
                    << "\\bconst\\b" << "\\bconstexpr\\b" << "\\bcontinue\\b"
                    << "\\bdecltype\\b" << "\\bdefault\\b" << "\\bdelete\\b"
                    << "\\bdo\\b" << "\\bdouble\\b" << "\\bdynamic_cast\\b"
                    << "\\belse\\b" << "\\bemit\\b" << "\\benum\\b"
                    << "\\bexplicit\\b" << "\\bexport\\b" << "\\bextern\\b"
                    << "\\bfalse\\b" << "\\bfloat\\b" << "\\bfor\\b"
                    << "\\bforeach\\b" << "\\bforever\\b" << "\\bfriend\\b"
                    << "\\bgoto\\b" << "\\bif\\b" << "\\binline\\b"
                    << "\\bint\\b" << "\\blong\\b" << "\\bmutable\\b"
                    << "\\bnamespace\\b" << "\\bnew\\b" << "\\bnoexcept\\b"
                    << "\\bnullptr\\b" << "\\boperator\\b"<< "\\bprivate\\b"
                    << "\\bprotected\\b" << "\\bpublic\\b" << "\\bregister\\b"
                    << "\\breinterpret_cast\\b" << "\\breturn\\b"
                    << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
                    << "\\bsizeof\\b" << "\\bslots\\b" << "\\bstatic\\b"
                    << "\\bstatic_assert\\b" << "\\bstatic_cast\\b" << "\\bstruct\\b"
                    << "\\bswitch\\b" << "\\btemplate\\b" << "\\bthis\\b"
                    << "\\bthread_local\\b" << "\\bthrow\\b" << "\\btrue\\b"
                    << "\\btry\\b" << "\\btypedef\\b" << "\\btypeid\\b"
                    << "\\btypename\\b" << "\\bunion\\b" << "\\bunsigned\\b"
                    << "\\busing\\b" << "\\bvirtual\\b" << "\\bvoid\\b"
                    << "\\bvolatile\\b" << "\\bwchar_t\\b" << "\\bwhile\\b"
                    << "\\b__attribute__\\b" << "\\b__thread\\b" << "\\b__typeof__\\b";

    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    digitsFormat.setForeground(Qt::darkBlue);
    rule.pattern = QRegExp("\\d+");
    rule.format = digitsFormat;
    highlightingRules.append(rule);

    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);

    defineFormat.setForeground(Qt::darkBlue);
    rule.pattern = QRegExp("^#define\\s+[A-Z_]+");
    rule.format = defineFormat;
    highlightingRules.append(rule);

    includeFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("^#include\\s+.+");
    rule.format = includeFormat;
    highlightingRules.append(rule);

    macroFormat.setForeground(Qt::darkBlue);
    rule.pattern = QRegExp("^#[^\\s]+");
    rule.format = macroFormat;
    highlightingRules.append(rule);

    qAndDPointersFormat.setForeground(Qt::darkRed);
    rule.pattern = QRegExp("\\bq\\b | \\bd\\b");
    rule.format = qAndDPointersFormat;
    highlightingRules.append(rule);

    semicolonFormat.setForeground(QColor("#444"));
    rule.pattern = QRegExp(":+");
    rule.format = semicolonFormat;
    highlightingRules.append(rule);

    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::darkGreen);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");

    GetMemberVariables();
}

void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0) {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}

void Highlighter::GetMemberVariables()
{

}
