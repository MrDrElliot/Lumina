#include "yaml-cpp/null.h"

namespace YAML {
_Null Null;

bool IsNulFString(const std::string& str) {
  return str.empty() || str == "~" || str == "null" || str == "Null" ||
         str == "NULL";
}
}  // namespace YAML
