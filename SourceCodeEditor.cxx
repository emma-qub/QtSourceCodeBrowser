#include "SourceCodeEditor.hxx"

SourceCodeEditor::SourceCodeEditor(QWidget* p_parent):
  QWidget(p_parent),
  m_highlighters() {

  setupUi(this);

  QFont font;
  font.setFamily("Mono");
  font.setFixedPitch(true);
  font.setPointSize(10);

  m_codeEditor->setFont(font);
  m_codeEditor->setReadOnly(true);
  m_codeEditor->setTextInteractionFlags(m_codeEditor->textInteractionFlags() | Qt::TextSelectableByKeyboard);
  m_highlighters = new Highlighter(m_codeEditor->document());

  connect(m_codeEditor, SIGNAL(methodListReady(QMap<int,QString>)), this, SLOT(fillMethodsComboBox(QMap<int,QString>)));
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

void SourceCodeEditor::openSourceCode(QString const& p_content, CodeEditor::FileType p_fileType) {
  m_codeEditor->openSourceCode(p_content, p_fileType);
}

void SourceCodeEditor::setFocusToSourceEditor() {
  m_codeEditor->setFocus();
}

void SourceCodeEditor::clear() {
  m_codeEditor->clear(); m_methodsComboBox->clear();
}

