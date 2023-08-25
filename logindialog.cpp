#include "logindialog.hpp"
#include "ui_logindialog.h"
#include <util/helper.hpp>
#include <QMessageBox>
#include "resetpassworddialog.hpp"
#include <QDebug>


LogInDialog::LogInDialog(QWidget *parent) :
  QDialog(parent), ui(new Ui::LogInDialog){

  ui->setupUi(this);
  setUp_Form();
  readSettings();

  QObject::connect(ui->pbCancel, &QPushButton::clicked, this, &LogInDialog::reject_form);

  QObject::connect(ui->pbLogIn, &QPushButton::clicked, this, [&](){

      if(!helperdb_.logIn(ui->txtUser->text().simplified(), ui->txtPassword->text().simplified())){
          QMessageBox::warning(this, qApp->applicationName(), "<span>"
                                                              "<strong>"
                                                              "Los datos que ingreso son incorrectos\n"
                                                              "vuelva a intentarlo."
                                                              "</strong>"
                                                              "</span>");
          ui->txtUser->selectAll();
          ui->txtUser->setFocus(Qt::OtherFocusReason);

          return;
        }

      //    qInfo() << getUser_id();
      userName_ = ui->txtUser->text();
      accept();
    });

  //coneccion del boton de mas opciones
  ui->btnOtherOptions->setCheckable(true);

  QObject::connect(ui->btnOtherOptions, &QToolButton::toggled, ui->widget, [&](){
      if(ui->btnOtherOptions->isChecked()){
          ui->btnOtherOptions->setIcon(QIcon(":/img/up.png"));
          ui->widget->setVisible(true);
          setStateControls(true);
          ui->txtNewUser->setFocus(Qt::OtherFocusReason);
          ui->btnOtherOptions->setToolTip("<span>"
                                          "Volver a Inicio de sesión!"
                                          "</span>");
          ui->btnCreateUser->setDefault(true);
        }else{
          ui->btnOtherOptions->setIcon(QIcon(":/img/down.png"));
          ui->widget->setVisible(false);
          ui->groupBox->setEnabled(true);
          setStateControls(false);
          ui->txtUser->setFocus(Qt::OtherFocusReason);
          ui->btnOtherOptions->setToolTip("<span>"
                                          "Crear un nuevo usuario y/o<br>"
                                          "restablecer clave o password!"
                                          "</span>");
          ui->pbLogIn->setDefault(true);
        }
    });

  //coneccion de combo box metodo de recuperacion
  QObject::connect(ui->cboRestoreType, &QComboBox::activated, this, &LogInDialog::setOptionsToComboBox);
  ui->widget->hide();
  //connect to create user button
  QObject::connect(ui->btnCreateUser, &QAbstractButton::clicked, this, [&](){
      if(Validate_hasNoEmpty()){
          QMessageBox::warning(this, qApp->applicationName(), "<span><em>Todos los campos son requeridos!</em></span>");
          ui->txtNewUser->setFocus();
          return;
        }
      if(!SW::Helper_t::verify_Values(ui->txtNewPassword->text(), ui->txtRePassword->text())){
          QMessageBox::warning(this, qApp->applicationName(),
                               "<span>"
                               "<strong>"
                               "<em>"
                               "El password o clave de confirmación no coincide!"
                               "</em>"
                               "</strong>"
                               "</span>");
          ui->txtRePassword->selectAll();
          ui->txtRePassword->setFocus();
          return;
        }
      if(ui->cboRestoreType->currentText() == "Pin numérico"){
          if(ui->txtfirstValue->text().count() < 4 || ui->txtConfirmValue->text().count() <4){
              QMessageBox::warning(this, qApp->applicationName(), "<span><em>El PIN numérico debe contener 4 digitos!</em></span>");
              ui->txtfirstValue->selectAll();
              ui->txtfirstValue->setFocus();
              return;
            }
          if(!SW::Helper_t::verify_Values(ui->txtfirstValue->text(), ui->txtConfirmValue->text())){
              QMessageBox::warning(this, qApp->applicationName(), "<span><strong><em>El número de confirmación no coincide!</em></strong></span>");
              ui->txtConfirmValue->selectAll();
              ui->txtConfirmValue->setFocus();
              return;
            }

        }
      if(!ui->checkBox->isChecked()){
          QMessageBox::warning(this, qApp->applicationName(), "<span><em>Debe marcar siempre el perfil de usuario!</em></span>");
          return;
        }

      if(helperdb_.userExists(ui->txtNewUser->text())){
          QMessageBox::warning(this, qApp->applicationName(), tr("<span><em>El nombre de usuario: <strong>%1</strong> ya esta registrado.<br>"
                                                                 "Vuelva a intentarlo con otro nombre porfavor!"
                                                                 "</em></span>").arg(ui->txtNewUser->text()));
          ui->txtNewUser->selectAll();
          ui->txtNewUser->setFocus(Qt::OtherFocusReason);
          return;
        }




      auto password = SW::Helper_t::hashGenerator(ui->txtRePassword->text().toLatin1());
      QString first_value{};
      QString confirm_value{};
      if(ui->cboRestoreType->currentText() == "Pin numérico"){

          first_value = SW::Helper_t::hashGenerator(ui->txtfirstValue->text().toLatin1());
          confirm_value = SW::Helper_t::hashGenerator(ui->txtConfirmValue->text().toLatin1());
        }else{
          first_value = ui->txtfirstValue->text();
          confirm_value = SW::Helper_t::hashGenerator(ui->txtConfirmValue->text().toLatin1());
        }


      if(helperdb_.createUser(ui->txtNewUser->text(), password, ui->checkBox->text(),
                              ui->cboRestoreType->currentText(), first_value, confirm_value)){
          QMessageBox::information(this, qApp->applicationName(), "<span><em>El nuevo usuario fue creado con éxito!</em></span>");
          clearControls();
          ui->btnOtherOptions->toggle();
        }


    });

  QObject::connect(ui->btnResetPassword, &QPushButton::clicked, this, [&](){
      ResetPasswordDialog resetPassword{this};
      resetPassword.exec();

    });

}//end constructor

LogInDialog::~LogInDialog()
{
  delete ui;
}



void LogInDialog::setUp_Form() noexcept{

  setWindowTitle(QApplication::applicationName().append(" - inicio de sesión"));
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


  //new user section
  ui->txtNewPassword->setPlaceholderText("Ingrese clave o password");
  ui->txtNewPassword->setEchoMode(QLineEdit::Password);
  ui->txtRePassword->setEchoMode(QLineEdit::Password);
  ui->txtNewUser->setPlaceholderText("Ingrese un nombre de usuario");
  ui->txtRePassword->setPlaceholderText("Vuelva a ingresar su clave o password");

  ui->txtfirstValue->setPlaceholderText("Ingrese una pregunta!");
  ui->txtConfirmValue->setPlaceholderText("Ingrese su respuesta!");

  //set the combo box options
  ui->cboRestoreType->addItem(QIcon(":/img/paper_pin.png"), "Pregunta secreta");
  ui->cboRestoreType->addItem(QIcon(":/img/paper_pin.png"), "Pin numérico");
  ui->checkBox->setChecked(true);

}

void LogInDialog::setStateControls(bool op) noexcept{
  ui->txtPassword->setDisabled(op);
  ui->txtUser->setDisabled(op);
  ui->pbCancel->setDisabled(op);
  ui->pbLogIn->setDisabled(op);

}

void LogInDialog::setOptionsToComboBox(int index) noexcept{

  if(index == 0){
      ui->txtfirstValue->clear();
      ui->txtConfirmValue->clear();
      ui->txtfirstValue->setPlaceholderText("Ingrese una pregunta!");
      ui->txtConfirmValue->setPlaceholderText("Ingrese su respuesta!");
      ui->txtfirstValue->setValidator(nullptr);
      ui->txtConfirmValue->setValidator(nullptr);
      ui->txtfirstValue->setFocus(Qt::OtherFocusReason);

    }else{
      ui->txtfirstValue->clear();
      ui->txtConfirmValue->clear();
      ui->txtfirstValue->setPlaceholderText("Ingrese PIN numérico de 4 cifras!");
      ui->txtConfirmValue->setPlaceholderText("Vuelva a ingresar el número");
      auto* validator = new QRegularExpressionValidator(QRegularExpression("^\\d{4}$"), this);
      ui->txtfirstValue->setValidator(validator);
      ui->txtConfirmValue->setValidator(validator);
      ui->txtfirstValue->setFocus(Qt::OtherFocusReason);

    }
}


void LogInDialog::clearControls() noexcept{
  ui->txtNewUser->clear();
  ui->txtNewPassword->clear();
  ui->txtRePassword->clear();
  ui->txtfirstValue->clear();
  ui->txtConfirmValue->clear();
  ui->checkBox->setChecked(true);

}


bool LogInDialog::Validate_hasNoEmpty() const noexcept{
  return ui->txtNewUser->text().isEmpty() || ui->txtNewPassword->text().isEmpty() || ui->txtRePassword->text().isEmpty() ||
      ui->txtfirstValue->text().isEmpty() || ui->txtConfirmValue->text().isEmpty();
}

void LogInDialog::writeSettings(){
  QSettings settings(qApp->organizationName(), qApp->applicationName());

  settings.setValue("pos_login_form", saveGeometry());

}

void LogInDialog::readSettings(){
  QSettings settings(qApp->organizationName(), qApp->applicationName());

  restoreGeometry(settings.value("pos_login_form").toByteArray());

}

void LogInDialog::reject_form() noexcept{
  writeSettings();
  reject();
}

void LogInDialog::closeEvent(QCloseEvent *event){
  writeSettings();
  event->accept();
}
