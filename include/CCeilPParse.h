#ifndef CCEIL_PARSE_H
#define CCEIL_PARSE_H

#include <CStrParse.h>
#include <CCeilPValue.h>

class ClParserParser {
 private:
  ClParserParser *parent_;
  CStrParse       parse_;
  bool            assign_;
  int             error_code_;

 public:
  explicit ClParserParser(const std::string &str) :
   parent_(NULL), parse_(str), assign_(false), error_code_(0) {
  }

  explicit ClParserParser(ClParserParser *parent) :
   parent_(parent), parse_(parent->parse_.getAt()), assign_(false), error_code_(0) {
  }

  int getPos() const { return parse_.getPos(); }

  bool isAssign() const { return assign_; }

  int getErrorCode() const { return error_code_; }

  bool parse();
  bool skipExpression(bool *assign);

  bool isValidAssign();

  bool readUnaryOperator(ClParserOperatorType *op);
  bool skipUnaryOperator();

  bool readBinaryOperator(ClParserOperatorType *op);
  bool skipBinaryOperator(bool *assign);

  bool readNumericValue(ClParserValuePtr &value);
  bool skipNumericValue();

  bool readNumericHexValue(ClParserValuePtr &value);
  bool skipNumericHexValue();

  bool readNumericOctValue(ClParserValuePtr &value);

  bool readNumericDecValue(ClParserValuePtr &value);
  bool skipNumericDecValue();

  bool readStringValue(ClParserValuePtr &value);
  bool skipStringValue();

  bool readIdentifier(ClParserIdentifierPtr &identifier);
  bool skipIdentifier();

  bool readArgList();
  bool skipArgList();

  bool readArray();
  bool skipArray();

  bool readList();
  bool skipList();

  bool readDictionary();
  bool skipDictionary();

  void skipSpace();
};

#endif
