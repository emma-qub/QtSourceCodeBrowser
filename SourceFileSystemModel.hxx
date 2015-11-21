#ifndef SOURCEFILESYSTEMMODEL_HXX
#define SOURCEFILESYSTEMMODEL_HXX

#include <QFileSystemModel>


class SourceFileSystemModel: public QFileSystemModel {
  Q_OBJECT

public:
  SourceFileSystemModel(QObject* p_parent = nullptr);

protected:
  QVariant data(const QModelIndex& p_index, int p_role) const override;

private:
  QStringList m_emptyDirectories;
};

#endif // SOURCEFILESYSTEMMODEL_HXX
