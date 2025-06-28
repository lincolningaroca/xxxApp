#include "excelexporter.hpp"

#include <QAbstractItemModel>
#include <QTableView>
#include <QFileInfo>
#include <QDir>
#include <QSqlTableModel>
#include <QSqlRecord>

#include "xlsxdocument.h"

namespace  SW {


// ExcelExporter::ExcelExporter() {}

bool ExcelExporter::exportTableView(QTableView *tableView, const QString &filePath){

  lastError_.clear();

  QSet<QString> excludedColumns = {"url_id", "categoryid"};

  if(!tableView || !tableView->model()){
    lastError_ = "El tableView o su modelo son nulos..";
    return false;
  }


  auto savePath = filePath;
  if(!savePath.endsWith(".xlsx", Qt::CaseInsensitive)){

    savePath += ".xlsx";
  }

  QFileInfo fileInfo(savePath);
  auto dir = fileInfo.dir();

  if(!dir.exists()){

    lastError_ = QString("El directorio no existe: %1").arg(dir.absolutePath());
    return false;

  }

  QFileInfo dirInfo(dir.absolutePath());

  if (!(dirInfo.permission(QFile::WriteUser))) {
    lastError_ = QString("No se tienen permisos de escritura en el directorio: %1").arg(dir.absolutePath());
    return false;
  }

  if (fileInfo.exists() && !QFile::remove(savePath)) {
    lastError_ = QString("No se pudo sobrescribir el archivo existente: %1").arg(savePath);
    return false;
  }

  QXlsx::Document xlsxDocument;
  auto *model =tableView->model();


  auto colExcel = 1;
  for (auto col = 0; col < model->columnCount(); ++col) {
    QString fieldName{};
    if (auto sqlModel = qobject_cast<QSqlTableModel*>(model))
      fieldName = sqlModel->record().fieldName(col).toLower();

    if (excludedColumns.contains(fieldName))
      continue;

    QString header = model->headerData(col, Qt::Horizontal).toString();
    xlsxDocument.write(1, colExcel++, header);
  }

  for (auto row = 0; row < model->rowCount(); ++row) {
    colExcel = 1;
    for (auto col = 0; col < model->columnCount(); ++col) {
      QString fieldName{};
      if (auto sqlModel = qobject_cast<QSqlTableModel*>(model))
        fieldName = sqlModel->record().fieldName(col).toLower();

      if (excludedColumns.contains(fieldName))
        continue;

      auto data = model->data(model->index(row, col));
      xlsxDocument.write(row + 2, colExcel++, data);
    }
  }


  if (!xlsxDocument.saveAs(savePath)) {
    lastError_ = QString("No se pudo guardar el archivo en: %1").arg(savePath);
    return false;
  }

  return true;

}

}// namespace SW
