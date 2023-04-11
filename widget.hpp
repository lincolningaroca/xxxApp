#pragma once

#include <QWidget>
#include <QSqlDatabase>
#include <QHash>
#include <tuple>


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
    //slots


private:
    Ui::Widget *ui;
    const QSqlDatabase db{};
    QAction* delCategory{};
    QAction* openUrl_{};
    QAction* editUrl_{};
    QAction* quittUrl_{};
    QSqlTableModel* xxxModel_{};

    //custom functions
    void initFrm() noexcept;
    void editAction(bool op) noexcept;
    bool validateSelectedRow() noexcept;

    void setUpTable(uint32_t categoryId) noexcept;
    void setUpTableHeaders() const noexcept;
    void setTheme(Theme theme) const noexcept;
    void writeSettings() const noexcept;
    void readSettings() noexcept;
    bool urlValidate(const QString& url) const noexcept;

    void setLabelInfo(const QString &color) noexcept;
    QByteArray setColorReg(const QString &color) const noexcept;
    QString getColorReg(QByteArray dataColor) noexcept;

    void loadListCategory() noexcept;
    QStringList dataCategory(uint32_t category_id) noexcept;

    //menbers var
    QHash<uint32_t, QString> categoryList{};
    QHash<uint32_t, QString> urlList{};
    const QString darkModeColor{"#2e8b57"};
    const QString lightModeColor{"#ff7e00"};

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
    bool updateCategory(const QString &url, const QString &desc, uint32_t category_id) const noexcept;
    bool saveCategoryData(const QString &catName, const QString& desc) const noexcept;
    void btnEdit() noexcept;




    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *event) override;

};
