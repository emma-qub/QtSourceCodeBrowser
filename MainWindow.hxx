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

private:
  BrowseSourceWidget* m_centralWidget;

  QAction* m_editNotesOnHorizontalAction;
  QAction* m_editNotesOnVerticalAction;
  QAction* m_editNotesOffAction;
};

#endif // MAINWINDOW_HXX
