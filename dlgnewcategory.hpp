#pragma once

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
  class dlgNewCategory;
}

class dlgNewCategory : public QDialog
{
  Q_OBJECT


public:
  enum class OpenMode{ New, Edit};

  explicit dlgNewCategory(OpenMode mode, uint32_t id, const QStringList& list={}, QWidget *parent = nullptr);
  ~dlgNewCategory();

  QString category() const noexcept;
  QString description() noexcept;
  QString descriptionToolTip() noexcept;

private:
  Ui::dlgNewCategory *ui;
  const OpenMode mode_;
  const uint32_t id_{};
  const QSqlDatabase db_{};
  const QStringList list_{};

  void loadData() noexcept;
  bool validateData() const noexcept;

};

