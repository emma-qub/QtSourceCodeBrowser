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

protected slots:
  void cleanDirectory(const QString& p_directoryName);
  void computeDirectoriesToBeRemoved(const QString& p_rootDirectoryName);

private:
  bool computeDirectoriesToRemove(QString const& p_directoryName, QStringList& emptyDirectories);

  QStringList m_emptyDirectories;
};

#endif // SOURCEFILESYSTEMMODEL_HXX
