#include <CCeilPI.h>
#include <COSNaN.h>
#include <CInvNorm.h>
#include <boost/math/special_functions/erf.hpp>
#include <cerrno>

void
ClParserReal::
copy(const ClParserObj &obj)
{
  assert(base_type_ == obj.getBaseType());

  const ClParserReal &rhs = castObj(obj);

  *this = rhs;
}

ClParserReal *
ClParserReal::
dup() const
{
  return new ClParserReal(*this);
}

bool
ClParserReal::
isNanValue() const
{
  if (COSNaN::is_nan(real_))
    return true;

  return false;
}

long
ClParserReal::
getInteger() const
{
  // TODO: range check

  if (! isNanValue())
    return long(real_);
  else
    return 0;
}

bool
ClParserReal::
toBool() const
{
  return (! isNanValue() && real_ != 0.0);
}

//-----------------

// Inline Ops

const ClParserObj &
ClParserReal::
increment()
{
  if (! isNanValue())
    ++real_;

  return *this;
}

const ClParserObj &
ClParserReal::
decrement()
{
  if (! isNanValue())
    --real_;

  return *this;
}

//-----------------

// Unary Ops

ClParserValuePtr
ClParserReal::
unaryPlus() const
{
  return ClParserValueMgrInst->createValue( real_);
}

ClParserValuePtr
ClParserReal::
unaryMinus() const
{
  return ClParserValueMgrInst->createValue(-real_);
}

ClParserValuePtr
ClParserReal::
bitNot() const
{
  if (! isNanValue())
    return ClParserValueMgrInst->createValue(~getInteger());
  else
    return ClParserValueMgrInst->createValue( long(0    ));
}

//-----------------

// Binary Ops

ClParserValuePtr
ClParserReal::
plus(const ClParserObj &obj) const
{
  const ClParserReal &real = castObj(obj);

  double result;

  if (isNanValue() || real.isNanValue())
    COSNaN::set_nan(&result);
  else
    result = real_ + real.real_;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
minus(const ClParserObj &obj) const
{
  const ClParserReal &real = castObj(obj);

  double result;

  if (isNanValue() || real.isNanValue())
    COSNaN::set_nan(&result);
  else
    result = real_ - real.real_;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
times(const ClParserObj &obj) const
{
  const ClParserReal &real = castObj(obj);

  double result;

  if (isNanValue() || real.isNanValue())
    COSNaN::set_nan(&result);
  else
    result = real_ * real.real_;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
divide(const ClParserObj &obj) const
{
  const ClParserReal &real = castObj(obj);

  double result;

  if      (real.real_ == 0.0) {
    if (ClParserInst->getMathFail())
      ClErrThrow(ClErr::DIVIDE_BY_ZERO);

    if (real_ < 0.0)
      SetNegInf(result);
    else
      SetPosInf(result);
  }
  else if (isNanValue() || real.isNanValue())
    COSNaN::set_nan(&result);
  else
    result = real_ / real.real_;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
modulus(const ClParserObj &obj) const
{
  const ClParserReal &real = castObj(obj);

  errno = 0;

  double result = CMathGen::modulus(real_, real.real_);

  if (errno != 0 && ClParserInst->getMathFail())
    ClErrThrow(ClErr::DIVIDE_BY_ZERO);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
power(const ClParserObj &obj) const
{
  const ClParserReal &real = castObj(obj);

  errno = 0;

  double result = CMathGen::pow(real_, real.real_);

  if (errno != 0 && ClParserInst->getMathFail())
    ClErrThrow(ClErr::DIVIDE_BY_ZERO);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
approxEqual(const ClParserObj &obj) const
{
  const ClParserReal &real = castObj(obj);

  long result = long(::fabs(real_ - real.real_) < ClParserInst->getTolerance());

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
bitAnd(const ClParserObj &obj) const
{
  const ClParserReal &real = castObj(obj);

  long result = long(getInteger() & real.getInteger());

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
bitOr(const ClParserObj &obj) const
{
  const ClParserReal &real = castObj(obj);

  long result = long(getInteger() | real.getInteger());

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
bitXor(const ClParserObj &obj) const
{
  const ClParserReal &real = castObj(obj);

  long result = long(getInteger() ^ real.getInteger());

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
bitLShift(const ClParserObj &obj) const
{
  const ClParserReal &real = castObj(obj);

  double result = real_*CMathGen::pow(2.0, real.real_);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
bitRShift(const ClParserObj &obj) const
{
  const ClParserReal &real = castObj(obj);

  double result = real_/CMathGen::pow(2.0, real.real_);

  return ClParserValueMgrInst->createValue(result);
}

//-----------------

// Intern Fn

ClParserValuePtr
ClParserReal::
abs() const
{
  double result;

  if      (isNanValue())
    COSNaN::set_nan(&result);
  else if (real_ < 0.0)
    result = -real_;
  else
    result = real_;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
ceil() const
{
  double result;

  if      (isNanValue())
    COSNaN::set_nan(&result);
  else
    result = ::ceil(real_);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
floor() const
{
  double result;

  if      (isNanValue())
    COSNaN::set_nan(&result);
  else
    result = ::floor(real_);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
sign() const
{
  double result;

  if      (isNanValue())
    COSNaN::set_nan(&result);
  else if (real_ > 0.0)
    result =  1.0;
  else if (real_ < 0.0)
    result = -1.0;
  else
    result =  0.0;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
sqr() const
{
  double result;

  if (isNanValue())
    COSNaN::set_nan(&result);
  else
    result = real_ * real_;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
sqrt() const
{
  errno = 0;

  double result = CMathGen::sqrt(real_);

  if (errno != 0 && ClParserInst->getMathFail())
    ClErrThrow(ClErr::DIVIDE_BY_ZERO);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
cos() const
{
  double result;

  if (isNanValue())
    COSNaN::set_nan(&result);
  else
    result = ::cos(ClParserInst->toRadians(real_));

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
sin() const
{
  double result;

  if (isNanValue())
    COSNaN::set_nan(&result);
  else
    result = ::sin(ClParserInst->toRadians(real_));

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
tan() const
{
  double result;

  if (isNanValue())
    COSNaN::set_nan(&result);
  else
    result = ::tan(ClParserInst->toRadians(real_));

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
acos() const
{
  errno = 0;

  double result = ClParserInst->fromRadians(CMathGen::acos(real_));

  if (errno != 0 && ClParserInst->getMathFail())
    ClErrThrow(ClErr::DIVIDE_BY_ZERO);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
asin() const
{
  errno = 0;

  double result = ClParserInst->fromRadians(CMathGen::asin(real_));

  if (errno != 0 && ClParserInst->getMathFail())
    ClErrThrow(ClErr::DIVIDE_BY_ZERO);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
atan() const
{
  double result;

  if (isNanValue())
    COSNaN::set_nan(&result);
  else
    result = ClParserInst->fromRadians(::atan(real_));

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
atan(double real) const
{
  double result;

  if (isNanValue() || COSNaN::is_nan(real))
    COSNaN::set_nan(&result);
  else
    result = ClParserInst->fromRadians(::atan2(real_, real));

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
exp() const
{
  double result;

  if (isNanValue())
    COSNaN::set_nan(&result);
  else
    result = ::exp(real_);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
log() const
{
  errno = 0;

  double result = CMathGen::log(real_);

  if (errno != 0 && ClParserInst->getMathFail())
    ClErrThrow(ClErr::DIVIDE_BY_ZERO);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
log10() const
{
  errno = 0;

  double result = CMathGen::log10(real_);

  if (errno != 0 && ClParserInst->getMathFail())
    ClErrThrow(ClErr::DIVIDE_BY_ZERO);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
cosh() const
{
  double result;

  if (isNanValue())
    COSNaN::set_nan(&result);
  else
    result = ::cosh(ClParserInst->toRadians(real_));

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
sinh() const
{
  double result;

  if (isNanValue())
    COSNaN::set_nan(&result);
  else
    result = ::sinh(ClParserInst->toRadians(real_));

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
tanh() const
{
  double result;

  if (isNanValue())
    COSNaN::set_nan(&result);
  else
    result = ::tanh(ClParserInst->toRadians(real_));

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
norm() const
{
  double result = real_;

  if (isNanValue())
    COSNaN::set_nan(&result);
  else {
    result = 0.5*std::sqrt(2.0)*result;
    result = 0.5*erfc(-result);
  }

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
invnorm() const
{
  double result;

  if (isNanValue())
    COSNaN::set_nan(&result);
  else {
    result = CInvNorm::calc(real_);

    if (IsPosInf(result) || IsNegInf(result))
      SetNaN(result);
  }

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
toChar() const
{
  std::string result(" ");

  result[0] = char(getInteger());

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
toInt() const
{
  long result = getInteger();

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
toReal() const
{
  double result = real_;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
toString() const
{
  std::string result = asString();

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
isNan() const
{
  long result = isNanValue();

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
toLower() const
{
  int c = int(getInteger());

  if (isupper(c))
    c = tolower(c);

  long value = long(c);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserReal::
toUpper() const
{
  int c = int(getInteger());

  if (islower(c))
    c = toupper(c);

  long value = long(c);

  return ClParserValueMgrInst->createValue(value);
}

ClParserValuePtr
ClParserReal::
min() const
{
  double result = real_;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
max() const
{
  double result = real_;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
sum() const
{
  double result = real_;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
index(const ClParserObj &) const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserReal::
rindex(const ClParserObj &) const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserReal::
sort(ClParserSortDirection) const
{
  double result = real_;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
doAssert() const
{
  assert(real_);

  return ClParserValuePtr();
}

//-----------------

int
ClParserReal::
cmp(const ClParserObj &obj) const
{
  if (base_type_ != obj.getBaseType())
    return CMathGen::sign(long(base_type_ - obj.getBaseType()));

  const ClParserReal &rhs = castObj(obj);

  if      (isNanValue() && rhs.isNanValue())
    return 1;
  else if (isNanValue())
    return 1;
  else if (rhs.isNanValue())
    return -1;
  else if (real_ < rhs.real_)
    return -1;
  else if (real_ > rhs.real_)
    return 1;
  else
    return 0;
}

//------

std::string
ClParserReal::
asString() const
{
  if      (IsPosInf(real_))
    return "+Inf";
  else if (IsNegInf(real_))
    return "-Inf";
  else if (isNanValue())
    return "NaN";
  else
    return ClParserInst->toString(real_);
}

void
ClParserReal::
print() const
{
  ClParserInst->output("%s", asString().c_str());
}

void
ClParserReal::
print(std::ostream &os) const
{
  os << asString();
}

void
ClParserReal::
debugPrint() const
{
  fprintf(stderr, " %s", asString().c_str());
}
