#include "helper.hpp"
#include <QPalette>
#include <QApplication>
#include <QCryptographicHash>
#include <QStyle>

extern "C"{
#include "openssl/rand.h"
}

namespace SW {

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
    auto match = regex.match(url);
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

    static QRegularExpression passwordRegex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[!@#$%^&*(),.?\":{}|<>])[A-Za-z\\d!@#$%^&*(),.?\":{}|<>]{8,}$");

    return passwordRegex.match(password).hasMatch();

  }

  QPalette  Helper_t::set_Theme(SW::Theme theme) noexcept
  {
    QPalette mPalette{};
    //  qApp->setStyle("Fusion");
    if(theme == Theme::Light_Mode)
      mPalette = qApp->style()->standardPalette();
    else{
        mPalette.setColor(QPalette::Window, QColor(53, 53, 53));
        mPalette.setColor(QPalette::WindowText, Qt::white);
        mPalette.setColor(QPalette::Base, QColor(25, 25, 25));
        mPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
        mPalette.setColor(QPalette::ToolTipBase, QColor(53, 53, 53));
        mPalette.setColor(QPalette::ToolTipText, Qt::white);
        mPalette.setColor(QPalette::Text, Qt::white);
        mPalette.setColor(QPalette::PlaceholderText,QColor(127,127,127));
        mPalette.setColor(QPalette::Button, QColor(53, 53, 53));
        mPalette.setColor(QPalette::ButtonText, Qt::white);
        mPalette.setColor(QPalette::BrightText, Qt::red);
        mPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        mPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        mPalette.setColor(QPalette::HighlightedText, Qt::black);
        mPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(164, 166, 168));
        mPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(164, 166, 168));
        mPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(164, 166, 168));
        mPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(164, 166, 168));
        mPalette.setColor(QPalette::Disabled, QPalette::Base, QColor(68, 68, 68));
        mPalette.setColor(QPalette::Disabled, QPalette::Window, QColor(68, 68, 68));
        mPalette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(68, 68, 68));
      }

    return mPalette;

  }

  // QString Helper_t::encrypt(const QString& text, const QString& key)  {
  //   QByteArray encryptedData;
  //   QByteArray textData = text.toUtf8();
  //   QByteArray keyData = key.toUtf8();

  //   QCryptographicHash hash(QCryptographicHash::Sha512);
  //   hash.addData(keyData);
  //   QByteArray hashedKey = hash.result();

  //   // Encriptar el texto utilizando AES
  //   for (int i = 0; i < textData.size(); ++i) {
  //       char encryptedChar = textData.at(i) ^ hashedKey.at(i % hashedKey.size());
  //       encryptedData.append(encryptedChar);
  //     }

  //   return QString::fromUtf8(encryptedData.toBase64());

  // }
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

  // QString Helper_t::decrypt(const QString& encryptedText, const QString& key)  {
  //   QByteArray decryptedData;
  //   QByteArray encryptedData = QByteArray::fromBase64(encryptedText.toUtf8());
  //   QByteArray keyData = key.toUtf8();

  //   QCryptographicHash hash(QCryptographicHash::Sha512);
  //   hash.addData(keyData);
  //   QByteArray hashedKey = hash.result();

  //   // Desencriptar el texto utilizando AES
  //   for (int i = 0; i < encryptedData.size(); ++i) {
  //       char decryptedChar = encryptedData.at(i) ^ hashedKey.at(i % hashedKey.size());
  //       decryptedData.append(decryptedChar);
  //     }

  //   return QString::fromUtf8(decryptedData);

  // }
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

} // namespace SW

