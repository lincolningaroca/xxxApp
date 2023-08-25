#pragma once

#include <QWidget>
#include <QSqlDatabase>
#include <QHash>
#include <util/helper.hpp>
#include <helperdatabase/helperdb.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class QAction;
class QSqlTableModel;

class Widget : public QWidget{
  Q_OBJECT


public:
  explicit Widget(QWidget *parent = nullptr);
  ~Widget();


private:
  Ui::Widget *ui;
  const QSqlDatabase db_{};
  QAction* delCategory{ nullptr };
  QAction* openUrl_{ nullptr };
  QAction* editUrl_{ nullptr };
  QAction* quittUrl_{ nullptr };
  QSqlTableModel* xxxModel_{ nullptr };
  inline static uint32_t userId_{0};
  SW::SessionStatus sessionStatus_{SW::SessionStatus::Session_closed};
  SW::HelperDataBase_t helperdb_{};

  //custom functions
  void initFrm() noexcept;
  void editAction(bool op) noexcept;
  bool validateSelectedRow() noexcept;

  void setUpTable(uint32_t categoryId = 1) noexcept;
  void setUpTableHeaders() const noexcept;
  void setTheme(SW::Theme theme) const noexcept;
  void writeSettings() const noexcept;
  void readSettings() noexcept;


  void setLabelInfo(const QString &color, const QString& userName="PUBLIC") noexcept;
  void loadListCategory(uint32_t user_id) noexcept;

  //menbers var
  QHash<uint32_t, QString> categoryList{};
  QHash<uint32_t, QString> urlList{};

  void setUpCboCategoryContextMenu() noexcept;
  void setUptvUrlContextMenu() noexcept;


  bool deleteAll() noexcept;
  void verifyContextMenu() noexcept;
  void setCboCategoryToolTip() noexcept;

  void openUrl() noexcept;
  void quitUrl() noexcept;
  void hastvUrlData() noexcept;

  void btnEdit() noexcept;

  void has_data() noexcept;
  //    void userId();




  // QWidget interface
protected:
  virtual void closeEvent(QCloseEvent *event) override;

};
