#ifndef SET_H
#define SET_H

#include <string>
#include <vector>

using namespace std;

template <class T> class Set
{
  private:
    string name;
    vector<T> items;
    bool marked = false;

  public:
    Set(string, vector<T>);
    Set(vector<T>);
    Set();
    void set_name(string);
    string get_name();
    vector<T> get_items();
    void add(T);
    void mark();
    void unmark();
    bool is_marked();
    bool is_equal(Set<T>);
    bool is_in(Set<Set<T>>);
    bool empty();
    bool has_item(T);
    int size();
    bool operator==(Set &);
    bool operator<(Set &);
};

template <class T> Set<T> set_union(Set<T>, Set<T>);
template <class T> Set<T> set_intersec(Set<T>, Set<T>);
template <class T> Set<T> set_diff(Set<T>, Set<T>);

#endif