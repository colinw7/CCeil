#include <CCeilPI.h>

int
ClParserValueCmp::
operator()(ClParserValuePtr value1, ClParserValuePtr value2)
{
  int cmp = value1->cmp(value2);

  if (direction_ == ClParserSortDirection::DESCENDING)
    cmp = -cmp;

  return cmp;
}

//-------------

ClParserValueMgr *
ClParserValueMgr::
getInstance()
{
  static ClParserValueMgr *instance;

  if (! instance)
    instance = new ClParserValueMgr;

  return instance;
}

ClParserValuePtr
ClParserValueMgr::
createValue(ClParserValuePtr value)
{
  ClParserValue *value1 = value->dup();

  return ClParserValuePtr(value1);
}

ClParserValuePtr
ClParserValueMgr::
createValue(double real)
{
  ClParserValue *value = new ClParserValue(real);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(long integer)
{
  ClParserValue *value = new ClParserValue(integer);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(const std::string &str)
{
  ClParserValue *value = new ClParserValue(str);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(const char *str, uint len)
{
  ClParserValue *value = new ClParserValue(str, int(len));

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(ClParserArrayPtr array)
{
  ClParserValue *value = new ClParserValue(array);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(ClParserListPtr list)
{
  ClParserValue *value = new ClParserValue(list);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(ClParserDictPtr dict)
{
  ClParserValue *value = new ClParserValue(dict);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(ClParserStructPtr structure)
{
  ClParserValue *value = new ClParserValue(structure);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(const ClParserObj &obj)
{
  ClParserValue *value = new ClParserValue(obj);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(ClParserValueType type)
{
  ClParserValue *value = new ClParserValue(type);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(ClParserValueType type, ClParserValuePtr *values, int num_values)
{
  ClParserValue *value = new ClParserValue(type, values, num_values);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(ClParserValueType type, const ClParserValueArray &values)
{
  ClParserValue *value = new ClParserValue(type, values);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(ClParserValuePtr *values, int num_values)
{
  ClParserValue *value = new ClParserValue(values, num_values);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(const ClParserValueArray &values)
{
  ClParserValue *value = new ClParserValue(values);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(ClParserTypePtr type)
{
  ClParserValue *value = new ClParserValue(type);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(ClParserTypePtr type, ClParserValuePtr *values, int num_values)
{
  ClParserValue *value = new ClParserValue(type, values, num_values);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(ClParserTypePtr type, const ClParserValueArray &values)
{
  ClParserValue *value = new ClParserValue(type, values);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(uint *dims, uint num_dims, double *reals)
{
  ClParserValue *value = new ClParserValue(dims, num_dims, reals);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(uint *dims, uint num_dims, float *reals)
{
  ClParserValue *value = new ClParserValue(dims, num_dims, reals);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(uint *dims, uint num_dims, long *integers)
{
  ClParserValue *value = new ClParserValue(dims, num_dims, integers);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(uint *dims, uint num_dims, int *integers)
{
  ClParserValue *value = new ClParserValue(dims, num_dims, integers);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(uint *dims, uint num_dims, const char **strs)
{
  ClParserValue *value = new ClParserValue(dims, num_dims, strs);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(uint *dims, uint num_dims, const StringVectorT &strs)
{
  ClParserValue *value = new ClParserValue(dims, num_dims, strs);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(uint *dims, uint num_dims, const LongVectorT &integers)
{
  ClParserValue *value = new ClParserValue(dims, num_dims, integers);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(uint *dims, uint num_dims, double real)
{
  ClParserValue *value = new ClParserValue(dims, num_dims, real);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(const UIntVectorT &dims, double real)
{
  ClParserValue *value = new ClParserValue(dims, real);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(uint *dims, uint num_dims, long integer)
{
  ClParserValue *value = new ClParserValue(dims, num_dims, integer);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(const UIntVectorT &dims, long integer)
{
  ClParserValue *value = new ClParserValue(dims, integer);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(uint *dims, uint num_dims, const char *str)
{
  ClParserValue *value = new ClParserValue(dims, num_dims, str);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(const UIntVectorT &dims, const char *str)
{
  ClParserValue *value = new ClParserValue(dims, str);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(ClParserTypePtr type, uint *dims, uint num_dims)
{
  ClParserValue *value = new ClParserValue(type, dims, num_dims);

  return ClParserValuePtr(value);
}

ClParserValuePtr
ClParserValueMgr::
createValue(ClParserTypePtr type, const UIntVectorT &dims)
{
  ClParserValue *value = new ClParserValue(type, dims);

  return ClParserValuePtr(value);
}

//-------------

ClParserValue::
ClParserValue() :
 type_(CL_PARSER_VALUE_TYPE_NONE)
{
  data_.real = nullptr;
}

ClParserValue::
ClParserValue(double real) :
 type_(CL_PARSER_VALUE_TYPE_REAL)
{
  data_.real = nullptr;

  setReal(ClParserReal::createReal(real));
}

ClParserValue::
ClParserValue(ClParserRealPtr real) :
 type_(CL_PARSER_VALUE_TYPE_REAL)
{
  data_.real = nullptr;

  setReal(ClParserReal::createReal(real));
}

ClParserValue::
ClParserValue(long integer) :
 type_(CL_PARSER_VALUE_TYPE_INTEGER)
{
  data_.real = nullptr;

  setInteger(ClParserInteger::createInteger(integer));
}

ClParserValue::
ClParserValue(ClParserIntegerPtr integer) :
 type_(CL_PARSER_VALUE_TYPE_INTEGER)
{
  data_.real = nullptr;

  setInteger(ClParserInteger::createInteger(integer));
}

ClParserValue::
ClParserValue(const std::string &str) :
 type_(CL_PARSER_VALUE_TYPE_STRING)
{
  data_.real = nullptr;

  setString(ClParserString::createString(str));
}

ClParserValue::
ClParserValue(const char *text, int len) :
 type_(CL_PARSER_VALUE_TYPE_STRING)
{
  data_.real = nullptr;

  setString(ClParserString::createString(text, uint(len)));
}

ClParserValue::
ClParserValue(ClParserStringPtr str) :
 type_(CL_PARSER_VALUE_TYPE_STRING)
{
  data_.real = nullptr;

  setString(ClParserString::createString(str));
}

ClParserValue::
ClParserValue(ClParserArrayPtr array) :
 type_(CL_PARSER_VALUE_TYPE_ARRAY)
{
  data_.real = nullptr;

  setArray(array);
}

ClParserValue::
ClParserValue(ClParserListPtr list) :
 type_(CL_PARSER_VALUE_TYPE_LIST)
{
  data_.real = nullptr;

  setList(list);
}

ClParserValue::
ClParserValue(ClParserDictPtr dict) :
 type_(CL_PARSER_VALUE_TYPE_DICTIONARY)
{
  data_.real = nullptr;

  setDictionary(dict);
}

ClParserValue::
ClParserValue(ClParserStructPtr structure) :
 type_(CL_PARSER_VALUE_TYPE_STRUCTURE)
{
  data_.real = nullptr;

  setStructure(structure);
}

ClParserValue::
ClParserValue(const ClParserValue &value) :
 type_(value.type_)
{
  data_.real = nullptr;

  switch (type_) {
    case CL_PARSER_VALUE_TYPE_REAL:
      setReal(*value.data_.real);
      break;
    case CL_PARSER_VALUE_TYPE_INTEGER:
      setInteger(*value.data_.integer);
      break;
    case CL_PARSER_VALUE_TYPE_STRING:
      setString(*value.data_.str);
      break;
    case CL_PARSER_VALUE_TYPE_ARRAY:
      setArray(*value.data_.array);
      break;
    case CL_PARSER_VALUE_TYPE_LIST:
      setList(*value.data_.list);
      break;
    case CL_PARSER_VALUE_TYPE_DICTIONARY:
      setDictionary(*value.data_.dict);
      break;
    case CL_PARSER_VALUE_TYPE_STRUCTURE:
      setStructure(*value.data_.structure);
      break;
    default:
      ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);
      break;
  }
}

ClParserValue::
ClParserValue(const ClParserObj &obj) :
 type_(obj.getBaseType())
{
  data_.real = nullptr;

  switch (type_) {
    case CL_PARSER_VALUE_TYPE_REAL: {
      const ClParserReal *real = static_cast<const ClParserReal *>(&obj);
      setReal(ClParserReal::createReal(*real));
      break;
    }
    case CL_PARSER_VALUE_TYPE_INTEGER: {
      const ClParserInteger *integer = static_cast<const ClParserInteger *>(&obj);
      setInteger(ClParserInteger::createInteger(*integer));
      break;
    }
    case CL_PARSER_VALUE_TYPE_STRING: {
      const ClParserString *str = static_cast<const ClParserString *>(&obj);
      setString(ClParserString::createString(*str));
      break;
    }
    case CL_PARSER_VALUE_TYPE_ARRAY: {
      const ClParserArray *array = static_cast<const ClParserArray *>(&obj);
      setArray(ClParserArray::createArray(*array));
      break;
    }
    case CL_PARSER_VALUE_TYPE_LIST: {
      const ClParserList *list = static_cast<const ClParserList *>(&obj);
      setList(ClParserList::createList(*list));
      break;
    }
    case CL_PARSER_VALUE_TYPE_DICTIONARY: {
      const ClParserDict *dict = static_cast<const ClParserDict *>(&obj);
      setDictionary(ClParserDict::createDict(*dict));
      break;
    }
    case CL_PARSER_VALUE_TYPE_STRUCTURE: {
      const ClParserStruct *str = static_cast<const ClParserStruct *>(&obj);
      setStructure(ClParserStruct::createStruct(*str));
      break;
    }
    default:
      ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);
      break;
  }
}

ClParserValue::
ClParserValue(ClParserValueType type) :
 type_(type)
{
  data_.real = nullptr;

  switch (type_) {
    case CL_PARSER_VALUE_TYPE_REAL:
      setReal(ClParserReal::createReal());
      break;
    case CL_PARSER_VALUE_TYPE_INTEGER:
      setInteger(ClParserInteger::createInteger());
      break;
    case CL_PARSER_VALUE_TYPE_STRING:
      setString(ClParserString::createString());
      break;
    case CL_PARSER_VALUE_TYPE_ARRAY:
      setArray(ClParserArray::createArray());
      break;
    case CL_PARSER_VALUE_TYPE_LIST:
      setList(ClParserList::createList());
      break;
    case CL_PARSER_VALUE_TYPE_DICTIONARY:
      setDictionary(ClParserDict::createDict());
      break;
    case CL_PARSER_VALUE_TYPE_STRUCTURE:
      setStructure(ClParserStruct::createStruct());
      break;
    default:
      ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);
      break;
  }
}

ClParserValue::
ClParserValue(ClParserValueType type, ClParserValuePtr *values, int num_values) :
 type_(type)
{
  data_.real = nullptr;

  switch (type_) {
    case CL_PARSER_VALUE_TYPE_ARRAY:
      setArray(ClParserArray::createArray(values, uint(num_values)));
      break;
    case CL_PARSER_VALUE_TYPE_LIST:
      setList(ClParserList::createList(values, uint(num_values)));
      break;
    case CL_PARSER_VALUE_TYPE_DICTIONARY:
      setDictionary(ClParserDict::createDict(values, uint(num_values)));
      break;
    default:
      ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);
      break;
  }
}

ClParserValue::
ClParserValue(ClParserValueType type, const ClParserValueArray &values) :
 type_(type)
{
  data_.real = nullptr;

  switch (type_) {
    case CL_PARSER_VALUE_TYPE_ARRAY:
      setArray(ClParserArray::createArray(values));
      break;
    case CL_PARSER_VALUE_TYPE_LIST:
      setList(ClParserList::createList(values));
      break;
    case CL_PARSER_VALUE_TYPE_DICTIONARY:
      setDictionary(ClParserDict::createDict(values));
      break;
    default:
      ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);
      break;
  }
}

ClParserValue::
ClParserValue(ClParserValuePtr *values, int num_values) :
 type_(CL_PARSER_VALUE_TYPE_ARRAY)
{
  data_.real = nullptr;

  setArray(ClParserArray::createArray(values, uint(num_values)));
}

ClParserValue::
ClParserValue(const ClParserValueArray &values) :
 type_(CL_PARSER_VALUE_TYPE_ARRAY)
{
  data_.real = nullptr;

  setArray(ClParserArray::createArray(values));
}

ClParserValue::
ClParserValue(ClParserTypePtr type) :
 type_(CL_PARSER_VALUE_TYPE_STRUCTURE)
{
  data_.real = nullptr;

  setStructure(ClParserStruct::createStruct(type));
}

ClParserValue::
ClParserValue(ClParserTypePtr type, ClParserValuePtr *values, int num_values) :
 type_(CL_PARSER_VALUE_TYPE_STRUCTURE)
{
  data_.real = nullptr;

  setStructure(ClParserStruct::createStruct(type, values, uint(num_values)));
}

ClParserValue::
ClParserValue(ClParserTypePtr type, const ClParserValueArray &values) :
 type_(CL_PARSER_VALUE_TYPE_STRUCTURE)
{
  data_.real = nullptr;

  setStructure(ClParserStruct::createStruct(type, values));
}

ClParserValue::
ClParserValue(uint *dims, uint num_dims, double *reals) :
 type_(CL_PARSER_VALUE_TYPE_ARRAY)
{
  data_.real = nullptr;

  setArray(ClParserArray::createArray(dims, num_dims, reals));
}

ClParserValue::
ClParserValue(uint *dims, uint num_dims, float *reals) :
 type_(CL_PARSER_VALUE_TYPE_ARRAY)
{
  data_.real = nullptr;

  setArray(ClParserArray::createArray(dims, num_dims, reals));
}

ClParserValue::
ClParserValue(uint *dims, uint num_dims, long *integers) :
 type_(CL_PARSER_VALUE_TYPE_ARRAY)
{
  data_.real = nullptr;

  setArray(ClParserArray::createArray(dims, num_dims, integers));
}

ClParserValue::
ClParserValue(uint *dims, uint num_dims, int *integers) :
 type_(CL_PARSER_VALUE_TYPE_ARRAY)
{
  data_.real = nullptr;

  setArray(ClParserArray::createArray(dims, num_dims, integers));
}

ClParserValue::
ClParserValue(uint *dims, uint num_dims, const char **strs) :
 type_(CL_PARSER_VALUE_TYPE_ARRAY)
{
  data_.real = nullptr;

  setArray(ClParserArray::createArray(dims, num_dims, strs));
}

ClParserValue::
ClParserValue(uint *dims, uint num_dims, const StringVectorT &strs) :
 type_(CL_PARSER_VALUE_TYPE_ARRAY)
{
  data_.real = nullptr;

  setArray(ClParserArray::createArray(dims, num_dims, strs));
}

ClParserValue::
ClParserValue(uint *dims, uint num_dims, const LongVectorT &integers) :
 type_(CL_PARSER_VALUE_TYPE_ARRAY)
{
  data_.real = nullptr;

  setArray(ClParserArray::createArray(dims, num_dims, integers));
}

ClParserValue::
ClParserValue(uint *dims, uint num_dims, double real) :
 type_(CL_PARSER_VALUE_TYPE_ARRAY)
{
  data_.real = nullptr;

  setArray(ClParserArray::createArray(dims, num_dims, real));
}

ClParserValue::
ClParserValue(const UIntVectorT &dims, double real) :
 type_(CL_PARSER_VALUE_TYPE_ARRAY)
{
  data_.real = nullptr;

  setArray(ClParserArray::createArray(dims, real));
}

ClParserValue::
ClParserValue(uint *dims, uint num_dims, long integer) :
 type_(CL_PARSER_VALUE_TYPE_ARRAY)
{
  data_.real = nullptr;

  setArray(ClParserArray::createArray(dims, num_dims, integer));
}

ClParserValue::
ClParserValue(const UIntVectorT &dims, long integer) :
 type_(CL_PARSER_VALUE_TYPE_ARRAY)
{
  data_.real = nullptr;

  setArray(ClParserArray::createArray(dims, integer));
}

ClParserValue::
ClParserValue(uint *dims, uint num_dims, const char *str) :
 type_(CL_PARSER_VALUE_TYPE_ARRAY)
{
  data_.real = nullptr;

  setArray(ClParserArray::createArray(dims, num_dims, str));
}

ClParserValue::
ClParserValue(const UIntVectorT &dims, const char *str) :
 type_(CL_PARSER_VALUE_TYPE_ARRAY)
{
  data_.real = nullptr;

  setArray(ClParserArray::createArray(dims, str));
}

ClParserValue::
ClParserValue(ClParserTypePtr type, uint *dims, uint num_dims) :
 type_(CL_PARSER_VALUE_TYPE_ARRAY)
{
  data_.real = nullptr;

  if      (type->isIntegerType())
    setArray(ClParserArray::createArray(dims, num_dims, 0L));
  else if (type->isRealType())
    setArray(ClParserArray::createArray(dims, num_dims, 0.0));
  else if (type->isStringType())
    setArray(ClParserArray::createArray(dims, num_dims, ""));
  else
    setArray(ClParserArray::createArray(type, dims, num_dims));
}

ClParserValue::
ClParserValue(ClParserTypePtr type, const UIntVectorT &dims) :
 type_(CL_PARSER_VALUE_TYPE_ARRAY)
{
  data_.real = nullptr;

  if      (type->isIntegerType())
    setArray(ClParserArray::createArray(dims, 0L));
  else if (type->isRealType())
    setArray(ClParserArray::createArray(dims, 0.0));
  else if (type->isStringType())
    setArray(ClParserArray::createArray(dims, ""));
  else
    setArray(ClParserArray::createArray(type, dims));
}

ClParserValue::
ClParserValue(uint *dims, uint num_dims, ClParserValuePtr *values) :
 type_(CL_PARSER_VALUE_TYPE_ARRAY)
{
  data_.real = nullptr;

  setArray(ClParserArray::createArray(dims, num_dims, values));
}

ClParserValue::
~ClParserValue()
{
  resetValue();
}

ClParserValue &
ClParserValue::
operator=(const ClParserValue &value)
{
  resetValue();

  type_ = value.type_;

  switch (type_) {
    case CL_PARSER_VALUE_TYPE_REAL:
      setReal(*value.data_.real);
      break;
    case CL_PARSER_VALUE_TYPE_INTEGER:
      setInteger(*value.data_.integer);
      break;
    case CL_PARSER_VALUE_TYPE_STRING:
      setString(*value.data_.str);
      break;
    case CL_PARSER_VALUE_TYPE_ARRAY:
      setArray(*value.data_.array);
      break;
    case CL_PARSER_VALUE_TYPE_LIST:
      setList(*value.data_.list);
      break;
    case CL_PARSER_VALUE_TYPE_DICTIONARY:
      setDictionary(*value.data_.dict);
      break;
    case CL_PARSER_VALUE_TYPE_STRUCTURE:
      setStructure(*value.data_.structure);
      break;
    default:
      ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);
      break;
  }

  return *this;
}

void
ClParserValue::
resetValue()
{
  switch (type_) {
    case CL_PARSER_VALUE_TYPE_REAL:
      delete data_.real;
      break;
    case CL_PARSER_VALUE_TYPE_INTEGER:
      delete data_.integer;
      break;
    case CL_PARSER_VALUE_TYPE_STRING:
      delete data_.str;
      break;
    case CL_PARSER_VALUE_TYPE_ARRAY:
      delete data_.array;
      break;
    case CL_PARSER_VALUE_TYPE_LIST:
      delete data_.list;
      break;
    case CL_PARSER_VALUE_TYPE_DICTIONARY:
      delete data_.dict;
      break;
    case CL_PARSER_VALUE_TYPE_STRUCTURE:
      delete data_.structure;
      break;
    default:
      ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);
      break;
  }

  type_ = CL_PARSER_VALUE_TYPE_NONE;

  data_.real = nullptr;
}

ClParserValue *
ClParserValue::
dup() const
{
  switch (type_) {
    case CL_PARSER_VALUE_TYPE_REAL:
      return new ClParserValue(*data_.real);
    case CL_PARSER_VALUE_TYPE_INTEGER:
      return new ClParserValue(*data_.integer);
    case CL_PARSER_VALUE_TYPE_STRING:
      return new ClParserValue(*data_.str);
    case CL_PARSER_VALUE_TYPE_ARRAY:
      return new ClParserValue(*data_.array);
    case CL_PARSER_VALUE_TYPE_LIST:
      return new ClParserValue(*data_.list);
    case CL_PARSER_VALUE_TYPE_DICTIONARY:
      return new ClParserValue(*data_.dict);
    case CL_PARSER_VALUE_TYPE_STRUCTURE:
      return new ClParserValue(*data_.structure);
    default:
      ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);
      return nullptr;
  }
}

// Convert a value to a type which is compatible with
// the supplied value (i.e. the same type).

bool
ClParserValue::
convertToType(ClParserValuePtr type_value)
{
  ClParserValueType type = type_value->type_;

  if (type == type_)
    return true;

  if      (type == CL_PARSER_VALUE_TYPE_ARRAY) {
    ClParserValueType type1 = type_value->getArray()->getType();

    if (type1 == CL_PARSER_VALUE_TYPE_STRUCTURE) {
      ClParserValuePtr svalue = type_value->getArray()->getValue(1);

      return convertToArrayStructureType(svalue->getStructure()->getType());
    }
    else
      return convertToArrayType(type1);
  }
  else if (type == CL_PARSER_VALUE_TYPE_STRUCTURE)
    return convertToStructureType(type_value->getStructure()->getType());
  else
    return convertToType(type);
}

bool
ClParserValue::
convertToArrayStructureType(ClParserTypePtr type)
{
  if (type_ != CL_PARSER_VALUE_TYPE_ARRAY)
    return false;

  if (getArray()->getType() != CL_PARSER_VALUE_TYPE_STRUCTURE)
    return false;

  ClParserValuePtr svalue = getArray()->getValue(1);

  if (svalue->getStructure()->getType() != type)
    return false;

  return true;
}

bool
ClParserValue::
convertToArrayType(ClParserValueType type)
{
  if      (type == CL_PARSER_VALUE_TYPE_REAL)
    return convertToRealArrayValue();
  else if (type == CL_PARSER_VALUE_TYPE_INTEGER)
    return convertToIntegerArrayValue();
  else if (type == CL_PARSER_VALUE_TYPE_STRING)
    return convertToStringArrayValue();
  else if (type == CL_PARSER_VALUE_TYPE_LIST) {
    if (type_ != CL_PARSER_VALUE_TYPE_ARRAY)
      return false;

    if (getArray()->getType() != CL_PARSER_VALUE_TYPE_LIST)
      return false;
  }
  else if (type == CL_PARSER_VALUE_TYPE_DICTIONARY) {
    if (type_ != CL_PARSER_VALUE_TYPE_ARRAY)
      return false;

    if (getArray()->getType() != CL_PARSER_VALUE_TYPE_DICTIONARY)
      return false;
  }
  else if (type == CL_PARSER_VALUE_TYPE_STRUCTURE)
    return false;
  else
    return false;

  return true;
}

bool
ClParserValue::
convertToStructureType(ClParserTypePtr type)
{
  if (type_ != CL_PARSER_VALUE_TYPE_STRUCTURE)
    return false;

  if (type != getStructure()->getType())
    return false;

  return true;
}

bool
ClParserValue::
convertToType(ClParserValueType type)
{
  if      (type == CL_PARSER_VALUE_TYPE_REAL) {
    if (! convertToReal())
      return false;
  }
  else if (type == CL_PARSER_VALUE_TYPE_INTEGER) {
    if (! convertToInteger())
      return false;
  }
  else if (type == CL_PARSER_VALUE_TYPE_STRING) {
    if (! convertToString())
      return false;
  }
  else if (type == CL_PARSER_VALUE_TYPE_ARRAY)
    return false;
  else if (type == CL_PARSER_VALUE_TYPE_LIST)
    return false;
  else if (type == CL_PARSER_VALUE_TYPE_DICTIONARY)
    return false;
  else if (type == CL_PARSER_VALUE_TYPE_STRUCTURE)
    return false;
  else
    return false;

  return true;
}

// Convert a generic array value to a real array value.

bool
ClParserValue::
convertToRealArrayValue()
{
  if (type_ == CL_PARSER_VALUE_TYPE_ARRAY)
    return getArray()->toRealArray();
  else
    return false;
}

// Convert a generic array value to an integer array value.

bool
ClParserValue::
convertToIntegerArrayValue()
{
  if (type_ == CL_PARSER_VALUE_TYPE_ARRAY)
    return getArray()->toIntegerArray();
  else
    return false;
}

// Convert a generic array value to a str array value.

bool
ClParserValue::
convertToStringArrayValue()
{
  if (type_ == CL_PARSER_VALUE_TYPE_ARRAY)
    return getArray()->toStringArray();
  else
    return false;
}

bool
ClParserValue::
canConvertToReal() const
{
  if (type_ == CL_PARSER_VALUE_TYPE_REAL ||
      type_ == CL_PARSER_VALUE_TYPE_INTEGER)
    return true;

  return false;
}

// Convert a generic value to a real value.
//
// Note: Integers are converted, Reals are left unchanged, other
// types cause a conversion error.

bool
ClParserValue::
convertToReal()
{
  if (type_ == CL_PARSER_VALUE_TYPE_REAL)
    return true;

  if (type_ == CL_PARSER_VALUE_TYPE_INTEGER) {
    double real = double((*data_.integer)->getValue());

    setReal(ClParserReal::createReal(real));

    return true;
  }

  return false;
}

bool
ClParserValue::
canConvertToInteger() const
{
  if (type_ == CL_PARSER_VALUE_TYPE_REAL ||
      type_ == CL_PARSER_VALUE_TYPE_INTEGER)
    return true;

  return false;
}

// Convert a generic value to an integer value.
//
// Note: Reals are converted, Intgers are left unchanged, other
// types cause a conversion error.

bool
ClParserValue::
convertToInteger()
{
  if (type_ == CL_PARSER_VALUE_TYPE_INTEGER)
    return true;

  if (type_ == CL_PARSER_VALUE_TYPE_REAL) {
    long integer = (*data_.real)->getInteger();

    setInteger(ClParserInteger::createInteger(integer));

    return true;
  }

  return false;
}

// Convert a generic value to a str value.
//
// Note: Strings are left unchanged, other types cause a
// conversion error.

bool
ClParserValue::
convertToString()
{
  if (type_ == CL_PARSER_VALUE_TYPE_STRING)
    return true;

  return false;
}

// Convert a value to a real number.
//
// Note: Only Integer and Real Values can be converted to
// Real Numbers.

bool
ClParserValue::
realValue(double *real) const
{
  *real = 0.0;

  if      (type_ == CL_PARSER_VALUE_TYPE_REAL)
    *real = (*data_.real)->getValue();
  else if (type_ == CL_PARSER_VALUE_TYPE_INTEGER)
    *real = double((*data_.integer)->getValue());
  else
    return false;

  return true;
}

// Convert a value to an integer number.
//
// Note: Only Integer and Real Values can be converted to
// Integer Numbers.

bool
ClParserValue::
integerValue(long *integer) const
{
  *integer = 0;

  if      (type_ == CL_PARSER_VALUE_TYPE_REAL)
    *integer = long((*data_.real)->getInteger());
  else if (type_ == CL_PARSER_VALUE_TYPE_INTEGER)
    *integer = (*data_.integer)->getValue();
  else
    return false;

  return true;
}

// Convert a value to an integer number.
//
// Note: Only Integer and Real Values can be converted to
// Integer Numbers.

bool
ClParserValue::
integerValue(int *integer) const
{
  *integer = 0;

  if      (type_ == CL_PARSER_VALUE_TYPE_REAL)
    *integer = int((*data_.real)->getInteger());
  else if (type_ == CL_PARSER_VALUE_TYPE_INTEGER)
    *integer = int((*data_.integer)->getValue());
  else
    return false;

  return true;
}

// Convert a value to a str.
//
// Note: Only String Values can be converted to Strings.

bool
ClParserValue::
stringValue(std::string &str) const
{
  if (type_ == CL_PARSER_VALUE_TYPE_STRING)
    str = (*data_.str)->getText();
  else
    return false;

  return true;
}

// Convert a value to a character array.
//
// Note: Only String Values can be converted to Character Arrays.

bool
ClParserValue::
stringValue(char **chars, int *num_chars) const
{
  if (type_ == CL_PARSER_VALUE_TYPE_STRING) {
    *num_chars = int((*data_.str)->getLen());
    *chars     = new char [uint(*num_chars + 1)];

    for (uint i = 0; i < uint(*num_chars); ++i)
      (*chars)[i] = (*data_.str)->getChar(i);
  }
  else
    return false;

  return true;
}

// Convert a value to a real array.

bool
ClParserValue::
realArrayValue(double **reals, uint **dims, uint *num_dims) const
{
  if (type_ == CL_PARSER_VALUE_TYPE_ARRAY)
    return (*data_.array)->toReals(reals, dims, num_dims);

  double real;

  if (! realValue(&real))
    return false;

  *reals    = new double [1];
  *dims     = new uint [1];
  *num_dims = 1;

  (*reals)[0] = real;
  (*dims )[0] = 1;

  return true;
}

// Convert a value to a single precision real array.

bool
ClParserValue::
realArrayValue(float **reals, uint **dims, uint *num_dims) const
{
  if (type_ == CL_PARSER_VALUE_TYPE_ARRAY)
    return (*data_.array)->toReals(reals, dims, num_dims);

  double real;

  if (! realValue(&real))
    return false;

  *reals    = new float [1];
  *dims     = new uint [1];
  *num_dims = 1;

  (*reals)[0] = float(real);
  (*dims )[0] = 1;

  return true;
}

// Convert a value to an integer array.

bool
ClParserValue::
integerArrayValue(long **integers, uint **dims, uint *num_dims) const
{
  if (type_ == CL_PARSER_VALUE_TYPE_ARRAY)
    return (*data_.array)->toIntegers(integers, dims, num_dims);

  long integer;

  if (! integerValue(&integer))
    return false;

  *integers = new long [1];
  *dims     = new uint [1];
  *num_dims = 1;

  (*integers)[0] = integer;
  (*dims    )[0] = 1;

  return true;
}

// Convert a value to an integer array.

bool
ClParserValue::
integerArrayValue(int **integers, uint **dims, uint *num_dims) const
{
  if (type_ == CL_PARSER_VALUE_TYPE_ARRAY)
    return (*data_.array)->toIntegers(integers, dims, num_dims);

  long integer;

  if (! integerValue(&integer))
    return false;

  *integers = new int [1];
  *dims     = new uint [1];
  *num_dims = 1;

  (*integers)[0] = int(integer);
  (*dims    )[0] = 1;

  return true;
}

// Convert a value to a string array.
bool
ClParserValue::
stringArrayValue(char ***strs, uint **dims, uint *num_dims) const
{
  if (type_ == CL_PARSER_VALUE_TYPE_ARRAY)
    return (*data_.array)->toStrings(strs, dims, num_dims);

  std::string str;

  if (! stringValue(str))
    return false;

  *strs     = new char * [1];
  *dims     = new uint   [1];
  *num_dims = 1;

  (*strs)[0] = strdup(str.c_str());
  (*dims)[0] = 1;

  return true;
}

// Convert a value to an array of sub-values.
void
ClParserValue::
toSubValues(ClParserValuePtr **values, uint *num_values) const
{
  ClParserValueArray values1;

  toSubValues(values1);

  *num_values = uint(values1.size());
  *values     = new ClParserValuePtr [*num_values + 1];

  for (uint i = 0; i < *num_values; ++i)
    (*values)[i] = values1[i];
}

// Convert a value to an array of sub-values.
void
ClParserValue::
toSubValues(ClParserValueArray &values) const
{
  if      (type_ == CL_PARSER_VALUE_TYPE_STRING) {
    char text[2];

    text[1] = '\0';

    uint num_values = (*data_.str)->getLen();

    for (uint i = 0; i < num_values; ++i) {
      text[0] = (*data_.str)->getChar(i);

      values.push_back(ClParserValueMgrInst->createValue(text, 1));
    }
  }
  else if (type_ == CL_PARSER_VALUE_TYPE_ARRAY) {
    UIntVectorT dims;

    (*data_.array)->toValues(values, dims);
  }
  else if (type_ == CL_PARSER_VALUE_TYPE_LIST) {
    uint num_values = (*data_.list)->getNumValues();

    for (uint i = 0; i < num_values; ++i)
      values.push_back((*data_.list)->getValue(int(i + 1)));
  }
  else if (type_ == CL_PARSER_VALUE_TYPE_DICTIONARY) {
    uint num_values = (*data_.dict)->numValues();

    for (uint i = 0; i < num_values; ++i)
      values.push_back((*data_.dict)->getKeyValue(int(i)));
  }
  else if (type_ == CL_PARSER_VALUE_TYPE_STRUCTURE) {
    auto p1 = (*data_.structure)->getValuesBegin();
    auto p2 = (*data_.structure)->getValuesEnd  ();

    for (uint i = 0; p1 != p2; ++p1, ++i)
      values.push_back(p1->second);
  }
}

// Add a Key/Value pair to a Dictionary Value.
bool
ClParserValue::
addDictionaryValue(const std::string &key, ClParserValuePtr value)
{
  if (type_ != CL_PARSER_VALUE_TYPE_DICTIONARY)
    return false;

  const ClParserKey &key1 = (*data_.dict)->stringToKey(key);

  (*data_.dict)->addValue(key1, value);

  return true;
}

// Get the result of applying a Subscript Value to a Value.

bool
ClParserValue::
subscriptValue(ClParserValuePtr subscript, ClParserValuePtr &value) const
{
  if (! subscript) {
    value = ClParserValuePtr(dup());
    return true;
  }

  // subscript must be an array
  if (subscript->getType() != CL_PARSER_VALUE_TYPE_ARRAY)
    return false;

  ClParserArrayPtr array = subscript->getArray();

  // non-dictionary subscript must be an 1D integer array
  if (type_ != CL_PARSER_VALUE_TYPE_DICTIONARY) {
    if (! array->isType(CL_PARSER_VALUE_TYPE_INTEGER) ||
        array->getNumDims() != 1)
      return false;
  }

  // non-array subscript must be max of two values
  if (type_ != CL_PARSER_VALUE_TYPE_ARRAY) {
    if (array->getNumData() > 2)
      return false;
  }

  // a = "12345", a[2]

  if      (type_ == CL_PARSER_VALUE_TYPE_STRING) {
    int i1, i2;

    array->getSubscriptRange(&i1, &i2);

    std::string text;

    if (! (*data_.str)->getSubChars(i1, i2, text))
      return false;

    ClParserValuePtr value1 = ClParserValueMgrInst->createValue(text);

    value = value1;

    return true;
  }

  // a = [1,2,3,4,5], a[2]

  else if (type_ == CL_PARSER_VALUE_TYPE_ARRAY) {
    IntVectorT subscripts;

    array->toIntegers(subscripts);

    ClParserValuePtr value1 = (*data_.array)->getSubscriptValue(subscripts);

    if (! value1)
      return false;

    value = value1;

    return true;
  }

  // a = {1,2,3}, a[1]

  else if (type_ == CL_PARSER_VALUE_TYPE_LIST) {
    UIntVectorT subscripts;

    array->toIntegers(subscripts);

    ClParserValuePtr value2 = (*data_.list)->subscriptValue(subscripts);

    if (! value2)
      return false;

    value = value2;

    return true;
  }
  else if (type_ == CL_PARSER_VALUE_TYPE_DICTIONARY) {
    ClParserValuePtr value2 = (*data_.dict)->subscriptValue(&subscript, 1);

    if (! value2)
      return false;

    value = ClParserValuePtr(value2->dup());

    return true;
  }
  else
    return false;
}

// Check whether the two values are compatible for
// processing with a binary operator. If they are
// compatible but of different types one is promoted
// to the others type.

bool
ClParserValue::
checkBinaryTypes(ClParserValuePtr &value1, ClParserValuePtr &value2)
{
  if (! value1->isType(CL_PARSER_VALUE_TYPE_LIST) &&
        value2->isType(CL_PARSER_VALUE_TYPE_LIST)) {
    ClParserValuePtr value3 = ClParserValueMgrInst->createValue(CL_PARSER_VALUE_TYPE_LIST);

    value3->getList()->addValue(value1);

    return checkBinaryTypes(value3, value1);
  }

  if      (value1->isType(CL_PARSER_VALUE_TYPE_ARRAY) &&
           value2->isType(CL_PARSER_VALUE_TYPE_ARRAY)) {
    ClParserArrayPtr array1 = value1->getArray();
    ClParserArrayPtr array2 = value2->getArray();

    if (array1->getType() != array2->getType()) {
      if      (array1->getType() == CL_PARSER_VALUE_TYPE_REAL) {
        if      (array2->getType() == CL_PARSER_VALUE_TYPE_INTEGER) {
          if (! array2->toRealArray())
            return false;
        }
        else
          return false;
      }
      else if (array1->getType() == CL_PARSER_VALUE_TYPE_INTEGER) {
        if      (array2->getType() == CL_PARSER_VALUE_TYPE_REAL) {
          if (! array1->toRealArray())
            return false;
        }
        else
          return false;
      }
      else if (array1->getType() == CL_PARSER_VALUE_TYPE_STRING)
        return false;
      else if (array1->getType() == CL_PARSER_VALUE_TYPE_STRUCTURE)
        return false;
    }
    else if (array1->getType() == CL_PARSER_VALUE_TYPE_STRUCTURE) {
      ClParserValuePtr svalue1 = array1->getValue(1);
      ClParserValuePtr svalue2 = array2->getValue(1);

      if (svalue1->getStructure()->getType() != svalue2->getStructure()->getType())
        return false;
    }

    if (array1->getNumData() == 0 || array2->getNumData() == 0)
      return false;

    if (array2->sameDimension(*array1))
      return true;

    if (! array1->dimensionTo(*array2))
      return false;

    if (! array2->dimensionTo(*array1))
      return false;

    if (array1->getNumDims() < array2->getNumDims()) {
      if (! array1->expandTo(*array2))
        return false;
    }
    else {
      if (! array2->expandTo(*array1))
        return false;
    }
  }
  else if (value1->isType(CL_PARSER_VALUE_TYPE_ARRAY)) {
    ClParserArrayPtr array;

    if      (value1->getArray()->getType() == CL_PARSER_VALUE_TYPE_REAL) {
      if      (value2->isType(CL_PARSER_VALUE_TYPE_REAL)) {
        array = ClParserArray::createArray
                  (value1->getArray()->getDims(), value2->getReal()->getValue());

        ClParserValuePtr value3 = ClParserValueMgrInst->createValue(array);

        return checkBinaryTypes(value1, value3);
      }
      else if (value2->isType(CL_PARSER_VALUE_TYPE_INTEGER)) {
        array = ClParserArray::createArray
                  (value1->getArray()->getDims(), double(value2->getInteger()->getValue()));

        ClParserValuePtr value3 = ClParserValueMgrInst->createValue(array);

        return checkBinaryTypes(value1, value3);
      }
      else
        return false;
    }
    else if (value1->getArray()->getType() == CL_PARSER_VALUE_TYPE_INTEGER) {
      if      (value2->isType(CL_PARSER_VALUE_TYPE_REAL)) {
        if (! value1->getArray()->toRealArray())
          return false;

        array = ClParserArray::createArray
                  (value1->getArray()->getDims(), value2->getReal()->getValue());

        ClParserValuePtr value3 = ClParserValueMgrInst->createValue(array);

        return checkBinaryTypes(value1, value3);
      }
      else if (value2->isType(CL_PARSER_VALUE_TYPE_INTEGER)) {
        array = ClParserArray::createArray
                  (value1->getArray()->getDims(), value2->getInteger()->getValue());

        ClParserValuePtr value3 = ClParserValueMgrInst->createValue(array);

        return checkBinaryTypes(value1, value3);
      }
      else
        return false;
    }
    else if (value1->getArray()->getType() == CL_PARSER_VALUE_TYPE_STRING) {
      if (value2->isType(CL_PARSER_VALUE_TYPE_STRING)) {
        if (value2->getString()) {
          array = ClParserArray::createArray
                    (value1->getArray()->getDims(), value2->getString()->getText());

          ClParserValuePtr value3 = ClParserValueMgrInst->createValue(array);

          return checkBinaryTypes(value1, value3);
        }
        else {
          array = ClParserArray::createArray(value1->getArray()->getDims(), "");

          ClParserValuePtr value3 = ClParserValueMgrInst->createValue(array);

          return checkBinaryTypes(value1, value3);
        }
      }
      else
        return false;
    }
    else
      return false;
  }
  else if (value2->isType(CL_PARSER_VALUE_TYPE_ARRAY)) {
    ClParserArrayPtr array;

    if      (value2->getArray()->getType() == CL_PARSER_VALUE_TYPE_REAL) {
      if      (value1->isType(CL_PARSER_VALUE_TYPE_REAL)) {
        array = ClParserArray::createArray(value2->getArray()->getDims(),
                                           value1->getReal()->getValue());

        ClParserValuePtr value3 = ClParserValueMgrInst->createValue(array);

        return checkBinaryTypes(value3, value2);
      }
      else if (value1->isType(CL_PARSER_VALUE_TYPE_INTEGER)) {
        array = ClParserArray::createArray(value2->getArray()->getDims(),
                                           double(value1->getInteger()->getValue()));

        ClParserValuePtr value3 = ClParserValueMgrInst->createValue(array);

        return checkBinaryTypes(value3, value2);
      }
      else
        return false;
    }
    else if (value2->getArray()->getType() == CL_PARSER_VALUE_TYPE_INTEGER) {
      if      (value1->isType(CL_PARSER_VALUE_TYPE_REAL)) {
        if (! value2->getArray()->toRealArray())
          return false;

        array = ClParserArray::createArray(value2->getArray()->getDims(),
                                           value1->getReal()->getValue());

        ClParserValuePtr value3 = ClParserValueMgrInst->createValue(array);

        return checkBinaryTypes(value3, value2);
      }
      else if (value1->isType(CL_PARSER_VALUE_TYPE_INTEGER)) {
        array = ClParserArray::createArray(value2->getArray()->getDims(),
                                           value1->getInteger()->getValue());

        ClParserValuePtr value3 = ClParserValueMgrInst->createValue(array);

        return checkBinaryTypes(value3, value2);
      }
      else
        return false;
    }
    else if (value2->getArray()->getType() == CL_PARSER_VALUE_TYPE_STRING) {
      if (value1->isType(CL_PARSER_VALUE_TYPE_STRING)) {
        if (value1->getString()) {
          array = ClParserArray::createArray(value2->getArray()->getDims(),
                                             value1->getString()->getText());

          ClParserValuePtr value3 = ClParserValueMgrInst->createValue(array);

          return checkBinaryTypes(value3, value2);
        }
        else {
          array = ClParserArray::createArray(value2->getArray()->getDims(), "");

          ClParserValuePtr value3 = ClParserValueMgrInst->createValue(array);

          return checkBinaryTypes(value3, value2);
        }
      }
      else
        return false;
    }
    else
      return false;
  }
  else if (value1->getType() != value2->getType()) {
    if      (value1->isType(CL_PARSER_VALUE_TYPE_REAL)) {
      if      (value2->isType(CL_PARSER_VALUE_TYPE_INTEGER)) {
        if (! value2->convertToReal())
          return false;
      }
      else
        return false;
    }
    else if (value1->isType(CL_PARSER_VALUE_TYPE_INTEGER)) {
      if      (value2->isType(CL_PARSER_VALUE_TYPE_REAL)) {
        if (! value1->convertToReal())
          return false;
      }
      else
        return false;
    }
    else if (value1->isType(CL_PARSER_VALUE_TYPE_STRING))
      return false;
    else if (value1->isType(CL_PARSER_VALUE_TYPE_LIST))
      return false;
    else if (value1->isType(CL_PARSER_VALUE_TYPE_DICTIONARY))
      return false;
    else if (value1->isType(CL_PARSER_VALUE_TYPE_STRUCTURE))
      return false;
  }
  else {
    if (value1->isType(CL_PARSER_VALUE_TYPE_STRUCTURE)) {
      if (value1->getStructure()->getType() != value2->getStructure()->getType())
        return false;
    }
  }

  return true;
}

// Return whether the specified value is a real or
// integer array.

bool
ClParserValue::
isNumericArray() const
{
  return (type_ == CL_PARSER_VALUE_TYPE_ARRAY &&
          ((*data_.array)->getType() == CL_PARSER_VALUE_TYPE_REAL   ||
           (*data_.array)->getType() == CL_PARSER_VALUE_TYPE_INTEGER));
}

//   Return whether the specified value is a real array.

bool
ClParserValue::
isRealArray() const
{
  return (type_ == CL_PARSER_VALUE_TYPE_ARRAY &&
          (*data_.array)->getType() == CL_PARSER_VALUE_TYPE_REAL);
}

// Return whether the specified value is an integer array.

bool
ClParserValue::
isIntegerArray() const
{
  return (type_ == CL_PARSER_VALUE_TYPE_ARRAY &&
          (*data_.array)->getType() == CL_PARSER_VALUE_TYPE_INTEGER);
}

// Return whether the specified value is a str array.

bool
ClParserValue::
isStringArray() const
{
  return (type_ == CL_PARSER_VALUE_TYPE_ARRAY &&
          (*data_.array)->getType() == CL_PARSER_VALUE_TYPE_STRING);
}

// Return whether the specified value is a list array.

bool
ClParserValue::
isListArray() const
{
  return (type_ == CL_PARSER_VALUE_TYPE_ARRAY &&
          (*data_.array)->getType() == CL_PARSER_VALUE_TYPE_LIST);
}

// Return whether the specified value is a dictionary array.

bool
ClParserValue::
isDictionaryArray() const
{
  return (type_ == CL_PARSER_VALUE_TYPE_ARRAY &&
          (*data_.array)->getType() == CL_PARSER_VALUE_TYPE_DICTIONARY);
}

// Return whether the specified value is an array of structures.

bool
ClParserValue::
isStructureArray() const
{
  return (type_ == CL_PARSER_VALUE_TYPE_ARRAY &&
          (*data_.array)->getType() == CL_PARSER_VALUE_TYPE_STRUCTURE);
}

// Return whether the specified value is a real or integer value.

bool
ClParserValue::
isNumeric() const
{
  return (type_ == CL_PARSER_VALUE_TYPE_REAL   ||
          type_ == CL_PARSER_VALUE_TYPE_INTEGER);
}

// Return whether the specified value is a real value.

bool
ClParserValue::
isReal() const
{
  return (type_ == CL_PARSER_VALUE_TYPE_REAL);
}

// Return whether the specified value is an integer value.

bool
ClParserValue::
isInteger() const
{
  return (type_ == CL_PARSER_VALUE_TYPE_INTEGER);
}

// Return whether the specified value is a str value.

bool
ClParserValue::
isString() const
{
  return (type_ == CL_PARSER_VALUE_TYPE_STRING);
}

// Return whether the specified value is a list value.

bool
ClParserValue::
isList() const
{
  return (type_ == CL_PARSER_VALUE_TYPE_LIST);
}

// Return whether the specified value is a dictionary value.

bool
ClParserValue::
isDictionary() const
{
  return (type_ == CL_PARSER_VALUE_TYPE_DICTIONARY);
}

// Return whether the specified value is a structure value.

bool
ClParserValue::
isStructure() const
{
  return (type_ == CL_PARSER_VALUE_TYPE_STRUCTURE);
}

bool
ClParserValue::
toBool() const
{
  return getObj()->toBool();
}

int
ClParserValue::
cmp(ClParserValuePtr rhs) const
{
  if (type_ != rhs->type_)
    return (type_ - rhs->type_);

  return getObj()->cmp(*rhs->getObj());
}

//------

std::string
ClParserValue::
asString() const
{
  return getObj()->asString();
}

void
ClParserValue::
print() const
{
  getObj()->print();
}

void
ClParserValue::
print(std::ostream &os) const
{
  getObj()->print(os);
}

void
ClParserValue::
debugPrint() const
{
  getObj()->debugPrint();
}

ClParserValuePtr
ClParserValue::
operator+=(ClParserValuePtr value)
{
  getObj()->plus(*value->getObj());

  return ClParserValuePtr(this);
}

//------------

void
ClParserValue::
increment()
{
  getObj()->increment();
}

void
ClParserValue::
decrement()
{
  getObj()->decrement();
}

//-----------------

// Unary Ops

ClParserValuePtr
ClParserValue::
unaryPlus() const
{
  return getObj()->unaryPlus();
}

ClParserValuePtr
ClParserValue::
unaryMinus() const
{
  return getObj()->unaryMinus();
}

ClParserValuePtr
ClParserValue::
bitNot() const
{
  return getObj()->bitNot();
}

//------

// Binary Ops

ClParserValuePtr
ClParserValue::
plus(ClParserValuePtr value) const
{
  return getObj()->plus(*value->getObj());
}

ClParserValuePtr
ClParserValue::
minus(ClParserValuePtr value) const
{
  return getObj()->minus(*value->getObj());
}

ClParserValuePtr
ClParserValue::
times(ClParserValuePtr value) const
{
  return getObj()->times(*value->getObj());
}

ClParserValuePtr
ClParserValue::
divide(ClParserValuePtr value) const
{
  return getObj()->divide(*value->getObj());
}

ClParserValuePtr
ClParserValue::
modulus(ClParserValuePtr value) const
{
  return getObj()->modulus(*value->getObj());
}

ClParserValuePtr
ClParserValue::
power(ClParserValuePtr value) const
{
  return getObj()->power(*value->getObj());
}

ClParserValuePtr
ClParserValue::
approxEqual(ClParserValuePtr value) const
{
  return getObj()->approxEqual(*value->getObj());
}

ClParserValuePtr
ClParserValue::
bitAnd(ClParserValuePtr value) const
{
  return getObj()->bitAnd(*value->getObj());
}

ClParserValuePtr
ClParserValue::
bitOr(ClParserValuePtr value) const
{
  return getObj()->bitOr(*value->getObj());
}

ClParserValuePtr
ClParserValue::
bitXor(ClParserValuePtr value) const
{
  return getObj()->bitXor(*value->getObj());
}

ClParserValuePtr
ClParserValue::
bitLShift(ClParserValuePtr value) const
{
  return getObj()->bitLShift(*value->getObj());
}

ClParserValuePtr
ClParserValue::
bitRShift(ClParserValuePtr value) const
{
  return getObj()->bitRShift(*value->getObj());
}

//------

// Internal Fns

ClParserValuePtr
ClParserValue::
abs() const
{
  return getObj()->abs();
}

ClParserValuePtr
ClParserValue::
ceil() const
{
  return getObj()->ceil();
}

ClParserValuePtr
ClParserValue::
floor() const
{
  return getObj()->floor();
}

ClParserValuePtr
ClParserValue::
sign() const
{
  return getObj()->sign();
}

ClParserValuePtr
ClParserValue::
sqr() const
{
  return getObj()->sqr();
}

ClParserValuePtr
ClParserValue::
sqrt() const
{
  return getObj()->sqrt();
}

ClParserValuePtr
ClParserValue::
cos() const
{
  return getObj()->cos();
}

ClParserValuePtr
ClParserValue::
sin() const
{
  return getObj()->sin();
}

ClParserValuePtr
ClParserValue::
tan() const
{
  return getObj()->tan();
}

ClParserValuePtr
ClParserValue::
acos() const
{
  return getObj()->acos();
}

ClParserValuePtr
ClParserValue::
asin() const
{
  return getObj()->asin();
}

ClParserValuePtr
ClParserValue::
atan() const
{
  return getObj()->atan();
}

ClParserValuePtr
ClParserValue::
atan(double real) const
{
  return getObj()->atan(real);
}

ClParserValuePtr
ClParserValue::
exp() const
{
  return getObj()->exp();
}

ClParserValuePtr
ClParserValue::
log() const
{
  return getObj()->log();
}

ClParserValuePtr
ClParserValue::
log10() const
{
  return getObj()->log10();
}

ClParserValuePtr
ClParserValue::
cosh() const
{
  return getObj()->cosh();
}

ClParserValuePtr
ClParserValue::
sinh() const
{
  return getObj()->sinh();
}

ClParserValuePtr
ClParserValue::
tanh() const
{
  return getObj()->tanh();
}

ClParserValuePtr
ClParserValue::
norm() const
{
  return getObj()->norm();
}

ClParserValuePtr
ClParserValue::
invnorm() const
{
  return getObj()->invnorm();
}

ClParserValuePtr
ClParserValue::
toChar() const
{
  return getObj()->toChar();
}

ClParserValuePtr
ClParserValue::
toInt() const
{
  return getObj()->toInt();
}

ClParserValuePtr
ClParserValue::
toReal() const
{
  return getObj()->toReal();
}

ClParserValuePtr
ClParserValue::
toString() const
{
  return getObj()->toString();
}

ClParserValuePtr
ClParserValue::
isNan() const
{
  return getObj()->isNan();
}

ClParserValuePtr
ClParserValue::
toLower() const
{
  return getObj()->toLower();
}

ClParserValuePtr
ClParserValue::
toUpper() const
{
  return getObj()->toUpper();
}

ClParserValuePtr
ClParserValue::
dim() const
{
  return getObj()->dim();
}

ClParserValuePtr
ClParserValue::
ndim() const
{
  return getObj()->ndim();
}

ClParserValuePtr
ClParserValue::
len() const
{
  return getObj()->len();
}

ClParserValuePtr
ClParserValue::
min() const
{
  return getObj()->min();
}

ClParserValuePtr
ClParserValue::
max() const
{
  return getObj()->max();
}

ClParserValuePtr
ClParserValue::
sum() const
{
  return getObj()->sum();
}

ClParserValuePtr
ClParserValue::
index(ClParserValuePtr value) const
{
  return getObj()->index(*value->getObj());
}

ClParserValuePtr
ClParserValue::
rindex(ClParserValuePtr value) const
{
  return getObj()->rindex(*value->getObj());
}

ClParserValuePtr
ClParserValue::
sort(ClParserSortDirection direction) const
{
  return getObj()->sort(direction);
}

ClParserValuePtr
ClParserValue::
doAssert() const
{
  return getObj()->doAssert();
}

//------------

ClParserObj *
ClParserValue::
getObj() const
{
  return static_cast<ClParserObj *>(dynamic_cast<ClParserReal *>(data_.real->get()));
}

ClParserRealPtr
ClParserValue::
getReal() const
{
  if (type_ == CL_PARSER_VALUE_TYPE_REAL)
    return *data_.real;
  else
    return ClParserRealPtr();
}

ClParserIntegerPtr
ClParserValue::
getInteger() const
{
  if (type_ == CL_PARSER_VALUE_TYPE_INTEGER)
    return *data_.integer;
  else
    return ClParserIntegerPtr();
}

ClParserStringPtr
ClParserValue::
getString() const
{
  if (type_ == CL_PARSER_VALUE_TYPE_STRING)
    return *data_.str;
  else
    return ClParserStringPtr();
}

ClParserArrayPtr
ClParserValue::
getArray() const
{
  if (type_ == CL_PARSER_VALUE_TYPE_ARRAY)
    return *data_.array;
  else
    return ClParserArrayPtr();
}

ClParserListPtr
ClParserValue::
getList() const
{
  if (type_ == CL_PARSER_VALUE_TYPE_LIST)
    return *data_.list;
  else
    return ClParserListPtr();
}

ClParserDictPtr
ClParserValue::
getDictionary() const
{
  if (type_ == CL_PARSER_VALUE_TYPE_DICTIONARY)
    return *data_.dict;
  else
    return ClParserDictPtr();
}

ClParserStructPtr
ClParserValue::
getStructure() const
{
  if (type_ == CL_PARSER_VALUE_TYPE_STRUCTURE)
    return *data_.structure;
  else
    return ClParserStructPtr();
}

void
ClParserValue::
setReal(ClParserRealPtr real)
{
  resetValue();

  data_.real = new ClParserRealPtr;

  type_       = CL_PARSER_VALUE_TYPE_REAL;
  *data_.real = real;
}

void
ClParserValue::
setInteger(ClParserIntegerPtr integer)
{
  resetValue();

  data_.integer = new ClParserIntegerPtr;

  type_          = CL_PARSER_VALUE_TYPE_INTEGER;
  *data_.integer = integer;
}

void
ClParserValue::
setString(ClParserStringPtr str)
{
  resetValue();

  data_.str = new ClParserStringPtr;

  type_      = CL_PARSER_VALUE_TYPE_STRING;
  *data_.str = str;
}

void
ClParserValue::
setArray(ClParserArrayPtr array)
{
  resetValue();

  data_.array = new ClParserArrayPtr;

  type_        = CL_PARSER_VALUE_TYPE_ARRAY;
  *data_.array = array;
}

void
ClParserValue::
setList(ClParserListPtr list)
{
  resetValue();

  data_.list = new ClParserListPtr;

  type_       = CL_PARSER_VALUE_TYPE_LIST;
  *data_.list = list;
}

void
ClParserValue::
setDictionary(ClParserDictPtr dict)
{
  resetValue();

  data_.dict = new ClParserDictPtr;

  type_       = CL_PARSER_VALUE_TYPE_DICTIONARY;
  *data_.dict = dict;
}

void
ClParserValue::
setStructure(ClParserStructPtr structure)
{
  resetValue();

  data_.structure = new ClParserStructPtr;

  type_            = CL_PARSER_VALUE_TYPE_STRUCTURE;
  *data_.structure = structure;
}
