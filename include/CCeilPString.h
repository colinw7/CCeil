#ifndef CCEILP_STRING_H
#define CCEILP_STRING_H

#include <CCeilPObj.h>

class ClParserString : public ClParserObj {
 protected:
  friend class ClParserValue;

 protected:
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

  explicit ClParserString(const std::string &text = "");
  ClParserString(const char *text, uint len);
  explicit ClParserString(const ClParserStringPtr &str);

  ClParserString(const ClParserString &str);

  ClParserString &operator=(const ClParserString &str);

  ClParserString *dup() const override;

 public:
  static const ClParserString &castObj(const ClParserObj &obj) {
    assert(obj.getBaseType() == CL_PARSER_VALUE_TYPE_STRING);

    return reinterpret_cast<const ClParserString &>(obj);
  }

 ~ClParserString() { }

  void copy(const ClParserObj &obj) override;

  //--------

  bool toBool() const override;

  uint getLen() const { return uint(text_.size()); }

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

  int cmp(const ClParserObj &rhs) const override;

  ClParserStringPtr format(ClParserValuePtr value) const;

  bool format(ClParserValuePtr value, std::string &result) const;

  bool toInteger(long *integer) const;
  bool toReal(double *real) const;

  //------

  std::string asString() const override;

  void print() const override;
  void print(std::ostream &os) const override;

  void debugPrint() const override;

  //------

  // Inline Ops

  const ClParserObj &increment() override;
  const ClParserObj &decrement() override;

  //------

  // Unary Ops

  ClParserValuePtr unaryPlus () const override;
  ClParserValuePtr unaryMinus() const override;
  ClParserValuePtr bitNot    () const override;

  //------

  // Binary Ops

  ClParserValuePtr plus(const ClParserObj &obj) const override;
  ClParserValuePtr minus(const ClParserObj &obj) const override;
  ClParserValuePtr times(const ClParserObj &obj) const override;
  ClParserValuePtr divide(const ClParserObj &obj) const override;
  ClParserValuePtr modulus(const ClParserObj &obj) const override;
  ClParserValuePtr power(const ClParserObj &obj) const override;
  ClParserValuePtr approxEqual(const ClParserObj &obj) const override;
  ClParserValuePtr bitAnd(const ClParserObj &obj) const override;
  ClParserValuePtr bitOr(const ClParserObj &obj) const override;
  ClParserValuePtr bitXor(const ClParserObj &obj) const override;
  ClParserValuePtr bitLShift(const ClParserObj &obj) const override;
  ClParserValuePtr bitRShift(const ClParserObj &obj) const override;

  //------

  // Internal Fns

  ClParserValuePtr abs     () const override;
  ClParserValuePtr ceil    () const override;
  ClParserValuePtr floor   () const override;
  ClParserValuePtr sign    () const override;
  ClParserValuePtr sqr     () const override;
  ClParserValuePtr sqrt    () const override;
  ClParserValuePtr cos     () const override;
  ClParserValuePtr sin     () const override;
  ClParserValuePtr tan     () const override;
  ClParserValuePtr acos    () const override;
  ClParserValuePtr asin    () const override;
  ClParserValuePtr atan    () const override;
  ClParserValuePtr atan    (double real) const override;
  ClParserValuePtr exp     () const override;
  ClParserValuePtr log     () const override;
  ClParserValuePtr log10   () const override;
  ClParserValuePtr cosh    () const override;
  ClParserValuePtr sinh    () const override;
  ClParserValuePtr tanh    () const override;
  ClParserValuePtr norm    () const override;
  ClParserValuePtr invnorm () const override;
  ClParserValuePtr toChar  () const override;
  ClParserValuePtr toInt   () const override;
  ClParserValuePtr toReal  () const override;
  ClParserValuePtr toString() const override;
  ClParserValuePtr isNan   () const override;
  ClParserValuePtr toLower () const override;
  ClParserValuePtr toUpper () const override;
  ClParserValuePtr len     () const override;
  ClParserValuePtr min     () const override;
  ClParserValuePtr max     () const override;
  ClParserValuePtr sum     () const override;

  ClParserValuePtr index (const ClParserObj &obj) const override;
  ClParserValuePtr rindex(const ClParserObj &obj) const override;

  ClParserValuePtr sort(ClParserSortDirection direction) const override;

  ClParserValuePtr doAssert() const override;

 private:
  uint countSubStrings(const std::string &str) const;

  int getStringPos(const std::string &str) const;
  int getStringRPos(const std::string &str) const;

  bool indexToText(int *ind) const;

 private:
  std::string text_;
};

#endif
