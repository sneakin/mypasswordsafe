/* $Header: /home/cvsroot/MyPasswordSafe/src/exception.hpp,v 1.2 2004/05/04 22:48:44 nolan Exp $
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
