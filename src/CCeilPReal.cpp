#include <CCeilPI.h>
#include <COSNaN.h>
#include <cerrno>

using std::string;
using std::ostream;

void
ClParserReal::
copy(const ClParserObj &obj)
{
  assert(base_type_ == obj.getBaseType());

  const ClParserReal &rhs = reinterpret_cast<const ClParserReal &>(obj);

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
  if (! COSNaN::is_nan(real_))
    ++real_;

  return *this;
}

const ClParserObj &
ClParserReal::
decrement()
{
  if (! COSNaN::is_nan(real_))
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
  if (! COSNaN::is_nan(real_))
    return ClParserValueMgrInst->createValue(~long(real_));
  else
    return ClParserValueMgrInst->createValue( long(0    ));
}

//-----------------

// Binary Ops

ClParserValuePtr
ClParserReal::
plus(const ClParserObj &obj) const
{
  const ClParserReal &real =
    reinterpret_cast<const ClParserReal &>(obj);

  double result;

  if (COSNaN::is_nan(real_) || COSNaN::is_nan(real.real_))
    COSNaN::set_nan(&result);
  else
    result = real_ + real.real_;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
minus(const ClParserObj &obj) const
{
  const ClParserReal &real =
    reinterpret_cast<const ClParserReal &>(obj);

  double result;

  if (COSNaN::is_nan(real_) || COSNaN::is_nan(real.real_))
    COSNaN::set_nan(&result);
  else
    result = real_ - real.real_;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
times(const ClParserObj &obj) const
{
  const ClParserReal &real =
    reinterpret_cast<const ClParserReal &>(obj);

  double result;

  if (COSNaN::is_nan(real_) || COSNaN::is_nan(real.real_))
    COSNaN::set_nan(&result);
  else
    result = real_ * real.real_;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
divide(const ClParserObj &obj) const
{
  const ClParserReal &real =
    reinterpret_cast<const ClParserReal &>(obj);

  double result;

  if      (real.real_ == 0.0) {
    if (ClParserInst->getMathFail())
      CITHROW(CLERR_DIVIDE_BY_ZERO);

    if (real_ < 0.0)
      SetNegInf(result);
    else
      SetPosInf(result);
  }
  else if (COSNaN::is_nan(real_) || COSNaN::is_nan(real.real_))
    COSNaN::set_nan(&result);
  else
    result = real_ / real.real_;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
modulus(const ClParserObj &obj) const
{
  const ClParserReal &real =
    reinterpret_cast<const ClParserReal &>(obj);

  errno = 0;

  double result = CMathGen::modulus(real_, real.real_);

  if (errno != 0 && ClParserInst->getMathFail())
    CITHROW(CLERR_DIVIDE_BY_ZERO);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
power(const ClParserObj &obj) const
{
  const ClParserReal &real =
    reinterpret_cast<const ClParserReal &>(obj);

  errno = 0;

  double result = CMathGen::pow(real_, real.real_);

  if (errno != 0 && ClParserInst->getMathFail())
    CITHROW(CLERR_DIVIDE_BY_ZERO);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
approxEqual(const ClParserObj &obj) const
{
  const ClParserReal &real =
    reinterpret_cast<const ClParserReal &>(obj);

  long result = long(::fabs(real_ - real.real_) < ClParserInst->getTolerance());

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
bitAnd(const ClParserObj &obj) const
{
  const ClParserReal &real =
    reinterpret_cast<const ClParserReal &>(obj);

  long result = long(((long) real_) & ((long) real.real_));

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
bitOr(const ClParserObj &obj) const
{
  const ClParserReal &real =
    reinterpret_cast<const ClParserReal &>(obj);

  long result = long(((long) real_) | ((long) real.real_));

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
bitXor(const ClParserObj &obj) const
{
  const ClParserReal &real =
    reinterpret_cast<const ClParserReal &>(obj);

  long result = long(((long) real_) ^ ((long) real.real_));

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
bitLShift(const ClParserObj &obj) const
{
  const ClParserReal &real =
    reinterpret_cast<const ClParserReal &>(obj);

  double result = real_*CMathGen::pow(2.0, real.real_);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
bitRShift(const ClParserObj &obj) const
{
  const ClParserReal &real =
    reinterpret_cast<const ClParserReal &>(obj);

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

  if      (COSNaN::is_nan(real_))
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

  if      (COSNaN::is_nan(real_))
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

  if      (COSNaN::is_nan(real_))
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

  if      (COSNaN::is_nan(real_))
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

  if      (COSNaN::is_nan(real_))
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
    CITHROW(CLERR_DIVIDE_BY_ZERO);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
cos() const
{
  double result;

  if (COSNaN::is_nan(real_))
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

  if (COSNaN::is_nan(real_))
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

  if (COSNaN::is_nan(real_))
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
    CITHROW(CLERR_DIVIDE_BY_ZERO);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
asin() const
{
  errno = 0;

  double result = ClParserInst->fromRadians(CMathGen::asin(real_));

  if (errno != 0 && ClParserInst->getMathFail())
    CITHROW(CLERR_DIVIDE_BY_ZERO);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
atan() const
{
  double result;

  if (COSNaN::is_nan(real_))
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

  if (COSNaN::is_nan(real_) || COSNaN::is_nan(real))
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

  if (COSNaN::is_nan(real_))
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
    CITHROW(CLERR_DIVIDE_BY_ZERO);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
log10() const
{
  errno = 0;

  double result = CMathGen::log10(real_);

  if (errno != 0 && ClParserInst->getMathFail())
    CITHROW(CLERR_DIVIDE_BY_ZERO);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
cosh() const
{
  double result;

  if (COSNaN::is_nan(real_))
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

  if (COSNaN::is_nan(real_))
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

  if (COSNaN::is_nan(real_))
    COSNaN::set_nan(&result);
  else
    result = ::tanh(ClParserInst->toRadians(real_));

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
toChar() const
{
  string result(" ");

  result[0] = (char) real_;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
toInt() const
{
  long result;

  if (COSNaN::is_nan(real_))
    result = 0;
  else
    result = long(real_);

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
  string result;

  if (COSNaN::is_nan(real_))
    result = "NaN";
  else
    result = ClParserInst->toString(real_);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
isNan() const
{
  long result = COSNaN::is_nan(real_);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserReal::
toLower() const
{
  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserReal::
toUpper() const
{
  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
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
  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserReal::
rindex(const ClParserObj &) const
{
  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserReal::
sort(ClParserSortDirection) const
{
  double result = real_;

  return ClParserValueMgrInst->createValue(result);
}

//-----------------

int
ClParserReal::
cmp(const ClParserObj &obj) const
{
  if (base_type_ != obj.getBaseType())
    return CMathGen::sign((long) (base_type_ - obj.getBaseType()));

  const ClParserReal &rhs = reinterpret_cast<const ClParserReal &>(obj);

  if      (COSNaN::is_nan(real_) && COSNaN::is_nan(rhs.real_))
    return 1;
  else if (COSNaN::is_nan(real_))
    return 1;
  else if (COSNaN::is_nan(rhs.real_))
    return -1;
  else if (real_ < rhs.real_)
    return -1;
  else if (real_ > rhs.real_)
    return 1;
  else
    return 0;
}

void
ClParserReal::
print() const
{
  if      (IsPosInf(real_))
    ClParserInst->output("%s", ClParserInst->toString("+Inf").c_str());
  else if (IsNegInf(real_))
    ClParserInst->output("%s", ClParserInst->toString("-Inf").c_str());
  else if (COSNaN::is_nan(real_))
    ClParserInst->output("%s", ClParserInst->toString("NaN").c_str());
  else
    ClParserInst->output("%s", ClParserInst->toString(real_).c_str());
}

void
ClParserReal::
print(ostream &os) const
{
  if      (IsPosInf(real_))
    os << ClParserInst->toString("+Inf");
  else if (IsNegInf(real_))
    os << ClParserInst->toString("-Inf");
  else if (COSNaN::is_nan(real_))
    os << ClParserInst->toString("NaN");
  else
    os << ClParserInst->toString(real_);
}

void
ClParserReal::
debugPrint() const
{
  if      (IsPosInf(real_))
    fprintf(stderr, " +Inf");
  else if (IsNegInf(real_))
    fprintf(stderr, " -Inf");
  else if (COSNaN::is_nan(real_))
    fprintf(stderr, " NaN ");
  else
    fprintf(stderr, " %g ", real_);
}
