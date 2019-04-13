// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <irritator/data-array.hpp>
#include <irritator/linker.hpp>

#include <cassert>

static void
check_data_array()
{
    struct position
    {
        float x;
    };

    irr::data_array<position, irr::IDs> array;

    assert(array.items == nullptr);
    assert(array.max_size == 0);
    assert(array.max_used == 0);
    assert(array.capacity == 0);
    assert(array.next_key == 1);
    assert(array.free_head == -1);

    bool is_init = array.init(3);

    assert(array.items != nullptr);
    assert(array.max_size == 0);
    assert(array.max_used == 0);
    assert(array.capacity == 3);
    assert(array.next_key == 1);
    assert(array.free_head == -1);

    assert(is_init);

    {
        auto& first = array.alloc();
        first.x = 0.f;
        assert(array.max_size == 1);
        assert(array.max_used == 1);
        assert(array.capacity == 3);
        assert(array.next_key == 2);
        assert(array.free_head == -1);

        auto& second = array.alloc();
        assert(array.max_size == 2);
        assert(array.max_used == 2);
        assert(array.capacity == 3);
        assert(array.next_key == 3);
        assert(array.free_head == -1);
        second.x = 1.f;

        auto& third = array.alloc();
        assert(array.max_size == 3);
        assert(array.max_used == 3);
        assert(array.capacity == 3);
        assert(array.next_key == 4);
        assert(array.free_head == -1);
        third.x = 2.f;

        assert(array.full());
    }

    array.clear();

    assert(array.items == nullptr);
    assert(array.max_size == 0);
    assert(array.max_used == 0);
    assert(array.capacity == 0);
    assert(array.next_key == 1);
    assert(array.free_head == -1);

    is_init = array.init(3);

    {
        auto& d1 = array.alloc();
        d1.x = 1.f;
        auto& d2 = array.alloc();
        d2.x = 2.f;
        auto& d3 = array.alloc();
        d3.x = 3.f;

        assert(array.items);
        assert(array.max_size == 3);
        assert(array.max_used == 3);
        assert(array.capacity == 3);
        assert(array.next_key == 4);
        assert(array.free_head == -1);
        array.free(d1);

        assert(array.items);
        assert(array.max_size == 2);
        assert(array.max_used == 3);
        assert(array.capacity == 3);
        assert(array.next_key == 4);
        assert(array.free_head == 0);
        array.free(d2);

        assert(array.items);
        assert(array.max_size == 1);
        assert(array.max_used == 3);
        assert(array.capacity == 3);
        assert(array.next_key == 4);
        assert(array.free_head == 1);

        array.free(d3);
        assert(array.items);
        assert(array.max_size == 0);
        assert(array.max_used == 3);
        assert(array.capacity == 3);
        assert(array.next_key == 4);
        assert(array.free_head == 2);

        auto& n1 = array.alloc();
        auto& n2 = array.alloc();
        auto& n3 = array.alloc();

        assert(irr::get_index(array.get_id(n1)) == 2);
        assert(irr::get_index(array.get_id(n2)) == 1);
        assert(irr::get_index(array.get_id(n3)) == 0);

        assert(array.items);
        assert(array.max_size == 3);
        assert(array.max_used == 3);
        assert(array.capacity == 3);
        assert(array.next_key == 7);
        assert(array.free_head == -1);
    }
}

int
main(int /* argc */, char* /* argv */[])
{
    check_data_array();

    return 0;
}
