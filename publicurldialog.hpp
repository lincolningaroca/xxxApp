#pragma once

#include <QDialog>

namespace Ui {
  class PublicUrlDialog;
}

class PublicUrlDialog : public QDialog
{
  Q_OBJECT

public:
  explicit PublicUrlDialog(QWidget *parent = nullptr);
  ~PublicUrlDialog();

private:
  Ui::PublicUrlDialog *ui;
};

