#include <string>
#include <vector>
#include <qapplication.h>
#include <qclipboard.h>
#include "myutil.hpp"

using namespace std;

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
  for(int i = 0; i < sizeof(unsigned char) * 8; i++) {
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
