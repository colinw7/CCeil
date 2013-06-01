#include <CCeilPI.h>
#include <COSNaN.h>

using std::string;
using std::list;
using std::ostream;

ClParserString::
ClParserString(const string &text) :
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

const ClParserString &
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

  const ClParserString &rhs = reinterpret_cast<const ClParserString &>(obj);

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
setText(const string &text)
{
  text_ = text;
}

void
ClParserString::
setText(const char *text, uint len)
{
  text_ = string(text, len);
}

void
ClParserString::
getChars(char **text, uint *len) const
{
  if (text == NULL) {
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
  *text = NULL;
  *len  = 0;

  string str;

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
getSubChars(int i1, int i2, string &text) const
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
  string::size_type pos = text_.find(str->text_);

  if (pos == string::npos)
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
countSubStrings(const string &text) const
{
  uint count = 0;

  string text1 = text_;

  string::size_type pos = text1.find(text);

  while (pos != string::npos) {
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
getStringPos(const string &text) const
{
  int pos = -1;

  string::size_type pos1 = text_.find(text);

  if (pos1 != string::npos)
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
getStringRPos(const string &text) const
{
  int pos = -1;

  string::size_type pos1 = text_.rfind(text);

  if (pos1 != string::npos)
    pos = pos1;

  return pos;
}

string
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
find(const string &str) const
{
  string::size_type pos = text_.find(str);

  if (pos == string::npos)
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
roll(int num, string &text) const
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

  const ClParserString &rhs = reinterpret_cast<const ClParserString &>(obj);

  return text_.compare(rhs.text_);
}

ClParserStringPtr
ClParserString::
format(ClParserValuePtr value) const
{
  string result;

  if (! format(value, result))
    return ClParserStringPtr();

  return createString(result);
}

bool
ClParserString::
format(ClParserValuePtr value, string &result) const
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

        string value_format = "";

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

          string name = text_.substr(k, i - k);

          ClParserValuePtr ind = ClParserValueMgrInst->createValue(name);

          values[0] = value->getDictionary()->getValue(ind);

          if (! values[0].isValid())
            return false;

          if (text_[i] == '>')
            ++i;
        }

        while (i < format_len && strchr("-+ #0", text_[i]) != NULL)
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

        while (i < format_len && strchr("hlL", text_[i]) != NULL) {
          //format_qualifier =  text_[i];

          value_format += text_[i++];
        }

        int format_code = text_[i];

        value_format += text_[i++];

        if (strchr("diouxXfeEgGcs%", format_code) == NULL) {
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
            string text;

            if (! values[value_num]->stringValue(text))
              return false;

            if (value->getType() != CL_PARSER_VALUE_TYPE_DICTIONARY)
              value_num++;

            char *buffer = new char [field_width + precision + 256];

            const char *value_format1 = value_format.c_str();

            if      (field_width_as_value && precision_as_value)
              ::sprintf(buffer, value_format1,
                        field_width, precision, text.c_str());
            else if (field_width_as_value)
              ::sprintf(buffer, value_format1,
                        field_width, text.c_str());
            else if (precision_as_value)
              ::sprintf(buffer, value_format1,
                        precision, text.c_str());
            else
              ::sprintf(buffer, value_format1,
                        text.c_str());

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
    //error_code = CLERR_INVALID_STRING_TO_INTEGER_CONV;
    return false;
  }

  pos = parser.getPos();

  CStrUtil::skipSpace(text_, &pos);

  if (pos < (int) text_.size()) {
    //error_code = CLERR_INVALID_STRING_TO_INTEGER_CONV;
    return false;
  }

  if (! value->convertToInteger()) {
    //error_code = CLERR_INVALID_CONVERSION;
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
    //error_code = CLERR_INVALID_STRING_TO_REAL_CONV;
    return false;
  }

  pos += parser.getPos();

  CStrUtil::skipSpace(text_, &pos);

  if (pos < (int) text_.size()) {
    //error_code = CLERR_INVALID_STRING_TO_REAL_CONV;
    return false;
  }

  if (! value->convertToReal()) {
    //error_code = CLERR_INVALID_CONVERSION;
    return false;
  }

  *real = factor*value->getReal()->getValue();

  return true;
}

void
ClParserString::
print() const
{
  ClParserInst->output("\"%s\"", ClParserInst->toString(text_.c_str()).c_str());
}

void
ClParserString::
print(ostream &os) const
{
  os << "\"" << ClParserInst->toString(text_.c_str()) << "\"";
}

void
ClParserString::
debugPrint() const
{
  fprintf(stderr, "\"%s\"", text_.c_str());
}

//-----------------

// Inline Ops

const ClParserObj &
ClParserString::
increment()
{
  CITHROW(CLERR_INVALID_OPERATOR);

  return *this;
}

const ClParserObj &
ClParserString::
decrement()
{
  CITHROW(CLERR_INVALID_OPERATOR);

  return *this;
}

//-----------------

// Unary Ops

ClParserValuePtr
ClParserString::
unaryPlus() const
{
  CITHROW(CLERR_INVALID_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
unaryMinus() const
{
  CITHROW(CLERR_INVALID_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
bitNot() const
{
  CITHROW(CLERR_INVALID_OPERATOR);

  return ClParserValuePtr();
}

//-----------------

// Binary Ops

ClParserValuePtr
ClParserString::
plus(const ClParserObj &obj) const
{
  const ClParserString &str = reinterpret_cast<const ClParserString &>(obj);

  string result = text_ + str.text_;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
minus(const ClParserObj &obj) const
{
  const ClParserString &str = reinterpret_cast<const ClParserString &>(obj);

  string result;

  string::size_type pos = text_.find(str.text_);

  if (pos != string::npos)
    result = text_.substr(0, pos) + text_.substr(pos + str.getLen());
  else
    result = text_;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
times(const ClParserObj &obj) const
{
  const ClParserInteger &integer = reinterpret_cast<const ClParserInteger &>(obj);

  long num = integer.getValue();

  if (num < 0)
    CITHROW(CLERR_VALUE_OUT_OF_RANGE);

  string result;

  if (num == 0)
    result = "";
  else {
    string text = text_;

    for (long i = 0; i < num; ++i)
      result += text;
  }

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
divide(const ClParserObj &obj) const
{
  const ClParserString &str = reinterpret_cast<const ClParserString &>(obj);

  if (str.text_ == "")
    CITHROW(CLERR_DIVIDE_BY_ZERO);

  long result = countSubStrings(str.text_);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
modulus(const ClParserObj &obj) const
{
  ClParserValuePtr value = ClParserValueMgrInst->createValue(obj);

  string result;

  if (! format(value, result))
    CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
power(const ClParserObj &) const
{
  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
approxEqual(const ClParserObj &obj) const
{
  const ClParserString &str = reinterpret_cast<const ClParserString &>(obj);

  CGlob compile(str.text_);

  if (! compile.isValid())
    CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  long result = compile.compare(text_);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
bitAnd(const ClParserObj &) const
{
  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
bitOr(const ClParserObj &) const
{
  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
bitXor(const ClParserObj &) const
{
  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
bitLShift(const ClParserObj &obj) const
{
  const ClParserInteger &integer = reinterpret_cast<const ClParserInteger &>(obj);

  long num = integer.getValue();

  string result = text_;

  roll(num, result);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
bitRShift(const ClParserObj &obj) const
{
  const ClParserInteger &integer = reinterpret_cast<const ClParserInteger &>(obj);

  long num = -integer.getValue();

  string result = text_;

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

  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
ceil() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::ceil(result));

  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
floor() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::floor(result));

  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
sign() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(result >= 0.0 ? 1.0 : -1.0);

  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
sqr() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(result*result);

  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
sqrt() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::sqrt(result));

  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
cos() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::cos(result));

  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
sin() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::sin(result));

  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
tan() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::tan(result));

  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
acos() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::acos(result));

  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
asin() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::asin(result));

  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
atan() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::atan(result));

  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
atan(double real) const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::atan2(result, real));

  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
exp() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::exp(result));

  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
log() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::log(result));

  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
log10() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::log10(result));

  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
cosh() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::cosh(result));

  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
sinh() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::sinh(result));

  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
tanh() const
{
  double result;

  if (toReal(&result))
    return ClParserValueMgrInst->createValue(::tanh(result));

  CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserString::
toChar() const
{
  string result(" ");

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
      CITHROW(CLERR_INVALID_STRING_TO_REAL_CONV);
  }

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
toString() const
{
  string result = text_;

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
      CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

    result = 0;
  }

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
toLower() const
{
  string result = CStrUtil::toLower(text_);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
toUpper() const
{
  string result = CStrUtil::toUpper(text_);

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
  string result = " ";

  result[0] = getMinChar();

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
max() const
{
  string result = " ";

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
  const ClParserString &str = reinterpret_cast<const ClParserString &>(obj);

  long result = getStringPos(str.text_) + 1;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
rindex(const ClParserObj &obj) const
{
  const ClParserString &str = reinterpret_cast<const ClParserString &>(obj);

  long result = getStringRPos(str.text_) + 1;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserString::
sort(ClParserSortDirection direction) const
{
  list<ClParserIntegerPtr> chars;

  uint len = getLen();

  for (uint i = 0; i < len; ++i)
    chars.push_back(ClParserInteger::createInteger(text_[i]));

  chars.sort(ClParserObjCmp(direction));

  string result = text_;

  list<ClParserIntegerPtr>::iterator p1 = chars.begin();
  list<ClParserIntegerPtr>::iterator p2 = chars.end  ();

  for (uint i = 0; p1 != p2; ++p1, ++i)
    result[i] = (*p1)->getValue();

  return ClParserValueMgrInst->createValue(result);
}

bool
ClParserString::
indexToText(int *ind) const
{
  if (*ind < 0)
    *ind += getLen() + 1;

  --(*ind);

  return (*ind >= 0 && *ind < (int) getLen());
}
