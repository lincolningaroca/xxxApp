#include "helper.hpp"
#include <QPalette>
#include <QApplication>
//#include <QGuiApplication>
#include <QStyle>

namespace SW {

  QString Helper_t::hashGenerator(const QByteArray &data) noexcept{
    QCryptographicHash crypto(QCryptographicHash::Sha3_256);
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
    //  static QRegularExpression regex("^(http|https:\\/\\/)?[\\w\\-]+(\\.[\\w\\-]+)+[/#?]?.*$");
    //  static QRegularExpression regex(R"(^(http:\/\/|https:\/\/)?[w]{3}(\.\w+)+[.\w]{2,3}[.\w]?[/#?]?.*$)");
    //  static QRegularExpression regex(R"(^((http|https):\/\/)?(www\.([\w+]))+\.([\w])?)");
    static QRegularExpression regex(R"(^(:?(http|https)://)?(:?www\..+\.\w{2,4})(:?\.\w)?(:?.+)?$)");
    auto match = regex.match(url);
    return match.hasMatch();
  }

  bool Helper_t::createDataBase_dir() noexcept{

    auto appLocation = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir docDir(appLocation);

    return docDir.mkpath("xxxdatabase");

  }

  QPalette  Helper_t::set_Theme(SW::Theme theme) noexcept
  {
    QPalette mPalette{};
    //  qApp->setStyle("Fusion");
    if(theme == Theme::Modo_Claro)
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

} // namespace SW

