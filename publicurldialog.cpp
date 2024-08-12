#include "publicurldialog.hpp"
#include "ui_publicurldialog.h"

#include <QSqlTableModel>
#include <QDebug>
#include <QDesktopServices>
#include <QMessageBox>

#include "helperdatabase/helperdb.hpp"
#include "util/helper.hpp"
#include "swtablemodel.hpp"

PublicUrlDialog::PublicUrlDialog(QWidget *parent) :
  QDialog(parent), ui(new Ui::PublicUrlDialog),
  db_{QSqlDatabase::database(QStringLiteral("xxxConection"))}{
  ui->setupUi(this);
  setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

  loadDataComboBox();
  loadDataTableView();

  QObject::connect(ui->categoryComboBox, &QComboBox::currentTextChanged, this, [&](){
      // ui->categoryComboBox->clear();
      loadDataTableView();
    });

  QObject::connect(ui->openPushButton, &QPushButton::clicked, this, [&](){

      if(!ui->urlTableView->selectionModel()->hasSelection()){
          QMessageBox::warning(this, SW::Helper_t::appName(), QStringLiteral("Seleccione una fila!\n"));
          return;
        }


      auto row_ = ui->urlTableView->currentIndex().row();
      auto url_ = ui->urlTableView->model()->index(row_, 1).data().toString();

      if(!QDesktopServices::openUrl(QUrl(url_))){
          QMessageBox::critical(this, SW::Helper_t::appName(), QStringLiteral("Error al abrir la dirección url.\n"));
          return;
        }
    });
}

PublicUrlDialog::~PublicUrlDialog(){
  delete ui;
}

void PublicUrlDialog::loadDataComboBox(){

  SW::HelperDataBase_t helperDb;

  auto userId_ = helperDb.getUser_id(SW::Helper_t::defaultUser, SW::User::U_public);
  data_ = helperDb.loadList_Category(userId_);

  ui->categoryComboBox->addItems(data_.values());

}

void PublicUrlDialog::loadDataTableView(){

  auto categoryId_ = data_.key(ui->categoryComboBox->currentText());
  SWTableModel* xxxModel_ = new SWTableModel(this, db_);
  xxxModel_->setTable(QStringLiteral("urls"));
  xxxModel_->setFilter(QString("categoryid=%1").arg(categoryId_));

  xxxModel_->select();

  ui->urlTableView->setModel(xxxModel_);

  //setup headers to table view

  ui->urlTableView->hideColumn(0);
  ui->urlTableView->hideColumn(3);

  ui->urlTableView->model()->setHeaderData(1,Qt::Horizontal, "Dirección URL");
  ui->urlTableView->model()->setHeaderData(2,Qt::Horizontal, "Descripción");

  ui->urlTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  ui->urlTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  ui->urlTableView->setSelectionBehavior(QAbstractItemView::SelectRows);

}








