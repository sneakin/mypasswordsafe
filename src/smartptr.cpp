/* $Header: /home/cvsroot/MyPasswordSafe/src/smartptr.cpp,v 1.4 2004/11/01 17:44:45 nolan Exp $
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
#include <map>
#include <assert.h>
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
