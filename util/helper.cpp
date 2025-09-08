#include "helper.hpp"
#include <QPalette>
#include <QApplication>
#include <QCryptographicHash>
#include <QIODevice>
#include <QStyle>
#include <QRegularExpression>
#include <QDir>
#include <QSettings>
#include <random>
#include <windows.h>
#include <QStyleFactory>

extern "C"{
#include "openssl/rand.h"
}

namespace SW {

Qt::ColorScheme Helper_t::detectSystemColorScheme() {
  HKEY hKey;
  DWORD dwType = REG_DWORD;
  DWORD dwValue = 0;
  DWORD dwSize = sizeof(dwValue);

  // Ruta del registro de Windows para el tema del sistema
  const wchar_t* subKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";

  if (RegOpenKeyExW(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
    RegQueryValueExW(hKey, L"AppsUseLightTheme", nullptr, &dwType, reinterpret_cast<LPBYTE>(&dwValue), &dwSize);
    RegCloseKey(hKey);
  }

  // Retorna Qt::ColorScheme::Light para modo claro o Qt::ColorScheme::Dark para modo oscuro
  return dwValue == 1 ? Qt::ColorScheme::Light : Qt::ColorScheme::Dark;
}

QString Helper_t::hashGenerator(const QByteArray &data) noexcept{
  QCryptographicHash crypto(QCryptographicHash::Sha3_512);
  crypto.addData(data);
  return QString{crypto.result().toHex()};

}

QByteArray Helper_t::setColorReg(const QString& color) noexcept
{
  QByteArray data;
  QDataStream dataStream(&data, QIODevice::WriteOnly);
  dataStream << color;
  return data;

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
QString Helper_t::getColorReg(QByteArray dataColor) noexcept
{
  //  QByteArray data(dataColor);
  QDataStream dataStream(&dataColor, QIODevice::ReadOnly);
  QString color;
  dataStream >> color;

  return color;

}

bool Helper_t::urlValidate(QStringView url) noexcept
{
  // static QRegularExpression regex(R"(^(https?|ftp)://[^\s/$.?#].[^\s]*$)");
  static QRegularExpression regex(R"(^(https?://|ftp://)?(www\.)?[a-zA-Z0-9.-]+(\.[a-zA-Z]{2,6})(/[^\s]*)?$)");
  auto match = regex.matchView(url);
  return match.hasMatch();
}

bool Helper_t::createDataBase_dir() noexcept{

  auto appLocation = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
  QDir docDir(appLocation);

  return docDir.mkpath(QStringLiteral("xxxdatabase"));

}

QString Helper_t::generateSecurePassword(uint32_t length) noexcept{

  // if (length < 4) {
  //     throw std::invalid_argument("La longitud mínima de la contraseña debe ser 4");
  //   }

  const QString chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*?:{}|<>~-_=+[]/;\\";
  const int lowercaseEnd = 26;
  const int uppercaseEnd = 52;
  const int numbersEnd = 62;

  QString password;

  // Generar bytes aleatorios
  QByteArray randomBytes(length, 0);
  if (RAND_bytes(reinterpret_cast<unsigned char*>(randomBytes.data()), length) != 1) {
    qFatal("Error al generar bytes aleatorios con OpenSSL");
  }

  // Asegurar un carácter de cada tipo
  password += chars[static_cast<unsigned char>(randomBytes[0]) % lowercaseEnd];
  password += chars[lowercaseEnd + (static_cast<unsigned char>(randomBytes[1]) % (uppercaseEnd - lowercaseEnd))];
  password += chars[uppercaseEnd + (static_cast<unsigned char>(randomBytes[2]) % (numbersEnd - uppercaseEnd))];
  password += chars[numbersEnd + (static_cast<unsigned char>(randomBytes[3]) % (chars.length() - numbersEnd))];

  // Llenar el resto de la contraseña
  for (uint32_t i = 4; i < length; ++i) {
    password += chars[static_cast<unsigned char>(randomBytes[i]) % chars.length()];
  }

  // Mezclar los caracteres
  std::shuffle(password.begin(), password.end(), std::default_random_engine(static_cast<unsigned long>(randomBytes[0])));

  return password;


}

bool Helper_t::isPasswordSecure(const QString &password) noexcept{

  static QRegularExpression passwordRegex(R"(^(?=.*[a-z])(?=.*[A-Z])(?=.*\d)(?=.*[!@#$%^&*(),.?\":{}|<>])[A-Za-z\d!@#$%^&*(),.?\":{}|<>]{8,}$)");

  return passwordRegex.match(password).hasMatch();

}

QPalette  Helper_t::set_Theme(Qt::ColorScheme theme) noexcept
{
  QPalette mPalette{};

  if(theme == Qt::ColorScheme::Light){
    qApp->setStyle(QStyleFactory::create("Windowsvista"));
    // qApp->setPalette(QGuiApplication::palette());
    mPalette.setColor(QPalette::Highlight, QColor(204, 232, 255));
    mPalette.setColor(QPalette::HighlightedText, Qt::black);
    mPalette.setColor(QPalette::Link, QColor(0, 109, 255));
  }else{
    qApp->setStyle(QStyleFactory::create("Fusion"));
    mPalette.setColor(QPalette::Window, QColor(30, 30, 30));
    mPalette.setColor(QPalette::Base, QColor(40, 40, 40));
    mPalette.setColor(QPalette::AlternateBase, QColor(50, 50, 50));
    mPalette.setColor(QPalette::Text, QColor(220, 220, 220));
    mPalette.setColor(QPalette::Button, QColor(45, 45, 45));
    mPalette.setColor(QPalette::ButtonText, QColor(220, 220, 220));
    mPalette.setColor(QPalette::Highlight, QColor(39, 191, 115, 60));
    mPalette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
    mPalette.setColor(QPalette::BrightText, QColor(255, 0, 0));
    mPalette.setColor(QPalette::Link, QColor(39, 191, 115, 250));
    mPalette.setColor(QPalette::PlaceholderText, QColor(150, 150, 150));
    mPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(130, 130, 130));
    mPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(130, 130, 130));

  }

  return mPalette;

}

Qt::ColorScheme Helper_t::checkSystemColorScheme() noexcept{
  return detectSystemColorScheme();

}

QString Helper_t::encrypt(const QString& plainText, const QByteArray& key, const QByteArray& iv){

  QByteArray plainData = plainText.toUtf8();
  QByteArray encryptedData(plainData.size() + EVP_MAX_BLOCK_LENGTH, 0);
  int encryptedLen = 0;

  EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
  EVP_EncryptInit(ctx, EVP_aes_256_cbc(), reinterpret_cast<const unsigned char*>(key.data()), reinterpret_cast<const unsigned char*>(iv.data()));
  EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(encryptedData.data()), &encryptedLen, reinterpret_cast<const unsigned char*>(plainData.data()), static_cast<int>(plainData.size()));

  int finalLen = 0;
  EVP_EncryptFinal(ctx, reinterpret_cast<unsigned char*>(encryptedData.data()) + encryptedLen, &finalLen);
  encryptedLen += finalLen;

  EVP_CIPHER_CTX_free(ctx);

  return QString::fromUtf8(encryptedData.left(encryptedLen).toBase64());
}


QString Helper_t::decrypt(const QString& encryptedText, const QByteArray& key, const QByteArray& iv){

  QByteArray encryptedData = QByteArray::fromBase64(encryptedText.toUtf8());
  QByteArray decryptedData(encryptedData.size(), 0);
  int decryptedLen = 0;

  EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
  EVP_DecryptInit(ctx, EVP_aes_256_cbc(), reinterpret_cast<const unsigned char*>(key.data()), reinterpret_cast<const unsigned char*>(iv.data()));
  EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(decryptedData.data()), &decryptedLen, reinterpret_cast<const unsigned char*>(encryptedData.data()), static_cast<int>(encryptedData.size()));

  int finalLen = 0;
  EVP_DecryptFinal(ctx, reinterpret_cast<unsigned char*>(decryptedData.data()) + decryptedLen, &finalLen);
  decryptedLen += finalLen;

  EVP_CIPHER_CTX_free(ctx);

  return QString::fromUtf8(decryptedData.left(decryptedLen));
}

QString Helper_t::getLastOpenedDirectory(){

  QSettings settings(qApp->organizationName(), qApp->applicationName());
  auto ret = readData(settings.value("lastOpenedDirectory", QDir::homePath()).toByteArray());
  return ret.toString();

}

void Helper_t::setLastOpenedDirectory(const QString &directory){

  QSettings settings(qApp->organizationName(), qApp->applicationName());
  settings.setValue("lastOpenedDirectory", writeData(directory));
}

QByteArray Helper_t::writeData(const QVariant &data){
  QByteArray data_{};
  QDataStream out{&data_, QIODevice::WriteOnly};
  out.setVersion(QDataStream::Qt_6_2);
  out << data;
  return data_;

}

bool Helper_t::nativeRegistryKeyExists(HKEY hive, const QString &subKey){

  HKEY hKey;
  LONG result = RegOpenKeyExW(hive, subKey.toStdWString().c_str(), 0, KEY_READ, &hKey);

  if (result == ERROR_SUCCESS) {
    RegCloseKey(hKey);
    return true;
  }
  return false;

}

QVariant Helper_t::readData(QByteArray &&data){

  QVariant data_{};
  QDataStream in(&data, QIODevice::ReadOnly);
  in.setVersion(QDataStream::Qt_6_2);
  in >> data_;
  return data_;

}


} // namespace SW

