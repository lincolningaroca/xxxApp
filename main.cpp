#include "widget.hpp"
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <util/helper.hpp>




int main(int argc, char *argv[]){

  QApplication a(argc, argv);
  a.setApplicationName("xxxApp");
  //  a.setApplicationDisplayName("xxxApp");
  a.setApplicationVersion("1.0");
  a.setOrganizationName("SWSystem's");
  a.setStyle("Fusion");

  QDir dir(SW::Helper_t::appLocation());
  if(dir.exists())
    qInfo() << "Lacarpeta ya existe";
  else{
      SW::Helper_t::createDataBase_dir();
      qInfo() << "Carpeta creada";
    }
  qInfo() << "Separator: " << QDir::separator();
  qInfo() << "path " << SW::Helper_t::appLocation();





  //establecer la conexion a la base d edatos
  QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE", "xxxConection");


  db.setDatabaseName(SW::Helper_t::appLocation().append("/xdatabase.db"));


  if(!db.open()){
      QMessageBox::critical(nullptr, qApp->applicationName(),
                            "Error desconocido!\n"+db.lastError().text());
      return 1;
    }




  Widget w;
  w.setWindowTitle(a.applicationName());
  w.show();
  return a.exec();
}
