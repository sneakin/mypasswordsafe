#include <string>
#include <vector>
#include <qapplication.h>
#include <qclipboard.h>
#include <qstring.h>
#include "safe.hpp"
#include "myutil.hpp"

using namespace std;

static const char GroupSeperator = '/';


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

/** Searches for a group given by its fullname, and creates it if it
 * isn't found.
 * @param safe The safe that is being searched/modified.
 * @group_name The group's fullname which may contain escaped characters.
 * @return The group. It'll be newly created if it isn't found.
 */
SafeGroup *findOrCreateGroup(Safe *safe, const QString &group_name)
{
  DBGOUT("findOrCreateGroup");

  if(safe == NULL)
    return NULL;

  // if the group's name is empty,
  if(group_name.isEmpty()) {
    DBGOUT("Group name is empty");
    return NULL;
  }
  // if name has zero seperators or
  else {
    QString group_path(group_name);

    if(group_path.at(0) == '/') {
      for(unsigned int i = 0; i < group_path.length(); i++) {
	if(group_path.at(i) != '/') {
	  group_path.remove(0, i);
	  break;
	}
      }
      DBGOUT("Group path: " << group_path);
    }

    SafeGroup *group = findGroup(safe, group_path);
    if(group != NULL) {
      DBGOUT("Found group " << group_path);
      return group;
    }

#ifdef DEBUG
    if(!group_path.isEmpty())
      DBGOUT(group_path);
#endif

    QString this_group = unescapeGroup(thisGroup(group_path));
    QString parent_group = parentGroup(group_path);
    DBGOUT("adding group: " << this_group << " to " << parent_group);

    if(parent_group.isEmpty()) {
      return new SafeGroup(safe, this_group);
    }
    else {
      DBGOUT("parent = " << parent_group);
      // search for the parent
      // if it doesn't exist add it
      SafeGroup *parent = findOrCreateGroup(safe, parent_group);
      return new SafeGroup(parent, this_group);
    }
  }

  return NULL;
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

/** Finds the subgroup in group by full name.
 * @param group the group to search
 * @param full_group The group's fullname that may contain escaped characters.
 * @return The found group or NULL if it wasn't found.
 */
SafeGroup *findGroup(SafeGroup *group, const QString &full_group)
{
  if(group == NULL || full_group.isEmpty())
    return NULL;

  DBGOUT("findGroup: " << full_group);

  QString group_name;
  unsigned int index = 0;
  for(; index < full_group.length(); index++) {
    if(full_group.at(index) != GroupSeperator)
      break;
  }

  // get the first section
  for(; index < full_group.length(); index++) {
    if(full_group.at(index) == GroupSeperator) {
      if(index > 0 && full_group.at(index - 1) != '\\')
	break;
    }
    group_name += full_group.at(index);
  }

  group_name = unescapeGroup(group_name);
  //DBGOUT("\tName: " << group_name);

  SafeGroup::Iterator it(group);
  while(it.current()) {
    if(it.current()->rtti() == SafeGroup::RTTI) {
      SafeGroup *temp = (SafeGroup *)it.current();
      if(temp->name() == group_name) {
	if(index == full_group.length())
	  return temp;

	QString child_group = full_group.right(full_group.length() - (index + 1));
	return findGroup(temp, child_group);
      }
#ifdef DEBUG
      else {
	if(!group_name.isEmpty())
	  DBGOUT(temp->name() << " != " << group_name);
      }
#endif
    }
    ++it;
  }

  return NULL;
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
