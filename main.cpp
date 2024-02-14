#include "widget.hpp"
#include <QApplication>
#include <QSqlDatabase>
#include <util/helper.hpp>

bool validate{false};

bool createDataBase(){
  QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), QStringLiteral("xxxConection"));
  db.setDatabaseName(SW::Helper_t::AppLocalDataLocation().append(QStringLiteral("/xdatabase - copia.db")));
  return db.open();
}
bool dataBaseExists(){
  const auto path = SW::Helper_t::AppLocalDataLocation().append(QStringLiteral("/xdatabase - copia.db"));
  QFile file(path);
  return file.exists();
}
bool createTables(){
  QSqlQuery qry(QSqlDatabase::database(QStringLiteral("xxxConection")));
  return(qry.exec(QStringLiteral("CREATE TABLE users(user_id integer primary key autoincrement,"
                  "user text not null unique,"
                  "password text not null,"
                  "user_profile varchar(20) not null,"
                  "rescue_type varchar(40) not null,"
                  "first_value text not null,"
                  "confirm_value text not null);")) &&

         qry.exec(QStringLiteral("INSERT INTO users VALUES(1,'public','public','PUBLIC','PUBLIC','PUBLIC','PUBLIC');")) &&
         qry.exec(QStringLiteral("CREATE TABLE urls(url_id integer primary key autoincrement,"
                  "url text not null,"
                  "desc text,"
                  "categoryid integer references category(category_id));")) &&

         qry.exec(QStringLiteral("CREATE TABLE category(category_id integer primary key autoincrement,"
                  "category_name text not null,"
                  "desc text,"
                  "userid integer references users(user_id));")));


}


int main(int argc, char *argv[]){

  QApplication a(argc, argv);
  a.setApplicationName(QStringLiteral("xxxApp"));
  a.setApplicationVersion(QStringLiteral("1.0"));
  a.setOrganizationName(QStringLiteral("SWSystem's"));
  a.setStyle("Fusion");


  //Creacion de la carpeta de la aplicación
  QDir dir(SW::Helper_t::AppLocalDataLocation());
  if(!dir.exists()){
      SW::Helper_t::createDataBase_dir();
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
  w.setWindowTitle(a.applicationName());
  w.show();
  return a.exec();
}
