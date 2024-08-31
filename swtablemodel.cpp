#include "swtablemodel.hpp"
#include <QSqlRecord>
#include "util/helper.hpp"

SWTableModel::SWTableModel(QObject *parent, QSqlDatabase db)
  : QSqlTableModel{parent, db}
{

}

QVariant SWTableModel::data(const QModelIndex& index, int role) const{

  // if(index.column() == 1 && role == Qt::DisplayRole){
  //     QSqlRecord record = QSqlTableModel::record(index.row());
  //     auto encryptedText = record.value(1).toString();

  //     auto decryptedData = SW::Helper_t::decrypt(encryptedText);
  //     return decryptedData;

  //   }
  if(role == Qt::DisplayRole && (index.column() == 1 || index.column() == 2) ){
      // QSqlRecord record = QSqlTableModel::record(index.row());
      // auto encryptedText = record.value(1).toString();

      auto decryptedData = QSqlTableModel::data(index, role);
      return SW::Helper_t::decrypt(decryptedData.toString());

    }
  return QSqlTableModel::data(index, role);

}
