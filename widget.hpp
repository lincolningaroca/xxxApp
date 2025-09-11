#pragma once

#include <QWidget>
#include <QSqlDatabase>

#include "util/helper.hpp"
#include "helperdatabase/helperdb.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class QAction;
class QMenu;
struct SWTableModel;

class Widget : public QWidget{
  Q_OBJECT


public:
  explicit Widget(QWidget *parent = nullptr);
  ~Widget();

   void applyPreferredTheme(int pref);
   int loadSchemePreference();
   void loadLblSchemePreference();

  // inline static QString currentUser_{SW::Helper_t::currentUser_.value(SW::User::U_public)};
  // inline static SW::SessionStatus sessionStatus_{SW::SessionStatus::Session_closed};

private:
  Ui::Widget *ui;

  const QSqlDatabase db_{};

  QAction* delCategory_{ nullptr };
  QAction* openUrl_{ nullptr };
  QAction* editUrl_{ nullptr };
  QAction* quitUrl_{ nullptr };
  QAction* showDescDetail_{ nullptr };
  QAction* showPublicUrl_{ nullptr };
  QAction* moveUrl_{ nullptr };

  //excel exporter action

  QAction* exportToExcelFile_{nullptr};

  QMenu* contextMenu{nullptr};

  const QHash<Qt::ColorScheme, QString> themeType{
    {Qt::ColorScheme::Unknown, "Predeterminado del sistema"},
    {Qt::ColorScheme::Light, "Modo Claro"},
    {Qt::ColorScheme::Dark, "Modo Oscuro"}
  };



  // QSqlTableModel* xxxModel_{ nullptr };
  SWTableModel* xxxModel_{ nullptr };
  inline static uint32_t userId_{0};

  SW::HelperDataBase_t helperdb_{};

  void verifyAppColorScheme();
  //custom functions
  void initFrm() noexcept;
  void editAction(bool op) noexcept;
  bool validateSelectedRow() noexcept;

  void setUpTable(uint32_t categoryId = 1) noexcept;
  void setUpTableHeaders() const noexcept;
  void setTheme(Qt::ColorScheme theme) const noexcept;
  void writeSettings() const noexcept;
  void readSettings() noexcept;

  void showAlldescription() noexcept;

  void checkStatusContextMenu();
  void checkStatusSessionColor(const QString& text);

  void setLabelInfo(Qt::ColorScheme color, const QString& userName=SW::Helper_t::currentUser_.value(SW::User::U_public)) noexcept;
  void loadListCategory(uint32_t user_id) noexcept;
  void loadThemeComboBox() noexcept;

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
  void canRestoreDataBase() const noexcept;
  void canCreateBackUp() const noexcept;
  bool hasValidTableData() const noexcept;
  void canStartSession() noexcept;


  // QWidget interface
protected:
  virtual void closeEvent(QCloseEvent *event) override;


  // QObject interface
public:
  bool eventFilter(QObject* watched, QEvent* event) override;
};
