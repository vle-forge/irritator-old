// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

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

    {
        auto* name = model.names.try_to_get(model.name);
        assert(name);
        assert(irr::is_string_equal(*name, "name"));
    }

    {
        auto* name = model.names.try_to_get(model.author);
        assert(name);
        assert(irr::is_string_equal(*name, "me"));
    }

    assert(model.version_major == 1);
    assert(model.version_minor == 2);
    assert(model.version_patch == 3);
}

int
main(int /* argc */, char* /* argv */ [])
{
    check_json_01();

    return 0;
}
