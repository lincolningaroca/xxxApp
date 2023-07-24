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
  static QString hashGenerator(const QByteArray&) noexcept;

  [[nodiscard]]
  QString userName() const  noexcept{ return userName_;}

private:
  Ui::LogInDialog *ui;
  const QSqlDatabase db_{};
  QString userName_{};

  void setUp_Form() noexcept;
  void setStateControls(bool op) noexcept;
  void setOptionsToComboBox(int index) noexcept;
  bool logIn() const noexcept;

  void clearControls() noexcept;
  bool createUser(const QString&, const QString&, const QString&,
                  const QString&, const QString&, const QString&) noexcept;

  bool Validate_hasNoEmpty() const noexcept;
  bool verifyPassword() const noexcept;
  bool verifyPinNumber() const noexcept;
  bool userExists(const QString&) const noexcept;



};

