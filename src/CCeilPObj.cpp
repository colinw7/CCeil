#include <CCeilPI.h>

int
ClParserObjCmp::
operator()(const ClParserObj &obj1, const ClParserObj &obj2)
{
  int cmp = obj1.cmp(obj2);

  if (direction_ == CL_SORT_DESCENDING)
    cmp = -cmp;

  return (cmp < 0);
}

int
ClParserObjCmp::
operator()(const ClParserObj *obj1, const ClParserObj *obj2)
{
  int cmp = obj1->cmp(*obj2);

  if (direction_ == CL_SORT_DESCENDING)
    cmp = -cmp;

  return (cmp < 0);
}

int
ClParserObjCmp::
operator()(const ClParserIntegerPtr &obj1, const ClParserIntegerPtr &obj2)
{
  int cmp = obj1->cmp(obj2);

  if (direction_ == CL_SORT_DESCENDING)
    cmp = -cmp;

  return (cmp < 0);
}

ClParserValuePtr
ClParserObj::
dim() const
{
  long result = 1;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserObj::
ndim() const
{
  long result = 1;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserObj::
len() const
{
  long result = 1;

  return ClParserValueMgrInst->createValue(result);
}
