#include "SourceFileSystemModel.hxx"

#include <QDebug>

SourceFileSystemModel::SourceFileSystemModel(QObject* p_parent):
  QFileSystemModel(p_parent)
{
  setFilter(QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot);
  setNameFilterDisables(false);
  setNameFilters(QStringList() << "*.cpp" << "*.h");
}

QVariant SourceFileSystemModel::data( const QModelIndex& index, int role ) const {
  if (role == Qt::DecorationRole)
  {
    QFileInfo info = SourceFileSystemModel::fileInfo(index);

    if(info.isFile())
    {
      if(info.suffix() == "cpp")
        return QPixmap("../QtSourceCodeBrowser/icons/cppFile.png");
      else if(info.suffix() == "h")
        return QPixmap("../QtSourceCodeBrowser/icons/hFile.png");
    }
  }
  return QFileSystemModel::data(index, role);
}
