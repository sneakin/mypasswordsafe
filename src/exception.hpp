/* $Header: /home/cvsroot/MyPasswordSafe/src/exception.hpp,v 1.1.1.1 2004/05/04 22:47:07 nolan Exp $
 */
#ifndef _EXCEPTION_HPP_
#define _EXCEPTION_HPP_

#include <string>

using std::string;

class Exception
{
private:
  string _desc;

public:
  Exception(const char *desc)
    : _desc(desc)
  {
  }

  const char *desc() { return _desc.c_str(); }
};

#endif
