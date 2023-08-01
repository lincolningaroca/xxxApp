#include "resetpassworddialog.hpp"
#include "ui_resetpassworddialog.h"
#include "widget.hpp"

#include <QMessageBox>
#include <QRegularExpressionValidator>
#include <QAction>

ResetPasswordDialog::ResetPasswordDialog(QWidget *parent)
  :QDialog(parent), ui(new Ui::ResetPasswordDialog){

  ui->setupUi(this);

  setInit_Form();

  QObject::connect(ui->btnValidarUsuario, &QPushButton::clicked, this, [&](){

      if(!helper.userExists(ui->txtUser->text().simplified())){
          QMessageBox::warning(this, qApp->applicationName(),
                               "<p><cite>Nombre de usuario incorrecto.</p></cite>");
          ui->txtUser->selectAll();
          ui->txtUser->setFocus(Qt::OtherFocusReason);
          return;
        }
      userId = helper.getUser_id(ui->txtUser->text().simplified(), "USER");
      auto rescue_type = helper.validateRescueType(userId);

      if(rescue_type == "Pregunta secreta"){
          ui->stackedWidget->setCurrentIndex(1);
          ui->txtPregunta->setPlainText(helper.getQuestion(userId));
          ui->btnRespuesta->setDefault(true);
          ui->txtRespuesta->setFocus(Qt::OtherFocusReason);

        }
      else{
          ui->stackedWidget->setCurrentIndex(2);
          ui->btnClaveNumerica->setDefault(true);
          ui->txtPIN->setFocus(Qt::OtherFocusReason);
        }


    });

  //btn pregunta secreta
  QObject::connect(ui->btnRespuesta, &QAbstractButton::clicked, this, [&](){
      if(!helper.validateAnswer(ui->txtRespuesta->text(), userId)){
          QMessageBox::warning(this, qApp->applicationName(),
                               "<p><cite>Su respuesta es incorrecta.</cite></p>");
          ui->txtRespuesta->selectAll();
          ui->txtRespuesta->setFocus(Qt::OtherFocusReason);
          return;
        }

      ui->stackedWidget->setCurrentIndex(3);
      ui->btnReset->setDefault(true);
    });

  //btn pin numerico
  QObject::connect(ui->btnClaveNumerica, &QPushButton::clicked, this, [&](){

      if(ui->txtPIN->text().count() < 4){
          QMessageBox::warning(this, qApp->applicationName(), "<span><em>El PIN numérico debe contener 4 digitos!</em></span>");
          ui->txtPIN->selectAll();
          ui->txtPIN->setFocus(Qt::OtherFocusReason);
          return;
        }
      if(!helper.validateAnswer(ui->txtPIN->text(), userId)){
          QMessageBox::warning(this, qApp->applicationName(),
                               "<p><cite>El número que ingreso es incorrecto.</cite></p>");
          ui->txtPIN->selectAll();
          ui->txtPIN->setFocus(Qt::OtherFocusReason);
          return;
        }

      ui->stackedWidget->setCurrentIndex(3);
      ui->btnReset->setDefault(true);
    });

  //btn volver atras

  QObject::connect(ui->btnAtras, &QPushButton::clicked, this, [&](){

      ui->txtUser->clear();
      ui->txtNewPassword->clear();
      ui->txtPIN->clear();
      ui->txtPregunta->clear();
      ui->txtRePassword->clear();
      ui->txtRespuesta->clear();

      ui->stackedWidget->setCurrentIndex(0);
      ui->btnValidarUsuario->setDefault(true);
      ui->txtUser->setFocus(Qt::OtherFocusReason);
    });
}

ResetPasswordDialog::~ResetPasswordDialog(){
  delete ui;
}

void ResetPasswordDialog::setFeatures(QLineEdit *w, QCheckBox *b, bool checked) noexcept{

  if(checked){
      w->setEchoMode(QLineEdit::Normal);
      b->setIcon(QIcon(":/img/close.png"));
      b->setToolTip("Ocultar los caracteres.");
    }
  else{
      w->setEchoMode(QLineEdit::Password);
      b->setIcon(QIcon(":/img/open.png"));
      b->setToolTip("Mostrar los caracteres.");

    }

}

void ResetPasswordDialog::setInit_Form() noexcept{

  setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
  ui->stackedWidget->setCurrentIndex(0);
  ui->txtUser->setClearButtonEnabled(true);

  auto* validator = new QRegularExpressionValidator(QRegularExpression("^\\d{4}$"), this);
  ui->txtPIN->setValidator(validator);

  ui->txtPIN->setEchoMode(QLineEdit::Password);
  ui->txtNewPassword->setEchoMode(QLineEdit::Password);
  ui->txtRePassword->setEchoMode(QLineEdit::Password);
  ui->txtRespuesta->setEchoMode(QLineEdit::Password);

  //iconos y acciones para QLineEdit para mostrar y/o ocultar los caracteres de los controles.
  QObject::connect(ui->checkBox, &QCheckBox::clicked, this, [&](bool checked){
      setFeatures(ui->txtRespuesta, ui->checkBox, checked);


    });
  QObject::connect(ui->checkBox_2, &QCheckBox::clicked, this, [&](bool checked){
      setFeatures(ui->txtPIN, ui->checkBox_2, checked);
    });
  QObject::connect(ui->checkBox_3, &QCheckBox::clicked, this, [&](bool checked){
      setFeatures(ui->txtNewPassword, ui->checkBox_3, checked);
    });
  QObject::connect(ui->checkBox_4, &QCheckBox::clicked, this, [&](bool checked){
      setFeatures(ui->txtRePassword, ui->checkBox_4, checked);
    });

}
