#pragma once

namespace SW {

struct HelperDataBase_t{

  explicit HelperDataBase_t() = default;

  HelperDataBase_t(const HelperDataBase_t&) = delete;
  HelperDataBase_t(HelperDataBase_t&&) = delete;
  HelperDataBase_t& operator=(const HelperDataBase_t&) = delete;
  HelperDataBase_t& operator=(HelperDataBase_t&&) = delete;

};

} // namespace SW
