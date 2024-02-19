#include "acercadedialog.hpp"
#include "ui_acercadedialog.h"

AcercaDeDialog::AcercaDeDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AcercaDeDialog)
{
  ui->setupUi(this);
}

AcercaDeDialog::~AcercaDeDialog()
{
  delete ui;
}
