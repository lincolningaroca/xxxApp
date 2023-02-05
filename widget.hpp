#pragma once

#include <QWidget>
#include <QSqlDatabase>
#include <QHash>
#include <tuple>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
    enum class Theme{ Modo_Claro, Modo_Oscuro };
    //slots
    bool saveData(const QString &url, const QString &desc, uint32_t id) const noexcept;
    bool saveCategoryData(const QString &catName, const QString& desc) const noexcept;
    void btnEdit() noexcept;

private:
    Ui::Widget *ui;
    const QSqlDatabase db{};
    QAction* delCategory{};

    //custom functions
    void initFrm() noexcept;
    void editAction(bool op) noexcept;
    bool validateSelectedRow() noexcept;
    //connect to data base
    //  bool connectToDB();
    void setUpTable(uint32_t categoryId) noexcept;
    void setUpTableHeaders() const noexcept;
    void setTheme(Theme theme) const noexcept;
    void writeSettings() const noexcept;
    void readSettings() noexcept;
    bool urlValidate(const QString& url) const noexcept;

    void setLabelInfo(const QString &color) noexcept;
    QByteArray setColorReg(const QString &color) const noexcept;
    QString getColorReg(QByteArray dataColor) noexcept;
    void loadCategopryData() noexcept;
    void loadListCategory() noexcept;

    //menbers var
    QHash<uint32_t, QString> categoryList{};
    const QString darkModeColor{"#2e8b57"};
    const QString lightModeColor{"#ff7e00"};

    bool verifyDeleteGroup()const noexcept;
    void setUpCboCategoryContextMenu() noexcept;

    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *event) override;
};
