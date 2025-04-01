#pragma once

#include <QDialog>
#include "util/helper.hpp"

namespace Ui {
  class AcercaDeDialog;
}

class AcercaDeDialog : public QDialog
{
  Q_OBJECT

public:
  // explicit AcercaDeDialog(const QString& colorMode, QWidget *parent = nullptr);
  explicit AcercaDeDialog(Qt::ColorScheme colorMode, QWidget *parent = nullptr);
  ~AcercaDeDialog();

private:
  Ui::AcercaDeDialog *ui;
  const QFont font_{"Fira Code", 11};


private:
  void loadInfo_app() const noexcept;
  void setTextToAbout() const;

};

