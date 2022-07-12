#include <CCeilPI.h>

int ClParserArray::error_code_ = 0;

ClParserArrayPtr
ClParserArray::
createArray()
{
  ClParserArray *array = new ClParserArray;

  return ClParserArrayPtr(array);
}

ClParserArrayPtr
ClParserArray::
createArray(ClParserValueType type, const uint *dims, uint num_dims)
{
  ClParserArray *array = new ClParserArray(type, dims, num_dims);

  return ClParserArrayPtr(array);
}

ClParserArrayPtr
ClParserArray::
createArray(ClParserValueType type, const ValueArray &values)
{
  ClParserArray *array = new ClParserArray(type, values);

  return ClParserArrayPtr(array);
}

ClParserArrayPtr
ClParserArray::
createArray(const uint *dims, uint num_dims, const double *reals)
{
  ClParserArray *array = new ClParserArray(dims, num_dims, reals);

  return ClParserArrayPtr(array);
}

ClParserArrayPtr
ClParserArray::
createArray(const uint *dims, uint num_dims, const float *reals)
{
  ClParserArray *array = new ClParserArray(dims, num_dims, reals);

  return ClParserArrayPtr(array);
}

ClParserArrayPtr
ClParserArray::
createArray(const uint *dims, uint num_dims, const long *integers)
{
  ClParserArray *array = new ClParserArray(dims, num_dims, integers);

  return ClParserArrayPtr(array);
}

ClParserArrayPtr
ClParserArray::
createArray(const uint *dims, uint num_dims, const int *integers)
{
  ClParserArray *array = new ClParserArray(dims, num_dims, integers);

  return ClParserArrayPtr(array);
}

ClParserArrayPtr
ClParserArray::
createArray(const uint *dims, uint num_dims, const char **strs)
{
  ClParserArray *array = new ClParserArray(dims, num_dims, strs);

  return ClParserArrayPtr(array);
}

ClParserArrayPtr
ClParserArray::
createArray(const uint *dims, uint num_dims, const StringVectorT &strs)
{
  ClParserArray *array = new ClParserArray(dims, num_dims, strs);

  return ClParserArrayPtr(array);
}

ClParserArrayPtr
ClParserArray::
createArray(const uint *dims, uint num_dims, const LongVectorT &integers)
{
  ClParserArray *array = new ClParserArray(dims, num_dims, integers);

  return ClParserArrayPtr(array);
}

ClParserArrayPtr
ClParserArray::
createArray(const uint *dims, uint num_dims, double real)
{
  ClParserArray *array = new ClParserArray(dims, num_dims, real);

  return ClParserArrayPtr(array);
}

ClParserArrayPtr
ClParserArray::
createArray(const UIntVectorT &dims, double real)
{
  ClParserArray *array = new ClParserArray(dims, real);

  return ClParserArrayPtr(array);
}

ClParserArrayPtr
ClParserArray::
createArray(const uint *dims, uint num_dims, long integer)
{
  ClParserArray *array = new ClParserArray(dims, num_dims, integer);

  return ClParserArrayPtr(array);
}

ClParserArrayPtr
ClParserArray::
createArray(const UIntVectorT &dims, long integer)
{
  ClParserArray *array = new ClParserArray(dims, integer);

  return ClParserArrayPtr(array);
}

ClParserArrayPtr
ClParserArray::
createArray(const uint *dims, uint num_dims, const char *str)
{
  ClParserArray *array = new ClParserArray(dims, num_dims, str);

  return ClParserArrayPtr(array);
}

ClParserArrayPtr
ClParserArray::
createArray(const UIntVectorT &dims, const char *str)
{
  ClParserArray *array = new ClParserArray(dims, str);

  return ClParserArrayPtr(array);
}

ClParserArrayPtr
ClParserArray::
createArray(const UIntVectorT &dims, const std::string &str)
{
  ClParserArray *array = new ClParserArray(dims, str);

  return ClParserArrayPtr(array);
}

ClParserArrayPtr
ClParserArray::
createArray(ClParserTypePtr type, const uint *dims, uint num_dims)
{
  ClParserArray *array = new ClParserArray(type, dims, num_dims);

  return ClParserArrayPtr(array);
}

ClParserArrayPtr
ClParserArray::
createArray(ClParserTypePtr type, const UIntVectorT &dims)
{
  ClParserArray *array = new ClParserArray(type, dims);

  return ClParserArrayPtr(array);
}

ClParserArrayPtr
ClParserArray::
createArray(const uint *dims, uint num_dims, const ClParserValuePtr *values)
{
  ClParserArray *array = new ClParserArray(dims, num_dims, values);

  return ClParserArrayPtr(array);
}

ClParserArrayPtr
ClParserArray::
createArray(const ClParserValuePtr *values, uint num_values)
{
  ClParserArray *array = new ClParserArray(values, num_values);

  return ClParserArrayPtr(array);
}

ClParserArrayPtr
ClParserArray::
createArray(const ClParserValueArray &values)
{
  ClParserArray *array = new ClParserArray(values);

  return ClParserArrayPtr(array);
}

ClParserArrayPtr
ClParserArray::
createArray(const ClParserValuePtr *values, const uint *dims, uint num_dims)
{
  ClParserArray *array = new ClParserArray(values, dims, num_dims);

  return ClParserArrayPtr(array);
}

ClParserArrayPtr
ClParserArray::
createArray(const ClParserValueArray &values, const UIntVectorT &dims)
{
  ClParserArray *array = new ClParserArray(values, dims);

  return ClParserArrayPtr(array);
}

ClParserArrayPtr
ClParserArray::
createArray(const ClParserArray &array)
{
  ClParserArray *parray = new ClParserArray(array);

  return ClParserArrayPtr(parray);
}

//--------------------

ClParserArray::
ClParserArray() :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), type_(CL_PARSER_VALUE_TYPE_NONE)
{
}

ClParserArray::
ClParserArray(ClParserValueType type) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), type_(type)
{
}

ClParserArray::
ClParserArray(ClParserValueType type, const uint *dims, uint num_dims) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), type_(type),
 values_(dims, num_dims, ClParserValuePtr())
{
}

ClParserArray::
ClParserArray(ClParserValueType type, const UIntVectorT &dims) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), type_(type), values_(dims, ClParserValuePtr())
{
}

ClParserArray::
ClParserArray(ClParserValueType type, const ValueArray &values) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), type_(type), values_(values)
{
}

ClParserArray::
ClParserArray(const uint *dims, uint num_dims, const double *reals) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), type_(CL_PARSER_VALUE_TYPE_REAL),
 values_(dims, num_dims, ClParserValuePtr())
{
  uint num_data = getNumData();

  for (uint i = 0; i < num_data; ++i)
    values_.setLinearValue(i, ClParserValueMgrInst->createValue(reals[i]));
}

ClParserArray::
ClParserArray(const UIntVectorT &dims, const double *reals) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), type_(CL_PARSER_VALUE_TYPE_REAL),
 values_(dims, ClParserValuePtr())
{
  uint num_data = getNumData();

  for (uint i = 0; i < num_data; ++i)
    values_.setLinearValue(i, ClParserValueMgrInst->createValue(reals[i]));
}

ClParserArray::
ClParserArray(const uint *dims, uint num_dims, const float *reals) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), type_(CL_PARSER_VALUE_TYPE_REAL),
 values_(dims, num_dims, ClParserValuePtr())
{
  uint num_data = getNumData();

  for (uint i = 0; i < num_data; ++i)
    values_.setLinearValue(i, ClParserValueMgrInst->createValue(reals[i]));
}

ClParserArray::
ClParserArray(const uint *dims, uint num_dims, const long *integers) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), type_(CL_PARSER_VALUE_TYPE_INTEGER),
 values_(dims, num_dims, ClParserValuePtr())
{
  uint num_data = getNumData();

  for (uint i = 0; i < num_data; ++i)
    values_.setLinearValue(i, ClParserValueMgrInst->createValue(integers[i]));
}

ClParserArray::
ClParserArray(const UIntVectorT &dims, const long *integers) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), type_(CL_PARSER_VALUE_TYPE_INTEGER),
 values_(dims, ClParserValuePtr())
{
  uint num_data = getNumData();

  for (uint i = 0; i < num_data; ++i)
    values_.setLinearValue(i, ClParserValueMgrInst->createValue(integers[i]));
}

ClParserArray::
ClParserArray(const uint *dims, uint num_dims, const int *integers) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), type_(CL_PARSER_VALUE_TYPE_INTEGER),
 values_(dims, num_dims, ClParserValuePtr())
{
  uint num_data = getNumData();

  for (uint i = 0; i < num_data; ++i)
    values_.setLinearValue(i, ClParserValueMgrInst->createValue(long(integers[i])));
}

ClParserArray::
ClParserArray(const uint *dims, uint num_dims, const LongVectorT &integers) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), type_(CL_PARSER_VALUE_TYPE_INTEGER),
 values_(dims, num_dims, ClParserValuePtr())
{
  uint num_data = getNumData();

  for (uint i = 0; i < num_data; ++i)
    values_.setLinearValue(i, ClParserValueMgrInst->createValue(integers[i]));
}

ClParserArray::
ClParserArray(const uint *dims, uint num_dims, const char **strings) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), type_(CL_PARSER_VALUE_TYPE_STRING),
 values_(dims, num_dims, ClParserValuePtr())
{
  uint num_data = getNumData();

  for (uint i = 0; i < num_data; ++i) {
    uint len = (strings[i] ? uint(strlen(strings[i])) : 0);

    values_.setLinearValue(i, ClParserValueMgrInst->createValue(strings[i], len));
  }
}

ClParserArray::
ClParserArray(const uint *dims, uint num_dims, const StringVectorT &strs) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), type_(CL_PARSER_VALUE_TYPE_STRING),
 values_(dims, num_dims, ClParserValuePtr())
{
  uint num_data = getNumData();

  for (uint i = 0; i < num_data; ++i)
    values_.setLinearValue(i, ClParserValueMgrInst->createValue(strs[i]));
}

ClParserArray::
ClParserArray(const UIntVectorT &dims, const char **strings) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), type_(CL_PARSER_VALUE_TYPE_STRING),
 values_(dims, ClParserValuePtr())
{
  uint num_data = getNumData();

  for (uint i = 0; i < num_data; ++i) {
    uint len = (strings[i] ? uint(strlen(strings[i])) : 0);

    values_.setLinearValue(i, ClParserValueMgrInst->createValue(strings[i], len));
  }
}

ClParserArray::
ClParserArray(const uint *dims, uint num_dims, double real) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), type_(CL_PARSER_VALUE_TYPE_REAL),
 values_(dims, num_dims, ClParserValueMgrInst->createValue(real))
{
}

ClParserArray::
ClParserArray(const UIntVectorT &dims, double real) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), type_(CL_PARSER_VALUE_TYPE_REAL),
 values_(dims, ClParserValueMgrInst->createValue(real))
{
}

ClParserArray::
ClParserArray(const uint *dims, uint num_dims, long integer) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), type_(CL_PARSER_VALUE_TYPE_INTEGER),
 values_(dims, num_dims, ClParserValueMgrInst->createValue(integer))
{
}

ClParserArray::
ClParserArray(const UIntVectorT &dims, long integer) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), type_(CL_PARSER_VALUE_TYPE_INTEGER),
 values_(dims, ClParserValueMgrInst->createValue(integer))
{
}

ClParserArray::
ClParserArray(const uint *dims, uint num_dims, const std::string &str) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), type_(CL_PARSER_VALUE_TYPE_STRING),
 values_(dims, num_dims, ClParserValueMgrInst->createValue(str))
{
}

ClParserArray::
ClParserArray(const UIntVectorT &dims, const std::string &str) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), type_(CL_PARSER_VALUE_TYPE_STRING),
 values_(dims, ClParserValueMgrInst->createValue(str))
{
}

ClParserArray::
ClParserArray(ClParserTypePtr type, const uint *dims, uint num_dims) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), type_(CL_PARSER_VALUE_TYPE_STRUCTURE),
 values_(dims, num_dims, ClParserValueMgrInst->createValue(type))
{
}

ClParserArray::
ClParserArray(ClParserTypePtr type, const UIntVectorT &dims) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), type_(CL_PARSER_VALUE_TYPE_STRUCTURE),
 values_(dims, ClParserValueMgrInst->createValue(type))
{
}

// TODO: use separate interface
ClParserArray::
ClParserArray(const uint *dims, uint num_dims, const ClParserValuePtr *values) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), type_(CL_PARSER_VALUE_TYPE_STRUCTURE),
 values_(dims, num_dims, ClParserValuePtr())
{
  ClParserTypePtr type;

  uint num_data = getNumData();

  for (uint i = 0; i < num_data; ++i) {
    if (values[i]->getType() != CL_PARSER_VALUE_TYPE_STRUCTURE ||
        (type.isValid() && values[i]->getStructure()->getType() != type)) {
      ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);
      return;
    }

    if (! type.isValid())
      type = values[i]->getStructure()->getType();
  }

  for (uint i = 0; i < num_data; ++i) {
    ClParserStructPtr structure = values[i]->getStructure();

    values_.setLinearValue(i, ClParserValueMgrInst->createValue(type));

    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    uint num_values = structure->getNumValues();

    for (uint j = 0; j < num_values; ++j) {
      const std::string &name = type->getSubType(j)->getName();

      ClParserValuePtr ivalue;

      if (values[i]->getStructure()->getValue(name, ivalue))
        values[i]->getStructure()->setValue(name, ivalue);
    }
  }
}

ClParserArray::
ClParserArray(const ClParserValuePtr *values, uint num_values) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), type_(CL_PARSER_VALUE_TYPE_NONE)
{
  if (num_values <= 0)
    return;

  if (values[0]->getType() == CL_PARSER_VALUE_TYPE_ARRAY) {
    ClParserArrayPtr array1 = values[0]->getArray();

    type_ = array1->getType();

    UIntVectorT dims;

    dims.push_back(num_values);

    uint num_dims = array1->getNumDims();

    for (uint i = 0; i < num_dims; ++i)
      dims.push_back(array1->getDim(i));

    ValueArray values1(dims);

    for (uint i = 0, k = 0; i < num_values; ++i) {
      array1 = values[i]->getArray();

      uint num_data = array1->getNumData();

      for (uint j = 0; j < num_data; ++j, ++k) {
        ClParserValuePtr value;

        array1->values_.getLinearValue(j, value);

        values1.setLinearValue(k, value);
      }
    }

    values_ = values1;
  }
  else {
    ValueArray values1(&num_values, 1, values);

    type_ = values[0]->getType();

    values_ = values1;
  }
}

ClParserArray::
ClParserArray(const ClParserValueArray &values) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), type_(CL_PARSER_VALUE_TYPE_NONE)
{
  uint num_values = uint(values.size());

  if (num_values <= 0)
    return;

  if (values[0]->getType() == CL_PARSER_VALUE_TYPE_ARRAY) {
    ClParserArrayPtr array1 = values[0]->getArray();

    type_ = array1->getType();

    UIntVectorT dims;

    dims.push_back(num_values);

    uint num_dims = array1->getNumDims();

    for (uint i = 0; i < num_dims; ++i)
      dims.push_back(array1->getDim(i));

    ValueArray values1(dims);

    for (uint i = 0, k = 0; i < num_values; ++i) {
      array1 = values[i]->getArray();

      uint num_data = array1->getNumData();

      for (uint j = 0; j < num_data; ++j, ++k) {
        ClParserValuePtr value;

        array1->values_.getLinearValue(j, value);

        values1.setLinearValue(k, value);
      }
    }

    values_ = values1;
  }
  else {
    ValueArray values1(&num_values, 1, const_cast<ClParserValuePtr *>(&values[0]));

    type_ = values[0]->getType();

    values_ = values1;
  }
}

ClParserArray::
ClParserArray(const ClParserValuePtr *values, const uint *dims, uint num_dims) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), values_(dims, num_dims, ClParserValuePtr())
{
  uint num_data = getNumData();

  if (num_data > 0) {
    assert(values[0]->getType() != CL_PARSER_VALUE_TYPE_ARRAY);

    type_ = values[0]->getType();

    for (uint i = 0; i < num_data; ++i)
      values_.setLinearValue(i, values[i]);
  }
}

ClParserArray::
ClParserArray(const ClParserValueArray &values, const UIntVectorT &dims) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), values_(dims, ClParserValuePtr())
{
  uint num_data = getNumData();

  if (num_data > 0) {
    assert(values[0]->getType() != CL_PARSER_VALUE_TYPE_ARRAY);

    type_ = values[0]->getType();

    for (uint i = 0; i < num_data; ++i)
      values_.setLinearValue(i, values[i]);
  }
}

ClParserArray::
ClParserArray(const ClParserArray &array) :
 ClParserObj(CL_PARSER_VALUE_TYPE_ARRAY), type_(array.type_), values_(array.values_)
{
}

ClParserArray::
~ClParserArray()
{
}

ClParserArray &
ClParserArray::
operator=(const ClParserArray &array)
{
  type_   = array.type_;
  values_ = array.values_;

  return *this;
}

void
ClParserArray::
copy(const ClParserObj &obj)
{
  assert(getBaseType() == obj.getBaseType());

  const ClParserArray &rhs = castObj(obj);

  *this = rhs;
}

ClParserArray *
ClParserArray::
dup() const
{
  return new ClParserArray(*this);
}

ClParserArrayPtr
ClParserArray::
dupArray() const
{
  ClParserArray *array = dup();

  return ClParserArrayPtr(array);
}

uint
ClParserArray::
getNumDims() const
{
  return values_.getNumDims();
}

uint
ClParserArray::
getNumData() const
{
  return values_.getNumData();
}

UIntVectorT
ClParserArray::
getDims() const
{
  UIntVectorT dims;

  values_.getDims(dims);

  return dims;
}

uint
ClParserArray::
getDim(uint i) const
{
  return values_.getDim(i);
}

bool
ClParserArray::
toBool() const
{
  return (getNumData() > 0);
}

bool
ClParserArray::
toRealArray()
{
  type_ = CL_PARSER_VALUE_TYPE_REAL;

  uint num_data = getNumData();

  for (uint i = 0; i < num_data; ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    values_.setLinearValue(i, value->toReal());
  }

  return true;
}

bool
ClParserArray::
toIntegerArray()
{
  type_ = CL_PARSER_VALUE_TYPE_INTEGER;

  uint num_data = getNumData();

  for (uint i = 0; i < num_data; ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    values_.setLinearValue(i, value->toInt());
  }

  return true;
}

bool
ClParserArray::
toStringArray()
{
  type_ = CL_PARSER_VALUE_TYPE_STRING;

  uint num_data = getNumData();

  for (uint i = 0; i < num_data; ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    values_.setLinearValue(i, value->toString());
  }

  return true;
}

bool
ClParserArray::
toReals(double **reals, uint **dims, uint *num_dims) const
{
  toReals(reals, nullptr);

  uint *dims1;

  values_.getDims(&dims1, num_dims);

  *dims = new uint [*num_dims];

  memcpy(*dims, dims1, (*num_dims)*sizeof(uint));

  return true;
}

bool
ClParserArray::
toReals(double **reals, UIntVectorT &dims) const
{
  toReals(reals, nullptr);

  values_.getDims(dims);

  return true;
}

bool
ClParserArray::
toReals(float **reals, uint **dims, uint *num_dims) const
{
  toReals(reals, nullptr);

  uint *dims1;

  values_.getDims(&dims1, num_dims);

  *dims = new uint [*num_dims];

  memcpy(*dims, dims1, (*num_dims)*sizeof(uint));

  return true;
}

bool
ClParserArray::
toReals(double **reals, uint *num_reals) const
{
  uint num_data = getNumData();

  *reals = new double [num_data];

  for (uint i = 0; i < num_data; ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    (*reals)[i] = double(value->toReal()->getReal()->getValue());
  }

  if (num_reals)
    *num_reals = num_data;

  return true;
}

bool
ClParserArray::
toReals(float **reals, uint *num_reals) const
{
  uint num_data = getNumData();

  *reals = new float [num_data];

  for (uint i = 0; i < num_data; ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    (*reals)[i] = float(value->toReal()->getReal()->getValue());
  }

  if (num_reals)
    *num_reals = num_data;

  return true;
}

bool
ClParserArray::
toIntegers(int **integers, uint **dims, uint *num_dims) const
{
  toIntegers(integers, nullptr);

  uint *dims1;

  values_.getDims(&dims1, num_dims);

  *dims = new uint [*num_dims];

  memcpy(*dims, dims1, (*num_dims)*sizeof(uint));

  return true;
}

bool
ClParserArray::
toIntegers(uint **integers, uint **dims, uint *num_dims) const
{
  toIntegers(integers, nullptr);

  uint *dims1;

  values_.getDims(&dims1, num_dims);

  *dims = new uint [*num_dims];

  memcpy(*dims, dims1, (*num_dims)*sizeof(uint));

  return true;
}

bool
ClParserArray::
toIntegers(long **integers, uint **dims, uint *num_dims) const
{
  toIntegers(integers, nullptr);

  uint *dims1;

  values_.getDims(&dims1, num_dims);

  *dims = new uint [*num_dims];

  memcpy(*dims, dims1, (*num_dims)*sizeof(uint));

  return true;
}

bool
ClParserArray::
toIntegers(long **integers, UIntVectorT &dims) const
{
  toIntegers(integers, nullptr);

  values_.getDims(dims);

  return true;
}

bool
ClParserArray::
toIntegers(int **integers, uint *num_integers) const
{
  uint num_data = getNumData();

  *integers = new int [num_data];

  for (uint i = 0; i < num_data; ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    (*integers)[i] = int(value->getInteger()->getValue());
  }

  if (num_integers)
    *num_integers = num_data;

  return true;
}

bool
ClParserArray::
toIntegers(uint **integers, uint *num_integers) const
{
  uint num_data = getNumData();

  *integers = new uint [num_data];

  for (uint i = 0; i < num_data; ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    (*integers)[i] = uint(value->getInteger()->getValue());
  }

  if (num_integers)
    *num_integers = num_data;

  return true;
}

bool
ClParserArray::
toIntegers(long **integers, uint *num_integers) const
{
  uint num_data = getNumData();

  *integers = new long [num_data];

  for (uint i = 0; i < num_data; ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    (*integers)[i] = long(value->getInteger()->getValue());
  }

  if (num_integers)
    *num_integers = num_data;

  return true;
}

bool
ClParserArray::
toIntegers(IntVectorT &integers) const
{
  uint num_data = getNumData();

  for (uint i = 0; i < num_data; ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    integers.push_back(int(value->getInteger()->getValue()));
  }

  return true;
}

bool
ClParserArray::
toIntegers(UIntVectorT &integers) const
{
  uint num_data = getNumData();

  for (uint i = 0; i < num_data; ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    integers.push_back(uint(value->getInteger()->getValue()));
  }

  return true;
}

bool
ClParserArray::
toIntegers(LongVectorT &integers) const
{
  uint num_data = getNumData();

  for (uint i = 0; i < num_data; ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    integers.push_back(long(value->getInteger()->getValue()));
  }

  return true;
}

bool
ClParserArray::
toStrings(char ***strings, uint **dims, uint *num_dims) const
{
  toStrings(strings, nullptr);

  uint *dims1;

  values_.getDims(&dims1, num_dims);

  *dims = new uint [*num_dims];

  memcpy(*dims, dims1, (*num_dims)*sizeof(uint));

  return true;
}

bool
ClParserArray::
toStrings(char ***strings, UIntVectorT &dims) const
{
  toStrings(strings, nullptr);

  values_.getDims(dims);

  return true;
}

bool
ClParserArray::
toStrings(char ***strings, uint *num_strings) const
{
  uint num_data = getNumData();

  *strings = new char * [num_data];

  for (uint i = 0; i < num_data; ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    std::string str = value->getString()->getText();

    (*strings)[i] = strdup(const_cast<char *>(str.c_str()));
  }

  if (num_strings)
    *num_strings = num_data;

  return true;
}

bool
ClParserArray::
toValues(ClParserValuePtr **values, uint **dims, uint *num_dims) const
{
  ClParserValueArray values1;

  if (! toValues(values1, dims, num_dims))
    return false;

  uint num_values = uint(values1.size());

  *values = new ClParserValuePtr [num_values];

  for (uint i = 0; i < num_values; ++i)
    (*values)[i] = values1[i];

  return true;
}

bool
ClParserArray::
toValues(ClParserValuePtr **values, UIntVectorT &dims) const
{
  ClParserValueArray values1;

  if (! toValues(values1, dims))
    return false;

  uint num_values = uint(values1.size());

  *values = new ClParserValuePtr [num_values];

  for (uint i = 0; i < num_values; ++i)
    (*values)[i] = values1[i];

  return true;
}

bool
ClParserArray::
toValues(ClParserValueArray &values, uint **dims, uint *num_dims) const
{
  uint num_data = getNumData();

  for (uint i = 0; i < num_data; ++i) {
    ClParserValuePtr value1;

    values_.getLinearValue(i, value1);

    values.push_back(value1);
  }

  uint *dims1;

  values_.getDims(&dims1, num_dims);

  *dims = new uint [*num_dims];

  memcpy(*dims, dims1, (*num_dims)*sizeof(uint));

  return true;
}

bool
ClParserArray::
toValues(ClParserValueArray &values, UIntVectorT &dims) const
{
  uint num_data = getNumData();

  for (uint i = 0; i < num_data; ++i) {
    ClParserValuePtr value1;

    values_.getLinearValue(i, value1);

    values.push_back(value1);
  }

  values_.getDims(dims);

  return true;
}

ClParserValuePtr
ClParserArray::
getValue(int i) const
{
  if (! indexToData(&i))
    return ClParserValuePtr();

  ClParserValuePtr value;

  values_.getLinearValue(uint(i), value);

  return value;
}

ClParserValuePtr
ClParserArray::
getSubscriptValue(const int *subscripts, uint num_subscripts) const
{
  UIntVectorT subscripts1;

  for (uint i = 0; i < num_subscripts; ++i) {
    int subscript = subscripts[i];

    if (! indexToData(&subscript))
      return ClParserValuePtr();

    subscripts1.push_back(uint(subscript));
  }

  //------

  ValueArray varray;

  values_.slice(subscripts1, varray);

  // TODO: necessary ?
  varray = varray.flattened();

  ClParserArrayPtr array = createArray(type_, varray);

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
getSubscriptValue(const IntVectorT &subscripts) const
{
  UIntVectorT subscripts1;

  uint num_subscripts = uint(subscripts.size());

  for (uint i = 0; i < num_subscripts; ++i) {
    int subscript = subscripts[i];

    if (! indexToData(&subscript))
      return ClParserValuePtr();

    subscripts1.push_back(uint(subscript));
  }

  //------

  ValueArray varray;

  values_.slice(subscripts1, varray);

  // TODO: necessary ?
  varray = varray.flattened();

  ClParserArrayPtr array = createArray(type_, varray);

  return ClParserValueMgrInst->createValue(array);
}

bool
ClParserArray::
setSubscriptValue(const int *subscripts, uint num_subscripts, ClParserValuePtr value)
{
  UIntVectorT subscripts1;

  for (uint i = 0; i < num_subscripts; ++i) {
    int subscript = subscripts[i];

    if (! indexToData(&subscript))
      return false;

    subscripts1.push_back(uint(subscript));
  }

  if (num_subscripts == getNumDims()) {
    // ensure right type
    if (value->getType() != type_)
      return false;

    values_.setValue(subscripts1, value);
  }
  else {
    // must be an array
    if (! value->isType(CL_PARSER_VALUE_TYPE_ARRAY))
      return false;

    ClParserArrayPtr array = value->getArray();

    // check dimensions
    if (array->getNumDims() != getNumDims() - num_subscripts)
      return false;

    for (uint i = 0; i < array->getNumDims(); ++i)
      if (getDim(i + num_subscripts) != getDim(i))
        return false;

    //------

    uint ind1, ind2;

    if (! values_.subscriptToIndex(subscripts1, &ind1))
      return false;

    ++subscripts1[0];

    if (! values_.subscriptToIndex(subscripts1, &ind2))
      return false;

    for (uint i = ind1, j = 0; i <= ind2; ++i, ++j)
      setValue(int(i), array->getValue(int(j)));
  }

  return true;
}

bool
ClParserArray::
setSubscriptValue(const IntVectorT &subscripts, ClParserValuePtr value)
{
  UIntVectorT subscripts1;

  uint num_subscripts = uint(subscripts.size());

  for (uint i = 0; i < num_subscripts; ++i) {
    int subscript = subscripts[i];

    if (! indexToData(&subscript))
      return false;

    subscripts1.push_back(uint(subscript));
  }

  if (num_subscripts == getNumDims()) {
    // ensure right type
    if (value->getType() != type_)
      return false;

    values_.setValue(subscripts1, value);
  }
  else {
    // must be an array
    if (! value->isType(CL_PARSER_VALUE_TYPE_ARRAY))
      return false;

    ClParserArrayPtr array = value->getArray();

    // check dimensions
    if (array->getNumDims() != getNumDims() - num_subscripts)
      return false;

    for (uint i = 0; i < array->getNumDims(); ++i)
      if (getDim(i + num_subscripts) != getDim(i))
        return false;

    //------

    uint ind1, ind2;

    if (! values_.subscriptToIndex(subscripts1, &ind1))
      return false;

    ++subscripts1[0];

    if (! values_.subscriptToIndex(subscripts1, &ind2))
      return false;

    for (uint i = ind1, j = 0; i <= ind2; ++i, ++j)
      setValue(int(i), array->getValue(int(j)));
  }

  return true;
}

ClParserArrayPtr
ClParserArray::
getSubArray(int ind) const
{
  error_code_ = 0;

  if (getNumDims() < 2) {
    error_code_ = int(ClErr::INVALID_TYPE_FOR_SUBSCRIPT);
    return ClParserArrayPtr();
  }

  if (! indexToData(&ind, 0)) {
    error_code_ = int(ClErr::SUBSCRIPT_OUT_OF_RANGE);
    return ClParserArrayPtr();
  }

  ValueArray varray;

  values_.slice(uint(ind), varray);

  // TODO: necessary ?
  varray = varray.flattened();

  ClParserArrayPtr array = createArray(type_, varray);

  return array;
}

ClParserArrayPtr
ClParserArray::
getSubArray(int start, int end) const
{
  if (! indexToData(&start, 0) || ! indexToData(&end, 0) || start > end) {
    error_code_ = int(ClErr::SUBSCRIPT_OUT_OF_RANGE);
    return ClParserArrayPtr();
  }

  ValueArray varray;

  values_.slice(uint(start), uint(end), varray);

  // TODO: necessary ?
  varray = varray.flattened();

  ClParserArrayPtr array = createArray(type_, varray);

  return array;
}

ClParserValuePtr
ClParserArray::
getSubValue(int ind) const
{
  if (getNumDims() != 1) {
    error_code_ = int(ClErr::INVALID_TYPE_FOR_SUBSCRIPT);
    return ClParserValuePtr();
  }

  if (! indexToData(&ind, 0)) {
    error_code_ = int(ClErr::SUBSCRIPT_OUT_OF_RANGE);
    return ClParserValuePtr();
  }

  ClParserValuePtr value;

  values_.getLinearValue(uint(ind), value);

  return value;
}

bool
ClParserArray::
setValue(int i, ClParserValuePtr value)
{
  if (value->getType() != type_)
    return false;

  if (! indexToData(&i))
    return false;

  values_.setLinearValue(uint(i), value);

  return true;
}

int
ClParserArray::
cmp(const ClParserObj &obj) const
{
  if (getBaseType() != obj.getBaseType())
    return CMathGen::sign(long(getBaseType() - obj.getBaseType()));

  const ClParserArray &rhs = castObj(obj);

  if (type_ != rhs.type_)
    return (type_ - rhs.type_);

  if (getNumData() != rhs.getNumData())
    return int(getNumData() - rhs.getNumData());

  if (getNumDims() != rhs.getNumDims())
    return int(getNumDims() - rhs.getNumDims());

  uint num_dims1 = getNumDims();

  for (uint i = 0; i < num_dims1; ++i)
    if (getDim(i) != rhs.getDim(i))
      return int(getDim(i) - rhs.getDim(i));

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value1, value2;

        values_.getLinearValue(i, value1);
    rhs.values_.getLinearValue(i, value2);

    long cmp = value1->cmp(value2);

    if (cmp != 0)
      return int(cmp);
  }

  return 0;
}

ClParserArrayPtr
ClParserArray::
concat(const ClParserArray &array) const
{
  if (type_ != array.type_)
    return ClParserArrayPtr();

  uint num_dims1 =       getNumDims();
  uint num_dims2 = array.getNumDims();

  if (num_dims1 != num_dims2)
    return ClParserArrayPtr();

  UIntVectorT dims;

  if (num_dims1 > 0) {
    dims.push_back(getDim(0) + array.getDim(0));

    for (uint i = 1; i < num_dims1; ++i) {
      if (getDim(i) != array.getDim(i))
         return ClParserArrayPtr();

      dims.push_back(getDim(i));
    }
  }

  ValueArray values(dims);

  uint j = 0;

  for (uint i = 0; i < getNumData(); ++i, ++j) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);
    values .setLinearValue(j, value);
  }

  for (uint i = 0; i < array.getNumData(); ++i, ++j) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);
    values .setLinearValue(j, value);
  }

  ClParserArrayPtr carray = createArray(type_, values);

  return carray;
}

bool
ClParserArray::
sameDimension(const ClParserArray &array) const
{
  return values_.sameDimension(array.values_);
}

bool
ClParserArray::
expandTo(const ClParserArray &array)
{
  error_code_ = 0;

  if (getNumDims() >= array.getNumDims())
    return false;

  uint num_dims1 =       getNumDims();
  uint num_dims2 = array.getNumDims();

  for (uint i = 0; i < num_dims1; ++i) {
    if (getDim(num_dims1 - i - 1) != array.getDim(num_dims2 - i - 1)) {
      error_code_ = int(ClErr::INVALID_TYPE_MIX);
      return false;
    }
  }

  ValueArray values(array.getDims(), ClParserValuePtr());

  uint num_data = getNumData();

  for (uint i = 0; i < num_data; ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);
    values .setLinearValue(i, value);
  }

  values_ = values;

  return true;
}

bool
ClParserArray::
dimensionTo(const ClParserArray &array)
{
  error_code_ = 0;

  uint num_dims1 = getNumDims();
  uint num_dims2 = array.getNumDims();

  UIntVectorT dims = getDims();

  for (uint i = 0; i < num_dims1; ++i) {
    if (i >= num_dims2)
      break;

    if (getDim(num_dims1 - i - 1) >= array.getDim(num_dims2 - i - 1))
      continue;

    dims[num_dims1 - i - 1] = array.getDim(num_dims2 - i - 1);
  }

  ValueArray values(dims, ClParserValuePtr());

  uint num_data = getNumData();

  for (uint i = 0; i < num_data; ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);
    values .setLinearValue(i, value);
  }

  values_ = values;

  return true;
}

bool
ClParserArray::
setIndexArray()
{
  uint num_data = getNumData();

  if      (type_ == CL_PARSER_VALUE_TYPE_INTEGER) {
    for (uint i = 0; i < num_data; ++i)
      values_.setLinearValue(i, ClParserValueMgrInst->createValue(long(i + 1)));
  }
  else if (type_ == CL_PARSER_VALUE_TYPE_REAL) {
    for (uint i = 0; i < num_data; ++i)
      values_.setLinearValue(i, ClParserValueMgrInst->createValue(double(i + 1)));
  }
  else if (type_ == CL_PARSER_VALUE_TYPE_STRING) {
    char temp_string[32];

    for (uint i = 0; i < num_data; ++i) {
      sprintf(temp_string, "%d", i + 1);

      uint len = uint(strlen(temp_string));

      values_.setLinearValue(i, ClParserValueMgrInst->createValue(temp_string, len));
    }
  }
  else
    return false;

  return true;
}

//-----------------

// Inline Ops

const ClParserObj &
ClParserArray::
increment()
{
  uint num_data = getNumData();

  for (uint i = 0; i < num_data; ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    value->increment();

    values_.setLinearValue(i, value);
  }

  return *this;
}

const ClParserObj &
ClParserArray::
decrement()
{
  uint num_data = getNumData();

  for (uint i = 0; i < num_data; ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    value->decrement();

    values_.setLinearValue(i, value);
  }

  return *this;
}

//-----------------

// Unary Ops

ClParserValuePtr
ClParserArray::
unaryPlus() const
{
  uint num_data = getNumData();

  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < num_data; ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->unaryPlus());
  }

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
unaryMinus() const
{
  uint num_data = getNumData();

  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < num_data; ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->unaryMinus());
  }

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
bitNot() const
{
  uint num_data = getNumData();

  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < num_data; ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->bitNot());
  }

  return ClParserValueMgrInst->createValue(array);
}

//------

// Binary Ops

// <array> + <obj>
ClParserValuePtr
ClParserArray::
plus(const ClParserObj &obj) const
{
  ClParserArrayPtr array1 = this->dupArray();

  uint n = getNumData();

  if (obj.getBaseType() == CL_PARSER_VALUE_TYPE_ARRAY) {
    const ClParserArray &array = castObj(obj);

    for (uint i = 0; i < n; ++i) {
      ClParserValuePtr value1, value2;

            values_.getLinearValue(i, value1);
      array.values_.getLinearValue(i, value2);

      array1->values_.setLinearValue(i, value1->plus(value2));
    }
  }
  else {
    bool isReal = (getType() == CL_PARSER_VALUE_TYPE_REAL ||
                   obj.getBaseType() == CL_PARSER_VALUE_TYPE_REAL);

    ClParserValuePtr value2 = ClParserValueMgrInst->createValue(obj);

    for (uint i = 0; i < n; ++i) {
      ClParserValuePtr value1;

      values_.getLinearValue(i, value1);

      if (isReal) {
        value1->convertToReal();
        value2->convertToReal();
      }

      array1->values_.setLinearValue(i, value1->plus(value2));
    }
  }

  return ClParserValueMgrInst->createValue(array1);
}

// <array> - <obj>
ClParserValuePtr
ClParserArray::
minus(const ClParserObj &obj) const
{
  ClParserArrayPtr array1 = this->dupArray();

  uint n = getNumData();

  if (obj.getBaseType() == CL_PARSER_VALUE_TYPE_ARRAY) {
    const ClParserArray &array = castObj(obj);

    for (uint i = 0; i < n; ++i) {
      ClParserValuePtr value1, value2;

            values_.getLinearValue(i, value1);
      array.values_.getLinearValue(i, value2);

      array1->values_.setLinearValue(i, value1->minus(value2));
    }
  }
  else {
    bool isReal = (getType() == CL_PARSER_VALUE_TYPE_REAL ||
                   obj.getBaseType() == CL_PARSER_VALUE_TYPE_REAL);

    ClParserValuePtr value2 = ClParserValueMgrInst->createValue(obj);

    for (uint i = 0; i < n; ++i) {
      ClParserValuePtr value1;

      values_.getLinearValue(i, value1);

      if (isReal) {
        value1->convertToReal();
        value2->convertToReal();
      }

      array1->values_.setLinearValue(i, value1->minus(value2));
    }
  }

  return ClParserValueMgrInst->createValue(array1);
}

// <array> * <obj>
ClParserValuePtr
ClParserArray::
times(const ClParserObj &obj) const
{
  ClParserArrayPtr array1 = this->dupArray();

  uint n = getNumData();

  if (obj.getBaseType() == CL_PARSER_VALUE_TYPE_ARRAY) {
    const ClParserArray &array = castObj(obj);

    for (uint i = 0; i < n; ++i) {
      ClParserValuePtr value1, value2;

            values_.getLinearValue(i, value1);
      array.values_.getLinearValue(i, value2);

      array1->values_.setLinearValue(i, value1->times(value2));
    }
  }
  else {
    bool isReal = (getType() == CL_PARSER_VALUE_TYPE_REAL ||
                   obj.getBaseType() == CL_PARSER_VALUE_TYPE_REAL);

    ClParserValuePtr value2 = ClParserValueMgrInst->createValue(obj);

    for (uint i = 0; i < n; ++i) {
      ClParserValuePtr value1;

      values_.getLinearValue(i, value1);

      if (isReal) {
        value1->convertToReal();
        value2->convertToReal();
      }

      array1->values_.setLinearValue(i, value1->times(value2));
    }
  }

  return ClParserValueMgrInst->createValue(array1);
}

// <array> / <obj>
ClParserValuePtr
ClParserArray::
divide(const ClParserObj &obj) const
{
  ClParserArrayPtr array1 = this->dupArray();

  uint n = getNumData();

  if (obj.getBaseType() == CL_PARSER_VALUE_TYPE_ARRAY) {
    const ClParserArray &array = castObj(obj);

    for (uint i = 0; i < n; ++i) {
      ClParserValuePtr value1, value2;

            values_.getLinearValue(i, value1);
      array.values_.getLinearValue(i, value2);

      array1->values_.setLinearValue(i, value1->divide(value2));
    }
  }
  else {
    bool isReal = (getType() == CL_PARSER_VALUE_TYPE_REAL ||
                   obj.getBaseType() == CL_PARSER_VALUE_TYPE_REAL);

    ClParserValuePtr value2 = ClParserValueMgrInst->createValue(obj);

    for (uint i = 0; i < n; ++i) {
      ClParserValuePtr value1;

      values_.getLinearValue(i, value1);

      if (isReal) {
        value1->convertToReal();
        value2->convertToReal();
      }

      array1->values_.setLinearValue(i, value1->divide(value2));
    }
  }

  return ClParserValueMgrInst->createValue(array1);
}

// <array> % <obj>
ClParserValuePtr
ClParserArray::
modulus(const ClParserObj &obj) const
{
  ClParserArrayPtr array1 = this->dupArray();

  uint n = getNumData();

  if (obj.getBaseType() == CL_PARSER_VALUE_TYPE_ARRAY) {
    const ClParserArray &array = castObj(obj);

    for (uint i = 0; i < n; ++i) {
      ClParserValuePtr value1, value2;

            values_.getLinearValue(i, value1);
      array.values_.getLinearValue(i, value2);

      array1->values_.setLinearValue(i, value1->modulus(value2));
    }
  }
  else {
    bool isReal = (getType() == CL_PARSER_VALUE_TYPE_REAL ||
                   obj.getBaseType() == CL_PARSER_VALUE_TYPE_REAL);

    ClParserValuePtr value2 = ClParserValueMgrInst->createValue(obj);

    for (uint i = 0; i < n; ++i) {
      ClParserValuePtr value1;

      values_.getLinearValue(i, value1);

      if (isReal) {
        value1->convertToReal();
        value2->convertToReal();
      }


      array1->values_.setLinearValue(i, value1->modulus(value2));
    }
  }

  return ClParserValueMgrInst->createValue(array1);
}

// <array> ** <obj>
ClParserValuePtr
ClParserArray::
power(const ClParserObj &obj) const
{
  ClParserArrayPtr array1 = this->dupArray();

  uint n = getNumData();

  if (obj.getBaseType() == CL_PARSER_VALUE_TYPE_ARRAY) {
    const ClParserArray &array = castObj(obj);

    for (uint i = 0; i < n; ++i) {
      ClParserValuePtr value1, value2;

            values_.getLinearValue(i, value1);
      array.values_.getLinearValue(i, value2);

      array1->values_.setLinearValue(i, value1->power(value2));
    }
  }
  else {
    bool isReal = (getType() == CL_PARSER_VALUE_TYPE_REAL ||
                   obj.getBaseType() == CL_PARSER_VALUE_TYPE_REAL);

    ClParserValuePtr value2 = ClParserValueMgrInst->createValue(obj);

    for (uint i = 0; i < n; ++i) {
      ClParserValuePtr value1;

      values_.getLinearValue(i, value1);

      if (isReal) {
        value1->convertToReal();
        value2->convertToReal();
      }

      array1->values_.setLinearValue(i, value1->power(value2));
    }
  }

  return ClParserValueMgrInst->createValue(array1);
}

// <array> =~ <obj>
ClParserValuePtr
ClParserArray::
approxEqual(const ClParserObj &obj) const
{
#if 0
  const ClParserArray &array = castObj(obj);

  ClParserArrayPtr array1 = array.dupArray();

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value1, value2;

          values_.getLinearValue(i, value1);
    array.values_.getLinearValue(i, value2);

    array1->values_.setLinearValue(i, value1->approxEqual(value2));
  }

  return ClParserValueMgrInst->createValue(array1);
#else
  if (getBaseType() != obj.getBaseType())
    return ClParserValueMgrInst->createValue(0L);

  const ClParserArray &rhs = castObj(obj);

  if (type_ != rhs.type_)
    return ClParserValueMgrInst->createValue(0L);

  if (getNumData() != rhs.getNumData())
    return ClParserValueMgrInst->createValue(0L);

  if (getNumDims() != rhs.getNumDims())
    return ClParserValueMgrInst->createValue(0L);

  uint num_dims1 = getNumDims();

  for (uint i = 0; i < num_dims1; ++i)
    if (getDim(i) != rhs.getDim(i))
      return ClParserValueMgrInst->createValue(0L);

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value1, value2;

        values_.getLinearValue(i, value1);
    rhs.values_.getLinearValue(i, value2);

    ClParserValuePtr rval = value1->approxEqual(value2);

    long rc;

    if (! rval->integerValue(&rc) || rc == 0)
      return ClParserValueMgrInst->createValue(0L);
  }

  return ClParserValueMgrInst->createValue(1L);
#endif
}

// <array> & <obj>
ClParserValuePtr
ClParserArray::
bitAnd(const ClParserObj &obj) const
{
  ClParserArrayPtr array1 = this->dupArray();

  uint n = getNumData();

  if (obj.getBaseType() == CL_PARSER_VALUE_TYPE_ARRAY) {
    const ClParserArray &array = castObj(obj);

    for (uint i = 0; i < n; ++i) {
      ClParserValuePtr value1, value2;

            values_.getLinearValue(i, value1);
      array.values_.getLinearValue(i, value2);

      array1->values_.setLinearValue(i, value1->bitAnd(value2));
    }
  }
  else {
    bool isInteger = (getType() == CL_PARSER_VALUE_TYPE_INTEGER ||
                      obj.getBaseType() == CL_PARSER_VALUE_TYPE_INTEGER);

    ClParserValuePtr value2 = ClParserValueMgrInst->createValue(obj);

    for (uint i = 0; i < n; ++i) {
      ClParserValuePtr value1;

      values_.getLinearValue(i, value1);

      if (! isInteger) {
        value1->convertToInteger();
        value2->convertToInteger();
      }

      array1->values_.setLinearValue(i, value1->bitAnd(value2));
    }
  }

  return ClParserValueMgrInst->createValue(array1);
}

// <array> | <obj>
ClParserValuePtr
ClParserArray::
bitOr(const ClParserObj &obj) const
{
  ClParserArrayPtr array1 = this->dupArray();

  uint n = getNumData();

  if (obj.getBaseType() == CL_PARSER_VALUE_TYPE_ARRAY) {
    const ClParserArray &array = castObj(obj);

    for (uint i = 0; i < n; ++i) {
      ClParserValuePtr value1, value2;

            values_.getLinearValue(i, value1);
      array.values_.getLinearValue(i, value2);

      array1->values_.setLinearValue(i, value1->bitOr(value2));
    }
  }
  else {
    bool isInteger = (getType() == CL_PARSER_VALUE_TYPE_INTEGER ||
                      obj.getBaseType() == CL_PARSER_VALUE_TYPE_INTEGER);

    ClParserValuePtr value2 = ClParserValueMgrInst->createValue(obj);

    for (uint i = 0; i < n; ++i) {
      ClParserValuePtr value1;

      values_.getLinearValue(i, value1);

      if (! isInteger) {
        value1->convertToInteger();
        value2->convertToInteger();
      }

      array1->values_.setLinearValue(i, value1->bitOr(value2));
    }
  }

  return ClParserValueMgrInst->createValue(array1);
}

// <array> ^ <obj>
ClParserValuePtr
ClParserArray::
bitXor(const ClParserObj &obj) const
{
  ClParserArrayPtr array1 = this->dupArray();

  uint n = getNumData();

  if (obj.getBaseType() == CL_PARSER_VALUE_TYPE_ARRAY) {
    const ClParserArray &array = castObj(obj);

    for (uint i = 0; i < n; ++i) {
      ClParserValuePtr value1, value2;

            values_.getLinearValue(i, value1);
      array.values_.getLinearValue(i, value2);

      array1->values_.setLinearValue(i, value1->bitXor(value2));
    }
  }
  else {
    bool isInteger = (getType() == CL_PARSER_VALUE_TYPE_INTEGER ||
                      obj.getBaseType() == CL_PARSER_VALUE_TYPE_INTEGER);

    ClParserValuePtr value2 = ClParserValueMgrInst->createValue(obj);

    for (uint i = 0; i < n; ++i) {
      ClParserValuePtr value1;

      values_.getLinearValue(i, value1);

      if (! isInteger) {
        value1->convertToInteger();
        value2->convertToInteger();
      }

      array1->values_.setLinearValue(i, value1->bitXor(value2));
    }
  }

  return ClParserValueMgrInst->createValue(array1);
}

// <array> << <obj>
ClParserValuePtr
ClParserArray::
bitLShift(const ClParserObj &obj) const
{
  ClParserArrayPtr array1 = this->dupArray();

  uint n = getNumData();

  if (obj.getBaseType() == CL_PARSER_VALUE_TYPE_ARRAY) {
    const ClParserArray &array = castObj(obj);

    for (uint i = 0; i < n; ++i) {
      ClParserValuePtr value1, value2;

            values_.getLinearValue(i, value1);
      array.values_.getLinearValue(i, value2);

      array1->values_.setLinearValue(i, value1->bitLShift(value2));
    }
  }
  else {
    bool isInteger = (getType() == CL_PARSER_VALUE_TYPE_INTEGER ||
                      obj.getBaseType() == CL_PARSER_VALUE_TYPE_INTEGER);

    ClParserValuePtr value2 = ClParserValueMgrInst->createValue(obj);

    for (uint i = 0; i < n; ++i) {
      ClParserValuePtr value1;

      values_.getLinearValue(i, value1);

      if (! isInteger) {
        value1->convertToInteger();
        value2->convertToInteger();
      }

      array1->values_.setLinearValue(i, value1->bitLShift(value2));
    }
  }

  return ClParserValueMgrInst->createValue(array1);
}

// <array> >> <obj>
ClParserValuePtr
ClParserArray::
bitRShift(const ClParserObj &obj) const
{
  ClParserArrayPtr array1 = this->dupArray();

  uint n = getNumData();

  if (obj.getBaseType() == CL_PARSER_VALUE_TYPE_ARRAY) {
    const ClParserArray &array = castObj(obj);

    for (uint i = 0; i < n; ++i) {
      ClParserValuePtr value1, value2;

            values_.getLinearValue(i, value1);
      array.values_.getLinearValue(i, value2);

      array1->values_.setLinearValue(i, value1->bitRShift(value2));
    }
  }
  else {
    bool isInteger = (getType() == CL_PARSER_VALUE_TYPE_INTEGER ||
                      obj.getBaseType() == CL_PARSER_VALUE_TYPE_INTEGER);

    ClParserValuePtr value2 = ClParserValueMgrInst->createValue(obj);

    for (uint i = 0; i < n; ++i) {
      ClParserValuePtr value1;

      values_.getLinearValue(i, value1);

      if (! isInteger) {
        value1->convertToInteger();
        value2->convertToInteger();
      }

      array1->values_.setLinearValue(i, value1->bitRShift(value2));
    }
  }

  return ClParserValueMgrInst->createValue(array1);
}

//------

// Internal Fns

ClParserValuePtr
ClParserArray::
abs() const
{
  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->abs());
  }

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
ceil() const
{
  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->ceil());
  }

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
floor() const
{
  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->floor());
  }

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
sign() const
{
  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->sign());
  }

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
sqr() const
{
  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->sqr());
  }

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
sqrt() const
{
  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->sqrt());
  }

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
cos() const
{
  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->cos());
  }

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
sin() const
{
  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->sin());
  }

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
tan() const
{
  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->tan());
  }

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
acos() const
{
  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->acos());
  }

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
asin() const
{
  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->asin());
  }

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
atan() const
{
  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->atan());
  }

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
atan(double real) const
{
  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->atan(real));
  }

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
exp() const
{
  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->exp());
  }

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
log() const
{
  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->log());
  }

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
log10() const
{
  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->log10());
  }

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
cosh() const
{
  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->cosh());
  }

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
sinh() const
{
  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->sinh());
  }

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
tanh() const
{
  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->tanh());
  }

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
norm() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserArray::
invnorm() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserArray::
toChar() const
{
  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->toChar());
  }

  //array->type_ = CL_PARSER_VALUE_TYPE_STRING;

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
toInt() const
{
  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->toInt());
  }

  //array->type_ = CL_PARSER_VALUE_TYPE_INTEGER;

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
toReal() const
{
  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->toReal());
  }

  //array->type_ = CL_PARSER_VALUE_TYPE_REAL;

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
toString() const
{
  return ClParserValueMgrInst->createValue(asString());
}

ClParserValuePtr
ClParserArray::
isNan() const
{
  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->isNan());
  }

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
toLower() const
{
  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->toChar()->toLower());
  }

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
toUpper() const
{
  ClParserArrayPtr array = dupArray();

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    array->values_.setLinearValue(i, value->toChar()->toUpper());
  }

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
min() const
{
  ClParserValuePtr result;

  bool set = false;

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    if (! set || value->cmp(result) < 0) {
      result = value;
      set    = true;
    }
  }

  return result;
}

ClParserValuePtr
ClParserArray::
max() const
{
  ClParserValuePtr result;

  bool set = false;

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    if (! set || value->cmp(result) > 0) {
      result = value;
      set    = true;
    }
  }

  return result;
}

ClParserValuePtr
ClParserArray::
sum() const
{
  ClParserValuePtr result;

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value;

    values_.getLinearValue(i, value);

    if (i == 0)
      result = value;
    else
      result = result->plus(value);
  }

  return result;
}

ClParserValuePtr
ClParserArray::
dim() const
{
  if (getNumDims() > 1) {
    uint num_dims = getNumDims();

    ClParserArrayPtr array = createArray(&num_dims, 1, long(0));

    for (uint i = 0; i < num_dims; ++i)
      array->values_.setLinearValue(i, ClParserValueMgrInst->createValue(long(getDim(i))));

    return ClParserValueMgrInst->createValue(array);
  }
  else {
    long integer = 0;

    if (getNumDims() > 0)
      integer = getDim(0);

    return ClParserValueMgrInst->createValue(integer);
  }
}

ClParserValuePtr
ClParserArray::
ndim() const
{
  long integer = getNumDims();

  return ClParserValueMgrInst->createValue(integer);
}

ClParserValuePtr
ClParserArray::
len() const
{
  return ClParserValueMgrInst->createValue(long(getNumData()));
}

ClParserValuePtr
ClParserArray::
index(const ClParserObj &obj) const
{
  if (obj.getBaseType() != type_)
    ClErrThrow(ClErr::INVALID_TYPE_MIX);

  ClParserValuePtr value = ClParserValueMgrInst->createValue(obj);

  long integer = 0;

  for (uint i = 0; i < getNumData(); ++i) {
    ClParserValuePtr value1;

    values_.getLinearValue(i, value1);

    if (value->cmp(value1) == 0)
      integer = i + 1;
  }

  return ClParserValueMgrInst->createValue(integer);
}

ClParserValuePtr
ClParserArray::
rindex(const ClParserObj &obj) const
{
  if (obj.getBaseType() != type_)
    ClErrThrow(ClErr::INVALID_TYPE_MIX);

  ClParserValuePtr value = ClParserValueMgrInst->createValue(obj);

  long integer = 0;

  for (int i = int(getNumData() - 1); i >= 0; ++i) {
    ClParserValuePtr value1;

    values_.getLinearValue(uint(i), value1);

    if (value->cmp(value1) == 0)
      integer = i + 1;
  }

  return ClParserValueMgrInst->createValue(integer);
}

ClParserValuePtr
ClParserArray::
sort(ClParserSortDirection direction) const
{
  ClParserArrayPtr array = dupArray();

  uint num_data = array->getNumData();

  std::list<ClParserValuePtr> values;

  for (uint i = 0; i < num_data; ++i) {
    ClParserValuePtr value;

    array->values_.getLinearValue(i, value);

    values.push_back(value);
  }

  values.sort(ClParserValueCmp(direction));

  std::list<ClParserValuePtr>::const_iterator p1 = values.begin();
  std::list<ClParserValuePtr>::const_iterator p2 = values.end  ();

  for (uint i = 0; p1 != p2; ++p1, ++i)
    array->values_.setLinearValue(i, *p1);

  return ClParserValueMgrInst->createValue(array);
}

ClParserValuePtr
ClParserArray::
doAssert() const
{
  assert(false);

  return ClParserValuePtr();
}

//-----------------

ClParserValuePtr
ClParserArray::
internFn(ClParserInternFnPtr internfn, const ClParserValuePtr *values, uint num_values)
{
  error_code_ = 0;

  ClParserArrayPtr array1 = values[0]->getArray();

  switch (internfn->getType()) {
    case CLParserInternFnType::IS_CTYPE: {
      if (array1->type_ == CL_PARSER_VALUE_TYPE_INTEGER) {
        std::string is_type;

        if (! values[1]->stringValue(is_type))
          return ClParserValuePtr();

        CStrUtil::IsCType is_func = CStrUtil::getIsCType(is_type);

        if (! is_func) {
          error_code_ = int(ClErr::INVALID_TYPE_FOR_OPERATOR);
          return ClParserValuePtr();
        }

        ClParserArrayPtr array = createArray(CL_PARSER_VALUE_TYPE_INTEGER, &num_values, 1);

        for (uint i = 0; i < array->getNumData(); ++i) {
          ClParserValuePtr value;

          array1->values_.getLinearValue(i, value);

          long flag = (*is_func)(int(value->getInteger()->getValue()));

          array->values_.setLinearValue(i, ClParserValueMgrInst->createValue(flag));
        }

        return ClParserValueMgrInst->createValue(array);
      }
      else {
        error_code_ = int(ClErr::INVALID_TYPE_FOR_OPERATOR);
        return ClParserValuePtr();
      }
    }
    case CLParserInternFnType::SUBARR: {
      if (array1->getNumDims() > 1) {
        error_code_ = int(ClErr::INVALID_TYPE_FOR_OPERATOR);
        return ClParserValuePtr();
      }

      long integer1;

      if (! values[1]->integerValue(&integer1))
        return ClParserValuePtr();

      long integer2;

      if (! values[2]->integerValue(&integer2))
        return ClParserValuePtr();

      auto array = array1->getSubArray(int(integer1), int(integer2));

      if (! array.isValid())
        return ClParserValuePtr();

      return ClParserValueMgrInst->createValue(array);
    }
    case CLParserInternFnType::SUM: {
      ClParserValuePtr value;

      for (uint i = 0; i < array1->getNumData(); ++i) {
        ClParserValuePtr value1;

        array1->values_.getLinearValue(i, value1);

        if (i == 0)
          value = value1;
        else
          value = value->plus(value1);
      }

      return value;
    }
    case CLParserInternFnType::TOLOWER: {
      ClParserArrayPtr array = createArray(CL_PARSER_VALUE_TYPE_STRING, &num_values, 1);

      for (uint i = 0; i < array->getNumData(); ++i) {
        ClParserValuePtr value;

        array->values_.getLinearValue(i, value);

        std::string str = value->toString()->getString()->getText();

        str = CStrUtil::toLower(str);

        array->values_.setLinearValue(i, ClParserValueMgrInst->createValue(str));
      }

      return ClParserValueMgrInst->createValue(array);
    }
    case CLParserInternFnType::TOUPPER: {
      ClParserArrayPtr array = createArray(CL_PARSER_VALUE_TYPE_STRING, &num_values, 1);

      for (uint i = 0; i < array->getNumData(); ++i) {
        ClParserValuePtr value;

        array->values_.getLinearValue(i, value);

        std::string str = value->toString()->getString()->getText();

        str = CStrUtil::toUpper(str);

        array->values_.setLinearValue(i, ClParserValueMgrInst->createValue(str));
      }

      return ClParserValueMgrInst->createValue(array);
    }
    default:
      break;
  }

  return ClParserValuePtr();
}

ClParserArrayPtr
ClParserArray::
arrayTimes(const ClParserArray &array)
{
  error_code_ = 0;

  if (type_ != array.type_ || ! values_.sameDimension(array.values_)) {
    error_code_ = int(ClErr::INCOMPATIBLE_ARRAYS);
    return ClParserArrayPtr();
  }

  if (getNumDims() != 1) {
    error_code_ = int(ClErr::INCOMPATIBLE_ARRAYS);
    return ClParserArrayPtr();
  }

  UIntVectorT dims;

  dims.push_back(getNumData());
  dims.push_back(array.getNumData());

  ClParserArrayPtr tarray = createArray(type_, dims);

  for (uint i = 0, k = 0; i < getNumData(); ++i) {
    for (uint j = 0; j < array.getNumData(); ++j, ++k) {
      ClParserValuePtr value1, value2;

            values_.getLinearValue(i, value1);
      array.values_.getLinearValue(j, value2);

      tarray->values_.setLinearValue(k, value1->times(value2));
    }
  }

  return tarray;
}

//------

std::string
ClParserArray::
asString() const
{
  std::string str = "[";

  if       (getNumDims() == 1) {
    uint n = getDim(0);

    for (uint i = 0; i < n; ++i) {
      if (i > 0) str += ",";

      ClParserValuePtr value;

      values_.getLinearValue(i, value);

      str += value->asString();
    }
  }
  else if (getNumDims() == 2) {
    uint n1 = getDim(0);
    uint n2 = getDim(1);

    uint k = 0;

    for (uint i = 0; i < n1; ++i) {
      if (i > 0) str += ",";

      str += "[";

      for (uint j = 0; j < n2; ++j, ++k) {
        if (j > 0) str += ",";

        ClParserValuePtr value;

        values_.getLinearValue(k, value);

        str += value->asString();
      }

      str += "]";
    }
  }
  else if (getNumDims() == 3) {
    uint n1 = getDim(0);
    uint n2 = getDim(1);
    uint n3 = getDim(2);

    uint l = 0;

    for (uint i = 0; i < n1; ++i) {
      if (i > 0) str += ",";

      str += "[";

      for (uint j = 0; j < n2; ++j) {
        if (j > 0) str += ",";

        str += "[";

        for (uint k = 0; k < n3; ++k, ++l) {
          if (k > 0) str += ",";

          ClParserValuePtr value;

          values_.getLinearValue(l, value);

          str += value->asString();
        }

        str += "]";
      }

      str += "]";
    }
  }
  else {
    uint n = getNumData();

    for (uint i = 0; i < n; ++i) {
      if (i > 0) str += ",";

      ClParserValuePtr value;

      values_.getLinearValue(i, value);

      str += value->asString();
    }
  }

  str += "]";

  return str;
}

void
ClParserArray::
print() const
{
  ClParserInst->output("%s", asString().c_str());
}

void
ClParserArray::
print(std::ostream &os) const
{
  os << asString();
}

void
ClParserArray::
debugPrint() const
{
  fprintf(stderr, "%s", asString().c_str());
}

//------

bool
ClParserArray::
getSubscriptRange(int *i1, int *i2) const
{
  IntVectorT subscripts;

  if (! toIntegers(subscripts))
    return false;

  uint num_subscripts = uint(subscripts.size());

  if (num_subscripts > 0)
    *i1 = subscripts[0];
  else
    *i1 = 1;

  if (num_subscripts > 1)
    *i2 = subscripts[1];
  else
    *i2 = *i1;

  return true;
}

bool
ClParserArray::
indexToData(int *ind) const
{
  if (*ind < 0)
    *ind += getNumData() + 1;

  --(*ind);

  return (*ind >= 0 && *ind < int(getNumData()));
}

bool
ClParserArray::
indexToData(int *ind, uint dim) const
{
  if (*ind < 0)
    *ind += getDim(dim) + 1;

  --(*ind);

  return (*ind >= 0 && *ind < int(getDim(dim)));
}
