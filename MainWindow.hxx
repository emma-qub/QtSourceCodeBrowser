#ifndef MAINWINDOW_HXX
#define MAINWINDOW_HXX

#include <QMainWindow>

class BrowseSourceWidget;

class MainWindow: public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget* p_parent = nullptr);

protected:
  void closeEvent(QCloseEvent* p_event) override;

protected slots:
  void showHorizontal();
  void showVertical();
  void hideNotesTextEdit();
  void enableSplit();
  void disableSplit();
  void updateFileMenu(QString const& p_fileName);
  void enableSaveAction(bool p_value, bool p_allSave);
  void enableCloseAction(bool p_value);

private:
  BrowseSourceWidget* m_centralWidget;

  QAction* m_editNotesOnHorizontalAction;
  QAction* m_editNotesOnVerticalAction;
  QAction* m_editNotesOffAction;

  QAction* m_saveAction;
  QAction* m_saveAndCloseEditAction;
  QAction* m_saveAllAction;

  QAction* m_closeAction;
  QAction* m_closeAllAction;
};

#endif // MAINWINDOW_HXX
