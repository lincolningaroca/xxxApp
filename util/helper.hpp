#pragma once

#include <windows.h>

#include <QStandardPaths>
#include <QCryptographicHash>
#include <QStringView>
#include <QDesktopServices>
#include <QApplication>
#include <QPalette>

namespace SW {

enum class [[deprecated("Usar mejor Qt::ColorScheme")]] Theme{ Light_Mode, Dark_Mode };
enum class SessionStatus{ Session_start, Session_closed };
enum class User{ U_public, U_user };
enum class AuthType{ Numeric_pin, Secret_Question };

struct Helper_t{

  explicit Helper_t() = default;

  Helper_t(const Helper_t&) = delete;
  Helper_t(Helper_t&&) = delete;
  Helper_t& operator=(const Helper_t&) = delete;
  Helper_t& operator=(Helper_t&&) = delete;




  [[nodiscard]]static bool verify_Values(const QStringView text1, const QStringView text2) noexcept{return (text1.toString() == text2.toString());}
  [[nodiscard]]static bool open_Url(const QUrl& url) noexcept{return QDesktopServices::openUrl(url);}
  [[nodiscard]]static bool urlValidate(QStringView url) noexcept;


  [[nodiscard]]static bool createDataBase_dir() noexcept;

  [[nodiscard]]static QString generateSecurePassword(uint32_t length = 8) noexcept;
  [[nodiscard]]static bool isPasswordSecure(const QString& password) noexcept;


  [[nodiscard]]static QByteArray setColorReg(const QString &color) noexcept;
  [[nodiscard]]static QString getColorReg(QByteArray dataColor) noexcept;

  [[nodiscard]]static QString hashGenerator(const QByteArray&) noexcept;
  // static QByteArray encrypt_txt(const QString& txt) noexcept;
  // static QString decrypt_txt(const QByteArray &txt) noexcept;

  [[nodiscard]]static QPalette set_Theme(Qt::ColorScheme theme) noexcept;

  [[nodiscard]]static Qt::ColorScheme checkSystemColorScheme() noexcept;



  //var member

  // static constexpr std::string_view darkModeColor{"#2e8b57"};
  // static constexpr std::string_view lightModeColor{"#ff7e00"};

  inline static const QHash<Qt::ColorScheme, QString> lblColorMode{
    // {Qt::ColorScheme::Light, "#ff7e00"},
    {Qt::ColorScheme::Light, "#008cf9"},
    {Qt::ColorScheme::Dark, "#2e8b57"}
  };
  [[nodiscard]]static QString AppLocalDataLocation(){return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation).append(dbDir_name);}
  [[nodiscard]]static QString app_pathLocation(){return QApplication::applicationDirPath();}
  [[nodiscard]]static QString appName(){return QApplication::applicationName();}

  static void setLastOpenedDirectory(const QString &directory);
  [[nodiscard]]static QString getLastOpenedDirectory();

  [[nodiscard]] static QVariant readData(QByteArray&& data);
  [[nodiscard]]static QByteArray writeData(const QVariant& data);

  static bool nativeRegistryKeyExists(HKEY hive, const QString& subKey);

  //encryp/decrypt metods

  // static QString encrypt(const QString& text, const QString& key=key_);
  // static QString decrypt(const QString& encryptedText, const QString& key=key_);
  static QString encrypt(const QString& plainText, const QByteArray& key = hashKey, const QByteArray& iv = hashIv);
  static QString decrypt(const QString& encryptedText, const QByteArray& key = hashKey, const QByteArray& iv = hashIv);
  inline static const QHash<SW::User, QString> currentUser_{
    {SW::User::U_public, "PUBLIC"},
    {SW::User::U_user, "USER"}
  };

  inline static const QString defaultUser{QStringLiteral("public")};
  inline static QString current_user_{defaultUser};
  inline static SW::SessionStatus sessionStatus_{SW::SessionStatus::Session_closed};


private:
  inline static const QString dbDir_name{"/xxxdatabase"};
  inline static const QPalette standardPalette_{QGuiApplication::palette()};
  // inline static const QString key_{"AbCdEfGhIjKlMnOpQrStUvWxYz@!#$%&/()=?¡1234567890"};
  // inline static const QByteArray key_{"AbCdEfGhIjKlMnOpQrStUvWxYz@!#$%&/()=?¡1234567890"};
  // inline static const QByteArray iv_{"AbCdEfGhIjKlMnOpQrStUvWxYz@!#$%&/()=?¡1234567890"};

  static Qt::ColorScheme detectSystemColorScheme();

  //variables y constantes de encriptacion
  //inline static QAESEncryption encrypt{QAESEncryption::AES_256, QAESEncryption::CBC};
  inline static const QString key {"AbCdEfGhIjKlMnOpQrStUvWxYz@!#$%&/()=?¡1234567890"};
  inline static const QString iv {"lincoln_carolina@m1am0r"};
  inline static const QByteArray hashKey {QCryptographicHash::hash(key.toLatin1(), QCryptographicHash::Sha256)};
  inline static const QByteArray hashIv {QCryptographicHash::hash(iv.toLatin1(), QCryptographicHash::Md5)};

};

} // namespace SW
