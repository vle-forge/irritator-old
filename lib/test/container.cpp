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

static void
check_linker()
{
    struct position
    {
        float x;
    };

    struct direction
    {
        int d;
    };

    irr::data_array<position, irr::IDs> pos;
    irr::data_array<direction, irr::IDs> dirs;
    irr::linker<irr::IDs, irr::IDs> single;
    pos.init(3);
    dirs.init(4);
    single.init(10);

    assert(pos.items != nullptr);
    assert(pos.max_size == 0);
    assert(pos.max_used == 0);
    assert(pos.capacity == 3);
    assert(pos.next_key == 1);
    assert(pos.free_head == -1);
    assert(dirs.items != nullptr);
    assert(dirs.max_size == 0);
    assert(dirs.max_used == 0);
    assert(dirs.capacity == 4);
    assert(dirs.next_key == 1);
    assert(dirs.free_head == -1);

    pos.alloc().x = 1.f;
    pos.alloc().x = 2.f;
    pos.alloc().x = 3.f;

    dirs.alloc().d = 0;
    dirs.alloc().d = 1;
    dirs.alloc().d = 2;
    dirs.alloc().d = 4;

    assert(pos.items != nullptr);
    assert(pos.max_size == 3);
    assert(pos.max_used == 3);
    assert(pos.capacity == 3);
    assert(pos.next_key == 4);
    assert(pos.free_head == -1);
    assert(dirs.items != nullptr);
    assert(dirs.max_size == 4);
    assert(dirs.max_used == 4);
    assert(dirs.capacity == 4);
    assert(dirs.next_key == 5);
    assert(dirs.free_head == -1);

    assert(single.size() == 10);
    single.emplace(pos.get_id(pos.get(0)), dirs.get_id(dirs.get(0)));
    single.emplace(pos.get_id(pos.get(0)), dirs.get_id(dirs.get(1)));
    single.emplace(pos.get_id(pos.get(1)), dirs.get_id(dirs.get(2)));
    single.emplace(pos.get_id(pos.get(2)), dirs.get_id(dirs.get(3)));
    assert(single.size() == 10);

    assert(single[pos.get_id(pos.get(0))] == dirs.get_id(dirs.get(1)));
    assert(single[pos.get_id(pos.get(1))] == dirs.get_id(dirs.get(2)));
    assert(single[pos.get_id(pos.get(2))] == dirs.get_id(dirs.get(3)));
}
int
main(int /* argc */, char* /* argv */[])
{
    check_data_array();
    check_linker();

    return 0;
}
