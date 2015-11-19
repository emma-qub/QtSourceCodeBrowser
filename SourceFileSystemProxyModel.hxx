#ifndef SOURCEFILESYSTEMPROXYMODEL_HXX
#define SOURCEFILESYSTEMPROXYMODEL_HXX

#include <QSortFilterProxyModel>
#include <QModelIndex>

class SourceFileSystemProxyModel: public QSortFilterProxyModel {
  Q_OBJECT

public:
  explicit SourceFileSystemProxyModel(const QModelIndex& p_rootIndex, QObject* parent = nullptr);

protected:
  bool filterAcceptsRow(int p_sourceRow, QModelIndex const& p_sourceParent) const override;

private:
  void filterAux(bool& p_match, int p_sourceRow, QModelIndex const& p_sourceParent) const;
  bool matchRegExp(int p_sourceRow, QModelIndex const& p_sourceParent) const;

  QModelIndex m_rootIndex;
};

#endif // SOURCEFILESYSTEMPROXYMODEL_HXX
