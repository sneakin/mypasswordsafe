/* $Header: /home/cvsroot/MyPasswordSafe/src/smartptr.hpp,v 1.4 2004/11/01 17:44:45 nolan Exp $
 * Copyright (c) 2004, Semantic Gap (TM)
 * All rights reserved.
 *   
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted provided
 * that the following conditions are met:
 *  -  Redistributions of source code must retain the
 *     above copyright notice, this list of conditions
 *     and the following disclaimer.
 *  -  Redistributions in binary form must reproduce the
 *     above copyright notice, this list of conditions and
 *     the following disclaimer in the documentation and/or
 *     other materials provided with the distribution.
 *  -  Neither the name of Semantic Gap (TM) nor the
 *     names of its contributors may be used to endorse or
 *     promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
