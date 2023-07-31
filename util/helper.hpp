#pragma once
#include <QtCore>
#include <QStringView>
#include <QDesktopServices>

namespace SW {

enum class Theme{ Modo_Claro, Modo_Oscuro };
enum class SessionStatus{ Session_start, Session_closed };

struct Helper_t{

  explicit Helper_t() = default;

  Helper_t(const Helper_t&) = delete;
  Helper_t(Helper_t&&) = delete;
  Helper_t& operator=(const Helper_t&) = delete;
  Helper_t& operator=(Helper_t&&) = delete;




  static bool verify_Values(const QStringView text1, const QStringView text2) noexcept{return (text1.toString() == text2.toString());}
  static bool open_Url(const QUrl& url) noexcept{return QDesktopServices::openUrl(url);}
  static bool urlValidate(QStringView url) noexcept;


  static QByteArray setColorReg(const QString &color) noexcept;
  static QString getColorReg(QByteArray dataColor) noexcept;

  static QString hashGenerator(const QByteArray&) noexcept;

  static QPalette set_Theme(SW::Theme theme) noexcept;


  //var member

  static constexpr std::string_view darkModeColor{"#2e8b57"};
  static constexpr std::string_view lightModeColor{"#ff7e00"};
};

} // namespace SW
