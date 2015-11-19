#include "SourceFileSystemModel.hxx"

#include <QDebug>

SourceFileSystemModel::SourceFileSystemModel(QObject* p_parent):
  QFileSystemModel(p_parent)
{
  setFilter(QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot);
  setNameFilterDisables(false);
  setNameFilters(QStringList() << "*.cpp" << "*.h");
}

void SourceFileSystemModel::cleanDirectory(QString const& p_directoryName)
{
  qDebug() << "Directory to clean:" << p_directoryName;
  QStringList directoriesToRemove;
  computeDirectoriesToRemove(p_directoryName, directoriesToRemove);
  qDebug() << directoriesToRemove;

  for (QString const& directoryToRemove: directoriesToRemove)
  {
    QModelIndex indexToRemove = index(directoryToRemove);
    qDebug() << indexToRemove.data() << indexToRemove.row() << indexToRemove.parent().data();
    qDebug() << removeRow(indexToRemove.row(), indexToRemove.parent());
    //if (index(p_directoryName).child(0, 0) == QModelIndex())
    //  removeRow(index(p_directoryName).row(), index(p_directoryName).parent());
  }
  qDebug() << "";

  //computeDirectoriesToRemove(p_directoryName, m_emptyDirectories);
//  qDebug() << "Try to clean" << p_directoryName;
//  QDir directory(p_directoryName);
//  for (QString const& subDirectoryName: directory.entryList(QDir::AllDirs | QDir::NoDotAndDotDot))
//  {
//    if (m_emptyDirectories.contains(p_directoryName+QDir::separator()+subDirectoryName))
//    {
////      qDebug() << "Has to remove:" << subDirectoryName;
//      //removeRow();
//    }
//  }
}

void SourceFileSystemModel::computeDirectoriesToBeRemoved(QString const& p_rootDirectoryName)
{
//  qDebug() << "COMPUTE DIRECTORIES TO BE REMOVE" << p_rootDirectoryName;
  computeDirectoriesToRemove(p_rootDirectoryName, m_emptyDirectories);
//  qDebug() << m_emptyDirectories;
}

bool SourceFileSystemModel::computeDirectoriesToRemove(QString const& p_directoryName, QStringList& emptyDirectories)
{
  QDir directory(p_directoryName);
  QStringList sourcesList = directory.entryList(QStringList() << "*.cpp" << "*.h", QDir::Files | QDir::NoDotAndDotDot);
  QStringList directoriesList = directory.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);

  if (!sourcesList.isEmpty())
  {
    return true;
  }
  else
  {
    if (directoriesList.isEmpty())
    {
      return false;
    }
    else
    {
//      qDebug() << directoriesList;
      for (QString subDirectory: directoriesList)
      {
        QString subDirectoryName = p_directoryName+QDir::separator()+subDirectory;
        if (computeDirectoriesToRemove(subDirectoryName, emptyDirectories))
        {
          return true;
        }
      }
      bool hasToBeInserted = true;
      for (int k = 0; k < emptyDirectories.count(); ++k)
      {
        QString const& emptyDirectory = emptyDirectories.at(k);
        if (emptyDirectory.startsWith(p_directoryName))
        {
          if (!emptyDirectories.contains(p_directoryName))
            emptyDirectories.replace(k, p_directoryName);
          else
            emptyDirectories.replace(k, "");
          hasToBeInserted = false;
        }
        else if (p_directoryName.startsWith(emptyDirectory))
        {
          hasToBeInserted = false;
          break;
        }
      }
//      qDebug() << p_directoryName;
//      qDebug() << emptyDirectories;
      if (hasToBeInserted && !emptyDirectories.contains(p_directoryName))
        emptyDirectories << p_directoryName;
//      qDebug() << "";
      return false;
    }
  }
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
