// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <irritator/data-array.hpp>
#include <irritator/data-list.hpp>
#include <irritator/linker.hpp>
#include <irritator/string.hpp>

#include <string>

#include <fmt/format.h>

#include "catch.hpp"

TEST_CASE("check irr::string api", "[lib/string]")
{
    using my_string = irr::string<8>;

    my_string toto;
    REQUIRE(toto.empty());
    REQUIRE(toto.size() == 0);
    REQUIRE(toto.max_size() == 8 - 1);
    REQUIRE(toto == "");

    toto.fill(' ');
    REQUIRE(!toto.empty());
    REQUIRE(toto.size() == 7);
    REQUIRE(toto.max_size() == 8 - 1);
    REQUIRE(toto == "       ");

    toto = "test";
    REQUIRE(!toto.empty());
    REQUIRE(toto.size() == 4);
    REQUIRE(toto.max_size() == 8 - 1);
    REQUIRE(toto != "");
    REQUIRE(toto == "test");

    toto = "1234567890";
    REQUIRE(!toto.empty());
    REQUIRE(toto.size() == 7);
    REQUIRE(toto.max_size() == 8 - 1);
    REQUIRE(toto != "test");
    REQUIRE(toto == "1234567");

    my_string a("aa"), b("bb"), ab("ab");
    REQUIRE(a < b);
    REQUIRE(ab < b);
    REQUIRE(ab > a);
}

TEST_CASE("check irr::data_array api", "[lib/container]")
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

    REQUIRE(array.items == nullptr);
    REQUIRE(array.max_size == 0);
    REQUIRE(array.max_used == 0);
    REQUIRE(array.capacity == 0);
    REQUIRE(array.next_key == 1);
    REQUIRE(array.free_head == -1);

    bool is_init = array.init(3);

    REQUIRE(array.items != nullptr);
    REQUIRE(array.max_size == 0);
    REQUIRE(array.max_used == 0);
    REQUIRE(array.capacity == 3);
    REQUIRE(array.next_key == 1);
    REQUIRE(array.free_head == -1);

    REQUIRE(is_init);

    {
        auto& first = array.alloc();
        first.x = 0.f;
        REQUIRE(array.max_size == 1);
        REQUIRE(array.max_used == 1);
        REQUIRE(array.capacity == 3);
        REQUIRE(array.next_key == 2);
        REQUIRE(array.free_head == -1);

        auto& second = array.alloc();
        REQUIRE(array.max_size == 2);
        REQUIRE(array.max_used == 2);
        REQUIRE(array.capacity == 3);
        REQUIRE(array.next_key == 3);
        REQUIRE(array.free_head == -1);
        second.x = 1.f;

        auto& third = array.alloc();
        REQUIRE(array.max_size == 3);
        REQUIRE(array.max_used == 3);
        REQUIRE(array.capacity == 3);
        REQUIRE(array.next_key == 4);
        REQUIRE(array.free_head == -1);
        third.x = 2.f;

        REQUIRE(array.full());
    }

    array.clear();

    REQUIRE(array.items == nullptr);
    REQUIRE(array.max_size == 0);
    REQUIRE(array.max_used == 0);
    REQUIRE(array.capacity == 0);
    REQUIRE(array.next_key == 1);
    REQUIRE(array.free_head == -1);

    is_init = array.init(3);

    {
        auto& d1 = array.alloc(1.f);
        auto& d2 = array.alloc(2.f);
        auto& d3 = array.alloc(3.f);

        REQUIRE(array.items);
        REQUIRE(array.max_size == 3);
        REQUIRE(array.max_used == 3);
        REQUIRE(array.capacity == 3);
        REQUIRE(array.next_key == 4);
        REQUIRE(array.free_head == -1);
        array.free(d1);

        REQUIRE(array.items);
        REQUIRE(array.max_size == 2);
        REQUIRE(array.max_used == 3);
        REQUIRE(array.capacity == 3);
        REQUIRE(array.next_key == 4);
        REQUIRE(array.free_head == 0);
        array.free(d2);

        REQUIRE(array.items);
        REQUIRE(array.max_size == 1);
        REQUIRE(array.max_used == 3);
        REQUIRE(array.capacity == 3);
        REQUIRE(array.next_key == 4);
        REQUIRE(array.free_head == 1);

        array.free(d3);
        REQUIRE(array.items);
        REQUIRE(array.max_size == 0);
        REQUIRE(array.max_used == 3);
        REQUIRE(array.capacity == 3);
        REQUIRE(array.next_key == 4);
        REQUIRE(array.free_head == 2);

        auto& n1 = array.alloc();
        auto& n2 = array.alloc();
        auto& n3 = array.alloc();

        REQUIRE(irr::get_index(array.get_id(n1)) == 2);
        REQUIRE(irr::get_index(array.get_id(n2)) == 1);
        REQUIRE(irr::get_index(array.get_id(n3)) == 0);

        REQUIRE(array.items);
        REQUIRE(array.max_size == 3);
        REQUIRE(array.max_used == 3);
        REQUIRE(array.capacity == 3);
        REQUIRE(array.next_key == 7);
        REQUIRE(array.free_head == -1);
    }
}

TEST_CASE("check irr::data_list api", "[lib/container]")
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
        REQUIRE(y_array.try_to_get(elem) != nullptr);
    }

    REQUIRE(size == 3);
    size = 0;

    {
        auto it = x1.y.begin(links);
        auto et = x1.y.end(links);
        REQUIRE(it != et);

        while (it != et) {
            ++size;
            ++it;
        }

        REQUIRE(size == 3);
        --it;
        REQUIRE(et != it);
        REQUIRE(y_array.try_to_get(*it) != nullptr);

        --it;
        REQUIRE(et != it);
        REQUIRE(y_array.try_to_get(*it) != nullptr);

        --it;
        REQUIRE(et != it);
        REQUIRE(y_array.try_to_get(*it) != nullptr);
        REQUIRE(x1.y.begin(links) == it);
    }

    y_array.free(y1);
    y_array.free(y2);

    x1.y.clear(links, [&y_array](auto id) {
        return y_array.try_to_get(id) == nullptr;
    });

    size = 0;
    for (auto elem : x1.y(links)) {
        size++;
        REQUIRE(y_array.try_to_get(elem) != nullptr);
    }

    REQUIRE(size == 1);
}

TEST_CASE("check irr::linker api", "[lib/container]")
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

    REQUIRE(pos.items != nullptr);
    REQUIRE(pos.max_size == 0);
    REQUIRE(pos.max_used == 0);
    REQUIRE(pos.capacity == 3);
    REQUIRE(pos.next_key == 1);
    REQUIRE(pos.free_head == -1);
    REQUIRE(dirs.items != nullptr);
    REQUIRE(dirs.max_size == 0);
    REQUIRE(dirs.max_used == 0);
    REQUIRE(dirs.capacity == 4);
    REQUIRE(dirs.next_key == 1);
    REQUIRE(dirs.free_head == -1);

    pos.alloc().x = 1.f;
    pos.alloc().x = 2.f;
    pos.alloc().x = 3.f;

    dirs.alloc().d = 0;
    dirs.alloc().d = 1;
    dirs.alloc().d = 2;
    dirs.alloc().d = 4;

    REQUIRE(pos.items != nullptr);
    REQUIRE(pos.max_size == 3);
    REQUIRE(pos.max_used == 3);
    REQUIRE(pos.capacity == 3);
    REQUIRE(pos.next_key == 4);
    REQUIRE(pos.free_head == -1);
    REQUIRE(dirs.items != nullptr);
    REQUIRE(dirs.max_size == 4);
    REQUIRE(dirs.max_used == 4);
    REQUIRE(dirs.capacity == 4);
    REQUIRE(dirs.next_key == 5);
    REQUIRE(dirs.free_head == -1);

    REQUIRE(single.size() == 10);
    single.emplace(pos.get_id(pos.get(0)), dirs.get_id(dirs.get(0)));
    single.emplace(pos.get_id(pos.get(0)), dirs.get_id(dirs.get(1)));
    single.emplace(pos.get_id(pos.get(1)), dirs.get_id(dirs.get(2)));
    single.emplace(pos.get_id(pos.get(2)), dirs.get_id(dirs.get(3)));
    REQUIRE(single.size() == 10);

    REQUIRE(single[pos.get_id(pos.get(0))] == dirs.get_id(dirs.get(1)));
    REQUIRE(single[pos.get_id(pos.get(1))] == dirs.get_id(dirs.get(2)));
    REQUIRE(single[pos.get_id(pos.get(2))] == dirs.get_id(dirs.get(3)));
}
