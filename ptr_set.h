#ifndef PTR_SET_H
#define PTR_SET_H

#include "set.h"
#include "state.h"

template <class T> class PtrSet : public Set<T>
{
  public:
    PtrSet(string name, vector<T> items) : Set<T>(name, items)
    {
    }

    PtrSet(vector<T> items) : Set<T>(items)
    {
    }

    PtrSet()
    {
        this->name = "";
        this->items = vector<T>();
    }

    bool has_item(T item)
    {
        for (T i : this->items)
        {
            if (*i == *item)
            {
                return true;
            }
        }
        return false;
    }

    // <typename U = remove_pointer<T>::type> T get_item(T item)
    // {
    //     auto value_items = vector<U>();
    //     transform(this->items.begin(), this->items.end(), value_items.begin(), [=](T i) { return *i; });
    //     return find(value_items.begin(), value_items.end(), *item);
    // }
};

#endif