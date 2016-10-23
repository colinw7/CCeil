#ifndef CCEILP_IDENT_H
#define CCEILP_IDENT_H

#include <CCeilPTypes.h>

/**** Private Defines ****/

enum ClParserIdentifierType {
  CL_PARSER_IDENTIFIER_TYPE_NONE        = 0,
  CL_PARSER_IDENTIFIER_TYPE_VARIABLE    = 1,
  CL_PARSER_IDENTIFIER_TYPE_FUNCTION    = 2,
  CL_PARSER_IDENTIFIER_TYPE_ARGUMENT    = 3,
  CL_PARSER_IDENTIFIER_TYPE_STRUCT_PART = 4
};

/**** Private Structures ****/

#define ClParserIdentifierMgrInst ClParserIdentifierMgr::getInstance()

class ClParserIdentifierMgr {
 public:
  static ClParserIdentifierMgr *getInstance();

  ClParserIdentifierPtr createIdentifier(const std::string &name);
};

class ClParserIdentifier {
 public:
  typedef StringVectorT ScopeList;

 private:
  ClParserIdentifierType type_;
  std::string            name_;
  ScopeList              scopes_;

 protected:
  friend class ClParserIdentifierMgr;
  friend class CRefPtr<ClParserIdentifier>;

  ClParserIdentifier(const std::string &name) :
   type_(CL_PARSER_IDENTIFIER_TYPE_NONE), name_(name) {
  }

  ClParserIdentifier(const ClParserIdentifier &identifier) :
   type_(identifier.type_), name_(identifier.name_), scopes_(identifier.scopes_) {
  }

  const ClParserIdentifier &operator=(const ClParserIdentifier &identifier) {
    type_   = identifier.type_;
    name_   = identifier.name_;
    scopes_ = identifier.scopes_;

    return *this;
  }

  ClParserIdentifier *dup() const {
    return new ClParserIdentifier(*this);
  }

 public:
  const std::string &getName() const { return name_; }

  const ScopeList &getScopeList() const { return scopes_; }

  ClParserScopePtr getScope() const;

  bool isVariable() const {
    return (type_ == CL_PARSER_IDENTIFIER_TYPE_VARIABLE);
  }

  void setVariable()  {
    type_ = CL_PARSER_IDENTIFIER_TYPE_VARIABLE;
  }

  bool isFunction() const {
    return (type_ == CL_PARSER_IDENTIFIER_TYPE_FUNCTION);
  }

  void setFunction()  {
    type_ = CL_PARSER_IDENTIFIER_TYPE_FUNCTION;
  }

  bool isArgument() const {
    return (type_ == CL_PARSER_IDENTIFIER_TYPE_ARGUMENT);
  }

  void setArgument()  {
    type_ = CL_PARSER_IDENTIFIER_TYPE_ARGUMENT;
  }

  bool isStructPart() const {
    return (type_ == CL_PARSER_IDENTIFIER_TYPE_STRUCT_PART);
  }

  void setStructPart()  {
    type_ = CL_PARSER_IDENTIFIER_TYPE_STRUCT_PART;
  }

  void addScope(const std::string &scope) {
    scopes_.push_back(scope);
  }

  void print() const;
  void print(std::ostream &os) const;

  void debugPrint() const;
};

#endif
