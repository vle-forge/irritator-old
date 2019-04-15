// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <irritator/data-array.hpp>
#include <irritator/linker.hpp>

#include <string>

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
check_data_list()
{
    struct x_position
    {
        float x = { 0.f };
        irr::ListID y = { -1 };
    };

    struct y_position
    {
        float y = { 0.f };
    };

    irr::data_array<x_position, irr::IDs> x_array;
    irr::data_array<y_position, irr::IDs> y_array;
    irr::data_list<irr::data_array<y_position, irr::IDs>, irr::ListID, 3>
      links;

    x_array.init(10);
    y_array.init(10);
    links.init(&y_array, 32);

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

    links.emplace(x1.y, y_array.get_id(y1));
    links.emplace(x1.y, y_array.get_id(y2));
    links.emplace(x1.y, y_array.get_id(y3));

    int size = 0;
    for (auto it = links.begin(x1.y), et = links.end(); it != et; ++it)
        ++size;
    assert(size == 3);

    y_array.free(y1);

    size = 0;
    for (auto it = links.begin(x1.y), et = links.end(); it != et; ++it)
        ++size;

    assert(size == 2);

    y_array.free(y2);

    size = 0;
    for (auto it = links.begin(x1.y), et = links.end(); it != et; ++it)
        ++size;

    assert(size == 1);

    y_array.free(y3);

    size = 0;
    for (auto it = links.begin(x1.y), et = links.end(); it != et; ++it)
        ++size;

    assert(size == 0);

    size = 0;
    for (auto it = links.begin(x1.y), et = links.end(); it != et; ++it)
        ++size;

    assert(size == 0);
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

struct Name
{
    char item[16];
};

void
affect_name(Name& name, const char* str) noexcept
{
    std::strncpy(name.item, str, std::size(name.item));
}

struct Node
{
    float x{ 0.f };
    float y{ 0.f };
    float height{ 0.f };
    float width{ 0.f };
};

struct Model
{
    irr::IDs name = irr::Invalid_IDs;
    irr::IDs gui = irr::Invalid_IDs;
    irr::IDs dynamics = irr::Invalid_IDs;
    irr::ListID conditions = irr::Invalid_ListID;
    irr::ListID observables = irr::Invalid_ListID;
    irr::ListID input_slots = irr::Invalid_ListID;
    irr::ListID output_slots = irr::Invalid_ListID;
};

struct InputSlot
{
    irr::IDs name;
    float x{ 0.f };
    float y{ 0.f };
    int index{ 0 };
};

struct OutputSlot
{
    irr::IDs name;
    float x{ 0.f };
    float y{ 0.f };
    int index{ 0 };
};

static void
check_structued_data()
{
    using names_type = irr::data_array<Name, irr::IDs>;
    using nodes_type = irr::data_array<Node, irr::IDs>;
    using models_type = irr::data_array<Model, irr::IDs>;
    using input_slots_type = irr::data_array<InputSlot, irr::IDs>;
    using output_slots_type = irr::data_array<OutputSlot, irr::IDs>;

    using input_slots_list_type =
      irr::data_list<input_slots_type, irr::ListID, 3>;
    using output_slots_list_type =
      irr::data_list<output_slots_type, irr::ListID, 3>;

    names_type names;
    nodes_type nodes;
    models_type models;
    input_slots_type input_slots;
    output_slots_type output_slots;
    input_slots_list_type input_slots_list;
    output_slots_list_type output_slots_list;

    names.init(1024);
    nodes.init(256);
    models.init(256);
    input_slots.init(256);
    output_slots.init(256);
    input_slots_list.init(&input_slots, 65535);
    output_slots_list.init(&output_slots, 65535);

    auto& mdl = models.alloc();

    auto& n1 = names.alloc();
    affect_name(n1, "in");

    auto& n2 = names.alloc();
    affect_name(n2, "out");

    auto& in = input_slots.alloc();
    in.name = names.get_id(n1);
    auto& out = output_slots.alloc();
    out.name = names.get_id(n2);

    input_slots_list.emplace(mdl.input_slots, in);
    output_slots_list.emplace(mdl.input_slots, out);
}

int
main(int /* argc */, char* /* argv */ [])
{
    check_data_array();
    check_data_list();
    check_linker();

    check_structued_data();

    return 0;
}
