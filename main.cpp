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
  a.setStyle("Fusion");
  //establecer la conexion a la base d edatos
  QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE", "xxxConection");


  db.setDatabaseName(qApp->applicationDirPath().append("/db/xxxdb.db"));
  if(!db.open()){
    QMessageBox::critical(nullptr, qApp->applicationName(),
                          "Error desconocido!\n"+db.lastError().text());
    return 1;
  }
//fin conexion base de datos



//  LogInDialog login_;
//  login_.setWindowTitle(a.applicationName().append(" - LogIn"));


//  if(login_.exec() == QDialog::Accepted){
//   Widget *w = new Widget();
////    qInfo() <<LogInDialog::userid_;
//    w->setUserId(LogInDialog::userid_);
//    w->setWindowTitle(a.applicationName());
//    w->show();
//  }else{
//    return 2;
//  }
  Widget w;
  w.setWindowTitle(a.applicationName());
  w.show();
  return a.exec();
}
