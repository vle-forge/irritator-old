// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <irritator/simulation.hpp>

struct Ball
{
    double start(double t)
    {
        return t;
    }
    double transition(double t)
    {
        return t;
    }
    void output()
    {}
};

int
main()
{}
