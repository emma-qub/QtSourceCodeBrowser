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

private:
  BrowseSourceWidget* m_centralWidget;

  QAction* m_editNotesOnHorizontalAction;
  QAction* m_editNotesOnVerticalAction;
  QAction* m_editNotesOffAction;

  QAction* m_saveAction;
  QAction* m_closeAction;
};

#endif // MAINWINDOW_HXX
