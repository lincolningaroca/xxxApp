#include "dlgnewcategory.hpp"
#include "ui_dlgnewcategory.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>


dlgNewCategory::dlgNewCategory(OpenMode mode, uint32_t id, QWidget *parent) :
  QDialog(parent), ui(new Ui::dlgNewCategory),
  id_{id}, db_ { QSqlDatabase::database("xxxConection") }
{
  ui->setupUi(this);
  setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

  if(mode == OpenMode::Edit){
      setWindowTitle(qApp->applicationName().append(" - Edit category data."));
      loadData();
    }


  QObject::connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [&](){
      QSqlQuery qry(db_);
      [[ maybe_unused ]] auto res = qry.prepare("UPDATE category SET category_name=?, desc=? WHERE id=? ");
      qry.addBindValue(ui->txtCategory->text(), QSql::In);
      qry.addBindValue(ui->pteDesc->toPlainText(), QSql::In);
      qry.addBindValue(id_, QSql::In);
      if(!qry.exec()){
          QMessageBox::critical(this, qApp->applicationName(), "Error al ajecutar la consulta!\n"+
                                qry.lastError().text());
          return;
        }

      QMessageBox::information(this, qApp->applicationName(), "Datos actualizados!\n");
      dlgNewCategory::accept();
    });
  QObject::connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &dlgNewCategory::reject);
}

dlgNewCategory::~dlgNewCategory()
{
  delete ui;
}

QString dlgNewCategory::category() const noexcept
{
  return ui->txtCategory->text().toUpper().simplified();

}

QString dlgNewCategory::description() const noexcept
{
  return ui->pteDesc->toPlainText();

}

void dlgNewCategory::loadData() noexcept
{

  QSqlQuery qry(db_);
  [[ maybe_unused ]] auto res = qry.prepare("SELECT category_name,desc FROM category WHERE id=? ");
  qry.addBindValue(id_, QSql::In);
  if(!qry.exec()){
      QMessageBox::critical(this, qApp->applicationName(), "Error al ajecutar la consulta!\n"+
                            qry.lastError().text());
      return;
    }
  qry.next();
  ui->txtCategory->setText(qry.value(0).toString());
  ui->pteDesc->setPlainText(qry.value(1).toString());
}
