#include "common.h"
std::map<std::string, int> argcount;

int get_low(int a)
{
  int ans = -1;
  while (a > 0) {
    ans++;
    a /= 2;
  }
  return ans;
}

std::set<std::string> mySetUnion(const std::set<std::string>& s1,
				 const std::set<std::string>& s2)
{
  std::vector<std::string> v(s1.size() + s2.size());
  auto it = std::set_union(s1.begin(), s1.end(), s2.begin(), s2.end(), v.begin());
  return std::set<std::string>(v.begin(), it);
}

std::set<std::string> mySetDifference(const std::set<std::string>& s1,
				      const std::set<std::string>& s2)
{
  std::vector<std::string> v(s1.size());
  auto it = std::set_difference(s1.begin(), s1.end(), s2.begin(), s2.end(), v.begin());
  return std::set<std::string>(v.begin(), it);
}

std::set<std::string> mySetIntersect(const std::set<std::string>& s1,
				      const std::set<std::string>& s2)
{
  std::vector<std::string> v(s1.size());
  auto it = std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), v.begin());
  return std::set<std::string>(v.begin(), it);
}

std::set<int> mySetUnion(const std::set<int>& s1,
				 const std::set<int>& s2)
{
  std::vector<int> v(s1.size() + s2.size());
  auto it = std::set_union(s1.begin(), s1.end(), s2.begin(), s2.end(), v.begin());
  return std::set<int>(v.begin(), it);
}

std::set<int> mySetDifference(const std::set<int>& s1,
				      const std::set<int>& s2)
{
  std::vector<int> v(s1.size());
  auto it = std::set_difference(s1.begin(), s1.end(), s2.begin(), s2.end(), v.begin());
  return std::set<int>(v.begin(), it);
}

std::set<int> mySetIntersect(const std::set<int>& s1,
				      const std::set<int>& s2)
{
  std::vector<int> v(s1.size());
  auto it = std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), v.begin());
  return std::set<int>(v.begin(), it);
}
