#pragma once

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class LogInDialog;
}

class LogInDialog : public QDialog
{
  Q_OBJECT

public:
  explicit LogInDialog(QWidget *parent = nullptr);
  ~LogInDialog();


  void getUser_id()  noexcept;
  [[maybe_unused]]inline static uint32_t userid_{0};
private:
  Ui::LogInDialog *ui;
  const QSqlDatabase db_{};


  void setUp_Form() noexcept;
  void setStateControls(bool op) noexcept;
  void setOptionsToComboBox(int index) noexcept;
  bool logIn() const noexcept;

};

