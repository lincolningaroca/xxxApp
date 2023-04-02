#include "dlgnewcategory.hpp"
#include "ui_dlgnewcategory.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>


dlgNewCategory::dlgNewCategory(OpenMode mode, const QStringList &list, QWidget *parent) :
  QDialog(parent), ui(new Ui::dlgNewCategory)
{
  ui->setupUi(this);
  setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

  if(mode == OpenMode::Edit){
    setWindowTitle(qApp->applicationName().append(" - Edit category data."));
    ui->txtCategory->setText(list.value(0));
    ui->pteDesc->setPlainText(list.value(1));
  }else{
    setWindowTitle(qApp->applicationName().append(" - New category."));
  }


  QObject::connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [&](){
    QStringList list1 = {list};
    if(mode == OpenMode::New){
      if(validateData()){
        if(std::any_of(list1.begin(), list1.end(), [&](const QString& l){
                       return l.compare(ui->txtCategory->text().toUpper()) == 0;
      })){
          QMessageBox::warning(this, qApp->applicationName(),
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

QString dlgNewCategory::category() const noexcept
{
  return ui->txtCategory->text().toUpper().simplified();

}

QString dlgNewCategory::description() noexcept
{
  return ui->pteDesc->toPlainText().toUpper().simplified();

}

QString dlgNewCategory::descriptionToolTip() noexcept
{
  return ui->pteDesc->toPlainText();
}


bool dlgNewCategory::validateData() const noexcept
{
  if(ui->txtCategory->text().simplified().isEmpty()){
    QMessageBox::warning(nullptr, qApp->applicationName(), "Debe ingresar un nombre de categoría!\n");
    ui->txtCategory->setFocus(Qt::OtherFocusReason);
    return false;
  }
  return true;
}


