#include "widget.hpp"
#include "./ui_widget.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSettings>
#include <QAction>
#include <QFileDialog>
#include <QSqlTableModel>
//#include <QProcess>
#include "dlgnewcategory.hpp"
#include "logindialog.hpp"
#include "util/helper.hpp"
#include <QDebug>


Widget::Widget(QWidget *parent)
  : QWidget(parent), ui(new Ui::Widget),
    db_{QSqlDatabase::database("xxxConection")}{
  ui->setupUi(this);
  userId_ = helperdb_.getUser_id("public");
  ui->lblIcon->setPixmap(QPixmap(":/img/7278151.png").scaled(16,16));
  initFrm();

  loadListCategory(userId_);
  setUpTable(categoryList.key(ui->cboCategory->currentText()));

  has_data();

  readSettings();

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /// ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //context menu implementation
  setUpCboCategoryContextMenu();
  setUptvUrlContextMenu();
  verifyContextMenu();


  //action delete category
  QObject::connect(delCategory, &QAction::triggered, this, [&](){
      QMessageBox msgBox;
      msgBox.setWindowTitle(SW::Helper_t::appName().append(" - Advertencia"));
      msgBox.setText("<p style='color:#FB4934;'>"
                     "<cite><strong>Esta a punto eliminar ésta categoría y todo su contenido.<br>"
                     "Recuerde que al aceptar, eliminará de forma permanente estos datos.<br>"
                     "Desea continuar y eliminar los datos?</strong></cite></p>");
      msgBox.setIcon(QMessageBox::Warning);
      msgBox.addButton("Borrar categoría", QMessageBox::AcceptRole);
      msgBox.addButton("Cancelar", QMessageBox::RejectRole);

      if(msgBox.exec() == QMessageBox::RejectRole)
        return;
      if(deleteAll()){
          QMessageBox::information(this, SW::Helper_t::appName(),"Datos eliminados.");
          ui->cboCategory->clear();
          loadListCategory(userId_);
          has_data();
        }
    });

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //connect to slot to btnDeleteCategory
  QObject::connect(ui->btnDeleteCategory, &QPushButton::clicked, this, [&](){
      auto categoryId=categoryList.key(ui->cboCategory->currentText());
      auto [res, errMessage] = helperdb_.verifyDeleteCategory(categoryId);
      if(!res){
          QMessageBox::warning(this, SW::Helper_t::appName().append(" - Advertencia"),
                               QString("<p>"
                                       "<cite>"
                                       "No se puede eliminar ésta categoría.<br>"
                                       "Esto es debido a que ésta categoría tiene asociado uno o mas"
                                       " elementos.<br><br>"
                                       "<strong>Sugerencia:"
                                       "</strong><br>"
                                       "Si desea eliminar una categoría y todo su contenido, "
                                       "puede optar por dar click derecho sobre el nombre de la categoría y"
                                       "del menú contextual elegir:<br><br>"
                                       "<strong>"
                                       "<mark style='background:#FFFF00;color:#FF5500;'>"
                                       "->Forzar eliminación de categoría.</mark>"
                                       "</strong>"
                                       "</cite>"
                                       "</p>"));
          //            qDebug()<<count<<'\n';
          return;

        }
      auto ret = QMessageBox::warning(this, SW::Helper_t::appName(),
                                      "<p><cite>Esta seguro de eliminar esta categoría?</cite></p>"
                                      ,QMessageBox::Yes, QMessageBox::Cancel);
      if(ret==QMessageBox::Cancel)
        return;

      if(helperdb_.deleteCategory(categoryId)){
          QMessageBox::information(this, SW::Helper_t::appName(), "Categoría eliminada!");
          ui->cboCategory->clear();
          loadListCategory(userId_);
          has_data();
        }
    });
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //connect to slots to btnAdd new url
  QObject::connect(ui->btnAdd,&QPushButton::clicked, this, [&](){

      if(ui->btnAdd->text().compare("&Add") == 0){
          if(!SW::Helper_t::urlValidate(ui->txtUrl->text())){
              QMessageBox::warning(this, SW::Helper_t::appName(),
                                   QString("<p><cite>La dirección: <strong>\"%1\"</strong>, no es válida!<br>"
                                           "una dirección url válida debe tener una de las siguiente formas:"
                                           "<ol>"
                                           "<li><strong>http://www.url.dominio</strong></li>"
                                           "<li><strong>https://www.url.dominio</strong></li>"
                                           "<li><strong>http://www.url.dominio</strong></li>"
                                           "<li><strong>https://www.url.dominio</strong></li>"
                                           "</ol>"
                                           "Tenga en cuenta que para éste sistema:<br>"
                                           "http:// o https:// son requeridos.<br>"
                                           "Lo mínimo que se espera es <strong>(http | https)://www.url.domino</strong><br>"
                                           "<strong>Siendo (www.)</strong> opcional!"
                                           "</cite></p>").arg(ui->txtUrl->text()));
              ui->txtUrl->selectAll();
              ui->txtUrl->setFocus(Qt::OtherFocusReason);
              return;
            }
          //get the key from categoryList, with current selected text to cboCategory.
          auto categoryId = categoryList.key(ui->cboCategory->currentText());
          if(helperdb_.saveData_url(ui->txtUrl->text(), ui->pteDesc->toPlainText(), categoryId)){
              //              QMessageBox::information(this,SW::Helper_t::appName(),"Datos guardados!!");
              ui->txtUrl->clear();
              ui->pteDesc->clear();
              ui->txtUrl->setFocus(Qt::OtherFocusReason);
              setUpTable(categoryList.key(ui->cboCategory->currentText()));
              verifyContextMenu();
              hastvUrlData();
            }
        }else{

          QSqlQuery qry(db_);
          [[maybe_unused]] auto res=qry.prepare("UPDATE  urls SET url=?, desc=? WHERE url_id=? AND categoryid=?");
          qry.addBindValue(ui->txtUrl->text(), QSql::In);
          qry.addBindValue(ui->pteDesc->toPlainText().simplified().toUpper(), QSql::In);
          auto currentRow = ui->tvUrl->currentIndex().row();
          auto id = ui->tvUrl->model()->index(currentRow,0).data().toInt();
          qry.addBindValue(id, QSql::In);
          auto categoryId = categoryList.key(ui->cboCategory->currentText());
          qry.addBindValue(categoryId, QSql::In);
          if(!qry.exec()){
              QMessageBox::critical(this, SW::Helper_t::appName(), "Fallo la ejecución de la sentencia!\n"
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
      dlgNewCategory newCategory(dlgNewCategory::OpenMode::New, QStringList{}, this);

      if(newCategory.exec() == QDialog::Rejected)
        return;

      if(!helperdb_.saveCategoryData(newCategory.category(), newCategory.description(), userId_)){
          QMessageBox::critical(this, SW::Helper_t::appName(), "Error alguardar los datos!\n");
          return;
        }
      ui->cboCategory->clear();
      loadListCategory(userId_);
      has_data();

    });
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //btn edit category
  QObject::connect(ui->btnEditCategory, &QPushButton::clicked, this, [&](){

      auto id = categoryList.key(ui->cboCategory->currentText());
      QStringList data = helperdb_.dataCategory(id);
      dlgNewCategory editCategory(dlgNewCategory::OpenMode::Edit, data, this);
      if(editCategory.exec() == QDialog::Rejected){
          return;
        }
      if(helperdb_.updateCategory(editCategory.category(), editCategory.description(), id, userId_)){
          QMessageBox::information(this, SW::Helper_t::appName(), "Datos actualizados!\n");
          ui->cboCategory->clear();
          loadListCategory(userId_);
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

      quitUrl();

      hastvUrlData();

    });

  QObject::connect(quittUrl_, &QAction::triggered,this, [&](){
      if(!validateSelectedRow()) return;

      quitUrl();
      hastvUrlData();

    });
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //btn edit
  QObject::connect(ui->btnEdit, &QPushButton::clicked, this, &Widget::btnEdit);

  QObject::connect(editUrl_, &QAction::triggered, this, &Widget::btnEdit);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //btnopenUrl
  QObject::connect(ui->btnopen, &QPushButton::clicked, this, [&](){
      if(!validateSelectedRow()) return;

      openUrl();


    });
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  QObject::connect(openUrl_, &QAction::triggered, this, [&](){
      if(!validateSelectedRow()) return;

      openUrl();
    });

  //cboTheme
  QObject::connect(ui->cboTheme, &QComboBox::currentTextChanged, this, [&](const QString& text){
      if(text.compare("Modo Oscuro") == 0){
          setTheme(SW::Theme::Modo_Oscuro);
          setLabelInfo(SW::Helper_t::darkModeColor.data());
        }else{
          setTheme(SW::Theme::Modo_Claro);
          setLabelInfo(SW::Helper_t::lightModeColor.data());
        }

    });
  //cboCategory connection
  QObject::connect(ui->cboCategory, &QComboBox::currentIndexChanged, this, [&](){
      setUpTable(categoryList.key(ui->cboCategory->currentText()));
      verifyContextMenu();
      setCboCategoryToolTip();
      hastvUrlData();
      //      has_data();
    });

  setCboCategoryToolTip();
  hastvUrlData();

  //conect btn login
  QObject::connect(ui->btnLogIn, &QToolButton::clicked, this, [&](){
      LogInDialog logDialog;
      if(logDialog.exec() == QDialog::Accepted){

          userId_ = helperdb_.getUser_id(logDialog.userName(),"USER");

          ui->cboCategory->clear();
          loadListCategory(userId_);

          (ui->cboTheme->currentText() == "Modo Oscuro" ) ? setLabelInfo(SW::Helper_t::darkModeColor.data(), logDialog.userName()) : setLabelInfo(SW::Helper_t::lightModeColor.data(), logDialog.userName());
          ui->btnLogOut->setEnabled(true);
          ui->btnLogIn->setDisabled(true);
          setWindowTitle(QApplication::applicationName().append(" - Sesión inicada como: "+logDialog.userName()));
          ui->lblIcon->setPixmap(QPixmap(":/img/user.png").scaled(16,16));
          sessionStatus_ = SW::SessionStatus::Session_start;
          has_data();
        }
    });

  //connect to button logout
  QObject::connect(ui->btnLogOut, &QToolButton::clicked, this, [&](){
      userId_ = helperdb_.getUser_id("public");
      (ui->cboTheme->currentText() == "Modo Oscuro" ) ? setLabelInfo(SW::Helper_t::darkModeColor.data()) : setLabelInfo(SW::Helper_t::lightModeColor.data());
      ui->btnLogOut->setDisabled(true);
      ui->btnLogIn->setEnabled(true);
      setWindowTitle(QApplication::applicationName());
      ui->cboCategory->clear();
      loadListCategory(userId_);
      ui->lblIcon->setPixmap(QPixmap(":/img/7278151.png").scaled(16,16));
      sessionStatus_ = SW::SessionStatus::Session_closed;
      has_data();
    });

  //boton crear copia de seguridad
  QObject::connect(ui->btnBackUp, &QToolButton::clicked, this, [&](){
      auto databasePath = SW::Helper_t::AppLocalDataLocation().append("/xdatabase.db");
      auto filePath = QFileDialog::getSaveFileName(this, "Crear una copia de seguridad", QDir::rootPath(),
                                                   "Archivos de copia de seguridad (*.bak)");
      if(filePath.isEmpty())
        return;

      auto path_app = SW::Helper_t::app_pathLocation().append("/exec/.dump.bat");
      if(path_app.isEmpty())
        return;

      QStringList argv{};
      argv << databasePath << filePath;

      QProcess process;

      if(!process.startDetached(path_app, argv)){
          QMessageBox::critical(this, SW::Helper_t::appName(), "Error en la ejecución.\n"+
                                process.errorString());
          return;

        }
      QMessageBox::information(this, SW::Helper_t::appName(), "La copia de seguridad fue creada.");


    });

}//Fin del constructor
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

Widget::~Widget()
{
  delete ui;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::initFrm() noexcept{

  ui->txtUrl->setPlaceholderText("(http | https://)www.url.com");
  ui->pteDesc->setPlaceholderText("Description to url's");
  ui->btnNewCategory->setToolTip("New Category!");
  ui->btnEditCategory->setToolTip("Edit Category Data!");
  //btnAdd disabled
  ui->btnAdd->setDisabled(true);
  ui->btnLogIn->setShortcut(QKeySequence("Ctrl+L"));
  ui->btnLogOut->setDisabled(true);
  ui->btnLogOut->setShortcut(QKeySequence("Ctrl+Q"));


}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::editAction(bool op) noexcept{

  ui->btnEdit->setDisabled(op);
  ui->btnQuit->setDisabled(op);
  ui->btnopen->setDisabled(op);
  ui->tvUrl->setDisabled(op);


}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Widget::validateSelectedRow() noexcept{
  if(!ui->tvUrl->selectionModel()->hasSelection()){
      QMessageBox::warning(this, SW::Helper_t::appName(), "Seleccione una fila!\n");
      return false;
    }
  return true;

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void Widget::setUpTable(uint32_t categoryId) noexcept{

  xxxModel_ = new QSqlTableModel(this, db_);
  xxxModel_->setTable("urls");
  xxxModel_->setFilter(QString("categoryid=%1").arg(categoryId));

  xxxModel_->select();

  ui->tvUrl->setModel(xxxModel_);
  setUpTableHeaders();

  QSqlQuery reuseQry = xxxModel_->query();

  if(reuseQry.exec()){
      while(reuseQry.next()){
          urlList.insert(reuseQry.value(0).toUInt(),reuseQry.value(1).toString());
        }
    }

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::setUpTableHeaders() const noexcept{
  ui->tvUrl->hideColumn(0);
  ui->tvUrl->hideColumn(3);
  ui->tvUrl->model()->setHeaderData(1,Qt::Horizontal, "Dirección URL");
  ui->tvUrl->model()->setHeaderData(2,Qt::Horizontal, "Descripción");
  ui->tvUrl->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  ui->tvUrl->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);


}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::btnEdit() noexcept{
  if( !validateSelectedRow() ) return;

  auto currentRow = ui->tvUrl->currentIndex().row();
  ui->txtUrl->setText(ui->tvUrl->model()->index(currentRow,1).data().toString());
  ui->pteDesc->setPlainText(ui->tvUrl->model()->index(currentRow,2).data().toString());
  editAction(true);
  ui->txtUrl->selectAll();
  ui->txtUrl->setFocus(Qt::OtherFocusReason);
  ui->btnAdd->setText("&Update");

}

void  Widget::has_data() noexcept{
  if(categoryList.isEmpty()){
      ui->btnEditCategory->setDisabled(true);
      ui->btnDeleteCategory->setDisabled(true);
      ui->txtUrl->setDisabled(true);
      ui->pteDesc->setDisabled(true);

    }else{
      ui->btnEditCategory->setEnabled(true);
      ui->btnDeleteCategory->setEnabled(true);
      ui->txtUrl->setEnabled(true);
      ui->pteDesc->setEnabled(true);
    }

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::setTheme(SW::Theme theme) const noexcept{

  qApp->setPalette(SW::Helper_t::set_Theme(theme));

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::writeSettings() const noexcept{
  QSettings settings(qApp->organizationName(), SW::Helper_t::appName());
  settings.beginGroup("Theme");
  SW::Theme theme;
  QString color;
  if(ui->cboTheme->currentText().compare("Modo Claro") == 0){
      theme=SW::Theme::Modo_Claro;
      color=SW::Helper_t::lightModeColor.data();
    }else{
      theme=SW::Theme::Modo_Oscuro;
      color=SW::Helper_t::darkModeColor.data();
    }


  settings.setValue("theme Value", static_cast<uint32_t>(theme));
  settings.setValue("lblColor", SW::Helper_t::setColorReg(color));
  settings.endGroup();
  settings.setValue("position", saveGeometry());
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::readSettings() noexcept{
  QSettings settings(qApp->organizationName(), SW::Helper_t::appName());
  settings.beginGroup("Theme");
  auto theme = settings.value("theme Value").toUInt();
  setLabelInfo(SW::Helper_t::getColorReg(settings.value("lblColor").toByteArray()));

  setTheme(static_cast<SW::Theme>(theme));
  settings.endGroup();

  ui->cboTheme->setCurrentIndex(static_cast<int>(theme));
  restoreGeometry(settings.value("position").toByteArray());
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void Widget::setLabelInfo(const QString& color, const QString& userName) noexcept{
  ui->lblInfo->setText(QString("<span style='color:%1;'>"
                               "<strong>SWSystem's - Lincoln Ingaroca"
                               "</strong></span>").arg(color));


  ui->lblState->setText(QString("<span style='color:%1;'>"
                                "<strong>User: %2"
                                "</strong></span>").arg(color, userName));

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::loadListCategory(uint32_t user_id) noexcept{

  categoryList = helperdb_.loadList_Category(user_id);
  ui->cboCategory->addItems(categoryList.values());
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::setUpCboCategoryContextMenu() noexcept{
  QIcon icon(":/img/118277.png");
  ui->cboCategory->setContextMenuPolicy(Qt::ActionsContextMenu);
  delCategory = new QAction(icon, "Forzar eliminación de categoría",this);
  ui->cboCategory->addAction(delCategory);


}

void Widget::setUptvUrlContextMenu() noexcept{

  ui->tvUrl->setContextMenuPolicy(Qt::ActionsContextMenu);

  openUrl_ = new QAction(QIcon(":/img/openurl.png"), "Open Url", this);
  editUrl_ = new QAction(QIcon(":/img/editurl.png"), "Edit url", this);
  quittUrl_ = new QAction(QIcon(":/img/quiturl.png"), "Quit url", this);

  ui->tvUrl->addAction(openUrl_);
  ui->tvUrl->addAction(editUrl_);
  ui->tvUrl->addAction(quittUrl_);


}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////


bool Widget::deleteAll() noexcept{
  auto categoryId=categoryList.key(ui->cboCategory->currentText());
  if(helperdb_.deleteUrls(1, categoryId)){
      if(helperdb_.deleteCategory(categoryId))
        return true;
    }
  return false;
}

void Widget::verifyContextMenu() noexcept{
  auto categoryId=categoryList.key(ui->cboCategory->currentText());
  auto [res, errMessage] = helperdb_.verifyDeleteCategory(categoryId);
  //      qDebug()<<count;
  (res) ? delCategory->setDisabled(true) : delCategory->setEnabled(true);
}

void Widget::setCboCategoryToolTip() noexcept{
  auto id = categoryList.key(ui->cboCategory->currentText());
  QStringList categoryData = helperdb_.dataCategory(id);
  auto desc=categoryData.value(1);
  //  QString desc{};
  if(desc.isEmpty()){
      ui->cboCategory->setToolTip("<p><cite><strong>Descripción de la categoría:</strong><br><br>"
                                  "Esta categoría no cuenta con una descripción!</cite></p>");
      return;
    }

  ui->cboCategory->setToolTip(QString("<p>"
                                      "<cite><strong>Descripción de la categoría:</strong>"
                                      "<br><br>%1</cite></p>").arg(desc));

}

void Widget::openUrl() noexcept{
  auto currentRow = ui->tvUrl->currentIndex().row();
  auto url = ui->tvUrl->model()->index(currentRow, 1).data().toString();
  if(!SW::Helper_t::open_Url(QUrl(url))){
      QMessageBox::critical(this, SW::Helper_t::appName(), "Fallo al intentar abrir dirección url!\n");
      return;
    }
}

void Widget::quitUrl() noexcept{
  auto currentRow = ui->tvUrl->currentIndex().row();
  auto url = ui->tvUrl->model()->index(currentRow, 1).data().toString();

  QMessageBox msgBox;
  msgBox.setText(QString("<span>Confirma que desea eliminar esta dirección:<br>"
                         " <cite style='color:#ff0800;'><strong>%1</strong></cite></span>").arg(url));
  msgBox.setIcon(QMessageBox::Question);
  msgBox.addButton("Eliminar",QMessageBox::AcceptRole);
  msgBox.addButton("Cancelar",QMessageBox::RejectRole);
  if(msgBox.exec() == QMessageBox::AcceptRole){
      //      auto currentRow = ui->tvUrl->currentIndex().row();
      //      auto url = ui->tvUrl->model()->index(currentRow, 1).data().toString();
      auto urlId=urlList.key(url);
      if(helperdb_.deleteUrls(2, 0, urlId)){
          ui->tvUrl->model()->removeRow(ui->tvUrl->currentIndex().row());
          setUpTable(categoryList.key(ui->cboCategory->currentText()));
        }

    }
}

void Widget::hastvUrlData() noexcept{
  if(ui->tvUrl->model()->rowCount() == 0){
      openUrl_->setDisabled(true);
      ui->btnopen->setDisabled(true);
      ui->btnEdit->setDisabled(true);
      ui->btnQuit->setDisabled(true);
      editUrl_->setDisabled(true);
      quittUrl_->setDisabled(true);
    }else{
      openUrl_->setEnabled(true);
      ui->btnopen->setEnabled(true);
      ui->btnEdit->setEnabled(true);
      ui->btnQuit->setEnabled(true);
      editUrl_->setEnabled(true);
      quittUrl_->setEnabled(true);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::closeEvent(QCloseEvent *event){
  if(sessionStatus_ == SW::SessionStatus::Session_start){
      QMessageBox::warning(this, SW::Helper_t::appName(),
                           "<cite>Hay una sesión activa en este momento.<br>"
                           "Necesita cerrar sesión primero antes de salir, "
                           "haciendo click en el boton:<br>"
                           "<cite><strong style='background:#FFFF00;color:#FF5500;'>Cerrar sesión</strong></cite><br>"
                           "O presionando la combinación de teclas Ctrl+Q.</cite>");
      event->ignore();
      return;
    }
  writeSettings();
  event->accept();
}



