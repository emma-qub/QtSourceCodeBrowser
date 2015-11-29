#include "SourceCodeEditor.hxx"

#include <QDebug>

SourceCodeEditor::SourceCodeEditor(QWidget* p_parent):
  QWidget(p_parent),
  m_highlighters() {

  setupUi(this);

  QFont font;
  font.setFamily("Mono");
  font.setFixedPitch(true);
  font.setPointSize(10);

  // Code Editor
  m_codeEditor->setFont(font);
  m_codeEditor->setReadOnly(true);
  m_codeEditor->setTextInteractionFlags(m_codeEditor->textInteractionFlags() | Qt::TextSelectableByKeyboard);
  m_highlighters = new Highlighter(m_codeEditor->document());
  connect(m_codeEditor, SIGNAL(methodListReady(QMap<int,QString>)), this, SLOT(fillMethodsComboBox(QMap<int,QString>)));

  // Search Widget
  m_searchWidget->hide();
  m_nextToolButton->setText(">");
  connect(m_nextToolButton, SIGNAL(clicked()), this, SLOT(moveCursorToNextMatch()));
  connect(m_findLineEdit, SIGNAL(returnPressed()), this, SLOT(moveCursorToNextMatch()));
  m_previousToolButton->setText("<");
  connect(m_nextToolButton, SIGNAL(clicked()), this, SLOT(moveCursorToPreviousMatch()));
  m_regExpToolButton->setText("[]");
  m_caseSensitiveToolButton->setText("Aa");
  m_wholeWordToolButton->setText("\\b");
  m_closeToolButton->setText("X");
  connect(m_closeToolButton, SIGNAL(clicked()), m_searchWidget, SLOT(hide()));
  connect(m_findLineEdit, SIGNAL(textChanged(QString)), this, SLOT(overlineMatch(QString)));

  // Methods Combo Box
  connect(m_methodsComboBox, SIGNAL(activated(int)), this, SLOT(goToLine(int)));
}

SourceCodeEditor::~SourceCodeEditor()
{
  delete m_highlighters;
}

void SourceCodeEditor::fillMethodsComboBox(QMap<int, QString> const& p_methodsAndIndex) {
  m_methodsComboBox->clear();
  for (int index: p_methodsAndIndex.keys()) {
    m_methodsComboBox->addItem(p_methodsAndIndex.value(index), QVariant::fromValue(index));
  }
}

void SourceCodeEditor::goToLine(int p_index) {
  QTextCursor cursor = m_codeEditor->textCursor();
  m_codeEditor->moveCursor(QTextCursor::End);

  int middleLine = m_codeEditor->viewport()->height() / (2 * m_codeEditor->fontMetrics().height());

  cursor.setPosition(m_methodsComboBox->itemData(p_index).toInt());
  m_codeEditor->setTextCursor(cursor);

  for (int k = 0; k < middleLine; ++k) {
    m_codeEditor->moveCursor(QTextCursor::Up);
  }
  for (int k = 0; k < middleLine; ++k) {
    m_codeEditor->moveCursor(QTextCursor::Down);
  }
}

void SourceCodeEditor::overlineMatch(const QString& p_match) {
  Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive;
  if (m_caseSensitiveToolButton->isChecked()) {
    caseSensitivity = Qt::CaseSensitive;
  }

  QString match = p_match;
  if (m_wholeWordToolButton->isChecked()) {
    match = "\\b"+match+"\\b";
  }

  QRegExp regExp(match, caseSensitivity);
  QList<QTextEdit::ExtraSelection> extraSelections;
  m_codeEditor->moveCursor(QTextCursor::Start);
  QColor markColor = QColor(Qt::yellow).lighter(150);

  m_matchPositions.clear();
  m_currentMatchPosition = -1;

  int previousCursorPosition = m_codeEditor->textCursor().position();

  while (m_codeEditor->find(regExp)) {
    QTextCursor cursor = m_codeEditor->textCursor();

    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(markColor);
    selection.cursor = m_codeEditor->textCursor();
    extraSelections.append(selection);

    m_matchPositions << cursor.position();
  }

  m_codeEditor->setExtraSelections(extraSelections);

  for (int currentPosition: m_matchPositions) {
    if (previousCursorPosition < currentPosition) {
      m_currentMatchPosition = qMax(0, currentPosition-1);
      moveCursorToNextMatch();
    }
  }
}

void SourceCodeEditor::moveCursorToNextMatch() {
  m_currentMatchPosition = (m_currentMatchPosition+1)%m_matchPositions.size();

  int currentPosition = m_matchPositions.at(m_currentMatchPosition);
  QTextCursor cursor = m_codeEditor->textCursor();
  cursor.setPosition(currentPosition);
  cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, m_findLineEdit->text().length());
  m_codeEditor->setTextCursor(cursor);
}

void SourceCodeEditor::moveCursorToPreviousMatch() {
  m_currentMatchPosition = (m_currentMatchPosition-1)%m_matchPositions.size();

  int currentPosition = m_matchPositions.at(m_currentMatchPosition);
  QTextCursor cursor = m_codeEditor->textCursor();
  cursor.setPosition(currentPosition);
  cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, m_findLineEdit->text().length());
  m_codeEditor->setTextCursor(cursor);
}

void SourceCodeEditor::openSourceCode(QString const& p_content, CodeEditor::FileType p_fileType) {
  m_codeEditor->openSourceCode(p_content, p_fileType);
}

void SourceCodeEditor::setFocusToSourceEditor() {
  m_codeEditor->setFocus();
}

void SourceCodeEditor::findTextInSourceEditor() {
  m_searchWidget->show();
  m_findLineEdit->setFocus();

  QTextCursor cursor = m_codeEditor->textCursor();
  cursor.select(QTextCursor::WordUnderCursor);
  QString selectedText = cursor.selectedText();
  if (QRegularExpression("\\w+").match(selectedText).hasMatch()) {
    m_findLineEdit->setText(selectedText);
  }
}

void SourceCodeEditor::clear() {
  m_codeEditor->clear(); m_methodsComboBox->clear();
}

