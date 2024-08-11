#include "logindialog.hpp"
#include "ui_logindialog.h"
#include <util/helper.hpp>
#include <QMessageBox>
#include "resetpassworddialog.hpp"
#include <QDebug>
#include <QLineEdit>
#include <QCheckBox>


LogInDialog::LogInDialog(QWidget *parent) :
  QDialog(parent), ui(new Ui::LogInDialog){

  ui->setupUi(this);
  setUp_Form();
  readSettings();
  ui->pbLogIn->setDefault(true);


  QObject::connect(ui->pbCancel, &QPushButton::clicked, this, &LogInDialog::reject_form);

  QObject::connect(ui->pbLogIn, &QPushButton::clicked, this, [&](){

      if(!helperdb_.logIn(ui->txtUser->text().simplified(), ui->txtPassword->text().simplified())){
          QMessageBox::warning(this, SW::Helper_t::appName(), QStringLiteral("<span>"
                                                                             "<strong>"
                                                                             "Los datos que ingreso son incorrectos\n"
                                                                             "vuelva a intentarlo."
                                                                             "</strong>"
                                                                             "</span>"));
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
          ui->btnOtherOptions->setIcon(QIcon(QStringLiteral(":/img/up.png")));
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
          QMessageBox::warning(this, SW::Helper_t::appName(), QStringLiteral("<span><em>Todos los campos son requeridos!</em></span>"));
          ui->txtNewUser->setFocus();
          return;
        }

      if(ui->txtNewPassword->text().count() < 8 || ui->txtRePassword->text().count() < 8){
          QMessageBox::warning(this, SW::Helper_t::appName(),
                               QStringLiteral("<span>"
                                              "<em>"
                                              "El password o clave, debe tener 8 caracteres como mínimo."
                                              "</em>"
                                              "</span>"));
          ui->txtRePassword->selectAll();
          ui->txtRePassword->setFocus();
          return;
        }

      if(!SW::Helper_t::verify_Values(ui->txtNewPassword->text(), ui->txtRePassword->text())){
          QMessageBox::warning(this, SW::Helper_t::appName(),
                               QStringLiteral("<span>"
                                              "<strong>"
                                              "<em>"
                                              "El password o clave de confirmación no coincide!"
                                              "</em>"
                                              "</strong>"
                                              "</span>"));
          ui->txtRePassword->selectAll();
          ui->txtRePassword->setFocus();
          return;
        }

      if(!ui->chkGenPassword->isChecked()){

          if(!SW::Helper_t::isPasswordSecure(ui->txtRePassword->text())){
              QMessageBox::warning(this,
                                   SW::Helper_t::appName(),
                                   QStringLiteral("<span>"
                                                  "<em>"
                                                  "Debe ingresar un password o clave segura!<br>"
                                                  "Nota:<br>"
                                                  "Para que un password o clave se considere seguro(a), debe cumplir con lo siguiente:"
                                                  "<ul>"
                                                  "<li>Debe contener al menos un caracter en mayuscula y en minuscula. </li>"
                                                  "<li>Debe contener al menos un número.</li>"
                                                  "<li>Debe contener al menos un caracter especial por ejemplo: \"#$%&@\" etc...</li>"
                                                  "</ul>"
                                                  "Ejemplo de calve segura: <strong>\"MiClave@123\"</strong>"
                                                  "</em>"
                                                  "</span>"));
              ui->txtRePassword->selectAll();
              ui->txtRePassword->setFocus(Qt::OtherFocusReason);
              return;
            }

        }
      if(ui->cboRestoreType->currentText() == QStringLiteral("Pin numérico")){
          if(ui->txtfirstValue->text().count() < 4 || ui->txtConfirmValue->text().count() <4){
              QMessageBox::warning(this, SW::Helper_t::appName(), QStringLiteral("<span><em>El PIN numérico debe contener 4 digitos!</em></span>"));
              ui->txtfirstValue->selectAll();
              ui->txtfirstValue->setFocus();
              return;
            }
          if(!SW::Helper_t::verify_Values(ui->txtfirstValue->text(), ui->txtConfirmValue->text())){
              QMessageBox::warning(this, SW::Helper_t::appName(), QStringLiteral("<span><strong><em>El número de confirmación no coincide!</em></strong></span>"));
              ui->txtConfirmValue->selectAll();
              ui->txtConfirmValue->setFocus();
              return;
            }

        }


      if(helperdb_.userExists(ui->txtNewUser->text())){
          QMessageBox::warning(this, SW::Helper_t::appName(), tr("<span><em>El nombre de usuario: <strong>%1</strong> ya esta registrado.<br>"
                                                                 "Vuelva a intentarlo con otro nombre porfavor!"
                                                                 "</em></span>").arg(ui->txtNewUser->text()));
          ui->txtNewUser->selectAll();
          ui->txtNewUser->setFocus(Qt::OtherFocusReason);
          return;
        }




      auto password = SW::Helper_t::hashGenerator(ui->txtRePassword->text().toLatin1());
      QString first_value{};
      QString confirm_value{};
      if(ui->cboRestoreType->currentText() == QStringLiteral("Pin numérico")){

          first_value = SW::Helper_t::hashGenerator(ui->txtfirstValue->text().toLatin1());
          confirm_value = SW::Helper_t::hashGenerator(ui->txtConfirmValue->text().toLatin1());
        }else{
          first_value = ui->txtfirstValue->text();
          confirm_value = SW::Helper_t::hashGenerator(ui->txtConfirmValue->text().toLatin1());
        }


      if(helperdb_.createUser(ui->txtNewUser->text(), password, SW::Helper_t::currentUser_.value(SW::User::U_user),
                              ui->cboRestoreType->currentText(), first_value, confirm_value)){
          QMessageBox::information(this, SW::Helper_t::appName(), QStringLiteral("<span><em>El nuevo usuario fue creado con éxito!</em></span>"));
          clearControls();
          ui->btnOtherOptions->toggle();
        }


    });

  QObject::connect(ui->btnResetPassword, &QPushButton::clicked, this, [&](){
      ResetPasswordDialog resetPassword{this};
      resetPassword.setWindowTitle(SW::Helper_t::appName().append(QStringLiteral(" - Resetear clave o password")));
      resetPassword.exec();

    });


  QObject::connect(ui->checkBox_2, &QCheckBox::clicked, this, [&](bool checked){
      setFeatures(ui->txtNewPassword, ui->checkBox_2, checked);
    });
  QObject::connect(ui->checkBox_3, &QCheckBox::clicked, this, [&](bool checked){
      setFeatures(ui->txtRePassword, ui->checkBox_3, checked);
    });
  QObject::connect(ui->checkBox_4, &QCheckBox::clicked, this, [&](bool checked){
      setFeatures(ui->txtfirstValue, ui->checkBox_4, checked);
    });
  QObject::connect(ui->checkBox_5, &QCheckBox::clicked, this, [&](bool checked){
      setFeatures(ui->txtConfirmValue, ui->checkBox_5, checked);
    });

  QObject::connect(ui->chkGenPassword, &QCheckBox::clicked, this, [this](bool checked){

      if(checked){

          ui->txtNewPassword->setEchoMode(QLineEdit::Normal);
          ui->txtRePassword->setEchoMode(QLineEdit::Normal);
          ui->txtNewPassword->clear();
          ui->txtRePassword->clear();
        }else{
          ui->txtNewPassword->setEchoMode(QLineEdit::Password);
          ui->txtRePassword->setEchoMode(QLineEdit::Password);
          ui->txtNewPassword->clear();
          ui->txtRePassword->clear();
        }

      ui->btnGenPassword->setEnabled(checked);
      ui->txtNewPassword->setReadOnly(checked);
      ui->txtRePassword->setReadOnly(checked);
      ui->checkBox_2->setChecked(checked);
      ui->checkBox_3->setChecked(checked);
      ui->checkBox_2->setDisabled(checked);
      ui->checkBox_3->setDisabled(checked);
    });

  //gen passowrd button
  QObject::connect(ui->btnGenPassword, &QPushButton::clicked, this, [this](){

      auto password{SW::Helper_t::generateSecurePassword()};
      ui->txtNewPassword->setText(password);
      ui->txtRePassword->setText(password);
    });

}//end constructor

LogInDialog::~LogInDialog()
{
  delete ui;
}



void LogInDialog::setUp_Form() noexcept{

  setWindowTitle(SW::Helper_t::appName().append(QStringLiteral(" - inicio de sesión")));

  ui->txtUser->setPlaceholderText("Usuario");
  ui->txtUser->setClearButtonEnabled(true);

  ui->txtPassword->setEchoMode(QLineEdit::Password);
  ui->txtPassword->setPlaceholderText("Clave o password");
  ui->txtPassword->setClearButtonEnabled(true);

  ui->btnOtherOptions->setIcon(QIcon(QStringLiteral(":/img/down.png")));
  layout()->setSizeConstraint(QLayout::SetFixedSize);

  ui->btnOtherOptions->setToolTip("<p>"
                                  "<span> Crear un nuevo usuario y/o<br>"
                                  "restablecer clave o password!"
                                  "</span>"
                                  "</p>");
  ui->btnGenPassword->setDisabled(true);


  //new user section
  ui->txtNewPassword->setPlaceholderText("Ingrese clave o password (mínimo 8 caracteres)");
  ui->txtNewPassword->setClearButtonEnabled(true);
  ui->txtNewPassword->setEchoMode(QLineEdit::Password);

  ui->txtRePassword->setPlaceholderText("Vuelva a ingresar su clave o password (mínimo 8 caracteres)");
  ui->txtRePassword->setEchoMode(QLineEdit::Password);
  ui->txtRePassword->setClearButtonEnabled(true);

  ui->txtNewUser->setPlaceholderText("Ingrese un nombre de usuario");
  ui->txtNewUser->setClearButtonEnabled(true);


  ui->txtfirstValue->setPlaceholderText("Ingrese una pregunta!");
  ui->txtfirstValue->setClearButtonEnabled(true);
  ui->txtfirstValue->setEchoMode(QLineEdit::Password);


  ui->txtConfirmValue->setPlaceholderText("Ingrese su respuesta!");
  ui->txtConfirmValue->setClearButtonEnabled(true);
  ui->txtConfirmValue->setEchoMode(QLineEdit::Password);

  //set the combo box options
  ui->cboRestoreType->addItem(QIcon(QStringLiteral(":/img/paper_pin.png")), QStringLiteral("Pregunta secreta"));
  ui->cboRestoreType->addItem(QIcon(QStringLiteral(":/img/paper_pin.png")), QStringLiteral("Pin numérico"));
  ui->checkBox->setChecked(true);
  ui->checkBox->setDisabled(true);

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
      auto* validator = new QRegularExpressionValidator(QRegularExpression(QStringLiteral("^\\d{4}$")), this);
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

void LogInDialog::writeSettings() const noexcept{
  QSettings settings(qApp->organizationName(), SW::Helper_t::appName());

  settings.setValue(QStringLiteral("pos_login_form"), saveGeometry());

}

void LogInDialog::readSettings(){
  QSettings settings(qApp->organizationName(), SW::Helper_t::appName());

  restoreGeometry(settings.value(QStringLiteral("pos_login_form")).toByteArray());

}

void LogInDialog::reject_form() noexcept{
  writeSettings();
  reject();
}

void LogInDialog::setFeatures(QLineEdit *w, QCheckBox *b, bool checked) noexcept{

  if(checked){
      w->setEchoMode(QLineEdit::Normal);
      b->setIcon(QIcon(QStringLiteral(":/img/open.png")));
      b->setToolTip("Ocultar los caracteres.");
    }
  else{
      w->setEchoMode(QLineEdit::Password);
      b->setIcon(QIcon(QStringLiteral(":/img/close.png")));
      b->setToolTip("Mostrar los caracteres.");

    }

}

void LogInDialog::closeEvent(QCloseEvent *event){
  writeSettings();
  event->accept();
}
