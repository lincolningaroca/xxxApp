#pragma once

#include <QDialog>

namespace Ui {
  class AcercaDeDialog;
}

class AcercaDeDialog : public QDialog
{
  Q_OBJECT

public:
  explicit AcercaDeDialog(QWidget *parent = nullptr);
  ~AcercaDeDialog();

private:
  Ui::AcercaDeDialog *ui;
};

