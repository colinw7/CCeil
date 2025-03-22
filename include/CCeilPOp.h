#ifndef CCEILP_OP_H
#define CCEILP_OP_H

#include <map>
#include <CCeilPTypes.h>

/**** Private Defines ****/

/*
 *   Operator Token        | Precedence   | Associativity
 * ------------------------+--------------+--------------
 *                         |              |
 *  ( )  [ ]  { }  {{ }}   |  15          |  L -> R
 *  !   ~   ++  --  +   -  |  14 (unary)  |  R -> L
 *  **                     |  13          |  R -> L
 *  *   /   %   #          |  12          |  L -> R
 *  +   -                  |  11          |  L -> R
 *  <<  >>                 |  10          |  L -> R
 *  <   <=  >   >=         |   9          |  L -> R
 *  ==  !=  ~=             |   8          |  L -> R
 *  &                      |   7          |  L -> R
 *  ^                      |   6          |  L -> R
 *  |                      |   5          |  L -> R
 *  &&                     |   4          |  L -> R
 *  ||                     |   3          |  L -> R
 *  ?:                     |   2          |  R -> L
 *  =   +=  -=   *=  /= %= |   1          |  R -> L
 *  &=  ^=  |=  <<= >>=    |   1          |  R -> L
 *  ,                      |   0          |  R -> L
 *
 */

#define CL_PARSER_OP_INLINE      (1<<12)
#define CL_PARSER_OP_PUNCTUATION (1<<11)
#define CL_PARSER_OP_UNARY       (1<<10)
#define CL_PARSER_OP_BINARY      (1<< 9)
#define CL_PARSER_OP_ASSIGNMENT  (1<< 8)

enum class CLParserOperatorAssociate {
  L_TO_R = 1,
  R_TO_L = 2
};

/**** Private Structures ****/

#define ClParserOperatorMgrInst ClParserOperatorMgr::getInstance()

class ClParserOperatorMgr {
 private:
  typedef std::map<ClParserOperatorType,ClParserOperatorPtr> OperatorMap;

  OperatorMap operator_map_;

 public:
  static ClParserOperatorMgr *getInstance();

 ~ClParserOperatorMgr();

  ClParserOperatorPtr getOperator(ClParserOperatorType type) const;

 private:
  ClParserOperatorMgr();

  bool initOperatorMap();
};

//---

struct ClParserOperatorData {
  ClParserOperatorType      type;
  const char*               token         { nullptr };
  const char*               name          { nullptr };
  uint                      precedence    { 0 };
  CLParserOperatorAssociate associativity { CLParserOperatorAssociate::L_TO_R };
  uint                      types;
};

//---

struct ClParserOpData {
  ClParserOperatorType      type;
  std::string               token;
  std::string               name;
  uint                      precedence;
  CLParserOperatorAssociate associativity;
  uint                      types;
};

//---

class ClParserOperator {
 protected:
  friend class ClParserOperatorMgr;

  explicit ClParserOperator(const ClParserOperatorData &data);
  explicit ClParserOperator(const ClParserOpData &data);

  ClParserOperator *dup() const;

 public:
  ClParserOperatorType getType() const { return data_.type; }

  bool isType(ClParserOperatorType type) { return (data_.type == type); }

  const std::string &getToken() const { return data_.token; }
  const std::string &getName () const { return data_.name ; }

  bool isInline     () const { return (data_.type & CL_PARSER_OP_INLINE); }
  bool isUnary      () const { return (data_.type & CL_PARSER_OP_UNARY ); }
  bool isBinary     () const { return (data_.type & CL_PARSER_OP_BINARY); }
  bool isAssignment () const { return (data_.type & CL_PARSER_OP_ASSIGNMENT); }
  bool isPunctuation() const { return (data_.type & CL_PARSER_OP_PUNCTUATION); }

  bool checkUnstack(ClParserOperatorPtr last_operator) const;

  void print() const;
  void print(std::ostream &os) const;
  void debugPrint() const;

 private:
  ClParserOpData data_;
};

#endif
