#include <iostream>
#include <qstring.h>

using namespace std;

const char GroupSeperator = '/';

#define DBGOUT(str) cout << str << endl

QString parseGroup(const QString &group)
  // This parses Password Safe's ridiculus group names.
  // '.' seperates groups, '\' is the escape sequence,
  // but only '.' and '\' need to be escaped.
{
  QString ret;
  for(unsigned int i = 0; i < group.length(); i++) {
    QChar c(group[i]);
    if(c == '\\') {
      i++;
      if(i == group.length())
	break;

      c = group[i];
      if(c == '.')
	ret += c;
      else if(c == '\\')
	ret += "\\\\";
    }
    else if(c == '.')
      ret += GroupSeperator;
    else if(c == GroupSeperator)
      ret += "\\/";
    else {
      ret += c;
    }
  }

  return ret;
}

QString readyGroup(const QString &group)
{
  QString ret;
  unsigned int i = 0;
  QChar c(group[i]);

  // skip the first seperator, and find the next one
  if(c == GroupSeperator) {
    for(; i < group.length(); i++) {
      c = group[i];
      if(c != GroupSeperator)
	break;
    }
  }

  for(;i < group.length(); i++) {
    c = group[i];
    // handle escapes
    if(c == '\\') {
      i++;
      if(i == group.length())
	break;

      c = group[i];
      if(c == GroupSeperator)
	ret += c;
      else
	ret += "\\\\";
    }
    else if(c == GroupSeperator)
      ret += '.';
    else if(c == '.')
      ret += "\\.";
    else {
      ret += c;
    }
  }

  DBGOUT("\"" << group << "\" = \"" << ret << "\"");
  return ret;
}

int main(int argc, char *argv[])
{
  char input[1024];
  QString group;

  cout << "Group: ";
  cin >> input;
  group = input;

  QString parsed(parseGroup(group));
  cout << "Parsed group: " << parsed << endl;
  QString ready(readyGroup(parsed));
  cout << "Ready group: " << ready << endl;

  cout << endl;
  if(group == ready)
    cout << "Success" << endl;
  else
    cout << "Failed" << endl;

  return 0;
}
