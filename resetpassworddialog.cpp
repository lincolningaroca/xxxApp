#include "resetpassworddialog.hpp"
#include "ui_resetpassworddialog.h"

#include <helperdatabase/helperdb.hpp>
#include <QMessageBox>

ResetPasswordDialog::ResetPasswordDialog(QWidget *parent)
  :QDialog(parent), ui(new Ui::ResetPasswordDialog){

  ui->setupUi(this);

  ui->stackedWidget->setCurrentIndex(0);

  QObject::connect(ui->btnValidarUsuario, &QPushButton::clicked, this, [&](){
      SW::HelperDataBase_t helper;
      if(helper.userExists(ui->txtUser->text().simplified())){
          QMessageBox::information(this, qApp->applicationName(), "El usuario existe");
        }else{
          QMessageBox::information(this, qApp->applicationName(), "El usuario no existe");
        }
    });
}

ResetPasswordDialog::~ResetPasswordDialog(){
  delete ui;
}
