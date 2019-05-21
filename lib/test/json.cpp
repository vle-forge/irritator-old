// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <irritator/modeling.hpp>
#include <irritator/simulation.hpp>

#include <cassert>

static void
check_json_01()
{
    irr::VLE vle;

    std::filesystem::path example{ EXAMPLES_DIR };
    example /= "example-01.json";

    irr::Model model(4096);
    model.read(vle.context, example);

    assert(model.name == "name");
    assert(model.author == "me");
    assert(model.version_major == 1);
    assert(model.version_minor == 2);
    assert(model.version_patch == 3);

    assert(model.conditions.size() == 7);

    irr::Condition* cnd = nullptr;
    int read = 0;
    while (model.conditions.next(cnd)) {
        if (cnd->name == "x") {
            assert(cnd->type == irr::Condition::condition_type::integer32);
            ++read;
        }
        if (cnd->name == "y") {
            assert(cnd->type == irr::Condition::condition_type::real64);
            ++read;
        }
        if (cnd->name == "z") {
            assert(cnd->type == irr::Condition::condition_type::integer32);
            ++read;
        }
        if (cnd->name == "x2") {
            assert(cnd->type == irr::Condition::condition_type::string);
            ++read;
        }
        if (cnd->name == "y2") {
            assert(cnd->type == irr::Condition::condition_type::string);
            ++read;
        }
        if (cnd->name == "z2") {
            assert(cnd->type == irr::Condition::condition_type::string);
            ++read;
        }
    }

    assert(read == 6);
}

int
main(int /* argc */, char* /* argv */ [])
{
    check_json_01();

    return 0;
}
