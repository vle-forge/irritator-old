// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <irritator/modeling.hpp>
#include <irritator/simulation.hpp>

#include "catch.hpp"

TEST_CASE("check json api", "[lib/json]")
{
    irr::VLE vle;

    std::filesystem::path example{ EXAMPLES_DIR };
    example /= "example-01.json";

    irr::Model model(4096);
    auto ret = model.read(vle.context, example);
    REQUIRE(ret == irr::status::json_read_success);

    REQUIRE(model.name == "name");
    REQUIRE(model.author == "me");
    REQUIRE(model.version_major == 1);
    REQUIRE(model.version_minor == 2);
    REQUIRE(model.version_patch == 3);

    REQUIRE(model.conditions.size() == 7);

    irr::Condition* cnd = nullptr;
    int read = 0;
    while (model.conditions.next(cnd)) {
        if (cnd->name == "x") {
            REQUIRE(cnd->type == irr::Condition::condition_type::integer32);
            ++read;
        }
        if (cnd->name == "y") {
            REQUIRE(cnd->type == irr::Condition::condition_type::real64);
            ++read;
        }
        if (cnd->name == "z") {
            REQUIRE(cnd->type == irr::Condition::condition_type::integer32);
            ++read;
        }
        if (cnd->name == "x2") {
            REQUIRE(cnd->type == irr::Condition::condition_type::string);
            ++read;
        }
        if (cnd->name == "y2") {
            REQUIRE(cnd->type == irr::Condition::condition_type::string);
            ++read;
        }
        if (cnd->name == "z2") {
            REQUIRE(cnd->type == irr::Condition::condition_type::string);
            ++read;
        }
    }

    REQUIRE(read == 6);
}
