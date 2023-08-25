#pragma once

#include <QDialog>
#include <helperdatabase/helperdb.hpp>
#include <QCloseEvent>

namespace Ui {
class LogInDialog;
}

class LogInDialog : public QDialog
{
  Q_OBJECT

public:
  explicit LogInDialog(QWidget *parent = nullptr);
  ~LogInDialog();



  QString userName() const  noexcept{ return userName_;}

private:
  Ui::LogInDialog *ui;

  QString userName_{};
  SW::HelperDataBase_t helperdb_;

  void setUp_Form() noexcept;
  void setStateControls(bool op) noexcept;
  void setOptionsToComboBox(int index) noexcept;


  void clearControls() noexcept;
  bool Validate_hasNoEmpty() const noexcept;

  void writeSettings();
  void readSettings();
  void reject_form() noexcept;




  // QWidget interface
protected:
  void closeEvent(QCloseEvent *event) override;
};

