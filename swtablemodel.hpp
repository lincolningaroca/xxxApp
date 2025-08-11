#pragma once

#include <QSqlTableModel>

struct SWTableModel : public QSqlTableModel{

  Q_OBJECT
public:
  explicit SWTableModel(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase());

  // QAbstractItemModel interface
public:
  QVariant data(const QModelIndex& index, int role) const override;

  // QAbstractItemModel interface
public:
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
};

