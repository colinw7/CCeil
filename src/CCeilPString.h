#ifndef CCEILP_STRING_H
#define CCEILP_STRING_H

#include <CCeilPObj.h>

class ClParserString : public ClParserObj {
 private:
  std::string text_;

 protected:
  friend class CRefPtr<ClParserString>;
  friend class ClParserValue;

  static ClParserStringPtr createString(const std::string &str="") {
    ClParserString *pstr = new ClParserString(str);

    return ClParserStringPtr(pstr);
  }

  static ClParserStringPtr createString(const char *text, uint len) {
    ClParserString *pstr = new ClParserString(text, len);

    return ClParserStringPtr(pstr);
  }

  static ClParserStringPtr createString(const ClParserString &str) {
    ClParserString *pstr = new ClParserString(str);

    return ClParserStringPtr(pstr);
  }

  static ClParserStringPtr createString(ClParserStringPtr &str) {
    ClParserString *pstr = new ClParserString(str);

    return ClParserStringPtr(pstr);
  }

  ClParserString(const std::string &text = "");
  ClParserString(const char *text, uint len);
  ClParserString(const ClParserString &str);
  ClParserString(const ClParserStringPtr &str);

 ~ClParserString() { }

  const ClParserString &operator=(const ClParserString &str);

  ClParserString *dup() const;

 public:
  void copy(const ClParserObj &obj);

  //--------

  bool toBool() const;

  uint getLen() const { return text_.size(); }

  const std::string &getText() const { return text_; }

  char getChar(uint ind) const { return text_[ind]; }

  void setText(const std::string &text);
  void setText(const char *text, uint len);

  void getChars(char **text, uint *len) const;

  const char *getCharP() const { return text_.c_str(); }

  bool getSubChars(int i1, int i2, char **text, uint *len) const;
  bool getSubChars(int i1, int i2, std::string &text) const;

  bool replaceSubString(int i1, int i2, ClParserStringPtr str);

  bool removeString(ClParserStringPtr str);

  uint countSubStrings(ClParserStringPtr str) const;

  int getStringPos(ClParserStringPtr str) const;
  int getStringRPos(ClParserStringPtr str) const;

  std::string subString(uint pos, int len=-1) const;

  void append(ClParserStringPtr str);

  ClParserStringPtr concat(ClParserStringPtr str) const;

  int find(const std::string &str) const;

  void roll(int num);
  void roll(int num, std::string &text) const;

  uchar getMaxChar() const;
  uchar getMinChar() const;

  //------

  int cmp(const ClParserObj &rhs) const;

  ClParserStringPtr format(ClParserValuePtr value) const;

  bool format(ClParserValuePtr value, std::string &result) const;

  bool toInteger(long *integer) const;
  bool toReal(double *real) const;

  void print() const;
  void print(std::ostream &os) const;
  void debugPrint() const;

  //------

  // Inline Ops

  const ClParserObj &increment();
  const ClParserObj &decrement();

  //------

  // Unary Ops

  ClParserValuePtr unaryPlus () const;
  ClParserValuePtr unaryMinus() const;
  ClParserValuePtr bitNot    () const;

  //------

  // Binary Ops

  ClParserValuePtr plus(const ClParserObj &obj) const;
  ClParserValuePtr minus(const ClParserObj &obj) const;
  ClParserValuePtr times(const ClParserObj &obj) const;
  ClParserValuePtr divide(const ClParserObj &obj) const;
  ClParserValuePtr modulus(const ClParserObj &obj) const;
  ClParserValuePtr power(const ClParserObj &obj) const;
  ClParserValuePtr approxEqual(const ClParserObj &obj) const;
  ClParserValuePtr bitAnd(const ClParserObj &obj) const;
  ClParserValuePtr bitOr(const ClParserObj &obj) const;
  ClParserValuePtr bitXor(const ClParserObj &obj) const;
  ClParserValuePtr bitLShift(const ClParserObj &obj) const;
  ClParserValuePtr bitRShift(const ClParserObj &obj) const;

  //------

  // Internal Fns

  ClParserValuePtr abs() const;
  ClParserValuePtr ceil() const;
  ClParserValuePtr floor() const;
  ClParserValuePtr sign() const;
  ClParserValuePtr sqr() const;
  ClParserValuePtr sqrt() const;
  ClParserValuePtr cos() const;
  ClParserValuePtr sin() const;
  ClParserValuePtr tan() const;
  ClParserValuePtr acos() const;
  ClParserValuePtr asin() const;
  ClParserValuePtr atan() const;
  ClParserValuePtr atan(double real) const;
  ClParserValuePtr exp() const;
  ClParserValuePtr log() const;
  ClParserValuePtr log10() const;
  ClParserValuePtr cosh() const;
  ClParserValuePtr sinh() const;
  ClParserValuePtr tanh() const;
  ClParserValuePtr toChar() const;
  ClParserValuePtr toInt() const;
  ClParserValuePtr toReal() const;
  ClParserValuePtr toString() const;
  ClParserValuePtr isNan() const;
  ClParserValuePtr toLower() const;
  ClParserValuePtr toUpper() const;
  ClParserValuePtr len() const;
  ClParserValuePtr min() const;
  ClParserValuePtr max() const;
  ClParserValuePtr sum() const;
  ClParserValuePtr index(const ClParserObj &obj) const;
  ClParserValuePtr rindex(const ClParserObj &obj) const;
  ClParserValuePtr sort(ClParserSortDirection direction) const;

 private:
  uint countSubStrings(const std::string &str) const;

  int getStringPos(const std::string &str) const;
  int getStringRPos(const std::string &str) const;

  bool indexToText(int *ind) const;
};

#endif
