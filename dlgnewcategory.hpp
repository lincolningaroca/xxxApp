#pragma once

#include <QDialog>

namespace Ui {
class dlgNewCategory;
}

class dlgNewCategory : public QDialog
{
  Q_OBJECT


public:
  enum class OpenMode{ New, Edit};

  explicit dlgNewCategory(OpenMode mode, const QStringList& list={}, QWidget *parent = nullptr);
  ~dlgNewCategory();

  QString category() const noexcept;
  QString description() noexcept;
  QString descriptionToolTip() noexcept;

private:
  Ui::dlgNewCategory *ui;
//  const QStringList list_{};

  void loadData() noexcept;
  bool validateData() const noexcept;

};

