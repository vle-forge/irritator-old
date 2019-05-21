// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ORG_VLEPROJECT_IRRITATOR_DATA_LIST_HPP
#define ORG_VLEPROJECT_IRRITATOR_DATA_LIST_HPP

#include <algorithm>
#include <type_traits>
#include <vector>

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace irr {

template<typename Identifier>
class id_list;

template<typename Identifier>
class data_list;

using ListID = id_list<std::uint32_t>;
using ListWID = id_list<std::uint64_t>;

template<typename Identifier>
class id_list_iterator
{
public:
    using this_type = id_list_iterator<Identifier>;
    using identifier_type = Identifier;
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using pointer = identifier_type*;
    using reference = identifier_type&;

    using DataList = data_list<identifier_type>;
    using IDList = id_list<identifier_type>;

private:
    static constexpr int current_exceed_end = -2;
    static constexpr int current_exceed_first = -3;

    DataList* m_data_list = nullptr;
    IDList* m_id_list = nullptr;
    int m_current = current_exceed_end;

public:
    id_list_iterator() noexcept = default;
    ~id_list_iterator() noexcept = default;
    id_list_iterator(const id_list_iterator&) noexcept = default;
    id_list_iterator& operator=(const id_list_iterator&) noexcept = default;

    id_list_iterator(id_list_iterator&& other) noexcept
      : m_data_list(other.m_data_list)
      , m_id_list(other.m_id_list)
      , m_current(other.m_current)
    {
        other.m_data_list = nullptr;
        other.m_id_list = nullptr;
        other.m_current = current_exceed_end;
    }

    id_list_iterator& operator=(id_list_iterator&& other) noexcept
    {
        if (this != &other) {
            m_data_list = other.m_data_list;
            m_id_list = other.m_id_list;
            m_current = other.m_current;
            other.m_data_list = nullptr;
            other.m_id_list = nullptr;
            other.m_current = current_exceed_end;
        }

        return *this;
    }

    id_list_iterator(DataList* data_list, IDList* id_list) noexcept
      : m_data_list(data_list)
      , m_id_list(id_list)
      , m_current(id_list->first)
    {}

    id_list_iterator(DataList& data_list, IDList* id_list) noexcept
      : m_data_list(&data_list)
      , m_id_list(id_list)
      , m_current(id_list->first)
    {}

    id_list_iterator(DataList* data_list,
                     IDList* id_list,
                     int current) noexcept
      : m_data_list(data_list)
      , m_id_list(id_list)
      , m_current(current)
    {}

    id_list_iterator(DataList& data_list,
                     IDList* id_list,
                     int current) noexcept
      : m_data_list(&data_list)
      , m_id_list(id_list)
      , m_current(current)
    {}

    void advance() noexcept
    {
        if (m_current == current_exceed_first) {
            m_current = m_id_list->m_first;
            if (m_current == -1)
                m_current = current_exceed_first;
        } else if (m_current != current_exceed_end) {
            m_current = m_data_list->items[m_current].next;
            if (m_current == -1)
                m_current = current_exceed_end;
        }
    }

    void back() noexcept
    {
        if (m_current == current_exceed_end) {
            m_current = m_id_list->m_last;
            if (m_current == -1)
                m_current = current_exceed_end;
        } else if (m_current != current_exceed_first) {
            m_current = m_data_list->items[m_current].previous;
            if (m_current == -1)
                m_current = current_exceed_first;
        }
    }

    id_list_iterator operator++(int) noexcept
    {
        id_list_iterator copy(*this);
        advance();
        return copy;
    }

    id_list_iterator operator++() noexcept
    {
        advance();
        return *this;
    }

    id_list_iterator operator--(int) noexcept
    {
        id_list_iterator copy(*this);
        back();
        return copy;
    }

    id_list_iterator operator--() noexcept
    {
        back();
        return *this;
    }

    pointer operator->() const noexcept
    {
        return &m_data_list->items[m_current].id;
    }

    reference operator*() const noexcept
    {
        return m_data_list->items[m_current].id;
    }

    friend void swap(id_list_iterator& lhs, id_list_iterator& rhs) noexcept
    {
        auto copy = lhs;

        lhs.m_data_list = rhs.m_data_list;
        lhs.m_id_list = rhs.m_id_list;
        lhs.m_current = rhs.m_current;

        rhs.m_data_list = copy.m_data_list;
        rhs.m_id_list = copy.m_id_list;
        rhs.m_current = copy.m_current;
    }

    friend bool operator==(const id_list_iterator& lhs,
                           const id_list_iterator& rhs) noexcept
    {
        return lhs.m_data_list == rhs.m_data_list &&
               lhs.m_id_list == rhs.m_id_list &&
               lhs.m_current == rhs.m_current;
    }

    friend bool operator!=(const id_list_iterator& lhs,
                           const id_list_iterator& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    friend class id_list<identifier_type>;
    friend class data_list<identifier_type>;
};

template<typename Identifier>
class id_list_range;

template<typename Identifier>
class id_list
{
public:
    using this_type = id_list<Identifier>;
    using identifier_type = Identifier;
    using iterator = id_list_iterator<identifier_type>;
    using const_iterator = id_list_iterator<identifier_type>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reference = identifier_type&;
    using pointer = identifier_type*;
    using const_reference = identifier_type;
    using const_pointer = identifier_type*;

private:
    int m_first = -1;
    int m_last = -1;

    friend id_list_iterator<identifier_type>;

public:
    id_list() noexcept = default;

    id_list(int first, int last) noexcept
      : m_first(first)
      , m_last(last)
    {}

    reference front(const data_list<identifier_type>& list) noexcept
    {
        return list[m_first].id;
    }

    const_reference front(const data_list<identifier_type>& list) const
      noexcept
    {
        return list[m_first].id;
    }

    reference back(const data_list<identifier_type>& list) noexcept
    {
        return list[m_last].id;
    }

    const_reference back(const data_list<identifier_type>& list) const noexcept
    {
        return list[m_last].id;
    }

    void push_front(data_list<identifier_type>& list,
                    Identifier value) noexcept
    {
        if (m_first >= 0) {
            auto new_index = list.alloc(value);
            list[new_index].next = m_first;
            list[m_first].previous = new_index;
            m_first = new_index;
        } else {
            m_first = m_last = list.alloc(value);
        }
    }

    void push_back(data_list<identifier_type>& list, Identifier value) noexcept
    {
        if (m_last >= 0) {
            auto new_index = list.alloc(value);
            list.items[new_index].previous = m_last;
            list.items[m_last].next = new_index;
            m_last = new_index;
        } else {
            m_last = m_first = list.alloc(value);
        }
    }

    void pop_front(data_list<identifier_type>& list) noexcept
    {
        if (m_first >= 0)
            do_pop_front(list);
    }

    void pop_back(data_list<identifier_type>& list) noexcept
    {
        if (m_last >= 0)
            do_pop_back(list);
    }

    iterator erase(data_list<identifier_type>& list, iterator it) noexcept
    {
        return do_erase(list, it.m_current);
    }

    int size(data_list<identifier_type>& list) const noexcept
    {
        int ret = 0;

        for (auto i = m_first; i != -1; i = list.items[i].next)
            ++ret;

        return ret;
    }

    void clear(data_list<identifier_type>& list) noexcept
    {
        for (auto i = m_first; i != -1; i = list.items[i].next)
            free(i);

        m_first = -1;
        m_last = -1;
    }

    template<typename Function>
    void clear(data_list<identifier_type>& list, Function fct) noexcept
    {
        for (auto it = begin(list); it != end(list); ++it)
            if (fct(*it))
                it = erase(list, it);
    }

    id_list_range<identifier_type> operator()(
      data_list<identifier_type>& list) noexcept
    {
        return id_list_range(list, this);
    }

    id_list_range<identifier_type> operator()(
      const data_list<identifier_type>& list) const noexcept
    {
        return id_list_range(list, this);
    }

    bool empty() const noexcept
    {
        return m_first == -1;
    }

    iterator begin(data_list<identifier_type>& list) noexcept
    {
        return iterator(list, this, m_first);
    }

    iterator end(data_list<identifier_type>& list) noexcept
    {
        return iterator(list, this, iterator::current_exceed_end);
    }

    const_iterator begin(const data_list<identifier_type>& list) const noexcept
    {
        return const_iterator(list, this, m_first);
    }

    const_iterator end(const data_list<identifier_type>& list) const noexcept
    {
        return const_iterator(list, this, iterator::current_exceed_end);
    }

    const_iterator cbegin(const data_list<identifier_type>& list) const
      noexcept
    {
        return const_iterator(list, this, m_first);
    }

    const_iterator cend(const data_list<identifier_type>& list) const noexcept
    {
        return const_iterator(list, this, iterator::current_exceed_end);
    }

    reverse_iterator rbegin(data_list<identifier_type>& list) noexcept
    {
        return reverse_iterator(list, this, m_last);
    }

    reverse_iterator rend(data_list<identifier_type>& list) noexcept
    {
        return reverse_iterator(list, this, iterator::current_exceed_first);
    }

    const_reverse_iterator rbegin(const data_list<identifier_type>& list) const
      noexcept
    {
        return const_reverse_iterator(list, this, m_last);
    }

    const_reverse_iterator rend(const data_list<identifier_type>& list) const
      noexcept
    {
        return const_reverse_iterator(
          list, this, iterator::current_exceed_first);
    }

    const_reverse_iterator crbegin(
      const data_list<identifier_type>& list) const noexcept
    {
        return const_reverse_iterator(list, this, m_last);
    }

    const_reverse_iterator crend(const data_list<identifier_type>& list) const
      noexcept
    {
        return const_reverse_iterator(
          list, this, iterator::current_exceed_first);
    }

private:
    void do_pop_front(data_list<identifier_type>& list) noexcept
    {
        auto to_delete = m_first;
        m_first = list.items[to_delete].next;
        if (m_first == -1) {
            m_last = -1;
        } else {
            list.items[m_first].previous = -1;
        }
        list.free(to_delete);
    }

    void do_pop_back(data_list<identifier_type>& list) noexcept
    {
        auto to_delete = m_last;
        m_last = list.items[to_delete].previous;
        if (m_last == -1) {
            m_first = -1;
        } else {
            list.items[m_last].next = -1;
        }
        list.free(to_delete);
    }

    iterator do_erase(data_list<identifier_type>& list, int index) noexcept
    {
        if (m_first == index) {
            do_pop_front(list);
            return iterator(list, this, iterator::current_exceed_first);
        }

        if (m_last == index) {
            do_pop_back(list);
            return iterator(list, this, iterator::current_exceed_end);
        }

        auto previous = list.items[index].previous;
        auto next = list.items[index].next;
        auto current = previous;

        if (previous != -1) {
            list.items[previous].next = next;
            if (m_first == previous)
                m_first = next;
        }

        if (next != -1) {
            list.items[next].previous = previous;
            if (m_last == next)
                m_last = previous;
        }

        list.free(index);

        return iterator(list, this, current == -1 ? m_first : current);
    }
};

// A convenient class to be used in ranged-base loop
template<typename Identifier>
class id_list_range
{
public:
    using this_type = id_list<Identifier>;
    using identifier_type = Identifier;
    using iterator = id_list_iterator<identifier_type>;

private:
    iterator m_first, m_last;

public:
    id_list_range(data_list<identifier_type>& list,
                  id_list<identifier_type>* idlist)
      : m_first(idlist->begin(list))
      , m_last(idlist->end(list))
    {}

    iterator begin() noexcept
    {
        return m_first;
    }

    iterator end() noexcept
    {
        return m_last;
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
 * @tparam T The type of object the id_list holds.
 */
template<typename Identifier>
class data_list
{
public:
    static_assert(std::is_default_constructible<Identifier>::value,
                  "data_list needs a default constructor");
    static_assert(std::is_nothrow_default_constructible<Identifier>::value,
                  "data_list needs a nothrow default constructor");
    static_assert(std::is_nothrow_destructible<Identifier>::value,
                  "data_list needs a nothrow destructor");

    using this_type = data_list<Identifier>;
    using identifier_type = Identifier;
    using reference = identifier_type&;
    using pointer = identifier_type*;

    friend id_list_iterator<identifier_type>;
    friend id_list<identifier_type>;

private:
    struct item
    {
        identifier_type id = 0;
        int previous = -1;
        int next = -1;

        item() = default;

        item(identifier_type id_)
          : id(id_)
        {}

        item(identifier_type id_, int previous_, int next_)
          : id(id_)
          , previous(previous_)
          , next(next_)
        {}

        void swap(item& other) noexcept
        {
            identifier_type copy = id;
            id = other.id;
            other.id = copy;
        }

        template<typename DataArray>
        bool valid(DataArray& data_array)
        {
            return data_array.valid(id);
        }
    };

public:
    data_list() noexcept = default;

    ~data_list() noexcept
    {
        delete[] items;
    }

    bool init(int capacity_)
    {
        if (capacity_ <= 0)
            return false;
        clear();

        items = new item[capacity_];
        capacity = capacity_;
        free_head = -1;

        return true;
    }

    void clear()
    {
        delete[] items;
        items = nullptr;

        max_size = 0;
        max_used = 0;
        capacity = 0;
        free_head = -1;
    }

    int alloc(identifier_type id) noexcept
    {
        int new_index;

        if (free_head >= 0) {
            new_index = free_head;
            free_head = items[free_head].next;
        } else {
            new_index = max_used++;
        }

        items[new_index].id = id;
        items[new_index].previous = -1;
        items[new_index].next = -1;

        ++max_size;

        return new_index;
    }

    void free(int index) noexcept
    {
        items[index].id = 0;
        items[index].previous = -1;
        items[index].next = free_head;

        free_head = index;

        --max_size;
    }

    bool full() const noexcept;

    int size() const noexcept;

private:
    item* items = nullptr; // items vector.
    int max_size = 0;      // total size
    int max_used = 0;      // highest index ever allocated
    int capacity = 0;      // num allocated items
    int free_head = -1;    // index of first free entry
};

} // irr

#endif
