#include "SourceCodeEditor.hxx"

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
  m_closeToolButton->setText("X");
  m_nextToolButton->setText(">");
  m_previousToolButton->setText("<");
  m_regExpToolButton->setText("[]");
  m_wholeWordToolButton->setText("\\b");
  m_caseSensitiveToolButton->setText("Aa");
  connect(m_closeToolButton, SIGNAL(clicked()), m_searchWidget, SLOT(hide()));
  connect(m_searchLineEdit, SIGNAL(textChanged(QString)), this, SLOT(overlineMatch(QString)));

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
  QRegExp regExp(p_match);
  QList<QTextEdit::ExtraSelection> extraSelections;
  m_codeEditor->moveCursor(QTextCursor::Start);
  QColor markColor("#E99B63");

  while (m_codeEditor->find(regExp)) {
    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(markColor);
    selection.cursor = m_codeEditor->textCursor();
    extraSelections.append(selection);
  }

  m_codeEditor->setExtraSelections(extraSelections);
}

void SourceCodeEditor::openSourceCode(QString const& p_content, CodeEditor::FileType p_fileType) {
  m_codeEditor->openSourceCode(p_content, p_fileType);
}

void SourceCodeEditor::setFocusToSourceEditor() {
  m_codeEditor->setFocus();
}

void SourceCodeEditor::findTextInSourceEditor() {
  m_searchWidget->show();
  m_searchLineEdit->setFocus();

  QTextCursor cursor = m_codeEditor->textCursor();
  cursor.select(QTextCursor::WordUnderCursor);
  m_searchLineEdit->setText(cursor.selectedText());
}

void SourceCodeEditor::clear() {
  m_codeEditor->clear(); m_methodsComboBox->clear();
}

