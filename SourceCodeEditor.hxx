#ifndef SOURCECODEEDITOR_H
#define SOURCECODEEDITOR_H

#include <QWidget>

#include "ui_SourceCodeEditor.h"
#include "Highlighter.hxx"

class SourceCodeEditor: public QWidget, protected Ui::SourceCodeEditor {
  Q_OBJECT

public:
  explicit SourceCodeEditor(QWidget* p_parent = nullptr);
  virtual ~SourceCodeEditor();

  void openSourceCode(QString const& p_content, CodeEditor::FileType p_fileType);
  void setFocusToSourceEditor();

  void findTextInSourceEditor();

public slots:
  void clear();

protected slots:
  void fillMethodsComboBox(const QMap<int, QString>& p_methodsAndIndex);
  void goToLine(int p_index);
  void overlineMatch(QString const& p_match);
  void moveCursorToNextMatch();
  void moveCursorToPreviousMatch();

private:
  Highlighter* m_highlighters;
  QVector<int> m_matchPositions;
  int m_currentMatchPosition;
};

#endif // SOURCECODEEDITOR_H
