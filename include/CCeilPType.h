#ifndef CCEILP_TYPE_H
#define CCEILP_TYPE_H

#include <map>
#include <vector>
#include <iostream>

#include <CCeilPTypes.h>
#include <CCeilPValue.h>

class ClParserTypeMgr {
 private:
  typedef std::map<std::string, ClParserTypePtr> TypeMap;
  typedef std::vector<TypeMap *>                 TypeMapArray;

 public:
  ClParserTypeMgr();
 ~ClParserTypeMgr();

 private:
  void init();

 public:
  ClParserTypePtr getIntegerType() const { return integer_type_; }
  ClParserTypePtr getRealType   () const { return real_type_   ; }
  ClParserTypePtr getStringType () const { return string_type_ ; }

  ClParserTypePtr createType();
  ClParserTypePtr createType(const std::string &name);
  ClParserTypePtr createType(const std::string &name, const std::string &arg_string);

  bool isType(const std::string &name) const;

  ClParserTypePtr getType(const std::string &name) const;

  void deleteType(const std::string &name);
  void deleteAllTypes();

  void newTypeList();
  void oldTypeList();

  void printAllTypes() const;

 private:
  ClParserTypePtr addType(ClParserType *type);
  void            removeType(ClParserTypePtr type);

 private:
  ClParserTypePtr integer_type_;
  ClParserTypePtr real_type_;
  ClParserTypePtr string_type_;

  TypeMap      *type_map_ { nullptr };
  TypeMapArray  type_map_list_;
};

//---

class ClParserType {
 private:
  friend class ClParserTypeMgr;
  friend class CRefPtr<ClParserType>;

  ClParserType(const std::string &name="");
  ClParserType(const std::string &name, const ClParserSubTypeArray &sub_types);
 ~ClParserType();

  ClParserType *dup() const;

 public:
  const std::string &getName() const { return name_; }

  uint getNumSubTypes() const { return sub_types_.size(); }

  ClParserSubType *getSubType(uint i) const { return sub_types_[i]; }

  int getSubTypeNumber(const std::string &name);

  bool getIsFixed() const { return is_fixed_; }

  void setIsFixed(bool is_fixed) { is_fixed_ = is_fixed; }

  void addSubType(const std::string &name, uint *dims = NULL, uint num_dims = 0);
  void addSubType(const std::string &name, const UIntVectorT &dims);

  bool isGlobal() const { return (! name_.empty() && name_[0] == '_'); }

  void print() const;
  void print(std::ostream &os) const;
  void debugPrint() const;

  void printName() const;
  void printName(std::ostream &os) const;

  bool isIntegerType() const;
  bool isRealType() const;
  bool isStringType() const;
  bool isBuiltinType() const;

 private:
  std::string          name_;
  ClParserSubTypeArray sub_types_;
  bool                 is_fixed_ { false };
};

//---

class ClParserSubType {
 public:
  ClParserSubType(const std::string &name, ClParserTypePtr type, uint *dims, uint num_dims);
  ClParserSubType(const std::string &name, ClParserTypePtr type, const UIntVectorT &dims);
 ~ClParserSubType();

  const std::string &getName() const { return name_; }

  ClParserTypePtr getType() const { return type_; }

  ClParserValuePtr getValue() const { return value_; }

  void print() const;
  void print(std::ostream &os) const;

 private:
  void createTypeValue();

 private:
  std::string       name_;
  ClParserTypePtr   type_;
  UIntVectorT       dims_;
  ClParserValuePtr  value_;
};

#endif
