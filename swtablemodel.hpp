#pragma once

#include <QSqlTableModel>

class SWTableModel : public QSqlTableModel
{
  Q_OBJECT
public:
  explicit SWTableModel(QObject *parent = nullptr);
};

