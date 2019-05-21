// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <irritator/data-array.hpp>
#include <irritator/data-list.hpp>
#include <irritator/linker.hpp>
#include <irritator/string.hpp>

#include <string>

#include <fmt/format.h>

#include <cassert>

static void
check_string()
{
    using my_string = irr::string<8>;

    my_string toto;
    assert(toto.empty());
    assert(toto.size() == 0);
    assert(toto.max_size() == 8 - 1);
    assert(toto == "");

    toto.fill(' ');
    assert(!toto.empty());
    assert(toto.size() == 7);
    assert(toto.max_size() == 8 - 1);
    assert(toto == "       ");

    toto = "test";
    assert(!toto.empty());
    assert(toto.size() == 4);
    assert(toto.max_size() == 8 - 1);
    assert(toto != "");
    assert(toto == "test");

    toto = "1234567890";
    assert(!toto.empty());
    assert(toto.size() == 7);
    assert(toto.max_size() == 8 - 1);
    assert(toto != "test");
    assert(toto == "1234567");

    my_string a("aa"), b("bb"), ab("ab");
    assert(a < b);
    assert(ab < b);
    assert(ab > a);
}

static void
check_data_array()
{
    struct position
    {
        position() = default;
        constexpr position(float x_)
          : x(x_)
        {}

        float x;
    };

    irr::data_array<position, irr::ID> array;

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
        auto& d1 = array.alloc(1.f);
        auto& d2 = array.alloc(2.f);
        auto& d3 = array.alloc(3.f);

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
check_data_list()
{
    struct x_position
    {
        float x = { 0.f };
        irr::ListID y;
    };

    struct y_position
    {
        float y = { 0.f };
    };

    irr::data_array<x_position, irr::ID> x_array;
    irr::data_array<y_position, irr::ID> y_array;
    irr::data_list<irr::ID> links;

    x_array.init(10);
    y_array.init(10);
    links.init(32);

    auto& x1 = x_array.alloc();
    x1.x = 1.f;
    auto& x2 = x_array.alloc();
    x2.x = 2.f;
    auto& x3 = x_array.alloc();
    x3.x = 3.f;

    auto& y1 = y_array.alloc();
    y1.y = 1.f;
    auto& y2 = y_array.alloc();
    y2.y = 2.f;
    auto& y3 = y_array.alloc();
    y3.y = 3.f;

    x1.y.push_back(links, y_array.get_id(y1));
    x1.y.push_back(links, y_array.get_id(y2));
    x1.y.push_back(links, y_array.get_id(y3));

    int size = 0;
    for (auto elem : x1.y(links)) {
        size++;
        assert(y_array.try_to_get(elem) != nullptr);
    }

    assert(size == 3);
    size = 0;

    {
        auto it = x1.y.begin(links);
        auto et = x1.y.end(links);
        assert(it != et);

        while (it != et) {
            ++size;
            ++it;
        }

        assert(size == 3);
        --it;
        assert(et != it);
        assert(y_array.try_to_get(*it) != nullptr);

        --it;
        assert(et != it);
        assert(y_array.try_to_get(*it) != nullptr);

        --it;
        assert(et != it);
        assert(y_array.try_to_get(*it) != nullptr);
        assert(x1.y.begin(links) == it);
    }

    y_array.free(y1);
    y_array.free(y2);

    x1.y.clear(links, [&y_array](auto id) {
        return y_array.try_to_get(id) == nullptr;
    });

    size = 0;
    for (auto elem : x1.y(links)) {
        size++;
        assert(y_array.try_to_get(elem) != nullptr);
    }

    assert(size == 1);

    //     int size = 0;
    // for (auto it = links.begin(x1.y), et = links.end(); it != et; ++it)
    //     ++size;
    // assert(size == 3);

    // y_array.free(y1);

    // size = 0;
    // for (auto it = links.begin(x1.y), et = links.end(); it != et; ++it)
    //     ++size;

    // assert(size == 2);

    // y_array.free(y2);

    // size = 0;
    // for (auto it = links.begin(x1.y), et = links.end(); it != et; ++it)
    //     ++size;

    // assert(size == 1);

    // y_array.free(y3);

    // size = 0;
    // for (auto it = links.begin(x1.y), et = links.end(); it != et; ++it)
    //     ++size;

    // assert(size == 0);

    // size = 0;
    // for (auto it = links.begin(x1.y), et = links.end(); it != et; ++it)
    //     ++size;

    // assert(size == 0);
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

    irr::data_array<position, irr::ID> pos;
    irr::data_array<direction, irr::ID> dirs;
    irr::linker<irr::ID, irr::ID> single;
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
main(int /* argc */, char* /* argv */ [])
{
    check_string();
    check_data_array();
    check_data_list();
    check_linker();

    return 0;
}
