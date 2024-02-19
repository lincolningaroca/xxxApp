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

private:
  void loadInfo_app() const noexcept;
};

