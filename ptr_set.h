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

    T has_item(T item)
    {
        for (T i : this->items)
        {
            if (*i == *item)
            {
                return i;
            }
        }
        return nullptr;
    }

    bool operator==(PtrSet &s)
    {
        bool equal = true;
        if (this->size() == s.size())
        {
            auto other_items = s.get_items();
            auto self_items = this->items;
            sort(other_items.begin(), other_items.end());
            sort(self_items.begin(), self_items.end());
            for (int i = 0; i < self_items.size(); i++)
            {
                // TODO Define operator !=
                if (!(*self_items[i] == *other_items[i]))
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

    bool operator<(PtrSet &s)
    {
        if (stoi(this->get_name()) < stoi(*s.get_name()))
        {
            return true;
        }
        return false;
    }
};

#endif