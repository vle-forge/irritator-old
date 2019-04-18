// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "private.hpp"

namespace irr {

bool
Context::init(int verbose_level) noexcept
{
    cfile_logger = stdout;

    if (verbose_level != 6)
        log_priority = static_cast<Context::message_type>(
          verbose_level < 0 ? 0 : verbose_level > 7 ? 7 : verbose_level);

    return true;
}

bool
Context::init(FILE* f, int verbose_level) noexcept
{
    cfile_logger = f;

    if (verbose_level != 6)
        log_priority = static_cast<Context::message_type>(
          verbose_level < 0 ? 0 : verbose_level > 7 ? 7 : verbose_level);

    return true;
}

} // namespace irr
