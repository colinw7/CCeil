#include <CCeilPI.h>
#include <cerrno>

using std::string;
using std::ostream;

void
ClParserInteger::
copy(const ClParserObj &obj)
{
  assert(base_type_ == obj.getBaseType());

  const ClParserInteger &rhs = reinterpret_cast<const ClParserInteger &>(obj);

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
  const ClParserInteger &integer = reinterpret_cast<const ClParserInteger &>(obj);

  long value = integer_ + integer.integer_;

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
minus(const ClParserObj &obj) const
{
  const ClParserInteger &integer = reinterpret_cast<const ClParserInteger &>(obj);

  long value = integer_ - integer.integer_;

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
times(const ClParserObj &obj) const
{
  const ClParserInteger &integer = reinterpret_cast<const ClParserInteger &>(obj);

  long value = integer_ * integer.integer_;

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
divide(const ClParserObj &obj) const
{
  const ClParserInteger &integer = reinterpret_cast<const ClParserInteger &>(obj);

  if (integer.integer_ == 0)
    CITHROW(CLERR_DIVIDE_BY_ZERO);

  long value = integer_ / integer.integer_;

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
modulus(const ClParserObj &obj) const
{
  const ClParserInteger &integer = reinterpret_cast<const ClParserInteger &>(obj);

  if (integer.integer_ == 0)
    CITHROW(CLERR_DIVIDE_BY_ZERO);

  long value = integer_ % integer.integer_;

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
power(const ClParserObj &obj) const
{
  const ClParserInteger &integer = reinterpret_cast<const ClParserInteger &>(obj);

  double value = CMathGen::pow(integer_, integer.integer_);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
approxEqual(const ClParserObj &obj) const
{
  const ClParserInteger &integer = reinterpret_cast<const ClParserInteger &>(obj);

  long value = (integer_ == integer.integer_);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
bitAnd(const ClParserObj &obj) const
{
  const ClParserInteger &integer = reinterpret_cast<const ClParserInteger &>(obj);

  long value = (integer_ & integer.integer_);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
bitOr(const ClParserObj &obj) const
{
  const ClParserInteger &integer = reinterpret_cast<const ClParserInteger &>(obj);

  long value = (integer_ | integer.integer_);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
bitXor(const ClParserObj &obj) const
{
  const ClParserInteger &integer = reinterpret_cast<const ClParserInteger &>(obj);

  long value = (integer_ ^ integer.integer_);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
bitLShift(const ClParserObj &obj) const
{
  const ClParserInteger &integer = reinterpret_cast<const ClParserInteger &>(obj);

  long value = (integer_ << integer.integer_);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
bitRShift(const ClParserObj &obj) const
{
  const ClParserInteger &integer = reinterpret_cast<const ClParserInteger &>(obj);

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
  long value = (integer_ * integer_);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
sqrt() const
{
  errno = 0;

  double value = CMathGen::sqrt(integer_);

  if (errno != 0 && ClParserInst->getMathFail())
    CITHROW(CLERR_INVALID_SQRT_VALUE);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
cos() const
{
  double value = ::cos(ClParserInst->toRadians(integer_));

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
sin() const
{
  double value = ::sin(ClParserInst->toRadians(integer_));

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
tan() const
{
  double value = ::tan(ClParserInst->toRadians(integer_));

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
acos() const
{
  errno = 0;

  double value = ClParserInst->fromRadians(CMathGen::acos(integer_));

  if (errno != 0 && ClParserInst->getMathFail())
    CITHROW(CLERR_INVALID_ACOS_VALUE);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
asin() const
{
  errno = 0;

  double value = ClParserInst->fromRadians(CMathGen::asin(integer_));

  if (errno != 0 && ClParserInst->getMathFail())
    CITHROW(CLERR_INVALID_ASIN_VALUE);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
atan() const
{
  double value = ClParserInst->fromRadians(::atan(integer_));

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
atan(double real) const
{
  double value = ClParserInst->fromRadians(::atan2(integer_, real));

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
exp() const
{
  double value = ::exp(integer_);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
log() const
{
  errno = 0;

  double value = CMathGen::log(integer_);

  if (errno != 0 && ClParserInst->getMathFail())
    CITHROW(CLERR_INVALID_LOG_VALUE);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
log10() const
{
  errno = 0;

  double value = CMathGen::log10(integer_);

  if (errno != 0 && ClParserInst->getMathFail())
    CITHROW(CLERR_INVALID_LOG_VALUE);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
cosh() const
{
  double value = ::cosh(ClParserInst->toRadians(integer_));

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
sinh() const
{
  double value = ::sinh(ClParserInst->toRadians(integer_));

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
tanh() const
{
  double value = ::tanh(ClParserInst->toRadians(integer_));

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
toChar() const
{
  string text(" ");

  text[0] = (char) integer_;

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
  double value = integer_;

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
toString() const
{
  string text = ClParserInst->toString(integer_);

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
  int c = (int) integer_;

  if (isupper(c))
    c = tolower(c);

  long value = (long) c;

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserInteger::
toUpper() const
{
  int c = (int) integer_;

  if (islower(c))
    c = toupper(c);

  long value = (long) c;

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
  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValueMgrInst->createValue(0L);
}

ClParserValuePtr
ClParserInteger::
rindex(const ClParserObj &) const
{
  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValueMgrInst->createValue(0L);
}

ClParserValuePtr
ClParserInteger::
sort(ClParserSortDirection) const
{
  long value = integer_;

  return ClParserValueMgrInst->createValue(value);
}

//-----------------

int
ClParserInteger::
cmp(const ClParserObj &obj) const
{
  if (base_type_ != obj.getBaseType())
    return CMathGen::sign((long) (base_type_ - obj.getBaseType()));

  const ClParserInteger &rhs = reinterpret_cast<const ClParserInteger &>(obj);

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

void
ClParserInteger::
print() const
{
  ClParserInst->output("%s", ClParserInst->toString(integer_).c_str());
}

void
ClParserInteger::
print(ostream &os) const
{
  os << ClParserInst->toString(integer_);
}

void
ClParserInteger::
debugPrint() const
{
  fprintf(stderr, " %ld ", integer_);
}
