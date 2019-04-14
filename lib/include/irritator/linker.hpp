// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ORG_VLEPROJECT_IRRITATOR_LINKER_HPP
#define ORG_VLEPROJECT_IRRITATOR_LINKER_HPP

#include <vector>

#include <cassert>

namespace irr {

/**
 * @code
 * // ID reference an another ID data.
 * irr::linker <ID, ID> map_simple(size_t{200});
 * map_simple[0u] = 123u;
 * map_simple[1u] = 123u;
 * map_simple.destroy(0u);
 *
 * // ID reference an ID4 data.
 * irr::linker <ID, ID4> map(size_t{1024});
 * map[123u] = { 1u, 2u, 3u, 4u };
 * @endcode
 */
template<typename Identifier,
         typename Referenced,
         typename Allocator = std::allocator<Identifier>>
class linker
{
public:
    using identifier_type = Identifier;
    using referenced_type = Referenced;

private:
    std::vector<referenced_type, Allocator> items;

public:
    using container_type = std::vector<referenced_type, Allocator>;
    using value_type = typename container_type::value_type;
    using reference = typename container_type::reference;

    linker() = default;

    void init(int capacity)
    {
        assert(capacity > 0);

        items.resize(capacity, 0);
    }

    int size() const noexcept
    {
        return static_cast<int>(items.size());
    }

    void emplace(const identifier_type id, const referenced_type value) noexcept
    {
        assert(irr::valid(id));

        items[irr::get_index(id)] = value;
    }

    template<typename DataArray>
    typename DataArray::value_type* try_to_get(DataArray* array,
                                               identifier_type id) const
      noexcept
    {
        assert(irr::valid(id));

        return array->try_to_get(items[irr::get_index(id)]);
    }

    referenced_type operator[](const identifier_type id) const noexcept
    {
        assert(irr::valid(id));

        return items[irr::get_index(id)];
    }

    referenced_type& operator[](const identifier_type id) noexcept
    {
        assert(irr::valid(id));

        return items[irr::get_index(id)];
    }

    void destroy(const identifier_type id) noexcept
    {
        assert(irr::valid(id));

        items[irr::get_index(id)] = { 0 };
    }
};

template<typename T, typename Identifier>
struct data_array;

template<typename Referenced>
struct multi_linker_node
{
    multi_linker_node() = default;

    multi_linker_node(const Referenced id_, const int next_)
      : id(id_)
      , next(next_)
    {}

    Referenced id = { 0 };
    int next = { -1 }; // Next element is the (flat) linked list.
};

template<typename Identifier,
         typename Referenced,
         typename IdentifierAllocator = std::allocator<int>,
         typename NodeAllocator = std::allocator<multi_linker_node<Referenced>>>
class multi_linker
{
public:
    using this_type =
      multi_linker<Identifier, Referenced, IdentifierAllocator, NodeAllocator>;
    using identifier_type = Identifier;
    using referenced_type = Referenced;

private:
    /// Map ID to head in the linked list.
    std::vector<int, IdentifierAllocator> map;

    /// The (flat) linked list in contiguous container.
    std::vector<multi_linker_node<referenced_type>, NodeAllocator> list;

    /// The free linked list.
    int free_head = -1;

    template<typename DataArray>
    class view
    {
    public:
        class iterator
        {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = Identifier;
            using difference_type = std::ptrdiff_t;

            using pointer = typename DataArray::value_type*;
            using reference = typename DataArray::value_type&;

        private:
            this_type* list = nullptr;
            DataArray* dataarray = nullptr;
            int elem = -1;

        public:
            iterator() noexcept = default;
            iterator(const iterator&) noexcept = default;
            iterator& operator=(const iterator&) noexcept = default;

            iterator(this_type& list_,
                     DataArray& dataarray_,
                     int elem_) noexcept
              : list(&list_)
              , dataarray(&dataarray_)
              , elem(elem_)
            {
                // Build a valid first iterator
                while (elem >= 0) {
                    if (dataarray->try_to_get(list->list[elem].id))
                        break;

                    int to_delete = elem;
                    elem = list->list[elem].next;

                    list->list[to_delete].next = list->free_head;
                    list->free_head = to_delete;
                }
            }

            iterator(iterator&& other) noexcept
              : list(other.list)
              , dataarray(other.dataarray)
              , elem(other.elem)
            {
                other.list = nullptr;
                other.dataarray = nullptr;
                other.elem = -1;
            }

            iterator& operator=(iterator&& other) noexcept
            {
                if (this != &other) {
                    list = other.list;
                    dataarray = other.dataarray;
                    elem = other.elem;
                    other.list = nullptr;
                    other.dataarray = nullptr;
                    other.elem = { -1 };
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
                if (list && elem >= 0) {
                    int next = list->list[elem].next;
                    while (next >= 0) {
                        if (dataarray->try_to_get(list->list[next].id)) {
                            elem = next;
                            return;
                        } else {
                            list->list[elem].next = list->list[next].next;
                            list->list[next].next = list->free_head;
                            list->free_head = next;
                            next = list->list[elem].next;
                        }
                    }
                }
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
                return dataarray->try_to_get(list->list[elem].id);
            }

            reference operator*() const noexcept
            {
                auto* ptr = dataarray->try_to_get(list->list[elem].id);
                assert(ptr);
                return *ptr;
            }

            friend void swap(iterator& lhs, iterator& rhs) noexcept
            {
                auto items_ = lhs.items;
                auto id_ = lhs.id;
                lhs.items = rhs.items;
                lhs.id = rhs.id;
                rhs.items = items_;
                rhs.id = id_;
            }

            friend bool operator==(const iterator& lhs,
                                   const iterator& rhs) noexcept
            {
                return lhs.list == rhs.list && lhs.dataarray == rhs.dataarray &&
                       lhs.elem == rhs.elem;
            }

            friend bool operator!=(const iterator& lhs,
                                   const iterator& rhs) noexcept
            {
                return !(lhs == rhs);
            }
        };

    private:
        this_type& list;
        DataArray& dataarray;
        int elem = -1;

    public:
        view(this_type& list_, DataArray& dataarray_, Identifier elem_) noexcept
          : list(list_)
          , dataarray(dataarray_)
          , elem(elem_)
        {}

        iterator begin()
        {
            return iterator(list, dataarray, elem);
        }

        iterator end()
        {
            return iterator(list, dataarray, -1);
        }
    };

public:
    multi_linker() noexcept = default;

    void init(int capacity)
    {
        assert(capacity > 0);

        map.resize(capacity, 0);

        // The linked list starts at an empty state and grow automatically
        // except if at least one element is deleted before.
        list.reserve(capacity);

        free_head = -1;
    }

    void clear()
    {
        std::fill(std::begin(map), std::end(map), 0);
        list.clear();
        free_head = -1;
    }

    template<typename DataArray>
    view<DataArray> get_view(identifier_type id, DataArray& dataarray) noexcept
    {
        return view(*this, dataarray, id);
    }

    void emplace(Identifier ID, Referenced value) noexcept
    {
        assert(irr::valid(ID));

        auto index = irr::get_index(ID);

        int new_pos;
        if (free_head < 0) {
            list.emplace_back(value, -1);
            new_pos = static_cast<int>(list.size());
        } else {
            list[free_head].id = value;
            new_pos = free_head;
            free_head = list[free_head].next;
        }

        list[new_pos].next = map[index];
        map[index] = new_pos;
    }

    void destroy(Identifier ID)
    {
        assert(irr::valid(ID));

        auto index = irr::get_index(ID);
        auto id = map[index];
        map[index] = -1;

        while (id >= 0) {
            auto to_delete = id;
            index = list[id].next;

            list[to_delete].id = -1;
            list[to_delete].next = free_head;
            free_head = to_delete;
        }
    }
};

} // namespace irr

#endif // ORG_VLEPROJECT_IRRITATOR_LINKER_HPP
