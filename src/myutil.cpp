#include <string>
#include <vector>
#include <qapplication.h>
#include <qclipboard.h>
#include <qstring.h>
#include "safe.hpp"
#include "myutil.hpp"

using namespace std;

static const char GroupSeperator = '/';


vector<string> split(string &s, int c)
{
  vector<string> ret;
  string::iterator start = s.begin();
  string::iterator i = s.begin();
  for(;
      i != s.end();
      i++) {
    if(*i == c) {
      if(*start == c)
	start++;
      string item(start, i);
      ret.push_back(item);
      start = i;
    }
  }

  if(start != s.end() && start != i) {
    if(*start == c)
      start++;
    string item(start, i);
    ret.push_back(item);
  }

  return ret;
}

string getExtension(const string &path)
{
  string type(path);
  string::iterator i = type.end();
  for(; i != type.begin(); i--) {
    if(*i == '.')
      break;
  }

  if(i != type.begin()) {
    type.erase(type.begin(), (i+1));
    return type;
  }
  else
    return string("");
}

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

void trimRight(string &str)
{
  string::iterator i = str.end();
  for(; i != str.begin(); i--) {
    if(!isspace(*i) && *i != '\0')
      break;
  }
  str.erase(i+1, str.end());
}

void trimLeft(string &str)
{
  string::iterator i = str.begin();
  for(; i != str.end(); i++) {
    if(!isspace(*i))
      break;
  }
  str.erase(str.begin(), i);
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
  unsigned int i;

  DBGOUT("length: " << group.length());

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

  QString ret;
  for(; i != group.length(); i++) {
    if(group[i] == '\\') {
      i++;
      if(i != group.length()) {
	char c = group[i];
	if(c == '\\')
	  ret += '\\';
	else if(c == '/')
	  ret += '/';
      }
      continue;
    }

    ret += group[i];
  }
  return ret;
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
  return ret;
}

/** Finds the subgroup in group by full name.
 * @param group the group to search
 * @param full_group The group's fullname that may contain escaped characters.
 * @return The found group or NULL if it wasn't found.
 */
SafeGroup *findGroup(SafeGroup *group, const QString &full_group)
{
  DBGOUT("findGroup: " << full_group);

  if(!full_group.isEmpty()) {
    QString group_name;
    unsigned int index = 0;
    if(full_group.at(0) == GroupSeperator)
      index++;

    for(; index < full_group.length(); index++) {
      if(full_group.at(index) == GroupSeperator) {
	if(index > 0 && full_group.at(index - 1) != '\\')
	  break;
      }
      group_name += full_group.at(index);
    }

    group_name = unescapeGroup(group_name);
    DBGOUT("\tName: " << group_name);

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
	else {
	  DBGOUT(temp->name() << " != " << group_name);
	}
      }
      ++it;
    }
  }


  return NULL;
}

QString escapeGroup(const QString &g)
{
  QString ret(g);
  ret.replace('\\', "\\\\");
  ret.replace('/', "\\/");
  return ret;
}

QString unescapeGroup(const QString &g)
{
  QString ret(g);
  ret.replace("\\/", "/");
  ret.replace("\\\\", "\\");
  return ret;
}
