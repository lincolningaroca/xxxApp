#include "logindialog.hpp"
#include "ui_logindialog.h"


LogInDialog::LogInDialog(QWidget *parent) :
  QDialog(parent), ui(new Ui::LogInDialog)
{
  ui->setupUi(this);
  setUp_Form();


  QObject::connect(ui->pbCancel, &QPushButton::clicked, this, &LogInDialog::reject);
  QObject::connect(ui->pbLogIn, &QPushButton::clicked, this, [&](){

    if(ui->txtUser->text().compare("usuario") == 0 &&
       ui->txtPassword->text().compare("password") == 0){
      accept();
    }
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
