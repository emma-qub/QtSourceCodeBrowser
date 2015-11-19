#include "SourceFileSystemProxyModel.hxx"

#include <QDebug>

SourceFileSystemProxyModel::SourceFileSystemProxyModel(QModelIndex const& p_rootIndex, QObject *parent) :
  QSortFilterProxyModel(parent),
  m_rootIndex(p_rootIndex)
{
}

bool SourceFileSystemProxyModel::filterAcceptsRow(
  int p_sourceRow, QModelIndex const& p_sourceParent) const
{
  QModelIndex index0 = sourceModel()->index(p_sourceRow, 0, p_sourceParent);

  if (sourceModel()->hasChildren(index0))
  {
    bool matchRegExp = false;
    filterAux(matchRegExp, p_sourceRow, p_sourceParent);
    return matchRegExp;
  }

  return matchRegExp(p_sourceRow, p_sourceParent);
}

void SourceFileSystemProxyModel::filterAux(bool& p_match, int p_sourceRow, QModelIndex const& p_sourceParent) const
{
  QModelIndex index0 = sourceModel()->index(p_sourceRow, 0, p_sourceParent);

  if (matchRegExp(p_sourceRow, p_sourceParent))
  {
    p_match = true;
    return;
  }
  else
  {
    for (int k = 0; k < sourceModel()->rowCount(index0); ++k)
    {
      filterAux(p_match, k, index0);
      if (p_match)
      {
        return;
      }
    }
  }
}

bool SourceFileSystemProxyModel::matchRegExp(int p_sourceRow, QModelIndex const& p_sourceParent) const
{
  QModelIndex index = sourceModel()->index(p_sourceRow, 0, p_sourceParent);
  return (sourceModel()->data(index).toString().contains(filterRegExp()));
}
