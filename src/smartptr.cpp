#include <map>
#include "smartptr.hpp"
#include "myutil.hpp"

typedef std::map<void *, int> PointerMap;
PointerMap SmartPtr_pointers;

void SmartPtr_incRef(void *ptr)
{
  if(ptr == NULL)
    return;

  DBGOUT("incRef: " << ptr);
  PointerMap::iterator iter = SmartPtr_pointers.find(ptr);
  if(iter != SmartPtr_pointers.end()) {
    iter->second++;
    DBGOUT("cnt: " << iter->second);
  }
  else
    SmartPtr_pointers[ptr] = 1;
}


bool SmartPtr_decRef(void *ptr)
{
  if(ptr == NULL)
    return false;

  DBGOUT("decRef: " << ptr);
  PointerMap::iterator iter = SmartPtr_pointers.find(ptr);
  assert(iter != SmartPtr_pointers.end());

  iter->second--;
  DBGOUT("cnt: " << iter->second);
  if(iter->second <= 0) {
    DBGOUT("deleting " << ptr);
    SmartPtr_pointers.erase(iter);
    return true;
  }

  return false;
}
