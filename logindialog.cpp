#include "logindialog.hpp"
#include "ui_logindialog.h"
//#include "widget.hpp"
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include <QCryptographicHash>
#include <memory>



LogInDialog::LogInDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::LogInDialog),
  db_{QSqlDatabase::database("xxxConection")}{

  ui->setupUi(this);
  setUp_Form();


  QObject::connect(ui->pbCancel, &QPushButton::clicked, this, &LogInDialog::reject);
  //  QObject::connect(ui->pbLogIn, &QPushButton::clicked, this, &QDialog::accept);
  QObject::connect(ui->pbLogIn, &QPushButton::clicked, this, [&](){

    if(!logIn()){
      QMessageBox::warning(this, qApp->applicationName(), "Los datos que ingreso son incorrectos\n"
                                                          "vuelva a intentarlo.");
      ui->txtUser->selectAll();
      ui->txtUser->setFocus(Qt::OtherFocusReason);
      //      qInfo()<<hashGenerator(ui->txtPassword->text().toLatin1());
      //      QSqlQuery qry{db_};
      //      qry.prepare("SELECT  user, password FROM users WHERE user_id=7");
      //      qry.exec();
      //      qry.first();

      //      qInfo()<< qry.value(0).toString() << " " << qry.value(1).toString();
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
      ui->btnOtherOptions->setToolTip("<p>"
                                      "<span> Volver a Inicio de sesión!"
                                      "</span>"
                                      "</p>");
      ui->btnCreateUser->setDefault(true);
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
    if(!verifyPassword()){
      QMessageBox::warning(this, qApp->applicationName(), "<span><em>El password o clave de confirmación no coincide!</em></span>");
      ui->txtRePassword->selectAll();
      ui->txtRePassword->setFocus();
      return;
    }
    if(ui->cboRestoreType->currentText() == "Pin numérico"){
      if(ui->txtfirstValue->text().count() < 4 || ui->txtConfirmValue->text().count() <4){
        QMessageBox::warning(this, qApp->applicationName(), "<span><em>El PIN numérico de contener 4 digitos!</em></span>");
        ui->txtfirstValue->selectAll();
        ui->txtfirstValue->setFocus();
        return;
      }
      if(!verifyPinNumber()){
        QMessageBox::warning(this, qApp->applicationName(), "<span><em>El número de confirmación no coincide!</em></span>");
        ui->txtConfirmValue->selectAll();
        ui->txtConfirmValue->setFocus();
        return;
      }

    }
    if(!ui->checkBox->isChecked()){
      QMessageBox::warning(this, qApp->applicationName(), "<span><em>Debe marcar siempre el perfil de usuario!</em></span>");
      return;
    }

    if(userExists(ui->txtNewUser->text())){
      QMessageBox::warning(this, qApp->applicationName(), tr("<span><em>El nombre de usuario: <strong>%1</strong> ya esta registrado.<br>"
                                                             "Vuelva a intentarlo con otro nombre porfavor!"
                                                             "</em></span>").arg(ui->txtNewUser->text()));
      ui->txtNewUser->selectAll();
      ui->txtNewUser->setFocus(Qt::OtherFocusReason);
      return;
    }




    auto password = hashGenerator(ui->txtRePassword->text().toLatin1());
    QString first_value{};
    QString confirm_value{};
    if(ui->cboRestoreType->currentText() == "Pin numérico"){

      first_value = hashGenerator(ui->txtfirstValue->text().toLatin1());
      confirm_value = hashGenerator(ui->txtConfirmValue->text().toLatin1());
    }else{
      first_value = ui->txtfirstValue->text();
      confirm_value = hashGenerator(ui->txtConfirmValue->text().toLatin1());
    }


    if(createUser(ui->txtNewUser->text(), password, ui->checkBox->text(), ui->cboRestoreType->currentText(), first_value, confirm_value)){
      QMessageBox::information(this, qApp->applicationName(), "<span><em>El nuevo usuario fue creado con éxito!</em></span>");
      clearControls();
      ui->btnOtherOptions->toggle();
    }


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

bool LogInDialog::logIn() const noexcept{
  QSqlQuery qry{db_};
  [[maybe_unused]]
  auto res = qry.prepare("SELECT COUNT(*) FROM users WHERE user = ? AND password = ?");
  qry.addBindValue(ui->txtUser->text());
  qry.addBindValue(hashGenerator(ui->txtPassword->text().toLatin1()));
  if(!qry.exec())
    return false;
  qry.first();

  return(qry.value(0).toInt() == 1);



}

void LogInDialog::clearControls() noexcept{
  ui->txtNewUser->clear();
  ui->txtNewPassword->clear();
  ui->txtRePassword->clear();
  ui->txtfirstValue->clear();
  ui->txtConfirmValue->clear();
  ui->checkBox->setChecked(true);

}

bool LogInDialog::createUser(const QString &user, const QString &password, const QString &user_prof,
                             const QString &rescue_type, const QString &val1, const QString &val2) noexcept{

  QSqlQuery newUserQry{db_};
  [[maybe_unused]]
      auto res = newUserQry.prepare("INSERT INTO users(user,password,user_profile,rescue_type,first_value,confirm_value) "
                                    "VALUES(?,?,?,?,?,?)");
  newUserQry.addBindValue(user);
  newUserQry.addBindValue(password);
  newUserQry.addBindValue(user_prof);
  newUserQry.addBindValue(rescue_type);
  newUserQry.addBindValue(val1);
  newUserQry.addBindValue(val2);
  return newUserQry.exec();

}

bool LogInDialog::Validate_hasNoEmpty() const noexcept{
  return ui->txtNewUser->text().isEmpty() || ui->txtNewPassword->text().isEmpty() || ui->txtRePassword->text().isEmpty() ||
      ui->txtfirstValue->text().isEmpty() || ui->txtConfirmValue->text().isEmpty();
}

bool LogInDialog::verifyPassword() const noexcept{return (ui->txtNewPassword->text() == ui->txtRePassword->text());}
bool LogInDialog::verifyPinNumber() const noexcept{return (ui->txtfirstValue->text() == ui->txtConfirmValue->text());}

bool LogInDialog::userExists(const QString &user) const noexcept
{
  QSqlQuery userQry{db_};
  [[maybe_unused]]
  auto res = userQry.prepare("SELECT COUNT(*) FROM users WHERE user = ?");
  userQry.addBindValue(user);

  if(!userQry.exec())
    return false;
  userQry.first();
  return (userQry.value(0).toUInt() == 1);
}

QString LogInDialog::hashGenerator(const QByteArray &data) noexcept{
  QCryptographicHash crypto(QCryptographicHash::Sha3_256);
  crypto.addData(data);
  return QString{crypto.result().toHex()};

}

