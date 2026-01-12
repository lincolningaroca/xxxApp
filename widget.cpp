#include "widget.hpp"
#include "./ui_widget.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSettings>
#include <QAction>
#include <QFileDialog>
#include <QSqlTableModel>
#include <QFile>
#include <QProcess>
#include <QStyleHints>
#include <QMenu>

#include "dlgnewcategory.hpp"
#include "acercadedialog.hpp"
#include "logindialog.hpp"
#include "publicurldialog.hpp"
#include "categorydialog.hpp"
#include "swtablemodel.hpp"
#include "swlabel.hpp"
#include "util/excelexporter.hpp"
#include "resetpassworddialog.hpp"


Widget::Widget(QWidget *parent)
  : QWidget(parent), ui(new Ui::Widget),
  db_{QSqlDatabase::database(QStringLiteral("xxxConection"))}{

  ui->setupUi(this);

  userId_ = helperdb_.getUser_id(SW::Helper_t::defaultUser, SW::User::U_public);
  ui->lblIcon->setPixmap(QPixmap(QStringLiteral(":/img/7278151.png")).scaled(16,16));

  initFrm();

  loadListCategory(userId_);

  loadThemeComboBox();
  setUpTable(categoryList.key(ui->cboCategory->currentText()));
  canCreateBackUp();
  canStartSession();

  has_data();



  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /// ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //context menu implementation
  setUpCboCategoryContextMenu();
  setUptvUrlContextMenu();
  verifyContextMenu();

  canRestoreDataBase();

  QObject::connect(ui->lblInfo, &SWLabel::clicked, this, &Widget::load_aboutDialog);

  //connect to btnResetPassword

  QObject::connect(ui->btnResetPassword, &QToolButton::clicked, this, &Widget::load_resetPasswordDialog);


  //conect to export to excel file
  QObject::connect(exportToExcelFile_, &QAction::triggered, this, &Widget::exportToExcel);

  //action delete category
  QObject::connect(delCategory_, &QAction::triggered, this, &Widget::deleteCategory);

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //connect to slot to btnDeleteCategory
  QObject::connect(ui->btnDeleteCategory, &QPushButton::clicked, this, &Widget::delete_category);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //connect to slots to btnAdd new url
  QObject::connect(ui->btnAdd,&QPushButton::clicked, this, &Widget::addNewUrl);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

  //btnAddNewCategory
  QObject::connect(ui->btnNewCategory, &QPushButton::clicked, this, &Widget::newCategory);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //btn edit category
  QObject::connect(ui->btnEditCategory, &QPushButton::clicked, this, &Widget::editCategory);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

  //connect to slots to txtUrl
  QObject::connect(ui->txtUrl, &QLineEdit::textChanged, this, [this](const QString& text){
    (!text.simplified().isEmpty()) ? ui->btnAdd->setEnabled(true) : ui->btnAdd->setDisabled(true);
  });
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

  //connect to slots to btnQuit

  QObject::connect(ui->btnQuit, &QPushButton::clicked,this, &Widget::quitUrl);

  //connect to slots to context menu, quit url
  QObject::connect(quitUrl_, &QAction::triggered,this, &Widget::quitUrl);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //btn edit url
  QObject::connect(ui->btnEdit, &QPushButton::clicked, this, &Widget::btnEdit);

  QObject::connect(editUrl_, &QAction::triggered, this, &Widget::btnEdit);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //btnopenUrl
  QObject::connect(ui->btnopen, &QPushButton::clicked, this, [this](){
    if(!validateSelectedRow()) return;

    openUrl();


  });
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  QObject::connect(openUrl_, &QAction::triggered, this, [this](){
    if(!validateSelectedRow()) return;

    openUrl();
  });

  {

    QSignalBlocker bloquer(ui->cboTheme);
    auto index = loadSchemePreference();
    ui->cboTheme->setCurrentIndex(index);

  }

  if(!SW::Helper_t::nativeRegistryKeyExists(R"(Theme)") && ui->cboTheme->currentIndex() == 0){

    applyPreferredTheme(0);
    setLabelInfo(SW::Helper_t::detectSystemColorScheme());

  }else if(ui->cboTheme->currentIndex() == 0){

    readSettings();
    setLabelInfo(SW::Helper_t::detectSystemColorScheme());

  }else{

    readSettings();
    setLabelInfo(themeType.key(ui->cboTheme->currentText()));

  }

  QObject::connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged, this, [this](){

    auto ret = loadSchemePreference();
    applyPreferredTheme(ret);
    loadLblSchemePreference();
    // writeSettings();

  });
  QObject::connect(ui->cboTheme, &QComboBox::currentIndexChanged, this, &Widget::themeSelectedChanged);


  //cboCategory connection
  QObject::connect(ui->cboCategory, &QComboBox::currentTextChanged, this, &Widget::categorySelectedChanged);

  setCboCategoryToolTip();
  hastvUrlData();

  //conect btn login
  QObject::connect(ui->btnLogIn, &QToolButton::clicked, this, &Widget::load_loginForm);

  //connect to button logout
  QObject::connect(ui->btnLogOut, &QToolButton::clicked, this, &Widget::call_logOut);

  //connect boton crear copia de seguridad
  QObject::connect(ui->btnBackUp, &QToolButton::clicked, this, &Widget::make_backUp);

  //restore database
  QObject::connect(ui->btnRestore, &QToolButton::clicked, this, &Widget::restoreDataBase);

  //connect boton cancelar
  QObject::connect(ui->btnCancel, &QAbstractButton::clicked, this, &Widget::cancelAction);
  QObject::connect(showDescDetail_, &QAction::triggered, this, &Widget::showAlldescription);
  QObject::connect(showPublicUrl_, &QAction::triggered, this, &Widget::load_PublicUrlDialog);
  QObject::connect(moveUrl_, &QAction::triggered, this, &Widget::moveUrl);

  QObject::connect(ui->firstTimeLogInBtn, &QToolButton::clicked, this, &Widget::load_firstTimeLoginDialog);

  // qInfo() << helperdb_.isDataBase_empty();

}//Fin del constructor
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
Widget::~Widget()
{
  delete ui;
}

void Widget::applyPreferredTheme(int pref){

  Qt::ColorScheme scheme;
  switch (pref) {
    case 1:{
        scheme = Qt::ColorScheme::Light;
        ui->tvUrl->setStyleSheet("");
        break;
      }
    case 2:{
        scheme = Qt::ColorScheme::Dark;
        SW::Helper_t::applyManjaroDarkColor(ui->tvUrl);
        break;
      }
    default:{
        scheme = QGuiApplication::styleHints()->colorScheme();
        if(scheme == Qt::ColorScheme::Dark){
          SW::Helper_t::applyManjaroDarkColor(ui->tvUrl);
        }else{
          ui->tvUrl->setStyleSheet("");
        }
        break;
      }
  }

  qApp->setPalette(SW::Helper_t::set_Theme(scheme));
  ui->tvUrl->style()->unpolish(ui->tvUrl);
  ui->tvUrl->style()->polish(ui->tvUrl);
  this->update();

}

int Widget::loadSchemePreference(){

  QSettings settings(qApp->organizationName(), SW::Helper_t::appName());
  settings.beginGroup(QStringLiteral("Theme"));
  const auto theme = settings.value(QStringLiteral("theme Value")).toUInt();
  settings.endGroup();

  return theme;


}

void Widget::loadLblSchemePreference(){

  auto retSystem = SW::Helper_t::detectSystemColorScheme();
  setLabelInfo(retSystem);

}

void Widget::load_aboutDialog(){

  AcercaDeDialog acercaDe{themeType.key(ui->cboTheme->currentText()), this};
  acercaDe.setWindowTitle(SW::Helper_t::appName().append(" - Acerca de"));
  acercaDe.exec();

}

void Widget::load_resetPasswordDialog(){

  ResetPasswordDialog resetDialog(this);
  resetDialog.setWindowTitle(SW::Helper_t::appName().append(" - Restablecer clave o password"));
  resetDialog.exec();

}

void Widget::load_loginForm(){

  LogInDialog logDialog(this);
  if(logDialog.exec() == QDialog::Accepted){

    SW::Helper_t::current_user_ = logDialog.userName();

    const auto user = SW::Helper_t::hashGenerator(logDialog.userName().toLatin1());
    userId_ = helperdb_.getUser_id(user, SW::User::U_user);

    ui->cboCategory->clear();
    loadListCategory(userId_);

    ui->btnLogOut->setEnabled(true);
    ui->btnLogIn->setDisabled(true);
    ui->btnResetPassword->setVisible(false);

    const auto userDes = QString(" - Sesión inicada como: '%1'").arg(SW::Helper_t::current_user_);
    setWindowTitle(QApplication::applicationName().append(userDes));

    ui->lblIcon->setPixmap(QPixmap(QStringLiteral(":/img/user.png")).scaled(16,16));
    SW::Helper_t::sessionStatus_ = SW::SessionStatus::Session_start;
    has_data();
    checkStatusContextMenu();
    canRestoreDataBase();
    verifyAppColorScheme();
  }

}

void Widget::load_PublicUrlDialog(){

  auto colorScheme = themeType.key(ui->cboTheme->currentText());
  PublicUrlDialog publicDialog(colorScheme, this);
  publicDialog.setWindowTitle("Direcciones url públicas");

  publicDialog.exec();

}

void Widget::load_firstTimeLoginDialog(){

  LogInDialog login(this, LogInDialog::FIRST_TIME);
  login.setToggledToButton(true);
  if(login.exec() == QDialog::Accepted){
    canStartSession();
    ui->firstTimeLogInBtn->setVisible(false);
  }

}

void Widget::call_logOut(){

  userId_ = helperdb_.getUser_id(SW::Helper_t::defaultUser, SW::User::U_public);

  ui->btnLogOut->setDisabled(true);
  ui->btnLogIn->setEnabled(true);
  ui->btnResetPassword->setVisible(true);
  setWindowTitle(QApplication::applicationName());
  ui->cboCategory->clear();
  loadListCategory(userId_);
  ui->lblIcon->setPixmap(QPixmap(QStringLiteral(":/img/7278151.png")).scaled(16,16));
  SW::Helper_t::sessionStatus_ = SW::SessionStatus::Session_closed;
  has_data();
  checkStatusContextMenu();
  SW::Helper_t::current_user_ = SW::Helper_t::defaultUser;
  canRestoreDataBase();

  verifyAppColorScheme();

}

void Widget::make_backUp(){

  QProcess process(this);
  const auto path_app {SW::Helper_t::app_pathLocation().append("/tools/sqlite-tools-win-x64-3450100/sqlite3.exe")};
  // qInfo() << path_app << '\n';

  const auto databasePath = SW::Helper_t::AppLocalDataLocation().append("/xdatabase.db");
  const auto filePath = QFileDialog::getSaveFileName(this, QStringLiteral("Crear una copia de seguridad"), SW::Helper_t::getLastOpenedDirectory(),
                                                     QStringLiteral("Archivos de copia de seguridad (*.bak)"));


  if(filePath.isEmpty())
    return;

  if(filePath.contains(' ')){
    QMessageBox::warning(this, SW::Helper_t::appName(), QStringLiteral("El nombre del archivo no puede contener espacios."));
    return;
  }

  const QFileInfo fileInfo(filePath);


  const auto absolutePath{fileInfo.absolutePath()};
  const auto baseName{fileInfo.baseName()};
  const auto extension{fileInfo.suffix()};

  const auto fecha{QDateTime::currentDateTime().toString("yyyy-MM-dd_hhmmss")};
  // QString path{".backup %1/%2-%3.%4"};
  const auto backupCommand = QString(".backup '%1/%2-%3.%4'").arg(absolutePath, baseName, fecha, extension);

  // Verificar si el ejecutable sqlite3 existe
  if(!QFile::exists(path_app)) {
    QMessageBox::critical(this, SW::Helper_t::appName(), QStringLiteral("No se encontró el ejecutable sqlite3"));
    return;
  }

  SW::Helper_t::setLastOpenedDirectory(absolutePath);

  QStringList argv{};

  // argv << databasePath << path.arg(absolutePath,baseName,fecha,extension);
  argv << databasePath << backupCommand;
  qDebug() << "Ejecutando:" << path_app << argv;  // Para depuración

  qint64 pid = 0;
  bool started = QProcess::startDetached(path_app, argv, QDir::currentPath(), &pid);


  if(!started || pid == 0) {
    QMessageBox::critical(this, SW::Helper_t::appName(), tr("Error al crear la copia de seguridad.\n").arg(process.errorString()));
    return;
  }

  QMessageBox::information(this, SW::Helper_t::appName(), tr("La copia de seguridad fue creada en:\n%1").arg(filePath));

}

void Widget::restoreDataBase(){

  const auto dbasePath{SW::Helper_t::AppLocalDataLocation().append("/xdatabase.db")};

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

    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msg.button(QMessageBox::Yes)->setText("Restaurar la base de datos");
    msg.button(QMessageBox::No)->setText("Cancelar");
    if(msg.exec() == QMessageBox::No)
      return;
  }

  const auto pathBackup{QFileDialog::getOpenFileName(this, "Abrir archivo de respaldo", SW::Helper_t::getLastOpenedDirectory(),
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

  const QFileInfo fileInfo{pathBackup};
  SW::Helper_t::setLastOpenedDirectory(fileInfo.absolutePath());

  if(!process.startDetached(app, args)){
    QMessageBox::critical(this, SW::Helper_t::appName(), tr("Error en la ejecución.\n%1").arg(process.errorString()));
    return;

  }
  QMessageBox::information(this, SW::Helper_t::appName(), QStringLiteral("La base de datos, fue restaurada"));
  db.open();
  ui->cboCategory->clear();
  loadListCategory(userId_);
  has_data();
  canCreateBackUp();
  canStartSession();

}

void Widget::cancelAction(){

  ui->txtUrl->clear();
  ui->pteDesc->clear();
  ui->btnCancel->setDisabled(true);
  ui->txtUrl->setFocus();
  editAction(false);
  ui->btnAdd->setText(QStringLiteral("Agregar"));

}

void Widget::moveUrl(){

  const auto currentRow_ = ui->tvUrl->currentIndex().row();
  const auto url_ = xxxModel_->index(currentRow_, 1).data().toString();
  const auto currentCategoryId_ = categoryList.key(ui->cboCategory->currentText());
  const auto urlid = xxxModel_->index(currentRow_, 0).data().toUInt();
  auto data_ = categoryList;
  data_.remove(currentCategoryId_);

  CategoryDialog cDialog(data_, this);
  cDialog.setWindowTitle(QStringLiteral("Mover url a otra categoría"));

  if(cDialog.exec() == QDialog::Accepted){

    auto categoryid = cDialog.getCategoryId();
    if(helperdb_.urlExists(url_, categoryid)){

      auto warningMsg = QString("<p>"
                                "La url: <cite>"
                                "<strong>%1</strong>"
                                "</cite>"
                                "</p> ya esta registrada, en la categoría a la que desea mover!!").arg(url_);

      QMessageBox::warning(this, SW::Helper_t::appName(), warningMsg);
      return;

    }
    // qInfo() <<categoryid;

    if(!helperdb_.moveUrlToOtherCategory(categoryid, urlid)){
      QMessageBox::critical(this, SW::Helper_t::appName(), QStringLiteral("Error al intentar actualizar.\n"));
      return;
    }
    setUpTable(categoryList.key(ui->cboCategory->currentText()));
    verifyContextMenu();
    hastvUrlData();
  }

}

void Widget::addNewUrl(){

  const auto invalidUrlMsg = QString("<p>"
                                     "<span>"
                                     "La dirección: <strong>\"%1\"</strong>, no es válida!<br>"
                                     "una dirección url válida debe tener una de las siguiente formas:"
                                     "<ol>"
                                     "<li><strong>(http://www.)url.dominio</strong></li>"
                                     "<li><strong>(https://www.)url.dominio</strong></li>"
                                     "<li><strong>(ftp://)url.dominio</strong></li>"
                                     "<li><strong>(ftp://www.)url.dominio</strong></li>"
                                     "</ol>"
                                     "<br>Nota:<br>"
                                     "Tenga en cuenta que "
                                     "<strong>http://, https://, ftp://, www.</strong> son opcionales<br>"
                                     "Lo mínimo que se espera es una direccón de la forma: <strong>\"url.domino\"</strong>"
                                     "</span>"
                                     "</p>").arg(ui->txtUrl->text());

  if(ui->btnAdd->text().compare("Agregar") == 0){
    if(!SW::Helper_t::urlValidate(ui->txtUrl->text())){
      QMessageBox::warning(this, SW::Helper_t::appName(), invalidUrlMsg);
      ui->txtUrl->selectAll();
      ui->txtUrl->setFocus(Qt::OtherFocusReason);
      return;
    }

    const auto categoryId = categoryList.key(ui->cboCategory->currentText());

    if(helperdb_.urlExists(ui->txtUrl->text(), categoryId)){

      auto warningMsg = QString("<p>La url: <cite><strong>%1</strong></cite></p> ya esta registrada!!").arg(ui->txtUrl->text());
      QMessageBox::warning(this, SW::Helper_t::appName(), warningMsg);
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

      hastvUrlData();
      checkStatusContextMenu();
      canCreateBackUp();
    }
  }else{


    if(!SW::Helper_t::urlValidate(ui->txtUrl->text())){
      QMessageBox::warning(this, SW::Helper_t::appName(), invalidUrlMsg);
      ui->txtUrl->selectAll();
      ui->txtUrl->setFocus(Qt::OtherFocusReason);
      return;
    }

    QSqlQuery qry(db_);
    qry.prepare(R"(UPDATE  urls SET url=?, desc=? WHERE url_id=? AND categoryid=?)");
    const auto encryptedData = SW::Helper_t::encrypt(ui->txtUrl->text());
    qry.addBindValue(encryptedData, QSql::In);
    const auto descData = SW::Helper_t::encrypt(ui->pteDesc->toPlainText().simplified().toUpper());
    qry.addBindValue(descData, QSql::In);
    auto currentRow = ui->tvUrl->currentIndex().row();
    auto id = ui->tvUrl->model()->index(currentRow,0).data().toInt();
    qry.addBindValue(id, QSql::In);
    const auto categoryId = categoryList.key(ui->cboCategory->currentText());
    qry.addBindValue(categoryId, QSql::In);

    if(!qry.exec()){
      QMessageBox::critical(this, SW::Helper_t::appName(), tr("Fallo la ejecución de la sentencia!\n%1").arg(
                                                             qry.lastError().text()));
      return;

    }

    setUpTable(categoryList.key(ui->cboCategory->currentText()));

    ui->btnAdd->setText(QStringLiteral("Agregar"));
    editAction(false);

    ui->txtUrl->clear();
    ui->pteDesc->clear();
    ui->txtUrl->setFocus(Qt::OtherFocusReason);
  }

}

void Widget::exportToExcel(){

  const auto filePath = QFileDialog::getSaveFileName(this, "Guardar archivo excel", SW::Helper_t::getLastOpenedDirectory(), "Hojas de calculo (*.xlsx)" );
  if(filePath.isEmpty())
    return;

  if(!SW::ExcelExporter::exportTableView(ui->tvUrl, filePath)){

    QMessageBox::warning(this, SW::Helper_t::appName(), tr("Error al exportar el archivo.\n").arg(SW::ExcelExporter::lastError()));
    return;
  }

  const QFileInfo fileInfo(filePath);

  SW::Helper_t::setLastOpenedDirectory(fileInfo.absolutePath());
  QMessageBox::information(this, SW::Helper_t::appName(), tr("El archivo fue guardado en:\n%1").arg(filePath));

}

void Widget::deleteCategory(){

  QMessageBox msgBox(this);
  msgBox.setWindowTitle(SW::Helper_t::appName().append(QStringLiteral(" - Advertencia")));
  msgBox.setText(QStringLiteral("<p style='color:#FB4934;'>"
                                "<cite><strong>Esta a punto eliminar ésta categoría y todo su contenido.<br>"
                                "Recuerde que al aceptar, eliminará de forma permanente estos datos.<br>"
                                "Desea continuar y eliminar los datos?</strong></cite></p>"));

  msgBox.setIcon(QMessageBox::Warning);

  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.button(QMessageBox::Yes)->setText("Borrar categoría");
  msgBox.button(QMessageBox::No)->setText("Cancelar");

  if(msgBox.exec() == QMessageBox::No)
    return;
  if(deleteAll()){
    QMessageBox::information(this, SW::Helper_t::appName(),QStringLiteral("Datos eliminados."));
    ui->cboCategory->clear();
    loadListCategory(userId_);
    has_data();
    checkStatusContextMenu();
  }

}

void Widget::delete_category(){

  const auto categoryId=categoryList.key(ui->cboCategory->currentText());
  auto [res, errMessage] = helperdb_.verifyDeleteCategory(categoryId);
  if(!res){
    QMessageBox::warning(this, SW::Helper_t::appName().append(QStringLiteral(" - Advertencia")),
                         QStringLiteral("<p>"
                                        "<cite>"
                                        "No se puede eliminar ésta categoría.<br>"
                                        "Esto es debido a que ésta categoría tiene asociado uno o mas"
                                        " elementos.<br><br>"
                                        "<strong>Sugerencia:"
                                        "</strong><br>"
                                        "Si desea eliminar una categoría y todo su contenido, "
                                        "puede optar por dar click derecho sobre el nombre de la categoría y"
                                        " del menú contextual elegir:<br><br>"
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
  if(ret == QMessageBox::Cancel)
    return;

  if(helperdb_.deleteCategory(categoryId)){
    QMessageBox::information(this, SW::Helper_t::appName(), QStringLiteral("Categoría eliminada!"));
    ui->cboCategory->clear();
    loadListCategory(userId_);
    has_data();
    checkStatusContextMenu();
  }

}

void Widget::newCategory(){

  dlgNewCategory newCategory(dlgNewCategory::OpenMode::New, QStringList(), this);

  if(newCategory.exec() == QDialog::Rejected)
    return;

  if(!helperdb_.saveCategoryData(newCategory.category(), newCategory.description(), userId_)){
    QMessageBox::critical(this, SW::Helper_t::appName(), QStringLiteral("Error al guardar los datos!\n"));
    return;
  }
  ui->cboCategory->clear();
  loadListCategory(userId_);
  has_data();

  hastvUrlData();
  checkStatusContextMenu();

}

void Widget::editCategory(){

  const auto id = categoryList.key(ui->cboCategory->currentText());
  const QStringList dataLocal = helperdb_.dataCategory(id);
  dlgNewCategory editCategory(dlgNewCategory::OpenMode::Edit, dataLocal, this);
  if(editCategory.exec() == QDialog::Rejected){
    return;
  }
  if(helperdb_.updateCategory(editCategory.category(), editCategory.description(), id, userId_)){
    QMessageBox::information(this, SW::Helper_t::appName(), QStringLiteral("Datos actualizados!\n"));
    ui->cboCategory->clear();
    loadListCategory(userId_);
  }

}

void Widget::themeSelectedChanged(int index){

  Qt::ColorScheme scheme = themeType.key(ui->cboTheme->currentText());
  applyPreferredTheme(index);
  checkStatusSessionColor(themeType.value(scheme));
  verifyAppColorScheme();
  writeSettings();

}

void Widget::categorySelectedChanged(const QString &text){

  setUpTable(categoryList.key(text));
  verifyContextMenu();
  // setUpCboCategoryContextMenu();
  setCboCategoryToolTip();
  hastvUrlData();
  checkStatusContextMenu();
  //      has_data();

}

void Widget::verifyAppColorScheme(){

  auto sessionStatus = SW::Helper_t::sessionStatus_;
  auto u_public = SW::Helper_t::currentUser_.value(SW::User::U_public);

  if(ui->cboTheme->currentText() == themeType.value(Qt::ColorScheme::Unknown)){

    setLabelInfo(SW::Helper_t::detectSystemColorScheme(), ((sessionStatus == SW::SessionStatus::Session_start) ? SW::Helper_t::current_user_ : u_public ));

  }else{

    (ui->cboTheme->currentText() == themeType.value(Qt::ColorScheme::Dark))
    ? setLabelInfo(Qt::ColorScheme::Dark, ((sessionStatus == SW::SessionStatus::Session_start) ? SW::Helper_t::current_user_ : u_public ))
    : setLabelInfo(Qt::ColorScheme::Light, ((sessionStatus == SW::SessionStatus::Session_start) ? SW::Helper_t::current_user_ : u_public ));

  }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::initFrm() noexcept{

  ui->txtUrl->setPlaceholderText(QStringLiteral("(http:// | https:// | ftp://)(www.)url.com(.pe | .abc)"));
  ui->pteDesc->setPlaceholderText(QStringLiteral("Description to url's"));
  ui->btnNewCategory->setToolTip(QStringLiteral("New Category!"));
  ui->btnEditCategory->setToolTip(QStringLiteral("Edit Category Data!"));
  //btnAdd disabled
  ui->btnAdd->setDisabled(true);
  ui->btnLogIn->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
  ui->btnLogOut->setDisabled(true);
  ui->btnLogOut->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q));

  ui->btnCancel->setDisabled(true);

  //set shortcuts, to button box url
  ui->btnAdd->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_A));
  ui->btnEdit->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_E));
  ui->btnQuit->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Q));
  ui->btnopen->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_O));
  ui->btnCancel->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_C));

  //set the focus to txturl control
  ui->txtUrl->setFocus(Qt::OtherFocusReason);
  ui->lblInfo->setCursor(Qt::PointingHandCursor);

  ui->btnResetPassword->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_P));

  ui->firstTimeLogInBtn->setToolTip("<p>Crear un usuario: <br>"
                                    "<cite>\"Este boton se muestra solo por una vez; "
                                    "esto es por la razón de que, al abrir la aplicación por primera vez, no existen usuarios,"
                                    " aparte del usario por defecto\"</cite></p>");

  ui->tvUrl->setMouseTracking(true);

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::editAction(bool op) noexcept{

  ui->btnEdit->setDisabled(op);
  ui->btnQuit->setDisabled(op);
  ui->btnopen->setDisabled(op);
  ui->tvUrl->setDisabled(op);
  // ui->tvUrl->setState();
  contextMenu->setDisabled(op);

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

  xxxModel_ = new SWTableModel(this, db_);
  xxxModel_->setTable("urls");
  xxxModel_->setFilter(QString("categoryid=%1").arg(categoryId));
  xxxModel_->select();

  ui->tvUrl->setModel(xxxModel_);
  setUpTableHeaders();


  QSqlQuery query(xxxModel_->query().lastQuery(), db_);


  if(query.exec()){
    while(query.next()){
      urlList.insert(query.value(0).toUInt(),query.value(1).toString());
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
  // ui->tvUrl->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->tvUrl->setAlternatingRowColors(true);


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

  ui->btnRestore->setVisible(static_cast<bool>(SW::Helper_t::sessionStatus_));
}

void Widget::canCreateBackUp() const noexcept{

  ui->btnBackUp->setVisible(hasValidTableData());

}

bool Widget::hasValidTableData() const noexcept {

  const auto tables = db_.tables(QSql::Tables);
  const QStringList excludedTables{"users", "sqlite_sequence"};

  return std::any_of(tables.cbegin(), tables.cend(),
                     [&excludedTables, this](const QString& tableName) -> bool {
                       if (excludedTables.contains(tableName)) {
                         return false;
                       }

                       QSqlQuery query(db_);
                       return query.exec(QString("SELECT 1 FROM %1 LIMIT 1").arg(tableName)) &&
                              query.next();
                     });
}

void Widget::canStartSession() noexcept{

  ui->btnLogIn->setEnabled(helperdb_.userExists());
  ui->btnResetPassword->setEnabled(helperdb_.userExists());
  ui->firstTimeLogInBtn->setVisible(!helperdb_.userExists());

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::writeSettings() const noexcept{

  const auto currentText = ui->cboTheme->currentText();

  QSettings settings(qApp->organizationName(), SW::Helper_t::appName());

  settings.setValue(QStringLiteral("position"), saveGeometry());

  if(ui->cboCategory->count() > 1 && SW::Helper_t::sessionStatus_ == SW::SessionStatus::Session_closed){

    const auto categoryName = ui->cboCategory->currentText();
    settings.setValue(QStringLiteral("category name"), categoryName);

  }

  settings.beginGroup(QStringLiteral("Theme"));

  settings.setValue(QStringLiteral("theme name"), themeType.value(static_cast<Qt::ColorScheme>(ui->cboTheme->currentIndex())));
  settings.setValue(QStringLiteral("theme Value"), static_cast<uint32_t>(themeType.key(currentText)));

  QString lblColor_{};

  if(currentText == themeType.value(Qt::ColorScheme::Unknown)){
    auto retSystemColorScheme = SW::Helper_t::detectSystemColorScheme();

    lblColor_ = SW::Helper_t::lblColorMode.value(retSystemColorScheme);

  }else{
    lblColor_ = SW::Helper_t::lblColorMode.value(themeType.key(currentText));
  }

  settings.setValue(QStringLiteral("lblColor"), SW::Helper_t::setColorReg(lblColor_));
  settings.endGroup();

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::readSettings() noexcept{

  QSettings settings(qApp->organizationName(), SW::Helper_t::appName());

  restoreGeometry(settings.value(QStringLiteral("position")).toByteArray());

  auto ret = SW::Helper_t::nativeRegistryKeyExists("category name");

  QString categoryName{};
  if(ret && SW::Helper_t::sessionStatus_ == SW::SessionStatus::Session_closed){

    categoryName = settings.value("category name", QString()).toString();

    if(!categoryName.isEmpty() && ui->cboCategory->count() > 1){

      {

        QSignalBlocker signalBlocker(ui->cboCategory);
        ui->cboCategory->setCurrentText(categoryName);
      }
      categorySelectedChanged(categoryName);
    }
  }





  settings.beginGroup(QStringLiteral("Theme"));
  const auto theme = settings.value(QStringLiteral("theme Value")).toInt();
  const auto colorData = settings.value(QStringLiteral("lblColor")).toByteArray();

  const auto lblColor = SW::Helper_t::getColorReg(colorData);
  setLabelInfo(SW::Helper_t::lblColorMode.key(lblColor));

  {
    QSignalBlocker signalblocker(ui->cboTheme);
    ui->cboTheme->setCurrentIndex(theme);

  }

  settings.endGroup();

  applyPreferredTheme(theme);

}

void Widget::showAlldescription() noexcept{

  // auto* otherModel = dynamic_cast<QSqlTableModel*>(ui->tvUrl->model());
  auto row = ui->tvUrl->currentIndex().row();
  const auto desc = ui->tvUrl->model()->index(row,2).data().toString();
  const auto url = ui->tvUrl->model()->index(row,1).data().toString();

  QMessageBox msgDescription(this);

  // msgDescription.setIcon(QMessageBox::Information);
  QPixmap pixMap(QStringLiteral(":/img/desc.png"));
  // pixMap.scaled(32,32);
  msgDescription.setWindowTitle(qApp->applicationName().append(" - Descripción completa de la URL"));
  msgDescription.setIconPixmap(pixMap.scaled(64, 64));
  msgDescription.setText(desc);
  msgDescription.setDetailedText(url);
  msgDescription.addButton(QStringLiteral("Cerrar descripción"), QMessageBox::AcceptRole);

  msgDescription.exec();

  // QMessageBox::information(this, qApp->applicationName(), desc);

}

void Widget::checkStatusContextMenu(){
  (static_cast<bool>(SW::Helper_t::sessionStatus_)) ? showPublicUrl_->setVisible(false) : showPublicUrl_->setVisible(true);

  (ui->cboCategory->model()->rowCount() > 1 && ui->tvUrl->model()->rowCount() > 0)
    ? moveUrl_->setVisible(true) : moveUrl_->setVisible(false);

}

void Widget::checkStatusSessionColor(const QString& text){

  if(!static_cast<bool>(SW::Helper_t::sessionStatus_)){

    ( text == themeType.value(Qt::ColorScheme::Dark) )
    ? setLabelInfo(Qt::ColorScheme::Dark, SW::Helper_t::current_user_)
    : setLabelInfo(Qt::ColorScheme::Light, SW::Helper_t::current_user_);
  }else{
    auto u_public = SW::Helper_t::currentUser_.value(SW::User::U_public);
    ( text == themeType.value(Qt::ColorScheme::Light) )
      ? setLabelInfo(Qt::ColorScheme::Light, u_public)
      : setLabelInfo(Qt::ColorScheme::Dark, u_public);

  }

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void Widget::setLabelInfo(Qt::ColorScheme color, const QString& userName) noexcept{

  // auto userName_ = SW::Helper_t::currentUser_.value(userName);

  ui->lblInfo->setText(QString("<span style='color:%1;'>"
                               "<strong>SWSystem's - Lincoln Ingaroca"
                               "</strong></span>").arg(SW::Helper_t::lblColorMode.value(color)));


  ui->lblState->setText(QString("<span style='color:%1;'>"
                                "<strong>User: %2"
                                "</strong></span>").arg(SW::Helper_t::lblColorMode.value(color), userName));

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::loadListCategory(uint32_t user_id) noexcept{

  categoryList = helperdb_.loadList_Category(user_id);
  ui->cboCategory->addItems(categoryList.values());
}

void Widget::loadThemeComboBox() noexcept{

  const auto unknownIcon = QIcon{QStringLiteral(":/img/flat_seo-47-64.png")};
  const auto whiteIcon = QIcon{QStringLiteral(":/img/whitetheme.png")};
  const auto darkIcon = QIcon{QStringLiteral(":/img/darktheme.png")};

  ui->cboTheme->addItem(unknownIcon, themeType.value(Qt::ColorScheme::Unknown));
  ui->cboTheme->addItem(whiteIcon, themeType.value(Qt::ColorScheme::Light));
  ui->cboTheme->addItem(darkIcon, themeType.value(Qt::ColorScheme::Dark));

  // ui->cboTheme->setItemData(0, false, Qt::UserRole-1);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::setUpCboCategoryContextMenu() noexcept{

  const QIcon icon(QStringLiteral(":/img/118277.png"));
  ui->cboCategory->setContextMenuPolicy(Qt::ActionsContextMenu);
  delCategory_ = new QAction(icon, QStringLiteral("Forzar eliminación de categoría"),this);
  ui->cboCategory->addAction(delCategory_);

}

void Widget::setUptvUrlContextMenu() noexcept{

  // ui->tvUrl->setContextMenuPolicy(Qt::ActionsContextMenu);
  contextMenu = new QMenu(this);

  const auto openUrlIcon = QIcon(QStringLiteral(":/img/openurl.png"));
  const auto editUrlIcon = QIcon(QStringLiteral(":/img/editurl.png"));
  const auto quitUrlIcon = QIcon(QStringLiteral(":/img/quiturl.png"));

  openUrl_ = contextMenu->addAction(openUrlIcon, QStringLiteral("Abrir url en el navegador"));
  editUrl_ = contextMenu->addAction(editUrlIcon, QStringLiteral("Editar url"));
  quitUrl_ = contextMenu->addAction(quitUrlIcon, QStringLiteral("Quitar url"));
  contextMenu->addSeparator();
  showDescDetail_ = contextMenu->addAction(QStringLiteral("Ver descripción de URL completa"));

  contextMenu->addSeparator();
  showPublicUrl_ = contextMenu->addAction(QStringLiteral("Ver url's públicas"));

  contextMenu->addSeparator();
  moveUrl_ = contextMenu->addAction(QStringLiteral("Mover url, a otra categoría"));

  //add a export to excel context menu

  contextMenu->addSeparator();
  const auto exportToExcelFileIcon = QIcon(QStringLiteral(":/img/excelDocument.png"));
  exportToExcelFile_ = contextMenu->addAction(exportToExcelFileIcon, QStringLiteral("Exportar datos a excel"));

  ui->tvUrl->installEventFilter(this);

  checkStatusContextMenu();

}

bool Widget::eventFilter(QObject* watched, QEvent* event){
  if(watched == ui->tvUrl && event->type() == QEvent::ContextMenu){
    QContextMenuEvent* contextMenuEvent = dynamic_cast<QContextMenuEvent*>(event);
    if(contextMenuEvent){
      contextMenu->exec(contextMenuEvent->globalPos());

      return true;

    }
  }
  return QWidget::eventFilter(watched, event);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////


bool Widget::deleteAll() noexcept{
  const auto categoryId=categoryList.key(ui->cboCategory->currentText());
  if(helperdb_.deleteUrls(1, categoryId)){
    if(helperdb_.deleteCategory(categoryId))
      return true;
  }
  return false;
}

void Widget::verifyContextMenu() noexcept{
  const auto categoryId=categoryList.key(ui->cboCategory->currentText());
  auto [res, errMessage] = helperdb_.verifyDeleteCategory(categoryId);
  //      qDebug()<<count;
  (res) ? delCategory_->setDisabled(true) : delCategory_->setEnabled(true);
}

void Widget::setCboCategoryToolTip() noexcept{
  const auto id = categoryList.key(ui->cboCategory->currentText());
  const auto categoryData = helperdb_.dataCategory(id);
  const auto desc=categoryData.value(1);
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
  const auto url = ui->tvUrl->model()->index(currentRow, 1).data().toString();
  if(!SW::Helper_t::open_Url(QUrl(url))){
    QMessageBox::critical(this, SW::Helper_t::appName(), QStringLiteral("Fallo al intentar abrir dirección url!\n"));
    return;
  }
}

void Widget::quitUrl() noexcept{

  if(!validateSelectedRow()) return;

  auto currentRow = ui->tvUrl->currentIndex().row();

  const auto url = ui->tvUrl->model()->index(currentRow, 1).data().toString();

  QMessageBox msgBox(this);
  msgBox.setText(QString("<span>Confirma que desea eliminar esta dirección:<br>"
                         " <cite style='color:#ff0800;'><strong>%1</strong></cite></span>").arg(url));
  msgBox.setIcon(QMessageBox::Question);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.button(QMessageBox::Yes)->setText("Eliminar");
  msgBox.button(QMessageBox::No)->setText("Cancelar");

  if(msgBox.exec() == QMessageBox::Yes){
    const auto urlId=urlList.key(url);
    if(helperdb_.deleteUrls(2, 0, urlId)){
      ui->tvUrl->model()->removeRow(ui->tvUrl->currentIndex().row());
      setUpTable(categoryList.key(ui->cboCategory->currentText()));
    }

  }

  verifyContextMenu();
  hastvUrlData();

}

void Widget::hastvUrlData() noexcept{
  if(ui->tvUrl->model()->rowCount() == 0){
    openUrl_->setDisabled(true);
    ui->btnopen->setDisabled(true);
    ui->btnEdit->setDisabled(true);
    ui->btnQuit->setDisabled(true);
    editUrl_->setDisabled(true);
    quitUrl_->setDisabled(true);
    moveUrl_->setVisible(false);
    showDescDetail_->setVisible(false);
    exportToExcelFile_->setVisible(false);
  }else{
    openUrl_->setEnabled(true);
    ui->btnopen->setEnabled(true);
    ui->btnEdit->setEnabled(true);
    ui->btnQuit->setEnabled(true);
    editUrl_->setEnabled(true);
    quitUrl_->setEnabled(true);
    moveUrl_->setVisible(true);
    showDescDetail_->setVisible(true);
    exportToExcelFile_->setVisible(true);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::closeEvent(QCloseEvent *event){
  if(!static_cast<bool>(SW::Helper_t::sessionStatus_)){
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



