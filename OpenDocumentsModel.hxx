#ifndef OPENDOCUMENTSMODEL_HXX
#define OPENDOCUMENTSMODEL_HXX

#include <QStringListModel>

class OpenDocumentsModel: public QStringListModel {
  Q_OBJECT

public:
  explicit OpenDocumentsModel(QObject* p_parent = nullptr);

  bool insertDocument(QString const& p_fileName, QString const& p_absoluteFilePath);
  QModelIndex indexFromFile(QString const& p_fileName, QString const& p_absoluteFilePath);

  bool setData(QModelIndex const& p_index, QVariant const& p_value, int p_role = Qt::DisplayRole) override;
  QVariant data(QModelIndex const& p_index, int p_role = Qt::EditRole) const override;

private:
  bool documentAlreadyOpen(QString const& p_fileName, QString const& p_absoluteFilePath);

  QMap<QModelIndex, QString> m_toolTipMap;
};

#endif // OPENDOCUMENTSMODEL_HXX
