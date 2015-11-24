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

private:
  BrowseSourceWidget* m_centralWidget;
};

#endif // MAINWINDOW_HXX
