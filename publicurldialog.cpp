#include "publicurldialog.hpp"
#include "ui_publicurldialog.h"

PublicUrlDialog::PublicUrlDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::PublicUrlDialog)
{
  ui->setupUi(this);
}

PublicUrlDialog::~PublicUrlDialog()
{
  delete ui;
}
