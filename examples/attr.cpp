#include <iostream>
#include <string>

using namespace std;

#define ATTRIBUTE(type, name) \
public: \
inline const type &name() const { return m_##name; } \
inline void set_##name(const type &v) { m_##name = v; } \
private: \
type m_##name

class Test
{
  ATTRIBUTE(int, age);
  ATTRIBUTE(string, name);

public:
  Test(int age, const char *name)
    : m_age(age), m_name(name)
  {
  }
};

int main()
{
  Test t(20, "Hello");
  int age;
  cin >> age;
  string name;
  cin >> name;

  t.set_age(age);
  t.set_name(name);
  cout << t.age() << "\t" << t.name() << endl;

  return 0;
}
