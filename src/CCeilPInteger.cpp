#include <CCeilPI.h>
#include <CInvNorm.h>
#include <boost/math/special_functions/erf.hpp>
#include <cerrno>

void
ClParserInteger::
copy(const ClParserObj &obj)
{
  assert(base_type_ == obj.getBaseType());

  const ClParserInteger &rhs = castObj(obj);

  *this = rhs;
}

ClParserInteger *
ClParserInteger::
dup() const
{
  return new ClParserInteger(*this);
}

bool
ClParserInteger::
toBool() const
{
  return (integer_ != 0);
}

//-----------------

// Inline Ops

const ClParserObj &
ClParserInteger::
increment()
{
  ++integer_;

  return *this;
}

const ClParserObj &
ClParserInteger::
decrement()
{
  --integer_;

  return *this;
}

//-----------------

// Unary Ops

ClParserValuePtr
ClParserInteger::
unaryPlus() const
{
  return ClParserValueMgrInst->createValue( integer_);
}

ClParserValuePtr
ClParserInteger::
unaryMinus() const
{
  return ClParserValueMgrInst->createValue(-integer_);
}

ClParserValuePtr
ClParserInteger::
bitNot() const
{
  return ClParserValueMgrInst->createValue(~integer_);
}

//-----------------

// Binary Ops

ClParserValuePtr
ClParserInteger::
plus(const ClParserObj &obj) const
{
  const ClParserInteger &integer = castObj(obj);

  long value = integer_ + integer.integer_;

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
minus(const ClParserObj &obj) const
{
  const ClParserInteger &integer = castObj(obj);

  long value = integer_ - integer.integer_;

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
times(const ClParserObj &obj) const
{
  const ClParserInteger &integer = castObj(obj);

  long value = integer_ * integer.integer_;

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
divide(const ClParserObj &obj) const
{
  const ClParserInteger &integer = castObj(obj);

  if (integer.integer_ == 0)
    ClErrThrow(ClErr::DIVIDE_BY_ZERO);

  long value = integer_ / integer.integer_;

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
modulus(const ClParserObj &obj) const
{
  const ClParserInteger &integer = castObj(obj);

  if (integer.integer_ == 0)
    ClErrThrow(ClErr::DIVIDE_BY_ZERO);

  long value = integer_ % integer.integer_;

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
power(const ClParserObj &obj) const
{
  const ClParserInteger &integer = castObj(obj);

  double real = CMathGen::pow(integer_, integer.integer_);

  long ireal;

  if (isInteger(real, ireal))
    return ClParserValueMgrInst->createValue(ireal);
  else
    return ClParserValueMgrInst->createValue(real);
}

ClParserValuePtr
ClParserInteger::
approxEqual(const ClParserObj &obj) const
{
  const ClParserInteger &integer = castObj(obj);

  long value = (integer_ == integer.integer_);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
bitAnd(const ClParserObj &obj) const
{
  const ClParserInteger &integer = castObj(obj);

  long value = (integer_ & integer.integer_);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
bitOr(const ClParserObj &obj) const
{
  const ClParserInteger &integer = castObj(obj);

  long value = (integer_ | integer.integer_);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
bitXor(const ClParserObj &obj) const
{
  const ClParserInteger &integer = castObj(obj);

  long value = (integer_ ^ integer.integer_);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
bitLShift(const ClParserObj &obj) const
{
  const ClParserInteger &integer = castObj(obj);

  long value = (integer_ << integer.integer_);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
bitRShift(const ClParserObj &obj) const
{
  const ClParserInteger &integer = castObj(obj);

  long value = (integer_ >> integer.integer_);

  return ClParserValueMgrInst->createValue(value);
}

//-----------------

// Intern Fn

ClParserValuePtr
ClParserInteger::
abs() const
{
  long value = (integer_ < 0 ? -integer_ : integer_);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
ceil() const
{
  long value = integer_;

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
floor() const
{
  long value = integer_;

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
sign() const
{
  long value;

  if      (integer_ > 0)
    value =  1;
  else if (integer_ < 0)
    value = -1;
  else
    value =  0;

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
sqr() const
{
  double real = double(integer_)*double(integer_);

  long ireal;

  if (isInteger(real, ireal))
    return ClParserValueMgrInst->createValue(ireal);
  else
    return ClParserValueMgrInst->createValue(real);
}

ClParserValuePtr
ClParserInteger::
sqrt() const
{
  errno = 0;

  double real = CMathGen::sqrt(double(integer_));

  if (errno != 0 && ClParserInst->getMathFail())
    ClErrThrow(ClErr::INVALID_SQRT_VALUE);

  long ireal;

  if (isInteger(real, ireal))
    return ClParserValueMgrInst->createValue(ireal);
  else
    return ClParserValueMgrInst->createValue(real);
}

ClParserValuePtr
ClParserInteger::
cos() const
{
  double value = ::cos(ClParserInst->toRadians(double(integer_)));

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
sin() const
{
  double value = ::sin(ClParserInst->toRadians(double(integer_)));

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
tan() const
{
  double value = ::tan(ClParserInst->toRadians(double(integer_)));

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
acos() const
{
  errno = 0;

  double value = ClParserInst->fromRadians(CMathGen::acos(double(integer_)));

  if (errno != 0 && ClParserInst->getMathFail())
    ClErrThrow(ClErr::INVALID_ACOS_VALUE);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
asin() const
{
  errno = 0;

  double value = ClParserInst->fromRadians(CMathGen::asin(double(integer_)));

  if (errno != 0 && ClParserInst->getMathFail())
    ClErrThrow(ClErr::INVALID_ASIN_VALUE);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
atan() const
{
  double value = ClParserInst->fromRadians(::atan(double(integer_)));

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
atan(double real) const
{
  double value = ClParserInst->fromRadians(::atan2(double(integer_), real));

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
exp() const
{
  double value = ::exp(double(integer_));

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
log() const
{
  errno = 0;

  double value = CMathGen::log(double(integer_));

  if (errno != 0 && ClParserInst->getMathFail())
    ClErrThrow(ClErr::INVALID_LOG_VALUE);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
log10() const
{
  errno = 0;

  double value = CMathGen::log10(double(integer_));

  if (errno != 0 && ClParserInst->getMathFail())
    ClErrThrow(ClErr::INVALID_LOG_VALUE);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
cosh() const
{
  double value = ::cosh(ClParserInst->toRadians(double(integer_)));

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
sinh() const
{
  double value = ::sinh(ClParserInst->toRadians(double(integer_)));

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
tanh() const
{
  double value = ::tanh(ClParserInst->toRadians(double(integer_)));

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
norm() const
{
  double result = double(integer_);

  result = 0.5*std::sqrt(2.0)*result;
  result = 0.5*erfc(-result);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserInteger::
invnorm() const
{
  double result = CInvNorm::calc(double(integer_));

  if (IsPosInf(result) || IsNegInf(result))
    SetNaN(result);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserInteger::
toChar() const
{
  std::string text(" ");

  text[0] = char(integer_);

  return ClParserValueMgrInst->createValue(text);
}

ClParserValuePtr
ClParserInteger::
toInt() const
{
  long value = integer_;

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
toReal() const
{
  double value = double(integer_);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
toString() const
{
  std::string text = asString();

  return ClParserValueMgrInst->createValue(text);
}

ClParserValuePtr
ClParserInteger::
isNan() const
{
  long value = false;

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
toLower() const
{
  int c = int(integer_);

  if (isupper(c))
    c = tolower(c);

  long value = long(c);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
toUpper() const
{
  int c = int(integer_);

  if (islower(c))
    c = toupper(c);

  long value = long(c);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
min() const
{
  long value = integer_;

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
max() const
{
  long value = integer_;

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
sum() const
{
  long value = integer_;

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
index(const ClParserObj &) const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValueMgrInst->createValue(0L);
}

ClParserValuePtr
ClParserInteger::
rindex(const ClParserObj &) const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValueMgrInst->createValue(0L);
}

ClParserValuePtr
ClParserInteger::
sort(ClParserSortDirection) const
{
  long value = integer_;

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
doAssert() const
{
  assert(integer_);

  return ClParserValuePtr();
}

//-----------------

int
ClParserInteger::
cmp(const ClParserObj &obj) const
{
  if (base_type_ != obj.getBaseType())
    return CMathGen::sign(long(base_type_ - obj.getBaseType()));

  const ClParserInteger &rhs = castObj(obj);

  return cmp(rhs);
}

int
ClParserInteger::
cmp(const ClParserInteger &rhs) const
{
  if      (integer_ < rhs.integer_)
    return -1;
  else if (integer_ > rhs.integer_)
    return 1;
  else
    return 0;
}

//------

bool
ClParserInteger::
isInteger(double real, long &ireal)
{
  ireal = long(real);

  double real1 = double(ireal);

  return (fabs(real1 - real) < 1E-6);
}

//------

std::string
ClParserInteger::
asString() const
{
  return ClParserInst->toString(integer_);
}

void
ClParserInteger::
print() const
{
  ClParserInst->output("%s", asString().c_str());
}

void
ClParserInteger::
print(std::ostream &os) const
{
  os << asString();
}

void
ClParserInteger::
debugPrint() const
{
  fprintf(stderr, " %s ", asString().c_str());
}
