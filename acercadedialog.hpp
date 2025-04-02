#pragma once

#include <QDialog>
#include <QFontDataBase>
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

private:
  void loadInfo_app() const noexcept;
  void setTextToAbout() const;

  void setImage(Qt::ColorScheme colorMode);

  int id = QFontDatabase::addApplicationFont(":/font/FiraCode-Regular.ttf");
  QString family = QFontDatabase::applicationFontFamilies(id).at(0);
  const QFont customFont{family, 10};

};

