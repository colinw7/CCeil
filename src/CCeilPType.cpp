#include <CCeilPI.h>

ClParserTypeMgr::
ClParserTypeMgr()
{
  integer_type_ = new ClParserType("int");
  real_type_    = new ClParserType("real");
  string_type_  = new ClParserType("str");

  type_map_ = new TypeMap;
}

void
ClParserTypeMgr::
init()
{
}

ClParserTypeMgr::
~ClParserTypeMgr()
{
  delete type_map_;

  uint num = uint(type_map_list_.size());

  for (uint i = 0; i < num; ++i)
    delete type_map_list_[i];
}

ClParserTypePtr
ClParserTypeMgr::
createType()
{
  ClParserType *type = new ClParserType;

  return addType(type);
}

ClParserTypePtr
ClParserTypeMgr::
createType(const std::string &name)
{
  ClParserType *type = new ClParserType(name);

  return addType(type);
}

ClParserTypePtr
ClParserTypeMgr::
createType(const std::string &name, const std::string &arg_string)
{
  /* Check if Valid Structure Name */

  if (! CStrUtil::isIdentifier(name) ||
      ! ClParserInst->isValidNewName(ClParserNameType::STRUCT, name))
    ClErrThrow(ClErr::INVALID_STRUCT_NAME);

  ClParserType *type = new ClParserType(name);

  type->setIsFixed(true);

  /* Parse Arg String */

  std::string type_string;
  std::string variable;

  uint len = uint(arg_string.size());

  uint i = 0;

  while (i < len) {
    CStrUtil::skipSpace(arg_string, &i);

    //uint j = 0;

    if (! CStrUtil::readIdentifier(arg_string, &i, type_string))
      ClErrThrow(ClErr::INVALID_STRUCTURE_TYPE_NAME);

    ClParserTypePtr type1 = ClParserInst->getType(type_string);

    if (! type1.isValid())
      ClErrThrow(ClErr::UNDEFINED_STRUCT_TYPE);

    CStrUtil::skipSpace(arg_string, &i);

    if (! CStrUtil::readIdentifier(arg_string, &i, variable))
      ClErrThrow(ClErr::INVALID_STRUCT_VAR_NAME);

    UIntVectorT dims;

    if (arg_string[i] == '[') {
      do {
        ++i;

        CStrUtil::skipSpace(arg_string, &i);

        //j = 0;

        std::string dim_string = "";

        while (arg_string[i] != '\0' && isdigit(arg_string[i]))
          dim_string += arg_string[i++];

        if (dim_string == "")
          ClErrThrow(ClErr::STRUCT_DIM_SYNTAX_ERROR);

        int dim;

        if (! CStrUtil::toInteger(dim_string, &dim))
          ClErrThrow(ClErr::ZERO_STRUCT_DIM);

        dims.push_back(uint(dim));

        CStrUtil::skipSpace(arg_string, &i);
      }
      while (i < len && arg_string[i] == ',');

      if (arg_string[i] != ']')
        ClErrThrow(ClErr::STRUCT_DIM_SYNTAX_ERROR);

      ++i;

      uint num_dims = uint(dims.size());

      if (num_dims == 0)
        ClErrThrow(ClErr::MISSING_STRUCT_DIMS);
    }

    type->addSubType(variable, dims);

    CStrUtil::skipSpace(arg_string, &i);

    if (i < len && arg_string[i] != ',')
      ClErrThrow(ClErr::STRUCT_SYNTAX_ERROR);

    if (arg_string[i] == ',')
      ++i;
  }

  //-----

  // Delete any existing type
  // TODO: existing variables of this type

  ClParserTypePtr old_type = ClParserInst->getType(name);

  removeType(old_type);

  //-----

  return addType(type);
}

ClParserTypePtr
ClParserTypeMgr::
addType(ClParserType *type)
{
  const std::string &name = type->getName();

  TypeMap *type_map = type_map_;

  if (type->isGlobal() && ! type_map_list_.empty())
    type_map = type_map_list_[0];

  (*type_map)[name] = type;

  return (*type_map)[name];
}

void
ClParserTypeMgr::
deleteType(const std::string &name)
{
  removeType(getType(name));
}

void
ClParserTypeMgr::
removeType(ClParserTypePtr type)
{
  if (type->isBuiltinType())
    return;

  const std::string &name = type->getName();

  TypeMap::iterator p = type_map_->find(name);

  if (p != type_map_->end()) {
    type_map_->erase(p);
    return;
  }

  uint num = uint(type_map_list_.size());

  for (int i = int(num); i >= 1; i--) {
    TypeMap *type_map = type_map_list_[uint(i - 1)];

    TypeMap::iterator pt = type_map->find(name);

    if (pt != type_map->end()) {
      type_map->erase(pt);
      return;
    }
  }
}

bool
ClParserTypeMgr::
isType(const std::string &name) const
{
  return (getType(name).isValid());
}

ClParserTypePtr
ClParserTypeMgr::
getType(const std::string &name) const
{
  if      (name == "int")
    return getIntegerType();
  else if (name == "real")
    return getRealType();
  else if (name == "str")
    return getStringType();

  TypeMap::const_iterator p = type_map_->find(name);

  if (p != type_map_->end())
    return (*p).second;

  uint num = uint(type_map_list_.size());

  for (int i = int(num); i >= 1; i--) {
    TypeMap *type_map = type_map_list_[uint(i - 1)];

    TypeMap::const_iterator pt = type_map->find(name);

    if (pt != type_map->end())
      return (*pt).second;
  }

  return ClParserTypePtr();
}

void
ClParserTypeMgr::
deleteAllTypes()
{
  type_map_->clear();
}

void
ClParserTypeMgr::
newTypeList()
{
  CL_PARSER_TRACE("ClParserTypeMgr::newTypeList")

  type_map_list_.push_back(type_map_);

  type_map_ = new TypeMap;
}

void
ClParserTypeMgr::
oldTypeList()
{
  assert(! type_map_list_.empty());

  CL_PARSER_TRACE("ClParserTypeMgr::oldTypeList")

  delete type_map_;

  type_map_ = type_map_list_.back();

  type_map_list_.pop_back();
}

void
ClParserTypeMgr::
printAllTypes() const
{
  TypeMap::const_iterator p1 = type_map_->begin();
  TypeMap::const_iterator p2 = type_map_->end  ();

  for ( ; p1 != p2; ++p1)
    (*p1).second->print();

  uint num = uint(type_map_list_.size());

  for (int i = int(num); i >= 1; i--) {
    TypeMap *type_map = type_map_list_[uint(i - 1)];

    ClParserInst->output("\n");

    TypeMap::const_iterator pt1 = type_map->begin();
    TypeMap::const_iterator pt2 = type_map->end  ();

    for ( ; pt1 != pt2; ++pt1)
      (*pt1).second->print();
  }
}

//-----------------------

ClParserType::
ClParserType(const std::string &name) :
 name_(name), is_fixed_(false)
{
}

ClParserType::
ClParserType(const std::string &name, const ClParserSubTypeArray &sub_types) :
 name_(name), is_fixed_(false)
{
  uint num_sub_types = uint(sub_types.size());

  for (uint i = 0; i < num_sub_types; ++i)
    sub_types_.push_back(sub_types[i]);
}

ClParserType::
~ClParserType()
{
}

ClParserType *
ClParserType::
dup() const
{
  return new ClParserType(name_, sub_types_);
}

void
ClParserType::
addSubType(const std::string &name, uint *dims, uint num_dims)
{
  ClParserTypePtr type = ClParserInst->getType(name_);

  ClParserSubType *sub_type = new ClParserSubType(name, type, dims, num_dims);

  sub_types_.push_back(sub_type);
}

void
ClParserType::
addSubType(const std::string &name, const UIntVectorT &dims)
{
  ClParserTypePtr type = ClParserInst->getType(name_);

  ClParserSubType *sub_type = new ClParserSubType(name, type, dims);

  sub_types_.push_back(sub_type);
}

int
ClParserType::
getSubTypeNumber(const std::string &name)
{
  if (isBuiltinType())
    return -1;

  uint num_sub_types = uint(sub_types_.size());

  for (uint i = 0; i < num_sub_types; ++i)
    if (name == sub_types_[i]->getName())
      return int(i);

  return -1;
}

bool
ClParserType::
isIntegerType() const
{
  return (this == ClParserInst->getIntegerType().getPtr());
}

bool
ClParserType::
isRealType() const
{
  return (this == ClParserInst->getRealType().getPtr());
}

bool
ClParserType::
isStringType() const
{
  return (this == ClParserInst->getStringType().getPtr());
}

bool
ClParserType::
isBuiltinType() const
{
  return (isIntegerType() || isRealType() || isStringType());
}

void
ClParserType::
print() const
{
  ClParserInst->output("struct %s { ", name_.c_str());

  uint num_sub_types = uint(sub_types_.size());

  for (uint i = 0; i < num_sub_types; i++) {
    ClParserInst->output("  ");

    sub_types_[i]->print();

    if (i != num_sub_types - 1)
      ClParserInst->output(", ");
  }

  ClParserInst->output(" }");

  ClParserInst->output("\n");
}

void
ClParserType::
print(std::ostream &os) const
{
  os << "struct " << name_ << " { ";

  uint num_sub_types = uint(sub_types_.size());

  for (uint i = 0; i < num_sub_types; i++) {
    os << "  ";

    sub_types_[i]->print(os);

    if (i != num_sub_types - 1)
      os << ", ";
  }

  os << " }";

  os << "\n";
}

void
ClParserType::
debugPrint() const
{
  if      (isIntegerType())
    fprintf(stderr, "int");
  else if (isRealType())
    fprintf(stderr, "real");
  else if (isStringType())
    fprintf(stderr, "str");
  else
    fprintf(stderr, "%s", getName().c_str());
}

void
ClParserType::
printName() const
{
  if      (isIntegerType())
    ClParserInst->output("int");
  else if (isRealType())
    ClParserInst->output("real");
  else if (isStringType())
    ClParserInst->output("str");
  else
    ClParserInst->output("%s", getName().c_str());
}

void
ClParserType::
printName(std::ostream &os) const
{
  if      (isIntegerType())
    os << "int";
  else if (isRealType())
    os << "real";
  else if (isStringType())
    os << "str";
  else
    os << getName();
}

//-----------------

ClParserSubType::
ClParserSubType(const std::string &name, ClParserTypePtr type, uint *dims, uint num_dims) :
 name_(name), type_(type)
{
  for (uint i = 0; i < num_dims; i++)
    dims_.push_back(dims[i]);

  createTypeValue();
}

ClParserSubType::
ClParserSubType(const std::string &name, ClParserTypePtr type, const UIntVectorT &dims) :
 name_(name), type_(type), dims_(dims)
{
  createTypeValue();
}

ClParserSubType::
~ClParserSubType()
{
}

void
ClParserSubType::
createTypeValue()
{
  uint num_dims = uint(dims_.size());

  if      (type_->isIntegerType()) {
    if (num_dims == 0)
      value_ = ClParserValueMgrInst->createValue(0L);
    else
      value_ = ClParserValueMgrInst->createValue(dims_, 0L);
  }
  else if (type_->isRealType()) {
    if (num_dims == 0)
      value_ = ClParserValueMgrInst->createValue(0.0);
    else
      value_ = ClParserValueMgrInst->createValue(dims_, 0.0);
  }
  else if (type_->isStringType()) {
    if (num_dims == 0)
      value_ = ClParserValueMgrInst->createValue("");
    else
      value_ = ClParserValueMgrInst->createValue(dims_, "");
  }
  else {
    if (num_dims == 0)
      value_ = ClParserValueMgrInst->createValue(type_);
    else
      value_ = ClParserValueMgrInst->createValue(type_, dims_);
  }
}

void
ClParserSubType::
print() const
{
  type_->printName();

  ClParserInst->output("%s", name_.c_str());

  uint num_dims = uint(dims_.size());

  if (num_dims > 0) {
    ClParserInst->output("[");

    for (uint j = 0; j < num_dims; j++) {
      ClParserInst->output("%d", dims_[j]);

      if (j != num_dims - 1)
        ClParserInst->output(", ");
    }

    ClParserInst->output("]");
  }
}

void
ClParserSubType::
print(std::ostream &os) const
{
  type_->printName(os);

  os << name_;

  uint num_dims = uint(dims_.size());

  if (num_dims > 0) {
    os << "[";

    for (uint j = 0; j < num_dims; j++) {
      os << dims_[j];

      if (j != num_dims - 1)
        os << ", ";
    }

    os << "]";
  }
}
