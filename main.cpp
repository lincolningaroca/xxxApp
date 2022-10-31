#include "widget.hpp"

#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  a.setApplicationName("xxxApp");
  a.setApplicationVersion("1.0");
  a.setOrganizationName("SWSystem's");

  QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE", "xxxConection");


  db.setDatabaseName(qApp->applicationDirPath().append("/db/xxx.db"));
  if(!db.open()){
      QMessageBox::critical(nullptr, qApp->applicationName(),
                            "Error desconocido!\n"+db.lastError().text());
      return 1;
    }


  Widget w;
  w.setWindowTitle(a.applicationName());
  a.setStyle("Fusion");
  w.show();
  return a.exec();
}
