#pragma once

#include <QSqlTableModel>

struct SWTableModel : public QSqlTableModel{

  Q_OBJECT
public:
  explicit SWTableModel(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase());

  // QAbstractItemModel interface
public:
  QVariant data(const QModelIndex& index, int role) const override;
};

