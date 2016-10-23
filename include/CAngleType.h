#ifndef CANGLE_TYPE_H
#define CANGLE_TYPE_H

namespace CAngleType {
  enum Type {
    NONE,
    DEGREES,
    RADIANS
  };

  inline Type toAngleType(const std::string &str) {
    if      (CStrUtil::casecmp(str, "degrees") == 0)
      return DEGREES;
    else if (CStrUtil::casecmp(str, "radians") == 0)
      return RADIANS;
    else
      return NONE;
  }

  inline std::string toString(Type type) {
    if      (type == DEGREES) return "degrees";
    else if (type == RADIANS) return "radians";
    else                      return "none";
  }
}

#endif
