
#include <QApplication>
#include <QSqlDatabase>
#include <QLocalServer>
#include <QLocalSocket>
#include <QDir>
#include <QMessageBox>
#include <QFontDatabase>
#include <QStyleHints>

#include "util/helper.hpp"
#include "util/systemthemewatcher.hpp"
#include "widget.hpp"

struct SingleIntsanceManager{

  static bool isRunning(const QString& serverName){

    QLocalSocket socket{};
    socket.connectToServer(serverName);

    if(socket.waitForConnected(500)){
      socket.disconnectFromServer();
      return true;
    }
    return false;
  }

  static bool initServer(const QString & serverName){

    auto* server = new QLocalServer();
    QLocalServer::removeServer(serverName);

    if(!server->listen(serverName)){
      delete server;
      return false;
    }
    return true;
  }
};

bool validate{false};

bool createDataBase(){
  QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), QStringLiteral("xxxConection"));
  db.setDatabaseName(SW::Helper_t::AppLocalDataLocation().append("/xdatabase.db"));
  return db.open();
}
bool dataBaseExists(){
  const auto path = SW::Helper_t::AppLocalDataLocation().append("/xdatabase.db");
  QFile file(path);
  return file.exists();
}
bool createTables(){
  QSqlQuery qry(QSqlDatabase::database(QStringLiteral("xxxConection")));
  return(qry.exec(R"(CREATE TABLE users(user_id INTEGER PRIMARY KEY AUTOINCREMENT,
                                  user TEXT NOT NULL UNIQUE,
                                  password TEXT NOT NULL,
                                  user_profile VARCHAR(20) NOT NULL,
                                  rescue_type VARCHAR(40) NOT NULL,
                                  first_value TEXT NOT NULL,
                                  confirm_value TEXT NOT NULL))") &&

          qry.exec(R"(INSERT INTO users VALUES(1,'public','public','PUBLIC','PUBLIC','PUBLIC','PUBLIC'))") &&
          qry.exec(R"(CREATE TABLE urls(url_id INTEGER PRIMARY KEY AUTOINCREMENT,
                                  url TEXT NOT NULL,
                                  desc TEXT,
                                  categoryid INTEGER REFERENCES category(category_id)))") &&

          qry.exec(R"(CREATE TABLE category(category_id INTEGER PRIMARY KEY AUTOINCREMENT,
                                  category_name TEXT NOT NULL,
                                  desc TEXT,
                                  userid INTEGER REFERENCES users(user_id)))")
          );


}


int main(int argc, char *argv[]){

  QApplication a(argc, argv);
  a.setApplicationName(QStringLiteral("xxxApp"));
  a.setApplicationVersion(QStringLiteral("1.0"));
  a.setOrganizationName(QStringLiteral("SWSystem's"));
  // a.setStyle("Fusion");

  const QString serverName{a.applicationName()};
  if(SingleIntsanceManager::isRunning(serverName)){
    // QMessageBox::warning(nullptr, qApp->applicationName(), "Ya existe una instancia de la aplicación corriendo.");
    return -1;
  }

  if(!SingleIntsanceManager::initServer(serverName)){
    QMessageBox::critical(nullptr, qApp->applicationName(), "No se pudo iniciar el control de instancia única.");
    return -1;
  }


  //Creacion de la carpeta de la aplicación
  QDir dir(SW::Helper_t::AppLocalDataLocation());
  if(!dir.exists()){
    if(SW::Helper_t::createDataBase_dir())
      qInfo() << "Carpeta del sistema creado!";
  }

  if(!dataBaseExists()){
    createDataBase();
    createTables();
    validate = true;
    qInfo() << "Todo los datos y tablas fueron creadas!";

  }
  if(!validate){
    createDataBase();
  }

  Widget w;

  if(SW::Helper_t::nativeRegistryKeyExists(HKEY_CURRENT_USER, "Software\\SWSystem's\\xxxApp\\Theme")){
    auto prefColor = w.loadSchemePreference();
    w.applyPreferredTheme(prefColor);
  }

  SW::SystemThemeWatcher watcher;

  qApp->installNativeEventFilter(&watcher);

  QObject::connect(&watcher, &SW::SystemThemeWatcher::systemThemeChanged, &w ,[&]() {
    if (w.loadSchemePreference() == 0) {
      w.applyPreferredTheme(static_cast<int>(QGuiApplication::styleHints()->colorScheme()));
      w.loadLblSchemePreference();
    }

  });



  w.setWindowTitle(a.applicationName());
  w.show();
  return a.exec();
}
