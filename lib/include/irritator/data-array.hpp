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

using ID = std::uint32_t;
using WID = std::uint64_t;

template<typename Identifier>
struct data_list;

constexpr int
get_index(WID id) noexcept
{
    return static_cast<int>(id & 0x00000000ffffffff);
}

constexpr unsigned
get_key(WID id) noexcept
{
    return static_cast<unsigned>((id & 0xffffffff00000000) >> 32);
}

constexpr int
get_index(ID id) noexcept
{
    return static_cast<int>(id & 0x0000ffff);
}

constexpr unsigned
get_key(ID id) noexcept
{
    return id & 0xffff0000;
}

template<typename T>
constexpr unsigned
get_max_key() noexcept;

template<>
constexpr unsigned
get_max_key<WID>() noexcept
{
    return static_cast<unsigned>(0xffffffff00000000 >> 32);
}

template<>
constexpr unsigned
get_max_key<ID>() noexcept
{
    return static_cast<unsigned>(0xffff0000 >> 16);
}

template<typename T>
constexpr int
size() noexcept;

template<>
constexpr int
size<WID>() noexcept
{
    return INT32_MAX;
}

template<>
constexpr int
size<ID>() noexcept
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
constexpr WID
make_id<WID>(unsigned key, int index) noexcept
{
    WID id = key;
    id <<= 32;
    return id | index;
}

template<>
constexpr ID
make_id<ID>(unsigned key, int index) noexcept
{
    return (key << 16 | index);
}

template<typename T>
constexpr unsigned
make_next_key(unsigned key) noexcept;

template<>
constexpr unsigned
make_next_key<WID>(unsigned key) noexcept
{
    return key == get_max_key<WID>() ? 1u : key + 1;
}

template<>
constexpr unsigned
make_next_key<ID>(unsigned key) noexcept
{
    return key == get_max_key<ID>() ? 1u : key + 1;
}

template<typename T>
struct array
{
    using this_type = array<T>;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using size_type = int;

    value_type* items = nullptr;
    int size = 0;

    array(int capacity)
      : items(new value_type[capacity])
      , size(capacity)
    {}

    bool init(int size_) noexcept
    {
        if (size_ < 0)
            return false;

        if (items)
            delete[] items;

        size = size_;
        items = new value_type[size_];
    }

    ~array() noexcept
    {
        if (items)
            delete[] items;
    }

    value_type& operator[](int i) noexcept
    {
        assert(i >= 0 && i < size);

        return items[i];
    }

    const value_type& operator[](int i) const noexcept
    {
        assert(i >= 0 && i < size);

        return items[i];
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
    T& alloc() noexcept;

    /* alloc and construct an item from freeList or items[max_used++], sets id
       to (next_key++ << 16) | index
     */
    template<typename... Args>
    T& alloc(Args&&... args) noexcept;

    // puts entry on free list (uses id to store next)
    void free(T&) noexcept;

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
Do_alloc(T& /*t*/, std::true_type) noexcept
{}

template<typename T, typename Identifier>
void
Do_alloc(T& t, std::false_type) noexcept
{
    new (&t) T();
}

template<typename T, typename Identifier>
T&
data_array<T, Identifier>::alloc() noexcept
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
template<typename... Args>
T&
data_array<T, Identifier>::alloc(Args&&... args) noexcept
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

    new (&items[new_index].item) T(std::forward<Args>(args)...);

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
Do_free(T& /*t*/, std::true_type) noexcept
{}

template<typename T, typename Identifier>
void
Do_free(T& t, std::false_type) noexcept
{
    // Will catch all exception?

    t.~T();
}

template<typename T, typename Identifier>
void
data_array<T, Identifier>::free(T& t) noexcept
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
