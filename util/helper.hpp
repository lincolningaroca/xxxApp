#pragma once
#include <QtCore>
#include <string_view>

namespace SW {

struct Helper_t{


  static constexpr std::string_view darkModeColor{"#2e8b57"};
  static constexpr std::string_view lightModeColor{"#ff7e00"};


  explicit Helper_t() = default;

  static QString hashGenerator(const QByteArray&) noexcept;
};

} // namespace SW
