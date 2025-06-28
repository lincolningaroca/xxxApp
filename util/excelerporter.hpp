#pragma once

#include <QString>


class QTableView;

namespace SW {

struct ExcelErporter{
public:
  explicit ExcelErporter() = default;

  static bool exportTableView(QTableView *tableView, const QString &filePath);
  static const QString& lastError(){return lastError_;}

private:
  inline static QString lastError_{};

};
}// namespace SW
