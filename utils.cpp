#include <vector>

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
