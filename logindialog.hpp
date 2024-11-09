#pragma once

#include <QDialog>
#include <helperdatabase/helperdb.hpp>
// #include <QCloseEvent>
#include <util/helper.hpp>

namespace Ui {
class LogInDialog;
}
class QLineEdit;
class QCheckBox;
class QCloseEvent;

class LogInDialog : public QDialog
{
  Q_OBJECT

public:
  explicit LogInDialog(QWidget *parent = nullptr);
  ~LogInDialog();

  const QString& userName() const  noexcept{ return userName_;}

  inline static const QHash<SW::AuthType, QString> authType{
    {SW::AuthType::Numeric_pin, "Pin numérico"},
    {SW::AuthType::Secret_Question, "Pregunta secreta"}
  };

private:
  Ui::LogInDialog *ui;

  QString userName_{};
  SW::HelperDataBase_t helperdb_;


  void setUp_Form() noexcept;
  void setStateControls(bool op) noexcept;
  void setOptionsToComboBox(int index) noexcept;


  void clearControls() noexcept;
  bool Validate_hasNoEmpty() const noexcept;

  void writeSettings() const noexcept;
  void readSettings();
  void reject_form() noexcept;

  void setFeatures(QLineEdit *w, QCheckBox *b, bool checked) noexcept;

  // QWidget interface
protected:
  void closeEvent(QCloseEvent *event) override;
};

