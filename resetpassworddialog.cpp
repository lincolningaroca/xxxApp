#include "resetpassworddialog.hpp"
#include "ui_resetpassworddialog.h"

ResetPasswordDialog::ResetPasswordDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ResetPasswordDialog)
{
  ui->setupUi(this);
}

ResetPasswordDialog::~ResetPasswordDialog()
{
  delete ui;
}
