#include "logindialog.hpp"
#include "ui_logindialog.h"
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>


LogInDialog::LogInDialog(QWidget *parent) :
  QDialog(parent), ui(new Ui::LogInDialog), db_{QSqlDatabase::database("xxxConection")}
{
  ui->setupUi(this);
  setUp_Form();


  QObject::connect(ui->pbCancel, &QPushButton::clicked, this, &LogInDialog::reject);
  QObject::connect(ui->pbLogIn, &QPushButton::clicked, this, [&](){

    if(!logIn()){
      QMessageBox::warning(this, qApp->applicationName(), "Los datos que ingreso son incorrectos\n"
                                                          "vuelva a intentarlo.");
      ui->txtUser->selectAll();
      ui->txtUser->setFocus(Qt::OtherFocusReason);

      return;
    }

//    qInfo() << getUser_id();
    accept();
  });
  //coneccion del boton de mas opciones
  ui->btnOtherOptions->setCheckable(true);
  setOptionsToComboBox(0);
  QObject::connect(ui->btnOtherOptions, &QToolButton::toggled, ui->widget, [&](){
    if(ui->btnOtherOptions->isChecked()){
      ui->btnOtherOptions->setIcon(QIcon(":/img/up.png"));
      ui->widget->setVisible(true);
      setStateControls(true);
      ui->txtNewUser->setFocus(Qt::OtherFocusReason);
      ui->btnOtherOptions->setToolTip("<p>"
                                      "<span> Volver a Inicio de sesión!"
                                      "</span>"
                                      "</p>");
    }else{
      ui->btnOtherOptions->setIcon(QIcon(":/img/down.png"));
      ui->widget->setVisible(false);
      ui->groupBox->setEnabled(true);
      setStateControls(false);
      ui->txtUser->setFocus(Qt::OtherFocusReason);
      ui->btnOtherOptions->setToolTip("<p>"
                                      "<span> Crear un nuevo usuario y/o<br>"
                                      "restablecer clave o password!"
                                      "</span>"
                                      "</p>");
    }
  });

  //coneccion de combo box metodo de recuperacion
  QObject::connect(ui->cboRestoreType, &QComboBox::activated, this, LogInDialog::setOptionsToComboBox);
  ui->widget->hide();

}

LogInDialog::~LogInDialog()
{
  delete ui;
}

uint32_t LogInDialog::getUser_id() noexcept
{
  QSqlQuery qry{db_};

  uint32_t ret_value{0};
  [[maybe_unused]] auto res = qry.prepare("SELECT user_id FROM users WHERE user = ?");
  qry.addBindValue(ui->txtUser->text());
  if(!qry.exec())
    return ret_value;
  qry.first();
  ret_value= qry.value(0).toUInt();
  return ret_value;
//  id=1;
}

void LogInDialog::setUp_Form() noexcept
{
  ui->txtPassword->setEchoMode(QLineEdit::Password);
  ui->txtUser->setPlaceholderText("Usuario");
  ui->txtPassword->setPlaceholderText("Clave o password");

  ui->btnOtherOptions->setIcon(QIcon(":/img/down.png"));
  layout()->setSizeConstraint(QLayout::SetFixedSize);

  ui->btnOtherOptions->setToolTip("<p>"
                                  "<span> Crear un nuevo usuario y/o<br>"
                                  "restablecer clave o password!"
                                  "</span>"
                                  "</p>");
  ui->btnPublic->setToolTip("<p>"
                            "<cite> Ingressar sin usuario:<br>"
                            "ver direcciones publicas."
                            "</cite>"
                            "</p>");


  //new user section
  ui->txtNewPassword->setPlaceholderText("Ingrese clave o password");
  ui->txtNewPassword->setEchoMode(QLineEdit::Password);
  ui->txtNewUser->setPlaceholderText("Ingrese un nombre de usuario");
  ui->txtRePassword->setPlaceholderText("Vuelva a ingresar su clave o password");

  //set the combo box options
  ui->cboRestoreType->addItem("Pregunta secreta");
  ui->cboRestoreType->addItem("Pin numérico");

}

void LogInDialog::setStateControls(bool op) noexcept
{
  ui->txtPassword->setDisabled(op);
  ui->txtUser->setDisabled(op);
  ui->pbCancel->setDisabled(op);
  ui->pbLogIn->setDisabled(op);
  ui->btnPublic->setDisabled(op);

}

void LogInDialog::setOptionsToComboBox(int index) noexcept
{
  if(index == 0){
    ui->lineEdit->setPlaceholderText("Ingrese una pregunta!");
    ui->lineEdit_2->setPlaceholderText("Ingrese su respuesta!");
    //    ui->lineEdit->setFocus(Qt::OtherFocusReason);

  }else{
    ui->lineEdit->setPlaceholderText("Ingrese PIN numérico de 4 cifras!");
    ui->lineEdit_2->setPlaceholderText("Vuelva a ingresar el el número");
    //    ui->lineEdit->setFocus(Qt::OtherFocusReason);
  }
}

bool LogInDialog::logIn() const noexcept
{
  QSqlQuery qry{db_};
  [[maybe_unused]] auto res = qry.prepare("SELECT COUNT(*) FROM users WHERE user = ? AND password = ?");
  qry.addBindValue(ui->txtUser->text());
  qry.addBindValue(ui->txtPassword->text());
  if(!qry.exec())
    return false;
  if(qry.first() && qry.value(0).toInt() == 1)
    return true;
  return false;


}

