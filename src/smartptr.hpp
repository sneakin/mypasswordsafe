/* $Header: /home/cvsroot/MyPasswordSafe/src/smartptr.hpp,v 1.5 2004/11/01 21:34:58 nolan Exp $
 * Copyright (c) 2004, Semantic Gap (TM)
 * http://www.semanticgap.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
