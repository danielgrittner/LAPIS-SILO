#include "silo/common/string_utils.h"

#include <algorithm>

namespace silo {

std::vector<std::string> splitBy(const std::string& value, const std::string_view delimiter) {
   std::vector<std::string> splits;

   std::string_view value_sv(value.c_str(), value.size());
   auto next_split_point = value_sv.find(delimiter);

   while (next_split_point != std::string::npos) {
      splits.emplace_back(value_sv.substr(0, next_split_point));
      value_sv = value_sv.substr(next_split_point + delimiter.size());
      next_split_point = value_sv.find(delimiter);
   }

   splits.emplace_back(value_sv);

   return splits;
}

}  // namespace silo