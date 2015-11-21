#ifndef SOURCEFILESYSTEMMODEL_HXX
#define SOURCEFILESYSTEMMODEL_HXX

#include <QFileSystemModel>

class SourceFileSystemModelData;

class SourceFileSystemModel: public QFileSystemModel {
  Q_OBJECT

public:
  SourceFileSystemModel(QObject* p_parent = nullptr);

protected:
  QVariant data(const QModelIndex& index, int role) const override;

private:
  QStringList m_emptyDirectories;
};

#endif // SOURCEFILESYSTEMMODEL_HXX
