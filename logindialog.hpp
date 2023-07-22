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


  [[nodiscard]] uint32_t getUser_id()  noexcept;

private:
  Ui::LogInDialog *ui;
  const QSqlDatabase db_{};
//  uint32_t userid_{0};

  void setUp_Form() noexcept;
  void setStateControls(bool op) noexcept;
  void setOptionsToComboBox(int index) noexcept;
  bool logIn() const noexcept;

};

