#include "helperdb.hpp"
#include <util/helper.hpp>
#include <QSqlError>


namespace SW {

  HelperDataBase_t::HelperDataBase_t()
    :db_{QSqlDatabase::database("xxxConection")},
      qry_{db_}
  {

  }

  bool HelperDataBase_t::userExists(QStringView user) noexcept{

    [[maybe_unused]]
        auto ret = qry_.prepare("SELECT COUNT(*) FROM users WHERE user = ?");
    qry_.addBindValue(user.toString());

    if(qry_.exec()){
        qry_.first();
      }
    return (qry_.value(0).toUInt() == 1);
  }

  bool HelperDataBase_t::createUser( QStringView user,  QStringView password,  QStringView user_prof,
                                     QStringView rescue_type,  QStringView val1,  QStringView val2) noexcept{

    [[maybe_unused]]
        auto res = qry_.prepare("INSERT INTO users(user,password,user_profile,rescue_type,first_value,confirm_value) "
                                "VALUES(?,?,?,?,?,?)");
    qry_.addBindValue(user.toString());
    qry_.addBindValue(password.toString());
    qry_.addBindValue(user_prof.toString());
    qry_.addBindValue(rescue_type.toString());
    qry_.addBindValue(val1.toString());
    qry_.addBindValue(val2.toString());
    return qry_.exec();

  }

  bool HelperDataBase_t::logIn(QStringView user, QStringView password) noexcept{

    [[maybe_unused]]
        auto res = qry_.prepare("SELECT COUNT(*) FROM users WHERE user = ? AND password = ?");
    qry_.addBindValue(user.toString().simplified());
    qry_.addBindValue(SW::Helper_t::hashGenerator(password.toString().simplified().toLatin1()));
    if(qry_.exec())
      qry_.first();

    return(qry_.value(0).toUInt() == 1);
  }


  bool HelperDataBase_t::saveCategoryData( QStringView catName,  QStringView desc, uint32_t userid)  noexcept{

    [[maybe_unused]]
        auto res = qry_.prepare("INSERT INTO category(category_name, desc, userid) VALUES(?,?,?)");
    qry_.addBindValue(catName.toString().simplified().toUpper(), QSql::In);
    qry_.addBindValue(desc.toString().simplified().toUpper(), QSql::In);
    qry_.addBindValue(userid, QSql::In);
    return qry_.exec();
  }

  bool HelperDataBase_t::updateCategory( QStringView url,  QStringView desc, uint32_t category_id, uint32_t user_id) noexcept{

    [[ maybe_unused ]]
        auto res = qry_.prepare("UPDATE category SET category_name=?, desc=? WHERE category_id=? AND userid=? ");
    qry_.addBindValue(url.toString().simplified(), QSql::In);
    qry_.addBindValue(desc.toString().simplified(), QSql::In);
    qry_.addBindValue(category_id, QSql::In);
    qry_.addBindValue(user_id, QSql::In);
    return qry_.exec();

  }

  bool HelperDataBase_t::saveData_url(QStringView url, QStringView desc, std::uint32_t id) noexcept{

    [[maybe_unused]]
        auto res = qry_.prepare("INSERT INTO urls(url,desc,categoryid) VALUES(?,?,?)");
    qry_.addBindValue(url.toString().simplified(), QSql::In);
    qry_.addBindValue(desc.toString().simplified().toUpper(), QSql::In);
    qry_.addBindValue(id, QSql::In);
    return qry_.exec();
  }

  bool HelperDataBase_t::deleteUrls(std::uint8_t op, uint32_t categoryId, uint32_t urlId) noexcept{

    if(op == 1){
        [[maybe_unused]] auto res=qry_.prepare("DELETE FROM urls WHERE categoryid=?");
        //        auto categoryId=categoryList.key(category);
        qry_.addBindValue(categoryId, QSql::In);
        return qry_.exec();

      }else{
        [[maybe_unused]] auto res=qry_.prepare("DELETE FROM urls WHERE url_id=?");
        //        auto currentRow = ui->tvUrl->currentIndex().row();
        //        auto url = ui->tvUrl->model()->index(currentRow, 1).data().toString();
        //        auto urlId=urlList.key(url);
        qry_.addBindValue(urlId, QSql::In);
        return qry_.exec();

      }
  }

  QHash<uint32_t, QString> HelperDataBase_t::loadList_Category(uint32_t user_id) noexcept
  {
    QHash<uint32_t, QString> categoryList{};
    categoryList.clear();

    [[maybe_unused]]
        auto res=qry_.prepare("SELECT category_id, category_name FROM category WHERE userid = ?");

    qry_.addBindValue(user_id);
    if(qry_.exec())
      while(qry_.next()){
          categoryList.insert(qry_.value(0).toUInt(),
                              qry_.value(1).toString());
        }
    return categoryList;

  }

  bool HelperDataBase_t::deleteCategory(uint32_t categoryId) noexcept
  {

    [[maybe_unused]]
        auto res=qry_.prepare("DELETE FROM category WHERE category_id=?");
    //    auto categoryId=categoryList.key(ui->cboCategory->currentText());
    qry_.addBindValue(categoryId, QSql::In);
    return qry_.exec();


  }

  bool HelperDataBase_t::validateAnswer(QStringView respuesta, uint32_t userId) noexcept{
    [[maybe_unused]]
        auto res = qry_.prepare("SELECT COUNT(*) FROM users WHERE confirm_value = ? AND user_id = ?");
    qry_.addBindValue(SW::Helper_t::hashGenerator(respuesta.toString().simplified().toLatin1()));
    qry_.addBindValue(userId);
    if(qry_.exec())
      qry_.first();

    return(qry_.value(0).toUInt() == 1);

  }

  bool HelperDataBase_t::resetPassword(QStringView password, uint32_t userId) noexcept
  {
    [[ maybe_unused ]]
    auto res = qry_.prepare("UPDATE users SET password=? WHERE user_id=?");
    auto pass = password.toString();
    qry_.addBindValue(SW::Helper_t::hashGenerator(pass.simplified().toLatin1()), QSql::In);
    qry_.addBindValue(userId, QSql::In);

    return qry_.exec();

  }

  uint32_t HelperDataBase_t::getUser_id(const QString& user, const QString& user_profile) noexcept  {

    uint32_t ret_value{0};
    [[maybe_unused]]
        auto res = qry_.prepare("SELECT user_id FROM users WHERE user = ? AND user_profile = ?");
    qry_.addBindValue(user.simplified());
    qry_.addBindValue(user_profile.simplified());
    if(qry_.exec())
      qry_.first();
    ret_value = qry_.value(0).toUInt();
    return ret_value;

  }

  QStringList HelperDataBase_t::dataCategory(uint32_t category_id) noexcept {

    QStringList dataCategory{};
    [[maybe_unused]]
        auto res=qry_.prepare("SELECT category_name, desc FROM category WHERE category_id=?");
    qry_.addBindValue(category_id);
    if(qry_.exec())
      if(qry_.next()){
          dataCategory.append(qry_.value(0).toString());
          dataCategory.append(qry_.value(1).toString());
        }

    return dataCategory;

  }

  std::tuple<bool, QString>
  HelperDataBase_t::verifyDeleteCategory(uint32_t categoryId) noexcept{
    bool ret{false};
    QString errMessage{};

    [[maybe_unused]]
        auto res=qry_.prepare("SELECT COUNT (*) FROM urls WHERE categoryid=?");

    qry_.addBindValue(categoryId, QSql::In);
    if(!qry_.exec()){
        errMessage=qry_.lastError().text();
        return std::tuple{ret, errMessage};
      }

    if(qry_.isSelect())
      qry_.first();

    if(qry_.value(0).toUInt() == 0) ret=true;


    return std::tuple{ret, errMessage};

  }

  QString HelperDataBase_t::validateRescueType(uint32_t userId) noexcept{

    [[maybe_unused]]
        auto res=qry_.prepare("SELECT rescue_type FROM users WHERE user_id = ?");

    qry_.addBindValue(userId, QSql::In);
    if(qry_.exec())
      qry_.first();

    return qry_.value(0).toString();

  }

  QString HelperDataBase_t::getQuestion(uint32_t userId) noexcept{
    [[maybe_unused]]
        auto res=qry_.prepare("SELECT first_value FROM users WHERE user_id = ?");

    qry_.addBindValue(userId, QSql::In);
    if(qry_.exec())
      qry_.first();

    return qry_.value(0).toString();

  }


}//namespace SW



