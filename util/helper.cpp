#include "helper.hpp"

QString SW::Helper_t::hashGenerator(const QByteArray &data) noexcept{
  QCryptographicHash crypto(QCryptographicHash::Sha3_256);
  crypto.addData(data);
  return QString{crypto.result().toHex()};

}
