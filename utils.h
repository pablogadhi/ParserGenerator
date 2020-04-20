#ifndef UTILS_H
#define UTILS_H

#include <vector>

namespace
{

template <class T> bool is_item_in_vector(T item, std::vector<T> vec)
{
    for (T i : vec)
    {
        if (i == item)
        {
            return true;
        }
    }
    return false;
}

template <class T> bool is_item_ptr_in_vector(T item, std::vector<T> vec)
{
    for (T i : vec)
    {
        if (*i == *item)
        {
            return true;
        }
    }
    return false;
}

} // namespace
#endif