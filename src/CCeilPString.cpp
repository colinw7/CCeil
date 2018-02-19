#include <CCeilPI.h>
#include <COSNaN.h>

ClParserString::
ClParserString(const std::string &text) :
 ClParserObj(CL_PARSER_VALUE_TYPE_STRING), text_(text)
{
}

ClParserString::
ClParserString(const char *text, uint len) :
 ClParserObj(CL_PARSER_VALUE_TYPE_STRING), text_(text, len)
{
}

ClParserString::
ClParserString(const ClParserString &str) :
 ClParserObj(CL_PARSER_VALUE_TYPE_STRING), text_(str.text_)
{
}

ClParserString::
ClParserString(const ClParserStringPtr &str) :
 ClParserObj(CL_PARSER_VALUE_TYPE_STRING), text_(str->text_)
{
}

ClParserString &
ClParserString::
operator=(const ClParserString &str)
{
  text_ = str.text_;

  return *this;
}

ClParserString *
ClParserString::
dup() const
{
  return new ClParserString(*this);
}

void
ClParserString::
copy(const ClParserObj &obj)
{
  assert(base_type_ == obj.getBaseType());

  const ClParserString &rhs = castObj(obj);

  *this = rhs;
}

bool
ClParserString::
toBool() const
{
  return (text_.size() != 0);
}

void
ClParserString::
setText(const std::string &text)
{
  text_ = text;
}

void
ClParserString::
setText(const char *text, uint len)
{
  text_ = std::string(text, len);
}

void
ClParserString::
getChars(char **text, uint *len) const
{
  if (! text) {
    *text = new char [1];
    *len  = 0;

    (*text)[0] = '\0';
  }
  else {
    *len  = text_.size();
    *text = new char [*len + 1];

    memcpy(*text, text_.c_str(), *len);

    (*text)[*len] = '\0';
  }
}

bool
ClParserString::
getSubChars(int i1, int i2, char **text, uint *len) const
{
  *text = nullptr;
  *len  = 0;

  std::string str;

  if (! getSubChars(i1, i2, str))
    return false;

  *len  = str.size();
  *text = new char [*len + 1];

  memcpy(*text, str.c_str(), *len);

  (*text)[*len] = '\0';

  return true;
}

bool
ClParserString::
getSubChars(int i1, int i2, std::string &text) const
{
  if (! indexToText(&i1))
    return false;

  if (! indexToText(&i2))
    return false;

  if (i1 > i2)
    return false;

  int len = i2 - i1 + 1;

  text = text_.substr(i1, len);

  return true;
}

bool
ClParserString::
replaceSubString(int i1, int i2, ClParserStringPtr str)
{
  if (! indexToText(&i1))
    return false;

  if (! indexToText(&i2))
    return false;

  if (i1 > i2)
    return false;

  text_ = text_.substr(0, i1) + str->text_ + text_.substr(i2 + 1);

  return true;
}

bool
ClParserString::
removeString(ClParserStringPtr str)
{
  std::string::size_type pos = text_.find(str->text_);

  if (pos == std::string::npos)
    return false;

  text_ = text_.substr(0, pos) + text_.substr(pos + str->getLen());

  return true;
}

uint
ClParserString::
countSubStrings(ClParserStringPtr str) const
{
  return countSubStrings(str->text_);
}

uint
ClParserString::
countSubStrings(const std::string &text) const
{
  uint count = 0;

  std::string text1 = text_;

  std::string::size_type pos = text1.find(text);

  while (pos != std::string::npos) {
    text1 = text1.substr(0, pos) + text1.substr(pos + text.size());

    pos = text1.find(text);

    ++count;
  }

  return count;
}

int
ClParserString::
getStringPos(ClParserStringPtr str) const
{
  return getStringPos(str->text_);
}

int
ClParserString::
getStringPos(const std::string &text) const
{
  int pos = -1;

  std::string::size_type pos1 = text_.find(text);

  if (pos1 != std::string::npos)
    pos = pos1;

  return pos;
}

int
ClParserString::
getStringRPos(ClParserStringPtr str) const
{
  return getStringRPos(str->text_);
}

int
ClParserString::
getStringRPos(const std::string &text) const
{
  int pos = -1;

  std::string::size_type pos1 = text_.rfind(text);

  if (pos1 != std::string::npos)
    pos = pos1;

  return pos;
}

std::string
ClParserString::
subString(uint pos, int len) const
{
  return text_.substr(pos, len);
}

void
ClParserString::
append(ClParserStringPtr str)
{
  text_ += str->text_;
}

ClParserStringPtr
ClParserString::
concat(ClParserStringPtr str) const
{
  return createString(text_ + str->text_);
}

int
ClParserString::
find(const std::string &str) const
{
  std::string::size_type pos = text_.find(str);

  if (pos == std::string::npos)
    return -1;

  return pos;
}

void
ClParserString::
roll(int num)
{
  roll(num, text_);
}

void
ClParserString::
roll(int num, std::string &text) const
{
  if (num == 0 || text.size() <= 1)
    return;

  if (num > 0) {
    num %= text.size();

    for ( ; num > 0; --num) {
      char c = text[0];

      text = text.substr(1) + c;
    }
  }
  else {
    num = -num;

    num %= text.size();

    for ( ; num > 0; --num) {
      char c = text[text.size() - 1];

      text = c + text.substr(0, text.size() - 1);
    }
  }
}

uchar
ClParserString::
getMaxChar() const
{
  if (text_.size() == 0)
    return 0;

  uchar c = text_[0];

  for (uint i = 1; i < text_.size(); ++i)
    if (text_[i] > c)
      c = (uchar) text_[i];

  return c;
}

uchar
ClParserString::
getMinChar() const
{
  if (text_.size() == 0)
    return 0;

  uchar c = text_[0];

  for (uint i = 1; i < text_.size(); ++i)
    if (text_[i] < c)
      c = (uchar) text_[i];

  return c;
}

int
ClParserString::
cmp(const ClParserObj &obj) const
{
  if (base_type_ != obj.getBaseType())
    return CMathGen::sign((long) (base_type_ - obj.getBaseType()));

  const ClParserString &rhs = castObj(obj);

  return text_.compare(rhs.text_);
}

ClParserStringPtr
ClParserString::
format(ClParserValuePtr value) const
{
  std::string result;

  if (! format(value, result))
    return ClParserStringPtr();

  return createString(result);
}

bool
ClParserString::
format(ClParserValuePtr value, std::string &result) const
{
  ClParserValueArray values;

  if      (value->getType() == CL_PARSER_VALUE_TYPE_REAL    ||
           value->getType() == CL_PARSER_VALUE_TYPE_INTEGER ||
           value->getType() == CL_PARSER_VALUE_TYPE_STRING)
    values.push_back(value);
  else if (value->getType() == CL_PARSER_VALUE_TYPE_ARRAY ||
           value->getType() == CL_PARSER_VALUE_TYPE_LIST)
    value->toSubValues(values);
  else if (value->getType() == CL_PARSER_VALUE_TYPE_DICTIONARY)
    values.push_back(ClParserValuePtr());
  else
    return false;

  uint num_values = values.size();

  //-----

  int value_num = 0;

  int format_len = text_.size();

  int i = 0;

  while (i < format_len) {
    switch (text_[i]) {
      case '%': {
        bool field_width_as_value = false;
        bool precision_as_value   = false;

        std::string value_format = "";

        value_format += text_[i++];

        if (value->getType() == CL_PARSER_VALUE_TYPE_DICTIONARY) {
          if (text_[i] != '<') {
            result += value_format;

            break;
          }

          ++i;

          int k = i;

          while (i < format_len && text_[i] != '>')
            ++i;

          std::string name = text_.substr(k, i - k);

          ClParserValuePtr ind = ClParserValueMgrInst->createValue(name);

          values[0] = value->getDictionary()->getValue(ind);

          if (! values[0].isValid())
            return false;

          if (text_[i] == '>')
            ++i;
        }

        while (i < format_len && strchr("-+ #0", text_[i]) != nullptr)
          value_format += text_[i++];

        if (text_[i] == '*') {
          field_width_as_value = true;

          value_format += text_[i++];
        }
        else {
          while (i < format_len && isdigit(text_[i]))
            value_format += text_[i++];
        }

        if (text_[i] == '.') {
          value_format += text_[i++];

          if (text_[i] == '*') {
            precision_as_value = true;

            value_format += text_[i++];
          }
          else {
            while (i < format_len && isdigit(text_[i]))
              value_format += text_[i++];
          }
        }

        //int format_qualifier = '\0';

        while (i < format_len && strchr("hlL", text_[i]) != nullptr) {
          //format_qualifier =  text_[i];

          value_format += text_[i++];
        }

        int format_code = text_[i];

        value_format += text_[i++];

        if (strchr("diouxXfeEgGcs%", format_code) == nullptr) {
          result += value_format;

          break;
        }

        if (format_code == '%') {
          result += format_code;

          break;
        }

        long field_width = 0;

        if (field_width_as_value) {
          if (value_num >= (int) num_values) {
            result += value_format;

            break;
          }

          if (! values[value_num]->integerValue(&field_width))
            field_width = 0;

          value_num++;
        }

        long precision = 0;

        if (precision_as_value) {
          if (value_num >= (int) num_values) {
            result += value_format;

            break;
          }

          if (! values[value_num]->integerValue(&precision))
            precision = 0;

          value_num++;
        }

        if (value_num >= (int) num_values) {
          result += value_format;

          break;
        }

        switch (format_code) {
          case 'd':
          case 'i':
          case 'o':
          case 'u':
          case 'x':
          case 'X':
          case 'c': {
            long integer;

            if (! values[value_num]->integerValue(&integer))
              return false;

            if (value->getType() != CL_PARSER_VALUE_TYPE_DICTIONARY)
              value_num++;

            if (format_code == 'c') {
              if (integer == 0 || ! isprint(integer))
                integer = '.';
            }

            char *buffer = new char [field_width + precision + 256];

            const char *value_format1 = value_format.c_str();

            if      (field_width_as_value && precision_as_value)
              ::sprintf(buffer, value_format1, field_width, precision, integer);
            else if (field_width_as_value)
              ::sprintf(buffer, value_format1, field_width, integer);
            else if (precision_as_value)
              ::sprintf(buffer, value_format1, precision, integer);
            else
              ::sprintf(buffer, value_format1, integer);

            result += buffer;

            delete [] buffer;

            break;
          }

          case 's': {
            std::string text;

            if (! values[value_num]->stringValue(text))
              return false;

            if (value->getType() != CL_PARSER_VALUE_TYPE_DICTIONARY)
              value_num++;

            char *buffer = new char [field_width + precision + 256];

            const char *value_format1 = value_format.c_str();

            if      (field_width_as_value && precision_as_value)
              ::sprintf(buffer, value_format1, field_width, precision, text.c_str());
            else if (field_width_as_value)
              ::sprintf(buffer, value_format1, field_width, text.c_str());
            else if (precision_as_value)
              ::sprintf(buffer, value_format1, precision, text.c_str());
            else
              ::sprintf(buffer, value_format1, text.c_str());

            result += buffer;

            delete [] buffer;

            break;
          }

          case 'f':
          case 'e':
          case 'E':
          case 'g':
          case 'G': {
            double real;

            if (! values[value_num]->realValue(&real))
              real = 0.0;

            if (value->getType() != CL_PARSER_VALUE_TYPE_DICTIONARY)
              value_num++;

            char *buffer = new char [field_width + precision + 256];

            const char *value_format1 = value_format.c_str();

            if      (field_width_as_value && precision_as_value)
              ::sprintf(buffer, value_format1, field_width, precision, real);
            else if (field_width_as_value)
              ::sprintf(buffer, value_format1, field_width, real);
            else if (precision_as_value)
              ::sprintf(buffer, value_format1, precision, real);
            else
              ::sprintf(buffer, value_format1, real);

            result += buffer;

            delete [] buffer;

            break;
          }
        }

        break;
      }
      default: {
        result += text_[i++];

        break;
      }
    }
  }

  return true;
}

bool
ClParserString::
toInteger(long *integer) const
{
  //int error_code = 0;

  *integer = 0;

  int pos = 0;

  CStrUtil::skipSpace(text_, &pos);

  int factor = 1;

  if (text_[pos] == '-')
    factor = -1;

  if (text_[pos] == '-' || text_[pos] == '+')
    pos++;

  ClParserParser parser(text_.substr(pos));

  ClParserValuePtr value;

  if (! parser.readNumericValue(value)) {
    //error_code = int(ClErr::INVALID_STRING_TO_INTEGER_CONV);
    return false;
  }

  pos = parser.getPos();

  CStrUtil::skipSpace(text_, &pos);

  if (pos < (int) text_.size()) {
    //error_code = int(ClErr::INVALID_STRING_TO_INTEGER_CONV);
    return false;
  }

  if (! value->convertToInteger()) {
    //error_code = int(ClErr::INVALID_CONVERSION);
    return false;
  }

  *integer = factor*value->getInteger()->getValue();

  return true;
}

bool
ClParserString::
toReal(double *real) const
{
  //int error_code = 0;

  *real = 0.0;

  if (text_ == "NaN") {
    COSNaN::set_nan(real);
    return false;
  }

  int pos = 0;

  CStrUtil::skipSpace(text_, &pos);

  double factor = 1.0;

  if (text_[pos] == '-')
    factor = -1.0;

  if (text_[pos] == '-' || text_[pos] == '+')
    pos++;

  ClParserParser parser(text_.substr(pos));

  ClParserValuePtr value;

  if (! parser.readNumericValue(value)) {
    //error_code = int(ClErr::INVALID_STRING_TO_REAL_CONV);
    return false;
  }

  pos += parser.getPos();

  CStrUtil::skipSpace(text_, &pos);

  if (pos < (int) text_.size()) {
    //error_code = int(ClErr::INVALID_STRING_TO_REAL_CONV);
    return false;
  }

  if (! value->convertToReal()) {
    //error_code = int(ClErr::INVALID_CONVERSION);
    return false;
  }

  *real = factor*value->getReal()->getValue();

  return true;
}

//------

std::string
ClParserString::
asString() const
{
  return "\"" + text_ + "\"";
}

void
ClParserString::
print() const
{
  ClParserInst->output("%s", asString().c_str());
}

void
ClParserString::
print(std::ostream &os) const
{
  os << asString();
}

void
ClParserString::
debugPrint() const
{
  fprintf(stderr, "%s", asString().c_str());
}

//-----------------

// Inline Ops

const ClParserObj &
ClParserString::
increment()
{
  ClErrThrow(ClErr::INVALID_OPERATOR);

  return *this;
}

const ClParserObj &
ClParserString::
decrement()
{
  ClErrThrow(ClErr::INVALID_OPERATOR);

  return *this;
}

//-----------------

// Unary Ops

ClParserValuePtr
ClParserString::
unaryPlus() const
{
  ClErrThrow(ClErr::INVALID_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
unaryMinus() const
{
  ClErrThrow(ClErr::INVALID_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
bitNot() const
{
  ClErrThrow(ClErr::INVALID_OPERATOR);

  return ClParserValuePtr();
}

//-----------------

// Binary Ops

ClParserValuePtr
ClParserString::
plus(const ClParserObj &obj) const
{
  const ClParserString &str = castObj(obj);

  std::string result = text_ + str.text_;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
minus(const ClParserObj &obj) const
{
  const ClParserString &str = castObj(obj);

  std::string result;

  std::string::size_type pos = text_.find(str.text_);

  if (pos != std::string::npos)
    result = text_.substr(0, pos) + text_.substr(pos + str.getLen());
  else
    result = text_;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
times(const ClParserObj &obj) const
{
  const ClParserInteger &integer = ClParserInteger::castObj(obj);

  long num = integer.getValue();

  if (num < 0)
    ClErrThrow(ClErr::VALUE_OUT_OF_RANGE);

  std::string result;

  if (num == 0)
    result = "";
  else {
    std::string text = text_;

    for (long i = 0; i < num; ++i)
      result += text;
  }

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
divide(const ClParserObj &obj) const
{
  const ClParserString &str = castObj(obj);

  if (str.text_ == "")
    ClErrThrow(ClErr::DIVIDE_BY_ZERO);

  long result = countSubStrings(str.text_);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
modulus(const ClParserObj &obj) const
{
  ClParserValuePtr value = ClParserValueMgrInst->createValue(obj);

  std::string result;

  if (! format(value, result))
    ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
power(const ClParserObj &) const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
approxEqual(const ClParserObj &obj) const
{
  const ClParserString &str = castObj(obj);

  CGlob compile(str.text_);

  if (! compile.isValid())
    ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  long result = compile.compare(text_);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
bitAnd(const ClParserObj &) const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
bitOr(const ClParserObj &) const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
bitXor(const ClParserObj &) const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
bitLShift(const ClParserObj &obj) const
{
  const ClParserInteger &integer = ClParserInteger::castObj(obj);

  long num = integer.getValue();

  std::string result = text_;

  roll(num, result);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
bitRShift(const ClParserObj &obj) const
{
  const ClParserInteger &integer = ClParserInteger::castObj(obj);

  long num = -integer.getValue();

  std::string result = text_;

  roll(num, result);

  return ClParserValueMgrInst->createValue(result);
}

//-----------------

// Intern Fn

ClParserValuePtr
ClParserString::
abs() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::fabs(result));

  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
ceil() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::ceil(result));

  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
floor() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::floor(result));

  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
sign() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(result >= 0.0 ? 1.0 : -1.0);

  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
sqr() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(result*result);

  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
sqrt() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::sqrt(result));

  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
cos() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::cos(result));

  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
sin() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::sin(result));

  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
tan() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::tan(result));

  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
acos() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::acos(result));

  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
asin() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::asin(result));

  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
atan() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::atan(result));

  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
atan(double real) const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::atan2(result, real));

  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
exp() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::exp(result));

  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
log() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::log(result));

  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
log10() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::log10(result));

  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
cosh() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::cosh(result));

  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
sinh() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::sinh(result));

  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
tanh() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::tanh(result));

  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
toChar() const
{
  std::string result(" ");

  result[0] = text_[0];

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
toInt() const
{
  long result;

  if (! toInteger(&result))
    result = 0;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
toReal() const
{
  double result;

  if (! toReal(&result)) {
    if (! ClParserInst->getMathFail())
      COSNaN::set_nan(&result);
    else
      ClErrThrow(ClErr::INVALID_STRING_TO_REAL_CONV);
  }

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
toString() const
{
  std::string result = text_;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
isNan() const
{
  long result;

  if (text_ == "NaN")
    result = 1;
  else {
    if (! toInteger(&result))
      ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

    result = 0;
  }

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
toLower() const
{
  std::string result = CStrUtil::toLower(text_);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
toUpper() const
{
  std::string result = CStrUtil::toUpper(text_);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
len() const
{
  long result = getLen();

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
min() const
{
  std::string result = " ";

  result[0] = getMinChar();

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
max() const
{
  std::string result = " ";

  result[0] = getMaxChar();

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
sum() const
{
  uint len = getLen();

  long result = 0;

  for (uint i = 0; i < len; ++i)
    result += (uchar) text_[i];

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
index(const ClParserObj &obj) const
{
  const ClParserString &str = castObj(obj);

  long result = getStringPos(str.text_) + 1;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
rindex(const ClParserObj &obj) const
{
  const ClParserString &str = castObj(obj);

  long result = getStringRPos(str.text_) + 1;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
sort(ClParserSortDirection direction) const
{
  std::list<ClParserIntegerPtr> chars;

  uint len = getLen();

  for (uint i = 0; i < len; ++i)
    chars.push_back(ClParserInteger::createInteger(text_[i]));

  chars.sort(ClParserObjCmp(direction));

  std::string result = text_;

  std::list<ClParserIntegerPtr>::iterator p1 = chars.begin();
  std::list<ClParserIntegerPtr>::iterator p2 = chars.end  ();

  for (uint i = 0; p1 != p2; ++p1, ++i)
    result[i] = (*p1)->getValue();

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
doAssert() const
{
  assert(getLen());

  return ClParserValuePtr();
}

//------

bool
ClParserString::
indexToText(int *ind) const
{
  if (*ind < 0)
    *ind += getLen() + 1;

  --(*ind);

  return (*ind >= 0 && *ind < (int) getLen());
}
