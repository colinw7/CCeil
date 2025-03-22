#ifndef CCEILP_IDENT_H
#define CCEILP_IDENT_H

#include <CCeilPTypes.h>

/**** Private Defines ****/

enum class ClParserIdentifierType {
  NONE        = 0,
  VARIABLE    = 1,
  FUNCTION    = 2,
  ARGUMENT    = 3,
  STRUCT_PART = 4
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

 protected:
  friend class ClParserIdentifierMgr;

  ClParserIdentifier(const std::string &name) :
   type_(ClParserIdentifierType::NONE), name_(name) {
  }

  ClParserIdentifier(const ClParserIdentifier &identifier) :
   type_(identifier.type_), name_(identifier.name_), scopes_(identifier.scopes_) {
  }

  ClParserIdentifier &operator=(const ClParserIdentifier &identifier) {
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
    return (type_ == ClParserIdentifierType::VARIABLE);
  }

  void setVariable()  {
    type_ = ClParserIdentifierType::VARIABLE;
  }

  bool isFunction() const {
    return (type_ == ClParserIdentifierType::FUNCTION);
  }

  void setFunction()  {
    type_ = ClParserIdentifierType::FUNCTION;
  }

  bool isArgument() const {
    return (type_ == ClParserIdentifierType::ARGUMENT);
  }

  void setArgument()  {
    type_ = ClParserIdentifierType::ARGUMENT;
  }

  bool isStructPart() const {
    return (type_ == ClParserIdentifierType::STRUCT_PART);
  }

  void setStructPart()  {
    type_ = ClParserIdentifierType::STRUCT_PART;
  }

  void addScope(const std::string &scope) {
    scopes_.push_back(scope);
  }

  void print() const;
  void print(std::ostream &os) const;

  void debugPrint() const;

 private:
  ClParserIdentifierType type_;
  std::string            name_;
  ScopeList              scopes_;
};

#endif
