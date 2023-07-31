#pragma once

#include <QDialog>

namespace Ui {
  class ResetPasswordDialog;
}

class ResetPasswordDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ResetPasswordDialog(QWidget *parent = nullptr);
  ~ResetPasswordDialog();

private:
  Ui::ResetPasswordDialog *ui;
};

