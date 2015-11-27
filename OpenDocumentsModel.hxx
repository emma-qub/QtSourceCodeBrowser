#ifndef OPENDOCUMENTSMODEL_HXX
#define OPENDOCUMENTSMODEL_HXX

#include <QStringListModel>

class OpenDocumentsModel: public QStringListModel {
  Q_OBJECT

public:
  explicit OpenDocumentsModel(QObject* p_parent = nullptr);

  bool insertDocument(QString const& p_fileName, QString const& p_absoluteFilePath);
  QModelIndex indexFromFile(QString const& p_absoluteFilePath);

  void closeOpenDocument(QString const& p_absoluteFilePath);
  void closeAllOpenDocument();

  bool setData(QModelIndex const& p_index, QVariant const& p_value, int p_role = Qt::DisplayRole) override;
  QVariant data(QModelIndex const& p_index, int p_role = Qt::EditRole) const override;

  QModelIndex getIndexFromFileNameAndAbsolutePath(QString const& p_fileName, QString const& p_absoluteFilePath) const;

private:
  bool documentAlreadyOpen(QString const& p_fileName, QString const& p_absoluteFilePath);
  bool removeRow(int p_row, QModelIndex const& p_parent = QModelIndex()) { return QStringListModel::removeRow(p_row, p_parent); }

  QMap<QModelIndex, QString> m_toolTipMap;
};

#endif // OPENDOCUMENTSMODEL_HXX
