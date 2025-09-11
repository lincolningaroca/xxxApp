#pragma once

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
  class PublicUrlDialog;
}

class PublicUrlDialog : public QDialog{
  Q_OBJECT

public:
  explicit PublicUrlDialog(QWidget *parent = nullptr);
  ~PublicUrlDialog();

private:
  Ui::PublicUrlDialog *ui;

  QHash<uint32_t, QString> data_{};
  const QSqlDatabase db_{};

  void loadDataComboBox();
  void loadDataTableView();
};

