#ifndef SET_H
#define SET_H

#include "state.h"
#include "utils.h"
#include <algorithm>
#include <string>
#include <vector>

using namespace std;

template <class T> class Set
{
  protected:
    string name;
    vector<T> items;
    bool marked = false;

  public:
    Set(string name, vector<T> items) : name(name), items(items)
    {
    }

    Set(vector<T> items) : items(items)
    {
    }

    Set()
    {
        name = "";
        items = vector<T>();
    }

    string get_name()
    {
        return name;
    }

    void set_name(string new_name)
    {
        name = new_name;
    }

    vector<T> get_items()
    {
        return items;
    }

    void add(T item)
    {
        items.push_back(item);
    }

    bool is_marked()
    {
        return marked;
    }

    void mark()
    {
        marked = true;
    }

    void unmark()
    {
        marked = false;
    }

    bool empty()
    {
        return items.empty();
    }

    int size()
    {
        return items.size();
    }

    bool has_item(T item)
    {
        return is_item_in_vector(item, items);
    }

    T get_item(T item)
    {
        return find(items.begin(), items.end(), item);
    }

    bool operator==(Set &s)
    {
        bool equal = true;
        if (size() == s.size())
        {
            auto other_items = s.get_items();
            auto self_items = items;
            sort(other_items.begin(), other_items.end());
            sort(self_items.begin(), self_items.end());
            for (int i = 0; i < self_items.size(); i++)
            {
                // TODO Define operator !=
                if (!(self_items[i] == other_items[i]))
                {
                    equal = false;
                    break;
                }
            }
        }
        else
        {
            equal = false;
        }
        return equal;
    }

    bool operator<(Set &s)
    {
        if (stoi(name) < stoi(s.get_name()))
        {
            return true;
        }
        return false;
    }
};

template <class T> Set<T> set_union(Set<T> set_a, Set<T> set_b)
{
    vector<T> output;
    std::set_union(set_a.get_items().begin(), set_a.get_items().end(), set_b.get_items().begin(),
                   set_b.get_items().end(), back_inserter(output));
    return Set<T>(output);
}
template <class T> Set<T> set_intersec(Set<T> set_a, Set<T> set_b)
{
    vector<T> output;
    std::set_intersection(set_a.get_items().begin(), set_a.get_items().end(), set_b.get_items().begin(),
                          set_b.get_items().end(), back_inserter(output));
    return Set<T>(output);
}
template <class T> Set<T> set_diff(Set<T> set_a, Set<T> set_b)
{
    vector<T> output;
    std::set_difference(set_a.get_items().begin(), set_a.get_items().end(), set_b.get_items().begin(),
                        set_b.get_items().end(), back_inserter(output));
    return Set<T>(output);
}

#endif