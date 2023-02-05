#include "widget.hpp"
#include "./ui_widget.h"


#include <QSqlError>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDesktopServices>
#include <QSettings>
#include <QRegularExpression>
#include <QAction>
#include "dlgnewcategory.hpp"
#include <QDebug>
//#include <QSqlRecord>


Widget::Widget(QWidget *parent)
  : QWidget(parent), ui(new Ui::Widget), db{QSqlDatabase::database("xxxConection")}
{
  ui->setupUi(this);
  initFrm();
  loadListCategory();
  loadCategopryData();
  setUpTable(categoryList.key(ui->cboCategory->currentText()));


  readSettings();
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /// ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //context menu implementation
  setUpCboCategoryContextMenu();

  QObject::connect(delCategory, &QAction::triggered, this, [&](){
      QMessageBox msgBox;
      msgBox.setWindowTitle(qApp->applicationName().append(" - Advertencia"));
      msgBox.setText("<p style='color:#FB4934;'>Esta a punto eliminar ésta categoría y todo su contenido.<br>"
                     "Recuerde que al aceptar, eliminará de forma permanente estos datos.<br>"
                     "Desea continuar y eliminar los datos?</p>");
      msgBox.setIcon(QMessageBox::Warning);
      msgBox.addButton("Borrar categoría", QMessageBox::AcceptRole);
      msgBox.addButton("Cancelar", QMessageBox::RejectRole);

      if(msgBox.exec() == QMessageBox::AcceptRole){
          if(deleteAll()){
              QMessageBox::information(this, qApp->applicationName(),"Datos eliminados.");
              loadListCategory();
              loadCategopryData();
            }
        }
      return;
    });

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //connect to slot to btnDeleteCategory
  QObject::connect(ui->btnDeleteCategory,&QPushButton::clicked, this, [&](){

      auto [res, count, errMessage] = verifyDeleteCategory();
      if(!res){
          QMessageBox::warning(this, qApp->applicationName().append(" - Advertencia"),
                               QString("<p>No se puede eliminar ésta categoría.<br>"
                                       "Esto es debido a que ésta categoría tiene asociado uno o mas elementos.<br><br>"
                                       "<strong>Sugerencia:</strong><br>"
                                       "Si desea eliminar una categoría y todo su contenido, "
                                       "puede optar por dar click derecho sobre el nombre de la categoría y "
                                       "del menú contextual elegir:<br><br> <mark style='background:#FABD2F;color:#FB4934;'>\"->Forzar eliminación de categoría.\"</mark></p>"));
          //            qDebug()<<count<<'\n';
          return;

        }
      auto ret = QMessageBox::warning(this, qApp->applicationName(), "Esta seguro de eliminar esta categoría?"
                                      ,QMessageBox::Yes, QMessageBox::Cancel);
      if(ret==QMessageBox::Yes){
          if(deleteCategory()){
              QMessageBox::information(this, qApp->applicationName(), "Categoría eliminada!");
              //          ui->cboCategory->clear();
              loadListCategory();
              loadCategopryData();
            }

        }else return;

    });
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //connect to slots to btnAdd
  QObject::connect(ui->btnAdd,&QPushButton::clicked, this, [&](){

      if(ui->btnAdd->text().compare("&Add") == 0){
          if(!urlValidate(ui->txtUrl->text())){
              QMessageBox::warning(this, qApp->applicationName(), "La dirección que ingreso no es válida!\n");
              ui->txtUrl->selectAll();
              ui->txtUrl->setFocus(Qt::OtherFocusReason);
              return;
            }
          //get the key from categoryList, with current selected text to cboCategory.
          auto categoryId = categoryList.key(ui->cboCategory->currentText());
          if(saveData(ui->txtUrl->text(), ui->pteDesc->toPlainText(), categoryId)){
              //              QMessageBox::information(this,qApp->applicationName(),"Datos guardados!!");
              ui->txtUrl->clear();
              ui->pteDesc->clear();
              ui->txtUrl->setFocus(Qt::OtherFocusReason);
              setUpTable(categoryList.key(ui->cboCategory->currentText()));
              verifyContextMenu();
            }
        }else{

          QSqlQuery qry(db);
          [[maybe_unused]] auto res=qry.prepare("UPDATE  urls SET url=?, desc=? WHERE id=?");
          qry.addBindValue(ui->txtUrl->text(), QSql::In);
          qry.addBindValue(ui->pteDesc->toPlainText(), QSql::In);
          auto currentRow = ui->tvUrl->currentIndex().row();
          auto id = ui->tvUrl->model()->index(currentRow,0).data().toInt();
          qry.addBindValue(id, QSql::In);
          if(!qry.exec()){
              QMessageBox::critical(this, qApp->applicationName(), "Fallo la ejecución de la sentencia!\n"
                                    +qry.lastError().text());
              return;

            }
          setUpTable(categoryList.key(ui->cboCategory->currentText()));
          ui->btnAdd->setText("&Add");
          editAction(false);
          ui->txtUrl->clear();
          ui->pteDesc->clear();
          ui->txtUrl->setFocus(Qt::OtherFocusReason);
        }

    });
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

  //btnAddNewCategory
  QObject::connect(ui->btnNewCategory, &QPushButton::clicked, this, [&](){
      dlgNewCategory newCategory(dlgNewCategory::OpenMode::New, 0 ,this);

      if(newCategory.exec() == QDialog::Accepted){
          if(!saveCategoryData(newCategory.category(), newCategory.description())){
              QMessageBox::critical(this, qApp->applicationName(), "Error alguardar los datos!\n");
              return;
            }
          ui->cboCategory->clear();
          loadListCategory();
          loadCategopryData();

        }else return;

    });
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //btn edit category
  QObject::connect(ui->btnEditCategory, &QPushButton::clicked, this, [&](){

      auto id = categoryList.key(ui->cboCategory->currentText());
      dlgNewCategory editCategory(dlgNewCategory::OpenMode::Edit, id, this);
      if(editCategory.exec() == QDialog::Accepted){
          ui->cboCategory->clear();
          loadListCategory();
          loadCategopryData();
        }

    });
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

  //connect to slots to txtUrl
  QObject::connect(ui->txtUrl, &QLineEdit::textChanged,this, [&](const QString& text){
      (!text.simplified().isEmpty()) ? ui->btnAdd->setEnabled(true) : ui->btnAdd->setDisabled(true);
    });
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

  //connect to slots to btnQuit
  QObject::connect(ui->btnQuit, &QPushButton::clicked,this, [&](){
      if(!validateSelectedRow()) return;

      auto currentRow = ui->tvUrl->currentIndex().row();
      auto url = ui->tvUrl->model()->index(currentRow, 1).data().toString();
      QMessageBox msgBox;
      msgBox.setText(QString("<span>Confirma que desea eliminar esta dirección:\n <h4 style='color:#ff0800;'>%1</h4></span>").arg(url));
      msgBox.setIcon(QMessageBox::Question);
      msgBox.addButton("Eliminar",QMessageBox::AcceptRole);
      msgBox.addButton("Cancelar",QMessageBox::RejectRole);
      if(msgBox.exec() == QMessageBox::AcceptRole){
          ui->tvUrl->model()->removeRow(ui->tvUrl->currentIndex().row());
          setUpTable(categoryList.key(ui->cboCategory->currentText()));
        }

    });
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //btn edit
  QObject::connect(ui->btnEdit, &QPushButton::clicked, this, &Widget::btnEdit);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //btnopenUrl
  QObject::connect(ui->btnopen, &QPushButton::clicked, this, [&](){
      if(!validateSelectedRow()) return;

      auto currentRow = ui->tvUrl->currentIndex().row();
      auto url = ui->tvUrl->model()->index(currentRow, 1).data().toString();
      if(!QDesktopServices::openUrl(QUrl(url))){
          QMessageBox::critical(this, qApp->applicationName(), "Fallo al intentar abrir dirección url!\n");
          return;
        }


    });
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////


  //cboTheme
  QObject::connect(ui->cboTheme, &QComboBox::currentTextChanged, this, [&](const QString& text){
      if(text.compare("Modo Oscuro") == 0){
          setTheme(Theme::Modo_Oscuro);
          setLabelInfo(darkModeColor);
        }else{
          setTheme(Theme::Modo_Claro);
          setLabelInfo(lightModeColor);
        }

    });
  QObject::connect(ui->cboCategory, &QComboBox::currentIndexChanged, this, [&](){
      setUpTable(categoryList.key(ui->cboCategory->currentText()));
      verifyContextMenu();
    });

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

Widget::~Widget()
{
  delete ui;
}

bool Widget::saveData(const QString& url, const QString& desc, std::uint32_t id) const noexcept
{
  QSqlQuery qry(db);
  [[maybe_unused]] auto res = qry.prepare("INSERT INTO urls(url,desc,category_id) VALUES(?,?,?)");
  qry.addBindValue(url.simplified(), QSql::In);
  qry.addBindValue(desc.simplified(), QSql::In);
  qry.addBindValue(id, QSql::In);
  return qry.exec();
}

bool Widget::saveCategoryData(const QString &catName, const QString &desc) const noexcept
{
  QSqlQuery qry(db);
  [[maybe_unused]] auto res = qry.prepare("INSERT INTO category(category_name, desc) VALUES(?,?)");
  qry.addBindValue(catName.simplified(), QSql::In);
  qry.addBindValue(desc.simplified(), QSql::In);
  return qry.exec();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::initFrm() noexcept
{
  setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
  //placeholder
  ui->txtUrl->setPlaceholderText("http://www.url.com");
  ui->pteDesc->setPlaceholderText("Description to url's");
  ui->btnNewCategory->setToolTip("New Category!");
  ui->btnEditCategory->setToolTip("Edit Category Data!");
  //btnAdd disabled
  ui->btnAdd->setDisabled(true);


}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::editAction(bool op) noexcept
{

  ui->btnEdit->setDisabled(op);
  ui->btnQuit->setDisabled(op);
  ui->btnopen->setDisabled(op);
  ui->tvUrl->setDisabled(op);


}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Widget::validateSelectedRow() noexcept
{
  if(!ui->tvUrl->selectionModel()->hasSelection()){
      QMessageBox::warning(this, qApp->applicationName(), "Seleccione una fila!\n");
      return false;
    }
  return true;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::setUpTable(uint32_t categoryId) noexcept
{
  //  qDebug()<<ui->tvUrl->geometry().width();
  QSqlQuery qry(db);
  [[maybe_unused]] auto res = qry.prepare("SELECT * FROM urls WHERE category_id=?");
  qry.addBindValue(categoryId);
  if(!qry.exec()) return;
  auto *model = new QSqlQueryModel(this);
  model->setQuery(std::move(qry));


  //  model->select();
  ui->tvUrl->setModel(model);
  //qDebug()<<model->record(0).value("url").toString();
  setUpTableHeaders();



}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::setUpTableHeaders() const noexcept
{
  ui->tvUrl->hideColumn(0);
  ui->tvUrl->hideColumn(3);
  ui->tvUrl->model()->setHeaderData(1,Qt::Horizontal, "Dirección URL");
  ui->tvUrl->setColumnWidth(1, 350);
  ui->tvUrl->model()->setHeaderData(2,Qt::Horizontal, "Descripción");
  ui->tvUrl->setColumnWidth(2, 300);



}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::btnEdit() noexcept
{
  if( !validateSelectedRow() ) return;

  auto currentRow = ui->tvUrl->currentIndex().row();
  ui->txtUrl->setText(ui->tvUrl->model()->index(currentRow,1).data().toString());
  ui->pteDesc->setPlainText(ui->tvUrl->model()->index(currentRow,2).data().toString());
  editAction(true);
  ui->txtUrl->selectAll();
  ui->txtUrl->setFocus(Qt::OtherFocusReason);
  ui->btnAdd->setText("&Update");

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::setTheme(Theme theme) const noexcept
{
  QPalette mPalette;
  qApp->setStyle("Fusion");
  if(theme == Theme::Modo_Claro)
    mPalette = qApp->style()->standardPalette();
  else{
      mPalette.setColor(QPalette::Window, QColor(53, 53, 53));
      mPalette.setColor(QPalette::WindowText, Qt::white);
      mPalette.setColor(QPalette::Base, QColor(25, 25, 25));
      mPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
      mPalette.setColor(QPalette::ToolTipBase, QColor(53, 53, 53));
      mPalette.setColor(QPalette::ToolTipText, Qt::white);
      mPalette.setColor(QPalette::Text, Qt::white);
      mPalette.setColor(QPalette::PlaceholderText,QColor(127,127,127));
      mPalette.setColor(QPalette::Button, QColor(53, 53, 53));
      mPalette.setColor(QPalette::ButtonText, Qt::white);
      mPalette.setColor(QPalette::BrightText, Qt::red);
      mPalette.setColor(QPalette::Link, QColor(42, 130, 218));
      mPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
      mPalette.setColor(QPalette::HighlightedText, Qt::black);
      mPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(164, 166, 168));
      mPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(164, 166, 168));
      mPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(164, 166, 168));
      mPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(164, 166, 168));
      mPalette.setColor(QPalette::Disabled, QPalette::Base, QColor(68, 68, 68));
      mPalette.setColor(QPalette::Disabled, QPalette::Window, QColor(68, 68, 68));
      mPalette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(68, 68, 68));
    }

  qApp->setPalette(mPalette);

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::writeSettings() const noexcept
{
  QSettings settings(qApp->organizationName(), qApp->applicationName());
  settings.beginGroup("Theme");
  Theme theme;
  QString color;
  if(ui->cboTheme->currentText().compare("Modo Claro") == 0){
      theme=Theme::Modo_Claro;
      color=lightModeColor;
    }else{
      theme=Theme::Modo_Oscuro;
      color=darkModeColor;
    }


  settings.setValue("theme Value", static_cast<uint32_t>(theme));
  settings.setValue("lblColor", setColorReg(color));
  settings.endGroup();
  settings.setValue("position", saveGeometry());
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::readSettings() noexcept
{
  QSettings settings(qApp->organizationName(), qApp->applicationName());
  settings.beginGroup("Theme");
  auto theme = settings.value("theme Value").toUInt();
  setLabelInfo(getColorReg(settings.value("lblColor").toByteArray()));

  //  qDebug()<<"color: "<<getColorReg(settings.value("lblColor").toByteArray());

  setTheme(static_cast<Theme>(theme));
  settings.endGroup();

  ui->cboTheme->setCurrentIndex(theme);
  restoreGeometry(settings.value("position").toByteArray());
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Widget::urlValidate(const QString &url) const noexcept
{
  //  static QRegularExpression regex("^(http|https:\\/\\/)?[\\w\\-]+(\\.[\\w\\-]+)+[/#?]?.*$");
  static QRegularExpression regex(R"(^(http|https:\/\/)?[\w\-]+(\.[\w\\-]+)+[/#?]?.*$)");
  auto match = regex.match(url);
  return match.hasMatch();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::setLabelInfo(const QString& color) noexcept
{
  ui->lblState->setText(QString("<span style='color:%1;'>SWSystem's - Lincoln Ingaroca</span>").arg(color));
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
QByteArray Widget::setColorReg(const QString& color) const noexcept
{
  QByteArray data;
  QDataStream dataStream(&data, QIODevice::WriteOnly);
  dataStream << color;
  return data;

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
QString Widget::getColorReg(QByteArray dataColor) noexcept
{
  //  QByteArray data(dataColor);
  QDataStream dataStream(&dataColor, QIODevice::ReadOnly);
  QString color;
  dataStream >> color;

  return color;

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::loadCategopryData() noexcept
{
  ui->cboCategory->clear();
  ui->cboCategory->addItems(categoryList.values());

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::loadListCategory() noexcept
{
  categoryList.clear();
  QSqlQuery qry(db);
  [[maybe_unused]] auto res=qry.prepare("SELECT id, category_name FROM category");
  if(!qry.exec()){
      QMessageBox::critical(this, qApp->applicationName(), "Error al ejecutar la sentencia!\n"+
                            qry.lastError().text());
      return;
    }
  while(qry.next()){
      categoryList.insert(qry.value(0).toUInt(), qry.value(1).toString());
    }

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
std::tuple<bool, uint32_t, QString>
Widget::verifyDeleteCategory() noexcept
{
  QSqlQuery qry(db);
  bool ret{};
  uint32_t count{};
  QString errMessage{};
  [[maybe_unused]] auto res=qry.prepare("SELECT COUNT (*) FROM urls WHERE category_id=?");
  auto categoryId=categoryList.key(ui->cboCategory->currentText());
  qry.addBindValue(categoryId, QSql::In);
  if(!qry.exec()){
      errMessage=qry.lastError().text();
      ret=false;
    }

  qry.next();
  count=qry.value(0).toUInt();
  if(count != 0) ret=false;
  else ret=true;

  return std::tuple{ret, count, errMessage};

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::setUpCboCategoryContextMenu() noexcept
{
  ui->cboCategory->setContextMenuPolicy(Qt::ActionsContextMenu);
  delCategory = new QAction("Forzar eliminación de categoría",this);
  ui->cboCategory->addAction(delCategory);


}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Widget::deleteCategory() const noexcept
{
  QSqlQuery qry(db);
  [[maybe_unused]] auto res=qry.prepare("DELETE FROM category WHERE id=?");
  auto categoryId=categoryList.key(ui->cboCategory->currentText());
  qry.addBindValue(categoryId, QSql::In);
  if(!qry.exec())
    return false;

  return true;

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Widget::deleteUrls() const noexcept
{
  QSqlQuery qry(db);
  [[maybe_unused]] auto res=qry.prepare("DELETE FROM urls WHERE category_id=?");
  auto categoryId=categoryList.key(ui->cboCategory->currentText());
  qry.addBindValue(categoryId, QSql::In);
  if(!qry.exec())
    return false;

  return true;
}

bool Widget::deleteAll() const noexcept
{
  if(deleteUrls()){
      if(deleteCategory())
        return true;
    }
  return false;
}

void Widget::verifyContextMenu() noexcept
{
  auto [res, count, errMessage] = verifyDeleteCategory();
//      qDebug()<<count;
  if(count == 0)
    delCategory->setDisabled(true);
  else
    delCategory->setEnabled(true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::closeEvent(QCloseEvent *event)
{
  writeSettings();
  event->accept();
}

