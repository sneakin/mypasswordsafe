/* $Header: /home/cvsroot/MyPasswordSafe/src/smartptr.hpp,v 1.2 2004/05/04 22:48:44 nolan Exp $
 */
#ifndef SMARTPTR_HPP
#define SMARTPTR_HPP

#include "myutil.hpp"

void SmartPtr_incRef(void *);
bool SmartPtr_decRef(void *);

template<typename T>
class SmartPtr
{
public:
  typedef T type;
  typedef T* pointer;

  SmartPtr()
    : m_ptr(NULL)
  {
  }

  SmartPtr(pointer ptr)
    : m_ptr(ptr)
  {
    SmartPtr_incRef((void *)m_ptr);
  }

  SmartPtr(const SmartPtr<type> &ptr)
    : m_ptr(ptr.m_ptr)
  {
    SmartPtr_incRef((void *)m_ptr);
  }

  ~SmartPtr()
  {
    if(SmartPtr_decRef((void *)m_ptr)) {
      DBGOUT("deleted " << m_ptr);
      delete m_ptr;
    }
  }

  void set(pointer ptr)
  {
    if(m_ptr != ptr) {
      if(SmartPtr_decRef((void *)m_ptr)) {
	DBGOUT("deleted " << m_ptr);
	delete m_ptr;
      }
      m_ptr = ptr;
      SmartPtr_incRef((void *)m_ptr);
    }
  }

  void set(const SmartPtr<type> &ptr)
  {
    if(m_ptr != ptr.m_ptr) {
      if(SmartPtr_decRef((void *)m_ptr)) {
	DBGOUT("deleted " << m_ptr);
	delete m_ptr;
      }
      m_ptr = ptr.m_ptr;
      SmartPtr_incRef((void *)m_ptr);
    }
  }

  inline void operator=(const SmartPtr<T> &ptr) { set(ptr); }
  inline pointer operator->() { return m_ptr; }
  inline T const *operator->() const { return m_ptr; }
  inline pointer get() { return m_ptr; }
  inline operator pointer () { return get(); }
  inline operator T const * () { return get(); }

private:
  pointer m_ptr;
};

#endif
