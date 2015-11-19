#include <QApplication>
#include <QSettings>


#include "MainWindow.hxx"


void checkScansDirectoryExists(void){
  QSettings settings("ValentinMicheletINC", "QtSourceBrowser");

  if (settings.value("SourceDirectory").toString().isEmpty()) {
//    bool ok;
//    QString sourceDirectory;
//    DirectoryDialog browseDialog;
//    do {
//      ok = (browseDialog.exec() == QDialog::Accepted);
//      sourceDirectory = browseDialog.getScansDirectory();
//    } while (!ok || sourceDirectory.isEmpty());
//    settings.setValue("SourceDirectory", sourceDirectory);

    QString sourceDirectory("/home/valentin/Documents/CompiledSoftware/qt-everywhere-opensource-src-5.5.1");
    settings.setValue("SourceDirectory", sourceDirectory);
  }
}


int main(int argc, char** argv) {
  QApplication app(argc, argv);

  checkScansDirectoryExists();

  MainWindow window;
  window.show();

  return app.exec();
}

