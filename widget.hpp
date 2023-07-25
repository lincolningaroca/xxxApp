#pragma once

#include <QWidget>
#include <QSqlDatabase>
#include <QHash>
#include <string_view>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class QAction;
class QSqlTableModel;
class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
    enum class Theme{ Modo_Claro, Modo_Oscuro };
    enum class SessionStatus{ Session_start, Session_closed };
//    inline static uint32_t userid_{0};
    //slots
    [[nodiscard]]uint32_t getUser_id(const QString &user, const QString &user_profile = "PUBLIC") noexcept;



private:
    Ui::Widget *ui;
    const QSqlDatabase db_{};
    QAction* delCategory{ nullptr };
    QAction* openUrl_{ nullptr };
    QAction* editUrl_{ nullptr };
    QAction* quittUrl_{ nullptr };
    QSqlTableModel* xxxModel_{ nullptr };
    inline static uint32_t userId_{0};
    SessionStatus sessionStatus_{SessionStatus::Session_closed};

    //custom functions
    void initFrm() noexcept;
    void editAction(bool op) noexcept;
    bool validateSelectedRow() noexcept;

    void setUpTable(uint32_t categoryId = 1) noexcept;
    void setUpTableHeaders() const noexcept;
    void setTheme(Theme theme) const noexcept;
    void writeSettings() const noexcept;
    void readSettings() noexcept;
    bool urlValidate(const QString& url) const noexcept;

    void setLabelInfo(const QString &color, const QString& userName="PUBLIC") noexcept;
    QByteArray setColorReg(const QString &color) const noexcept;
    QString getColorReg(QByteArray dataColor) noexcept;

    void loadListCategory(uint32_t user_id) noexcept;
    QStringList dataCategory(uint32_t category_id) noexcept;

    //menbers var
    QHash<uint32_t, QString> categoryList{};
    QHash<uint32_t, QString> urlList{};
    static constexpr std::string_view darkModeColor{"#2e8b57"};
    static constexpr std::string_view lightModeColor{"#ff7e00"};

    std::tuple<bool, QString> verifyDeleteCategory() noexcept;
    void setUpCboCategoryContextMenu() noexcept;
    void setUptvUrlContextMenu() noexcept;
    bool deleteCategory() const noexcept;
    bool deleteUrls(uint8_t op) const noexcept;
    bool deleteAll()const noexcept;
    void verifyContextMenu() noexcept;
    void setCboCategoryToolTip() noexcept;

    void openUrl() noexcept;
    void quitUrl() noexcept;
    void hastvUrlData() noexcept;

    bool saveData(const QString &url, const QString &desc, uint32_t id) const noexcept;
    bool updateCategory(const QString&, const QString&, uint32_t, uint32_t) const noexcept;
    bool saveCategoryData(const QString &catName, const QString& desc, uint32_t userid) const noexcept;
    void btnEdit() noexcept;
//    void userId();




    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *event) override;

};
