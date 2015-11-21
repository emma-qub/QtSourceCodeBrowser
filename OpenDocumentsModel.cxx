#include "OpenDocumentsModel.hxx"
#include <QDebug>

OpenDocumentsModel::OpenDocumentsModel(QObject* p_parent) :
  QStringListModel(p_parent) {
}

bool OpenDocumentsModel::insertDocument(const QString& p_fileName, const QString& p_absoluteFilePath) {
  if (documentAlreadyOpen(p_fileName, p_absoluteFilePath))
    return true;

  bool res = true;

  int rowToInsert = rowCount();
  res = res && insertRow(rowToInsert);
  res = res && setData(index(rowToInsert), p_fileName, Qt::DisplayRole);
  res = res && setData(index(rowToInsert), p_absoluteFilePath, Qt::ToolTipRole);

  return res;
}

QModelIndex OpenDocumentsModel::indexFromFile(const QString& p_fileName, const QString& p_absoluteFilePath) {
  for (int k = 0; k < rowCount(); ++k) {
    QModelIndex currentIndex = index(k);
    if (data(currentIndex, Qt::DisplayRole) == p_fileName && data(currentIndex, Qt::ToolTipRole) == p_absoluteFilePath)
      return currentIndex;
  }

  return QModelIndex();
}

bool OpenDocumentsModel::setData(QModelIndex const& p_index, QVariant const& p_value, int p_role)
{
  if (p_index.row() >= 0 && p_index.row() < stringList().size() && (p_role == Qt::ToolTipRole)) {
    m_toolTipMap.insert(p_index, p_value.toString());
    emit dataChanged(p_index, p_index, QVector<int>() << p_role);
    return true;
  }

  return QStringListModel::setData(p_index, p_value, p_role);
}

QVariant OpenDocumentsModel::data(QModelIndex const& p_index, int p_role) const {
  if (p_index.row() >= 0 && p_index.row() < stringList().size() && (p_role == Qt::ToolTipRole)) {
    return QVariant::fromValue(m_toolTipMap.value(p_index));
  }

  return QStringListModel::data(p_index, p_role);
}

bool OpenDocumentsModel::documentAlreadyOpen(QString const& p_fileName, QString const& p_absoluteFilePath) {
  for (int k = 0; k < rowCount(); ++k) {
    QModelIndex currentIndex = index(k);
    if (data(currentIndex, Qt::DisplayRole) == p_fileName && data(currentIndex, Qt::ToolTipRole) == p_absoluteFilePath)
      return true;
  }

  return false;
}
