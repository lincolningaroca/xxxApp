#pragma once

#include <QDialog>

namespace Ui {
  class AcercaDeDialog;
}

class AcercaDeDialog : public QDialog
{
  Q_OBJECT

public:
  explicit AcercaDeDialog(const QString& colorMode, QWidget *parent = nullptr);
  ~AcercaDeDialog();

private:
  Ui::AcercaDeDialog *ui;
  const QFont font_{"Fira Code", 11};

private:
  void loadInfo_app() const noexcept;
};

