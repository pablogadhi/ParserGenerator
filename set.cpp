#include "set.h"
#include "state.h"
#include "utils.h"
#include <algorithm>

template <class T> Set<T>::Set(string name, vector<T> items) : name(name), items(items)
{
}

template <class T> Set<T>::Set(vector<T> items) : items(items)
{
}

template <class T> Set<T>::Set()
{
    name = "";
    items = vector<T>();
}

template <class T> string Set<T>::get_name()
{
    return name;
}

template <class T> void Set<T>::set_name(string new_name)
{
    name = new_name;
}

template <class T> vector<T> Set<T>::get_items()
{
    return items;
}

template <class T> void Set<T>::add(T item)
{
    items.push_back(item);
}

template <class T> bool Set<T>::is_marked()
{
    return marked;
}

template <class T> void Set<T>::mark()
{
    marked = true;
}

template <class T> void Set<T>::unmark()
{
    marked = false;
}

template <class T> bool Set<T>::empty()
{
    return items.empty();
}

template <class T> int Set<T>::size()
{
    return items.size();
}

template <class T> bool Set<T>::has_item(T item)
{
    return is_item_in_vector(item, items);
}

template <class T> bool Set<T>::operator==(Set &s)
{
    bool equal = true;
    if (size() == s.size())
    {
        auto other_items = s.get_items();
        auto self_items = items;
        sort(other_items.begin(), other_items.end());
        sort(self_items.begin(), self_items.end());
        for (int i = 0; i < items.size(); i++)
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

template <class T> bool Set<T>::operator<(Set &s)
{
    if (stoi(name) < stoi(s.get_name()))
    {
        return true;
    }
    return false;
}

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

// TODO Remove this function
template <class T> bool Set<T>::is_equal(Set<T> other_set)
{
    auto diff = set_diff(*this, other_set);
    if (diff.empty())
    {
        return true;
    }
    return false;
}

template <class T> bool Set<T>::is_in(Set<Set<T>> other_set)
{
    for (auto &item : other_set.get_items())
    {
        if (*this == item)
        {
            return true;
        }
    }
    return false;
}

template class Set<State *>;
template class Set<Set<State *>>;
template class Set<Set<State *> *>;