#include "swtablemodel.hpp"

#include <QSqlRecord>

#include "util/helper.hpp"

SWTableModel::SWTableModel(QObject *parent, const QSqlDatabase &db)
  : QSqlTableModel{parent, db}
{

}

QVariant SWTableModel::data(const QModelIndex& index, int role) const{

  if(role == Qt::DisplayRole && (index.column() == 1 || index.column() == 2) ){

      const auto decryptedData = QSqlTableModel::data(index, role);
      return SW::Helper_t::decrypt(decryptedData.toString());

    }
  return QSqlTableModel::data(index, role);

}

QVariant SWTableModel::headerData(int section, Qt::Orientation orientation, int role) const{

  Q_UNUSED(section)
  if(orientation == Qt::Vertical && role == Qt::DisplayRole){
    return QVariant();
  }
  if(role == Qt::SizeHintRole){
    return QSize(10,0);
  }
  return QSqlTableModel::headerData(section, orientation, role);

}
