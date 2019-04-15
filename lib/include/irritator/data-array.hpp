// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ORG_VLEPROJECT_IRRITATOR_DATA_ARRAY_HPP
#define ORG_VLEPROJECT_IRRITATOR_DATA_ARRAY_HPP

#include <algorithm>
#include <type_traits>
#include <vector>

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace irr {

using ListID = int;
using ListIDw = long int;
using ID = std::uint64_t;
using IDs = std::uint32_t;

constexpr static ListID Invalid_ListID = -1;
constexpr static ListIDw Invalid_ListIDw = -1l;
constexpr static ID Invalid_ID = 0u;
constexpr static IDs Invalid_IDs = 0ul;

constexpr int
get_index(ID id) noexcept
{
    return static_cast<int>(id & 0x00000000ffffffff);
}

constexpr unsigned
get_key(ID id) noexcept
{
    return static_cast<unsigned>((id & 0xffffffff00000000) >> 32);
}

constexpr int
get_index(IDs id) noexcept
{
    return static_cast<int>(id & 0x0000ffff);
}

constexpr unsigned
get_key(IDs id) noexcept
{
    return id & 0xffff0000;
}

template<typename T>
constexpr unsigned
get_max_key() noexcept;

template<>
constexpr unsigned
get_max_key<ID>() noexcept
{
    return static_cast<unsigned>(0xffffffff00000000 >> 32);
}

template<>
constexpr unsigned
get_max_key<IDs>() noexcept
{
    return static_cast<unsigned>(0xffff0000 >> 16);
}

template<typename T>
constexpr int
size() noexcept;

template<>
constexpr int
size<ID>() noexcept
{
    return INT32_MAX;
}

template<>
constexpr int
size<IDs>() noexcept
{
    return UINT16_MAX;
}

template<typename Identifier>
constexpr bool
valid(Identifier id) noexcept
{
    return get_key(id) > 0;
}

template<typename T>
constexpr T
make_id(unsigned key, int index) noexcept;

template<>
constexpr ID
make_id<ID>(unsigned key, int index) noexcept
{
    ID id = key;
    id <<= 32;
    return id | index;
}

template<>
constexpr IDs
make_id<IDs>(unsigned key, int index) noexcept
{
    return (key << 16 | index);
}

template<typename T>
constexpr unsigned
make_next_key(unsigned key) noexcept;

template<>
constexpr unsigned
make_next_key<ID>(unsigned key) noexcept
{
    return key == get_max_key<ID>() ? 1u : key + 1;
}

template<>
constexpr unsigned
make_next_key<IDs>(unsigned key) noexcept
{
    return key == get_max_key<IDs>() ? 1u : key + 1;
}

template<typename DataArray, typename ListIdentifier, int nb_element = 1>
struct data_list
{
    static_assert(std::is_integral<ListIdentifier>::value,
                  "data_list needs a integer as list identifier");
    static_assert(std::is_signed<ListIdentifier>::value,
                  "data_array needs a signed as list identifier");
    static_assert(nb_element >= 1, "data_array needs positive nb_element");

    using this_type = data_list<DataArray, ListIdentifier, nb_element>;
    using value_type = typename DataArray::value_type;
    using identifier_type = typename DataArray::identifier_type;
    using list_identifier_type = ListIdentifier;

    struct item
    {
        list_identifier_type next = { -1 };
        identifier_type item[nb_element] = {};
    };

    class iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;

        using pointer = typename DataArray::value_type*;
        using reference = typename DataArray::value_type&;

    private:
        this_type* list = nullptr;
        DataArray* dataarray = nullptr;
        list_identifier_type list_id = -1;
        int element = 0;

    public:
        iterator() noexcept = default;
        iterator(const iterator&) noexcept = default;
        iterator& operator=(const iterator&) noexcept = default;

        iterator(this_type* list_,
                 DataArray* dataarray_,
                 list_identifier_type& list_id_) noexcept
          : list(list_)
          , dataarray(dataarray_)
        {
            while (list_id_ >= 0) {
                for (int i = 0; i < nb_element; ++i) {
                    if (dataarray->try_to_get(list->items[list_id_].item[i])) {
                        element = i;
                        list_id = list_id_;
                        return;
                    } else {
                        list->items[list_id_].item[i] = { 0 };
                    }
                }

                list_identifier_type to_delete = list_id_;
                list_id_ = list->items[list_id_].next;

                list->items[to_delete].next = list->free_head;
                list->free_head = to_delete;
            }

            list_id = list_id_;
            list = nullptr;
            dataarray = nullptr;
            list_id = -1;
            element = 0;
        }

        iterator(iterator&& other) noexcept
          : list(other.list)
          , dataarray(other.dataarray)
          , list_id(other.list_id)
          , element(other.element)
        {
            other.list = nullptr;
            other.dataarray = nullptr;
            other.list_id = -1;
        }

        iterator& operator=(iterator&& other) noexcept
        {
            if (this != &other) {
                list = other.list;
                dataarray = other.dataarray;
                list_id = other.list_id;
                element = other.element;
                other.list = nullptr;
                other.dataarray = nullptr;
                other.list_id = { -1 };
                other.element = 0;
            }

            return *this;
        }

        /**
         * @brief Advance the ID to the next valid ID in the linked list.
         *
         * @details If a element is invalid, this element is removed from
         * the list.
         */
        void advance() noexcept
        {
            if (!list || list_id < 0) {
                list = nullptr;
                dataarray = nullptr;
                list_id = -1;
                element = 0;
                return;
            }

            auto previous_list_id = -1;
            while (list_id >= 0) {
                for (int i = element + 1; i < nb_element; ++i) {
                    if (dataarray->try_to_get(list->items[list_id].item[i])) {
                        element = i;
                        return;
                    } else {
                        list->items[list_id].item[i] = { 0 };
                    }
                }

                // We need to destroy the current item from the linked list. We
                // use the previous_list_id to determine whether we destroy the
                // element: we do not have access to the user list identifier,
                // we can only remove internal element in the list.

                if (element == -1) {
                    if (previous_list_id >= 0) {
                        auto to_delete = list_id;
                        list_id = list->items[list_id].next;
                        list->items[previous_list_id].next = list_id;
                        list->items[to_delete].next = list->free_head;
                        list->free_head = to_delete;
                    }
                } else {
                    previous_list_id = list_id;
                    list_id = list->items[list_id].next;

                    element = -1; // For the next loop, we will start to the
                                  // 0th element in the sub-vector.
                }
            }

            /* If the iterator can not advance to the next valid element,
             * iterator must be equal to end().
             */

            list = nullptr;
            dataarray = nullptr;
            list_id = -1;
            element = 0;
        }

        iterator operator++(int)
        {
            iterator copy(*this);
            advance();
            return copy;
        }

        iterator operator++()
        {
            advance();
            return *this;
        }

        pointer operator->() const noexcept
        {
            return dataarray->try_to_get(list->items[list_id].item[element]);
        }

        reference operator*() const noexcept
        {
            auto* ptr =
              dataarray->try_to_get(list->items[list_id].item[element]);
            assert(ptr);
            return *ptr;
        }

        friend void swap(iterator& lhs, iterator& rhs) noexcept
        {
            auto copy = lhs;

            lhs.list = rhs.list;
            lhs.dataarray = rhs.dataarray;
            lhs.list_id = rhs.list_id;
            lhs.element = rhs.element;

            rhs.list = copy.list;
            rhs.dataarray = copy.dataarray;
            rhs.list_id = copy.list_id;
            rhs.element = copy.element;
        }

        friend bool operator==(const iterator& lhs,
                               const iterator& rhs) noexcept
        {
            return lhs.list == rhs.list && lhs.dataarray == rhs.dataarray &&
                   lhs.list_id == rhs.list_id && lhs.element == rhs.element;
        }

        friend bool operator!=(const iterator& lhs,
                               const iterator& rhs) noexcept
        {
            return !(lhs == rhs);
        }
    };

    DataArray* data = nullptr;
    item* items = nullptr;
    int max_size = 0;
    int max_used = 0;
    int capacity = 0;
    int free_head = -1;

    data_list() noexcept = default;
    ~data_list() noexcept
    {
        if (items)
            delete[] items;
    }

    bool init(DataArray* data_, int capacity_) noexcept
    {
        if (capacity_ <= 0)
            return false;

        clear();

        items = new item[capacity_];
        data = data_;

        return true;
    }

    void clear() noexcept
    {
        delete[] items;
        items = nullptr;
        data = nullptr;

        max_size = 0;
        max_used = 0;
        capacity = 0;
        free_head = -1;
    }

    iterator begin(list_identifier_type& list_id) noexcept
    {
        return iterator(this, data, list_id);
    }

    iterator end() noexcept
    {
        return iterator();
    }

    list_identifier_type alloc()
    {
        list_identifier_type new_index;

        if (free_head >= 0) {
            new_index = free_head;
            free_head = items[free_head].next;
        } else {
            new_index = max_used++;
        }

        for (int i = 0; i < nb_element; ++i)
            items[new_index].item[i] = { 0 };

        items[new_index].next = { -1 };

        ++max_size;

        return new_index;
    }

    void emplace(list_identifier_type& list_id,
                 const value_type& elem) noexcept
    {
        emplace(list_id, data->get_id(elem));
    }

    void emplace(list_identifier_type& list_id, identifier_type id) noexcept
    {
        if (list_id < 0) {
            list_id = alloc();
            items[list_id].item[0] = id;
            return;
        }

        for (int i = 0; i < nb_element; ++i) {
            if (items[list_id].item[i] == 0) {
                items[list_id].item[i] = id;
                return;
            }
        }

        auto new_list_id = alloc();
        items[new_list_id].item[0] = id;
        items[new_list_id].next = list_id;

        list_id = new_list_id;
    }

    void destroy(list_identifier_type& list_id)
    {
        while (list_id >= 0) {
            auto next = items[list_id].next;
            items[list_id].next = free_head;
            free_head = list_id;
            list_id = next;
        }
    }
};

/**
 * @brief An optimized fixed size array for dynamics objects.
 * @details Handles everything from any trivial, pod or object.
 * - linear memory/iteration
 * - O(1) alloc/free
 * - stable indices
 * - weak references
 * - zero overhead derefs
 *
 * @tparam T The type of object the data_array holds.
 */
template<typename T, typename Identifier>
struct data_array
{
    static_assert(std::is_default_constructible<T>::value,
                  "data_array needs a default constructor");
    static_assert(std::is_nothrow_default_constructible<T>::value,
                  "data_array needs a nothrow default constructor");
    static_assert(std::is_nothrow_destructible<T>::value,
                  "data_array needs a nothrow destructor");

    using identifier_type = Identifier;
    using value_type = T;

    struct item
    {
        T item;
        Identifier id;
    };

    data_array() = default;
    ~data_array();

    /** Allocate a vector of items (max 65536 items).
     *
     * @return true if success, false if capacity is not in [0..65536[.
     */
    bool init(int capacity_);

    /** Resets data members, (runs destructors* on outstanding items,
     * *optional
     */
    void clear();

    /* alloc (memclear* and/or construct*, *optional) an item from
       freeList or items[max_used++], sets id to (next_key++ << 16) | index
     */
    T& alloc();

    template<typename... Args>
    T& alloc(Args&&... args);

    // puts entry on free list (uses id to store next)
    void free(T&);

    void free(Identifier id);

    // accessor to the id part if Item
    Identifier get_id(const T&);

    T& get(Identifier id);             // return item[id & 0xFFFF];
    const T& get(Identifier id) const; // return item[id & 0xFFFF];

    /**
     * @brief Get a T from an ID.
     *
     * @details Validates ID, then returns item, returns null if invalid.
     * For cases like AI references and others where 'the thing might have
     * been deleted out from under me.
     */
    T* try_to_get(Identifier id);

    /**
     * @brief Return next item where id & 0xFFFF0000 != 0 (ie items not on
     * free list).
     *
     * @param  [description]
     * @return [description]
     */
    bool next(T*&);

    bool full() const noexcept;

    int size() const noexcept;

    item* items = nullptr;     // items vector.
    int max_size = 0;          // total size
    int max_used = 0;          // highest index ever allocated
    int capacity = 0;          // num allocated items
    unsigned int next_key = 1; // [1..2^16] (don't let == 0)
    int free_head = -1;        // index of first free entry
};

template<typename T, typename Identifier>
data_array<T, Identifier>::~data_array()
{
    if (items)
        delete[] items;
}

template<typename T, typename Identifier>
bool
data_array<T, Identifier>::init(int capacity_)
{
    clear();

    if (capacity_ < 0 || capacity_ > irr::size<ID>())
        return false;

    items = new item[capacity_ * sizeof(item)];
    max_size = 0;
    max_used = 0;
    capacity = capacity_;
    next_key = 1;
    free_head = -1;

    return true;
}

template<typename T, typename Identifier>
void
Do_clear(typename data_array<T, Identifier>::item* items,
         const int size,
         std::true_type)
{
    std::memset(
      items, 0, sizeof(typename data_array<T, Identifier>::item) * size);
}

template<typename T, typename Identifier>
void
Do_clear(typename data_array<T, Identifier>::item* items,
         const int size,
         std::false_type)
{
    for (int i = 0; i != size; ++i) {
        if (valid(items[i].id)) {
            items[i].item.~T();
            items[i].id = 0;
        }
    }
}

template<typename T, typename Identifier>
void
data_array<T, Identifier>::clear()
{
    Do_clear<T, Identifier>(items, max_size, std::is_trivial<T>());

    delete[] items;
    items = nullptr;
    max_size = 0;
    max_used = 0;
    capacity = 0;
    next_key = 1;
    free_head = -1;
}

template<typename T, typename Identifier>
void
Do_alloc(T& /*t*/, std::true_type)
{
    // std::memset(&t, 0, sizeof(T));
}

template<typename T, typename Identifier>
void
Do_alloc(T& t, std::false_type)
{
    new (&t) T();
}

template<typename T, typename Identifier>
T&
data_array<T, Identifier>::alloc()
{
    int new_index;

    if (free_head >= 0) {
        new_index = free_head;
        if (valid(items[free_head].id))
            free_head = -1;
        else
            free_head = get_index(items[free_head].id);
    } else {
        new_index = max_used++;
    }

    Do_alloc<T, Identifier>(items[new_index].item, std::is_trivial<T>());

#if 0
    printf("new index: %d next key: %u and ID: %lu\n",
           new_index,
           next_key,
           static_cast<long unsigned int>(
             make_id<Identifier>(next_key, new_index)));
#endif

    items[new_index].id = make_id<Identifier>(next_key, new_index);
    next_key = make_next_key<Identifier>(next_key);

    ++max_size;

    return items[new_index].item;
}

template<typename T, typename Identifier>
void
Do_free(T& /*t*/, std::true_type)
{
    // Really reset memory?
    // std::memset(&t, 0, sizeof(T));
}

template<typename T, typename Identifier>
void
Do_free(T& t, std::false_type)
{
    // Will catch all exception?

    t.~T();
}

template<typename T, typename Identifier>
void
data_array<T, Identifier>::free(T& t)
{
    auto id = get_id(t);
    auto index = get_index(id);

    assert(&items[index] == static_cast<void*>(&t));
    assert(items[index].id == id);
    assert(valid(id));

    Do_free<T, Identifier>(items[index].item, std::is_trivial<T>());

    items[index].id = free_head;
    free_head = index;

    --max_size;
}

template<typename T, typename Identifier>
void
data_array<T, Identifier>::free(Identifier id)
{
    auto index = get_index(id);

    assert(items[index].id == id);
    assert(valid(id));

    Do_free<T, Identifier>(items[index].item, std::is_trivial<T>());

    items[index].id = free_head;
    free_head = index;

    --max_size;
}

template<typename T, typename Identifier>
T&
data_array<T, Identifier>::get(Identifier id)
{
    return items[get_index(id)].item;
}

template<typename T, typename Identifier>
const T&
data_array<T, Identifier>::get(Identifier id) const
{
    return items[get_index(id)].item;
}

template<typename T, typename Identifier>
Identifier
data_array<T, Identifier>::get_id(const T& t)
{
    auto* ptr = reinterpret_cast<const item*>(&t);
    return ptr->id;
}

template<typename T, typename Identifier>
T*
data_array<T, Identifier>::try_to_get(Identifier id)
{
    if (get_key(id)) {
        auto index = get_index(id);
        if (items[index].id == id)
            return &items[index].item;
    }

    return nullptr;
}

template<typename T, typename Identifier>
bool
data_array<T, Identifier>::next(T*& t)
{
    int index;

    if (t) {
        auto id = get_id(*t);
        index = get_index(id);
        ++index;

        for (; index < max_used; ++index) {
            if (valid(items[index].id)) {
                t = &items[index].item;
                return true;
            }
        }
    } else {
        for (index = 0; index < max_used; ++index) {
            if (valid(items[index].id)) {
                t = &items[index].item;
                return true;
            }
        }
    }

    return false;
}

template<typename T, typename Identifier>
bool
data_array<T, Identifier>::full() const noexcept
{
    return free_head == -1 && max_used == capacity;
}

template<typename T, typename Identifier>
int
data_array<T, Identifier>::size() const noexcept
{
    return max_size;
}

} // irr

#endif // ORG_VLEPROJECT_IRRITATOR_DATA_ARRAY_HPP
