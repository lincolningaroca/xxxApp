#include "dlgnewcategory.hpp"
#include "ui_dlgnewcategory.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>


dlgNewCategory::dlgNewCategory(OpenMode mode, uint32_t id, const QStringList &list, QWidget *parent) :
  QDialog(parent), ui(new Ui::dlgNewCategory),
  mode_{mode}, id_{id},  db_ { QSqlDatabase::database("xxxConection")}, list_{list}
{
  ui->setupUi(this);
  setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

  if(mode_ == OpenMode::Edit){
      setWindowTitle(qApp->applicationName().append(" - Edit category data."));
      loadData();
    }else{
      setWindowTitle(qApp->applicationName().append(" - New category."));
    }


  QObject::connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [&](){

      if(mode_ == OpenMode::New){
          if(!validateData())
            return;

          if(std::any_of(list_.begin(), list_.end(), [&](const QString& l){
                         return l.compare(ui->txtCategory->text().toUpper()) == 0;
        })){
              QMessageBox::warning(this, qApp->applicationName(),
                                   QString("<p><cite>La categoría: <strong style='color:#ff0800;'>\"%1\"</strong>, ya esta registrada en la base de datos.<br>"
                                           "pruebe con otro nombre por favor!</cite></p>").arg(ui->txtCategory->text().toUpper()));
              ui->txtCategory->selectAll();
              ui->txtCategory->setFocus(Qt::OtherFocusReason);
              return;
            }

          accept();
        }else{

          if(!validateData()) return;

          QSqlQuery qry(db_);
          [[ maybe_unused ]] auto res = qry.prepare("UPDATE category SET category_name=?, desc=? WHERE id=? ");
          qry.addBindValue(ui->txtCategory->text().simplified().toUpper(), QSql::In);
          qry.addBindValue(ui->pteDesc->toPlainText().simplified().toUpper(), QSql::In);
          qry.addBindValue(id_, QSql::In);
          if(!qry.exec()){
              QMessageBox::critical(this, qApp->applicationName(), "Error al ajecutar la consulta!\n"+
                                    qry.lastError().text());
              return;
            }

          QMessageBox::information(this, qApp->applicationName(), "Datos actualizados!\n");
          accept();
        }
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

QString dlgNewCategory::description() noexcept
{
  return ui->pteDesc->toPlainText();

}

QString dlgNewCategory::descriptionToolTip() noexcept
{
  loadData();
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

bool dlgNewCategory::validateData() const noexcept
{
  if(ui->txtCategory->text().simplified().isEmpty()){
      QMessageBox::warning(nullptr, qApp->applicationName(), "Debe ingresar un nombre de categoría!\n");
      ui->txtCategory->setFocus(Qt::OtherFocusReason);
      return false;
    }
  return true;
}


