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
#include "publicurldialog.hpp"
#include "categorydialog.hpp"
#include "util/helper.hpp"
#include "swtablemodel.hpp"
#include <QDebug>
#include <QMenu>


Widget::Widget(QWidget *parent)
  : QWidget(parent), ui(new Ui::Widget),
    db_{QSqlDatabase::database(QStringLiteral("xxxConection"))}{
  ui->setupUi(this);
  userId_ = helperdb_.getUser_id(SW::Helper_t::defaultUser, SW::User::U_public);
  ui->lblIcon->setPixmap(QPixmap(QStringLiteral(":/img/7278151.png")).scaled(16,16));
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

  canRestoreDataBase();


  //action delete category
  QObject::connect(delCategory, &QAction::triggered, this, [&](){
      QMessageBox msgBox;
      msgBox.setWindowTitle(SW::Helper_t::appName().append(QStringLiteral(" - Advertencia")));
      msgBox.setText(QStringLiteral("<p style='color:#FB4934;'>"
                                    "<cite><strong>Esta a punto eliminar ésta categoría y todo su contenido.<br>"
                                    "Recuerde que al aceptar, eliminará de forma permanente estos datos.<br>"
                                    "Desea continuar y eliminar los datos?</strong></cite></p>"));
      msgBox.setIcon(QMessageBox::Warning);
      msgBox.addButton(QStringLiteral("Borrar categoría"), QMessageBox::AcceptRole);
      msgBox.addButton(QStringLiteral("Cancelar"), QMessageBox::RejectRole);

      if(msgBox.exec() == QMessageBox::RejectRole)
        return;
      if(deleteAll()){
          QMessageBox::information(this, SW::Helper_t::appName(),QStringLiteral("Datos eliminados."));
          ui->cboCategory->clear();
          loadListCategory(userId_);
          has_data();
          checkStatusContextMenu();
        }
    });

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //connect to slot to btnDeleteCategory
  QObject::connect(ui->btnDeleteCategory, &QPushButton::clicked, this, [&](){
      auto categoryId=categoryList.key(ui->cboCategory->currentText());
      auto [res, errMessage] = helperdb_.verifyDeleteCategory(categoryId);
      if(!res){
          QMessageBox::warning(this, SW::Helper_t::appName().append(QStringLiteral(" - Advertencia")),
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
                                      QStringLiteral("<p><cite>Esta seguro de eliminar esta categoría?</cite></p>")
                                      ,QMessageBox::Yes, QMessageBox::Cancel);
      if(ret==QMessageBox::Cancel)
        return;

      if(helperdb_.deleteCategory(categoryId)){
          QMessageBox::information(this, SW::Helper_t::appName(), QStringLiteral("Categoría eliminada!"));
          ui->cboCategory->clear();
          loadListCategory(userId_);
          has_data();
          checkStatusContextMenu();
        }
    });
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //connect to slots to btnAdd new url
  QObject::connect(ui->btnAdd,&QPushButton::clicked, this, [&](){

      if(ui->btnAdd->text().compare("Agregar") == 0){
          if(!SW::Helper_t::urlValidate(ui->txtUrl->text())){
              QMessageBox::warning(this, SW::Helper_t::appName(),
                                   QString("<p><cite>La dirección: <strong>\"%1\"</strong>, no es válida!<br>"
                                           "una dirección url válida debe tener una de las siguiente formas:"
                                           "<ol>"
                                           "<li><strong>http://www.url.dominio</strong></li>"
                                           "<li><strong>https://www.url.dominio</strong></li>"
                                           "<li><strong>http://url.dominio</strong></li>"
                                           "<li><strong>https://url.dominio</strong></li>"
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

          auto categoryId = categoryList.key(ui->cboCategory->currentText());

          if(helperdb_.urlExists(ui->txtUrl->text(), categoryId)){
              QMessageBox::warning(this, SW::Helper_t::appName(),
                                   QStringLiteral("<p>La url: <cite><strong>%1</strong></cite></p> ya esta registrada!!").arg(ui->txtUrl->text()));
              ui->txtUrl->selectAll();
              ui->txtUrl->setFocus(Qt::OtherFocusReason);
              return;
            }
          //get the key from categoryList, with current selected text to cboCategory.
          // auto categoryId = categoryList.key(ui->cboCategory->currentText());
          if(helperdb_.saveData_url(ui->txtUrl->text(), ui->pteDesc->toPlainText(), categoryId)){
              //              QMessageBox::information(this,SW::Helper_t::appName(),"Datos guardados!!");
              ui->txtUrl->clear();
              ui->pteDesc->clear();
              ui->txtUrl->setFocus(Qt::OtherFocusReason);
              setUpTable(categoryList.key(ui->cboCategory->currentText()));
              verifyContextMenu();
              checkStatusContextMenu();
              hastvUrlData();
            }
        }else{

          QSqlQuery qry(db_);
          [[maybe_unused]] auto res=qry.prepare(QStringLiteral("UPDATE  urls SET url=?, desc=? WHERE url_id=? AND categoryid=?"));
          auto encryptedData = SW::Helper_t::encrypt(ui->txtUrl->text());
          qry.addBindValue(encryptedData, QSql::In);
          qry.addBindValue(ui->pteDesc->toPlainText().simplified().toUpper(), QSql::In);
          auto currentRow = ui->tvUrl->currentIndex().row();
          auto id = ui->tvUrl->model()->index(currentRow,0).data().toInt();
          qry.addBindValue(id, QSql::In);
          auto categoryId = categoryList.key(ui->cboCategory->currentText());
          qry.addBindValue(categoryId, QSql::In);
          if(!qry.exec()){
              QMessageBox::critical(this, SW::Helper_t::appName(), QStringLiteral("Fallo la ejecución de la sentencia!\n")
                                    +qry.lastError().text());
              return;

            }
          setUpTable(categoryList.key(ui->cboCategory->currentText()));
          ui->btnAdd->setText(QStringLiteral("Agregar"));
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

      // auto userid = helperdb_.getUser_id(QStringLiteral("public"), SW::User::U_public);

      if(!helperdb_.saveCategoryData(newCategory.category(), newCategory.description(), userId_)){
          QMessageBox::critical(this, SW::Helper_t::appName(), QStringLiteral("Error alguardar los datos!\n"));
          return;
        }
      ui->cboCategory->clear();
      loadListCategory(userId_);
      has_data();
      checkStatusContextMenu();
      hastvUrlData();

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
          QMessageBox::information(this, SW::Helper_t::appName(), QStringLiteral("Datos actualizados!\n"));
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
  //btn edit url
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

      ( text == QStringLiteral("Modo Oscuro") ) ? setTheme(SW::Theme::Modo_Oscuro) : setTheme(SW::Theme::Modo_Claro);
      checkStatusSessionColor(text);


    });
  //cboCategory connection
  QObject::connect(ui->cboCategory, &QComboBox::currentIndexChanged, this, [&](){
      setUpTable(categoryList.key(ui->cboCategory->currentText()));
      verifyContextMenu();
      setCboCategoryToolTip();
      hastvUrlData();
      checkStatusContextMenu();
      //      has_data();
    });

  setCboCategoryToolTip();
  hastvUrlData();

  //conect btn login
  QObject::connect(ui->btnLogIn, &QToolButton::clicked, this, [&](){
      LogInDialog logDialog;
      if(logDialog.exec() == QDialog::Accepted){

          SW::Helper_t::current_user_ = logDialog.userName();

          // userId_ = helperdb_.getUser_id(logDialog.userName(),QStringLiteral("USER"));
          userId_ = helperdb_.getUser_id(SW::Helper_t::current_user_, SW::User::U_user);

          ui->cboCategory->clear();
          loadListCategory(userId_);

          // (ui->cboTheme->currentText() == QStringLiteral("Modo Oscuro") ) ? setLabelInfo(SW::Helper_t::darkModeColor.data(), logDialog.userName()) : setLabelInfo(SW::Helper_t::lightModeColor.data(), logDialog.userName());
          (ui->cboTheme->currentText() == QStringLiteral("Modo Oscuro") ) ? setLabelInfo(SW::Helper_t::darkModeColor.data(), SW::Helper_t::current_user_) : setLabelInfo(SW::Helper_t::lightModeColor.data(), SW::Helper_t::current_user_);
          ui->btnLogOut->setEnabled(true);
          ui->btnLogIn->setDisabled(true);
          // setWindowTitle(QApplication::applicationName().append(QStringLiteral(" - Sesión inicada como: ")+logDialog.userName()));
          setWindowTitle(QApplication::applicationName().append(QStringLiteral(" - Sesión inicada como: ").append(SW::Helper_t::current_user_)));
          ui->lblIcon->setPixmap(QPixmap(QStringLiteral(":/img/user.png")).scaled(16,16));
          SW::Helper_t::sessionStatus_ = SW::SessionStatus::Session_start;
          has_data();
          checkStatusContextMenu();
          canRestoreDataBase();
        }
    });

  //connect to button logout
  QObject::connect(ui->btnLogOut, &QToolButton::clicked, this, [&](){
      userId_ = helperdb_.getUser_id(SW::Helper_t::defaultUser, SW::User::U_public);
      (ui->cboTheme->currentText() == QStringLiteral("Modo Oscuro") ) ? setLabelInfo(SW::Helper_t::darkModeColor.data()) : setLabelInfo(SW::Helper_t::lightModeColor.data());
      ui->btnLogOut->setDisabled(true);
      ui->btnLogIn->setEnabled(true);
      setWindowTitle(QApplication::applicationName());
      ui->cboCategory->clear();
      loadListCategory(userId_);
      ui->lblIcon->setPixmap(QPixmap(QStringLiteral(":/img/7278151.png")).scaled(16,16));
      SW::Helper_t::sessionStatus_ = SW::SessionStatus::Session_closed;
      has_data();
      checkStatusContextMenu();
      SW::Helper_t::current_user_ = SW::Helper_t::defaultUser;
      canRestoreDataBase();
    });

  //connect boton crear copia de seguridad
  QObject::connect(ui->btnBackUp, &QToolButton::clicked, this, [&](){
      QProcess process;
      const auto path_app {SW::Helper_t::app_pathLocation().append("/tools/sqlite-tools-win-x64-3450100/sqlite3.exe")};
      QStringList argv{};
      auto databasePath = SW::Helper_t::AppLocalDataLocation().append(QStringLiteral("/xdatabase.db"));
      auto filePath = QFileDialog::getSaveFileName(this, QStringLiteral("Crear una copia de seguridad"), QDir::homePath(),
                                                   QStringLiteral("Archivos de copia de seguridad (*.bak)"));
      if(filePath.isEmpty())
        return;

      if(filePath.contains(' ')){
          QMessageBox::warning(this, SW::Helper_t::appName(), "El nombre del archivo no puede contener espacios.");
          return;
        }
      QFileInfo fileInfo(filePath);


      auto absolutePath{fileInfo.absolutePath()};
      auto baseName{fileInfo.baseName()};
      auto extension{fileInfo.completeSuffix()};

      auto fecha{QDateTime::currentDateTime().toString("yyyy-MM-dd_hhmmss")};
      QString path{".backup %1/%2-%3.%4"};


      argv << databasePath << path.arg(absolutePath,baseName,fecha,extension);



      if(!process.startDetached(path_app, argv)){
          QMessageBox::critical(this, SW::Helper_t::appName(), QStringLiteral("Error en la ejecución.\n")+
                                process.errorString());
          return;

        }
      QMessageBox::information(this, SW::Helper_t::appName(), QStringLiteral("La copia de seguridad fue creada."));


    });

  //restore database
  QObject::connect(ui->btnRestore, &QToolButton::clicked, this, [&](){

     const auto dbasePath{SW::Helper_t::AppLocalDataLocation().append(QStringLiteral("/xdatabase.db"))};

      // qInfo() << dbasePath <<'\n';
      // QFile file{dbasePath};

      if(!helperdb_.isDataBase_empty()){

          QMessageBox msg{this};
          msg.setWindowTitle(SW::Helper_t::appName());
          msg.setIcon(QMessageBox::Warning);
          msg.setText(QStringLiteral("<span>"
                                     "En éste momento hay una instancia de la base de datos en uso.<br/>"
                                     "<em>Tenga en cuenta que al restaurar la base de datos con una cópia de seguridad,<br/>"
                                     "se perderan todos los datos que tengan en éste momento, y seran reemplazados por los datos de la cópia.</em><br/><br/>"
                                     "<cite><strong>Consejo:</strong></cite>"
                                     "<ul><li>Tal vez antes de restaurar, una cópia de seguridad, quiera crear un backup, de la base de datos actual, para no perder los datos.</li></ul><br/>"
                                     "</span>"));
          msg.addButton(QStringLiteral("Restaurar la base de datos"), QMessageBox::AcceptRole);
          msg.addButton(QStringLiteral("Cancelar"), QMessageBox::RejectRole);

          if(msg.exec() == QMessageBox::RejectRole)
            return;
        }

      const auto pathBackup{QFileDialog::getOpenFileName(this, "Abrir archivo de respaldo", QDir::homePath(),
                                                         QStringLiteral("Archivo backup (*.bak)"))};
      if(pathBackup.isEmpty())
        return;

      auto db{QSqlDatabase::database("xxxConection")};

      if(db.isOpen())
        db.close();


      QStringList args{};
      QString cmd {".restore %1"};

      args << dbasePath << cmd.arg(pathBackup);

      const auto app{SW::Helper_t::app_pathLocation().append("/tools/sqlite-tools-win-x64-3450100/sqlite3.exe")};

      QProcess process{this};

      if(!process.startDetached(app, args)){
          QMessageBox::critical(this, SW::Helper_t::appName(), QStringLiteral("Error en la ejecución.\n")+
                                process.errorString());
          return;

        }
      QMessageBox::information(this, SW::Helper_t::appName(), QStringLiteral("La base de datos, fue restaurada"));
      db.open();
      ui->cboCategory->clear();
      loadListCategory(userId_);
      has_data();


    });

  //connect boton cancelar
  QObject::connect(ui->btnCancel, &QAbstractButton::clicked, this, [&](){

      ui->txtUrl->clear();
      ui->pteDesc->clear();
      ui->btnCancel->setDisabled(true);
      ui->txtUrl->setFocus();
      editAction(false);
      ui->btnAdd->setText(QStringLiteral("Agregar"));

    });
  QObject::connect(showDescDetail_, &QAction::triggered, this, &Widget::showAlldescription);
  QObject::connect(showPublicUrl_, &QAction::triggered, this, [&](){
      PublicUrlDialog publicDialog(this);
      publicDialog.setWindowTitle("Direcciones url públicas");

      publicDialog.exec();
    });
  QObject::connect(moveUrl_, &QAction::triggered, this, [this](){


      auto currentRow_ = ui->tvUrl->currentIndex().row();
      auto url_ = xxxModel_->index(currentRow_, 1).data().toString();
      auto currentCategoryId_ = categoryList.key(ui->cboCategory->currentText());
      auto urlid = xxxModel_->index(currentRow_, 0).data().toUInt();
      auto data_ = categoryList;
      data_.remove(currentCategoryId_);

      CategoryDialog cDialog(data_, this);
      cDialog.setWindowTitle(QStringLiteral("Mover url a otra categoría"));

      if(cDialog.exec() == QDialog::Accepted){

          auto categoryid = cDialog.getCategoryId();
          if(helperdb_.urlExists(url_, categoryid)){

              QMessageBox::warning(this, SW::Helper_t::appName(),
                                   QStringLiteral("<p>"
                                                  "La url: <cite>"
                                                  "<strong>%1</strong>"
                                                  "</cite>"
                                                  "</p> ya esta registrada, en la categoría a la que desea mover!!").arg(url_));
              return;

            }
          // qInfo() <<categoryid;

          if(!helperdb_.moveUrlToOtherCategory(categoryid, urlid)){
              QMessageBox::critical(this, SW::Helper_t::appName(), QStringLiteral("Error al intentar actualizar.\n"));
              return;
            }
          setUpTable(categoryList.key(ui->cboCategory->currentText()));
          hastvUrlData();
        }

    });

  qInfo() << helperdb_.isDataBase_empty();

}//Fin del constructor
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
Widget::~Widget()
{
  delete ui;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::initFrm() noexcept{

  ui->txtUrl->setPlaceholderText(QStringLiteral("(http | https://)www.url.com"));
  ui->pteDesc->setPlaceholderText(QStringLiteral("Description to url's"));
  ui->btnNewCategory->setToolTip(QStringLiteral("New Category!"));
  ui->btnEditCategory->setToolTip(QStringLiteral("Edit Category Data!"));
  //btnAdd disabled
  ui->btnAdd->setDisabled(true);
  ui->btnLogIn->setShortcut(QKeySequence("Ctrl+L"));
  ui->btnLogOut->setDisabled(true);
  ui->btnLogOut->setShortcut(QKeySequence("Ctrl+Q"));

  ui->btnCancel->setDisabled(true);

  //set shortcuts, to button box url
  ui->btnAdd->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_A));
  ui->btnEdit->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_E));
  ui->btnQuit->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Q));
  ui->btnopen->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_O));
  ui->btnCancel->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_C));

  //set the focus to txturl control
  ui->txtUrl->setFocus(Qt::OtherFocusReason);


}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::editAction(bool op) noexcept{

  ui->btnEdit->setDisabled(op);
  ui->btnQuit->setDisabled(op);
  ui->btnopen->setDisabled(op);
  ui->tvUrl->setDisabled(op);

  ui->btnCancel->setEnabled(op);


}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Widget::validateSelectedRow() noexcept{
  if(!ui->tvUrl->selectionModel()->hasSelection()){
      QMessageBox::warning(this, SW::Helper_t::appName(), QStringLiteral("Seleccione una fila!\n"));
      return false;
    }
  return true;

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void Widget::setUpTable(uint32_t categoryId) noexcept{

  // xxxModel_ = new QSqlTableModel(this, db_);
  // xxxModel_->setTable(QStringLiteral("urls"));
  // xxxModel_->setFilter(QString("categoryid=%1").arg(categoryId));

  // xxxModel_->select();

  // ui->tvUrl->setModel(xxxModel_);

  xxxModel_ = new SWTableModel(this, db_);
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
  ui->btnAdd->setText(QStringLiteral("Actualizar"));

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

void Widget::canRestoreDataBase() const noexcept{

  (static_cast<bool>(SW::Helper_t::sessionStatus_)) ? ui->btnRestore->setVisible(true) : ui->btnRestore->setVisible(false);
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
  settings.beginGroup(QStringLiteral("Theme"));
  SW::Theme theme;
  QString color;
  if(ui->cboTheme->currentText().compare("Modo Claro") == 0){
      theme=SW::Theme::Modo_Claro;
      color=SW::Helper_t::lightModeColor.data();
    }else{
      theme=SW::Theme::Modo_Oscuro;
      color=SW::Helper_t::darkModeColor.data();
    }


  settings.setValue(QStringLiteral("theme Value"), static_cast<uint32_t>(theme));
  settings.setValue(QStringLiteral("lblColor"), SW::Helper_t::setColorReg(color));
  settings.endGroup();
  settings.setValue(QStringLiteral("position"), saveGeometry());
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::readSettings() noexcept{
  QSettings settings(qApp->organizationName(), SW::Helper_t::appName());
  settings.beginGroup(QStringLiteral("Theme"));
  auto theme = settings.value(QStringLiteral("theme Value")).toUInt();
  setLabelInfo(SW::Helper_t::getColorReg(settings.value(QStringLiteral("lblColor")).toByteArray()));

  setTheme(static_cast<SW::Theme>(theme));
  settings.endGroup();

  ui->cboTheme->setCurrentIndex(static_cast<int>(theme));
  restoreGeometry(settings.value(QStringLiteral("position")).toByteArray());
}

void Widget::showAlldescription() noexcept{

  // auto* otherModel = dynamic_cast<QSqlTableModel*>(ui->tvUrl->model());
  auto row = ui->tvUrl->currentIndex().row();
  auto desc = ui->tvUrl->model()->index(row,2).data().toString();
  auto url = ui->tvUrl->model()->index(row,1).data().toString();

  QMessageBox msgDescription;

  // msgDescription.setIcon(QMessageBox::Information);
  QPixmap pixMap(QStringLiteral(":/img/desc.png"));
  // pixMap.scaled(32,32);
  msgDescription.setWindowTitle(qApp->applicationName().append(QStringLiteral(" - Descripción completa de la URL")));
  msgDescription.setIconPixmap(pixMap.scaled(64, 64));
  msgDescription.setText(desc);
  msgDescription.setDetailedText(url);
  msgDescription.addButton(QStringLiteral("Cerrar descripción"), QMessageBox::AcceptRole);

  msgDescription.exec();

  // QMessageBox::information(this, qApp->applicationName(), desc);

}

void Widget::checkStatusContextMenu(){
  (SW::Helper_t::sessionStatus_ == SW::SessionStatus::Session_closed) ? showPublicUrl_->setDisabled(true) : showPublicUrl_->setDisabled(false);
  (categoryList.count() > 1) ? moveUrl_->setEnabled(true) : moveUrl_->setDisabled(true);

}

void Widget::checkStatusSessionColor(const QString& text){

  if(SW::Helper_t::sessionStatus_ == SW::SessionStatus::Session_start){
      ( text == QStringLiteral("Modo Oscuro") ) ? setLabelInfo(SW::Helper_t::darkModeColor.data(), SW::Helper_t::current_user_):
                                                  setLabelInfo(SW::Helper_t::lightModeColor.data(), SW::Helper_t::current_user_);
    }else{
      ( text == QStringLiteral("Modo Claro") ) ? setLabelInfo(SW::Helper_t::lightModeColor.data(), SW::Helper_t::current_user_):
                                                 setLabelInfo(SW::Helper_t::darkModeColor.data(), SW::Helper_t::current_user_);
    }

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void Widget::setLabelInfo(const QString& color, const QString& userName) noexcept{

  // auto userName_ = SW::Helper_t::currentUser_.value(userName);

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
  QIcon icon(QStringLiteral(":/img/118277.png"));
  ui->cboCategory->setContextMenuPolicy(Qt::ActionsContextMenu);
  delCategory = new QAction(icon, QStringLiteral("Forzar eliminación de categoría"),this);
  ui->cboCategory->addAction(delCategory);


}

void Widget::setUptvUrlContextMenu() noexcept{

  // ui->tvUrl->setContextMenuPolicy(Qt::ActionsContextMenu);
  contextMenu = new QMenu(this);

  openUrl_ = contextMenu->addAction(QIcon(QStringLiteral(":/img/openurl.png")), QStringLiteral("Abrir url en el navegador"));
  editUrl_ = contextMenu->addAction(QIcon(QStringLiteral(":/img/editurl.png")), QStringLiteral("Editar url"));
  quittUrl_ = contextMenu->addAction(QIcon(QStringLiteral(":/img/quiturl.png")), QStringLiteral("Quitar url"));
  contextMenu->addSeparator();
  showDescDetail_ = contextMenu->addAction(QStringLiteral("Ver descripción de URL completa"));

  contextMenu->addSeparator();
  showPublicUrl_ = contextMenu->addAction(QStringLiteral("Ver url's públicas"));

  contextMenu->addSeparator();
  moveUrl_ = contextMenu->addAction(QStringLiteral("Mover url, a otra categoría"));

  ui->tvUrl->installEventFilter(this);

  checkStatusContextMenu();

  // ui->tvUrl->addAction(openUrl_);
  // ui->tvUrl->addAction(editUrl_);
  // ui->tvUrl->addAction(quittUrl_);


}

bool Widget::eventFilter(QObject* watched, QEvent* event){
  if(watched == ui->tvUrl && event->type() == QEvent::ContextMenu){
      QContextMenuEvent* contextMenuEvent = static_cast<QContextMenuEvent*>(event);
      contextMenu->exec(contextMenuEvent->globalPos());
    }
  return QWidget::eventFilter(watched, event);
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
      ui->cboCategory->setToolTip(QStringLiteral("<p><cite><strong>Descripción de la categoría:</strong><br><br>"
                                                 "Esta categoría no cuenta con una descripción!</cite></p>"));
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
      QMessageBox::critical(this, SW::Helper_t::appName(), QStringLiteral("Fallo al intentar abrir dirección url!\n"));
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
  msgBox.addButton(QStringLiteral("Eliminar"),QMessageBox::AcceptRole);
  msgBox.addButton(QStringLiteral("Cancelar"),QMessageBox::RejectRole);
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
      // moveUrl_->setDisabled(true);
      showDescDetail_->setDisabled(true);
    }else{
      openUrl_->setEnabled(true);
      ui->btnopen->setEnabled(true);
      ui->btnEdit->setEnabled(true);
      ui->btnQuit->setEnabled(true);
      editUrl_->setEnabled(true);
      quittUrl_->setEnabled(true);
      // moveUrl_->setEnabled(true);
      showDescDetail_->setEnabled(true);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::closeEvent(QCloseEvent *event){
  if(SW::Helper_t::sessionStatus_ == SW::SessionStatus::Session_start){
      QMessageBox::warning(this, SW::Helper_t::appName(),
                           QStringLiteral("<cite>Hay una sesión activa en este momento.<br>"
                                          "Necesita cerrar sesión primero antes de salir, "
                                          "haciendo click en el boton:<br>"
                                          "<cite><strong style='background:#FFFF00;color:#FF5500;'>Cerrar sesión</strong></cite><br>"
                                          "O presionando la combinación de teclas Ctrl+Q.</cite>"));
      event->ignore();
      return;
    }
  writeSettings();
  event->accept();
}



