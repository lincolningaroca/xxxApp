#include "dlgnewcategory.hpp"
#include "ui_dlgnewcategory.h"
#include <QMessageBox>
#include "util/helper.hpp"
#include "helperdatabase/helperdb.hpp"


dlgNewCategory::dlgNewCategory(OpenMode mode, const QStringList &list, QWidget *parent) :
  QDialog(parent), ui(new Ui::dlgNewCategory){
  ui->setupUi(this);
  setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);


  if(mode == OpenMode::Edit){
      setWindowTitle(SW::Helper_t::appName().append(QStringLiteral(" - Edit category data.")));
      ui->txtCategory->setText(list.value(0));
      ui->pteDesc->setPlainText(list.value(1));
    }else{
      setWindowTitle(SW::Helper_t::appName().append(QStringLiteral(" - New category.")));
    }


  QObject::connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [&](){

      SW::HelperDataBase_t helperdb_{};

      uint32_t userid {0};
      (static_cast<bool>(SW::Helper_t::sessionStatus_)) ?
           userid = helperdb_.getUser_id(SW::Helper_t::current_user_, SW::User::U_public) :
           userid = helperdb_.getUser_id(SW::Helper_t::current_user_, SW::User::U_user);


      if(mode == OpenMode::New){
          if(validateData()){

              if(helperdb_.categoryExists(ui->txtCategory->text().toUpper(), userid)){
                  QMessageBox::warning(this, SW::Helper_t::appName(),
                                       QString("<p><cite>La categoría: "
                                               "<strong style='color:#ff0800;'>\"%1\""
                                               "</strong>, ya esta registrada en la base de datos.<br>"
                                               "pruebe con otro nombre por favor!"
                                               "</cite>"
                                               "</p>").arg(ui->txtCategory->text().toUpper()));
                  ui->txtCategory->selectAll();
                  ui->txtCategory->setFocus(Qt::OtherFocusReason);
                  return;
                }
              accept();

            }
        }else{
          if(validateData())
            accept();
        }

    });


  QObject::connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);


}


dlgNewCategory::~dlgNewCategory()
{
  delete ui;
}

QString dlgNewCategory::category() const noexcept{
  return ui->txtCategory->text().toUpper().simplified();

}

QString dlgNewCategory::description() const noexcept{
  return ui->pteDesc->toPlainText().toUpper().simplified();

}

QString dlgNewCategory::descriptionToolTip() const noexcept{
  return ui->pteDesc->toPlainText();
}


bool dlgNewCategory::validateData() const noexcept{
  if(ui->txtCategory->text().simplified().isEmpty()){
      QMessageBox::warning(nullptr, SW::Helper_t::appName(), QStringLiteral("Debe ingresar un nombre de categoría!\n"));
      ui->txtCategory->setFocus(Qt::OtherFocusReason);
      return false;
    }
  return true;
}


