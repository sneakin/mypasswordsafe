/* $Header: /home/cvsroot/MyPasswordSafe/src/myutil.cpp,v 1.7 2004/12/06 12:32:05 nolan Exp $
 * Copyright 2004, Semantic Gap (TM)
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
#include <string>
#include <vector>
#include <qapplication.h>
#include <qclipboard.h>
#include <qstring.h>
#include "safe.hpp"
#include "myutil.hpp"

using namespace std;

const char GroupSeperator = '/';


void printBinary(std::ostream &str, unsigned char c)
{
  for(unsigned int i = 0; i < sizeof(unsigned char) * 8; i++) {
      str << (bool)(c & (1 << i));
  }
  str << " ";
}

void copyToClipboard(const QString &text)
{
  QClipboard *cb = QApplication::clipboard();
  cb->setText(text, QClipboard::Clipboard);
  if(cb->supportsSelection()) {
    cb->setText(text, QClipboard::Selection);
  }
}

/** Returns a group's name given its fullname.
 * If "/group/subgroup" is given, "subgroup" is returned.
 * @param group The group's fullname that may contain escaped characters.
 * @return The group's name.
 */
QString thisGroup(const QString &group)
{
  if(group.isEmpty())
    return QString::null;

  unsigned int i;

  for(i = (group.length() - 1); i != 0; i--) {
    if(group[i] == GroupSeperator) {
      if(i != 0 && group[i-1] == '\\') {
	continue;
      }
      else {
	break;
      }
    }
  }

  if(group[i] == GroupSeperator)
    i++;

  return group.right(group.length() - i);
}

/** Takes a group's fullname and returns its parent's fullname.
 * A fullname looks like a path, ie: "/group/subgroup/deep\ down\/ here".
 * "/group/subgroup" would be returned.
 * @param group A group's fullname that may contain escaped characters.
 * @return The group's parent's fullname.
 */
QString parentGroup(const QString &group)
{
  unsigned int i;

  for(i = group.length() - 1; i != 0; i--) {
    if(group[i] == GroupSeperator) {
      if(i != 0 && group[i-1] == '\\') {
	continue;
      }
      else {
	break;
      }
    }
  }

  QString ret(group);
  ret.truncate(i);

  DBGOUT(group << " -> " << ret);
  return ret;
}

QString escapeGroup(const QString &g)
{
  if(g.isEmpty())
    return QString::null;

  QString ret;
  unsigned int length = g.length();
  for(unsigned int i = 0; i < length; i++) {
    if(g[i] == GroupSeperator ||
       g[i] == '\\')
      ret += '\\';
    ret += g[i];
  }

  return ret;
}

QString unescapeGroup(const QString &g)
{
  if(g.isEmpty())
    return QString::null;

  DBGOUT("unescapeGroup(\"" << g << "\")");

  QString ret;
  unsigned int length = g.length();
  for(unsigned int i = 0; i < length; i++) {
    if(g[i] == '\\') {
      i++;
      if(i == length)
	throw EscapeException();
      else if(g[i] != GroupSeperator &&
	      g[i] != '\\')
	throw EscapeException();
    }
    ret += g[i];
  }
  return ret;
}

EscapeException::EscapeException()
  : Exception("invalid escape sequence")
{
}
