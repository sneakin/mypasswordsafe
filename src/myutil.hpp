#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>
#include <vector>
#include <iostream>
#include "config.h"

using std::cerr;
using std::endl;
using std::string;
using std::vector;

class QString;

//#define DEBUG 1

#ifdef DEBUG
#define DBGOUT(str) cerr << __FILE__ << "(" << __LINE__ << ")\t" << str << endl
//#define DBGOUT(str) cerr << __func__ << "(" << __LINE__ << "): " << str << endl
//#define DBGOUT(str) cerr << __PRETTY_FUNCTION__ << ": " << str << endl
#else
#define DBGOUT(str)
#endif

string getExtension(const string &path);
vector<string> split(string &s, int c);
void printBinary(std::ostream &str, unsigned char c);
void copyToClipboard(const QString &str);
void trimRight(string &str);
void trimLeft(string &str);

class SafeGroup;
class Safe;
class QString;

SafeGroup *findOrCreateGroup(Safe *safe, const QString &group_name);
QString thisGroup(const QString &);
QString parentGroup(const QString &);
SafeGroup *findGroup(SafeGroup *group, const QString &full_group);

QString escapeGroup(const QString &);
QString unescapeGroup(const QString &);


#endif
