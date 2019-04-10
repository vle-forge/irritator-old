// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ORG_VLEPROJECT_IRRITATOR_SOLVER_PRIVATE_HPP
#define ORG_VLEPROJECT_IRRITATOR_SOLVER_PRIVATE_HPP

#include <irritator/simulation.hpp>

#include <fmt/color.h>
#include <fmt/format.h>

#include <cstdio>

#define irr_stringify_detail(x) #x
#define irr_stringify(x) irr_stringify_detail(x)

#if defined(__clang__) || defined(__GNUC__)
#define irr_always_inline __attribute__((always_inline))
#define irr_likely(x) __builtin_expect(!!(x), 1)
#define irr_unlikely(x) __builtin_expect(!!(x), 0)
#else
#define irr_always_inline
#define irr_likely(x) (!!(x))
#define irr_unlikely(x) (!!(x))
#endif

namespace irr {

static const fmt::text_style irr_message_style[] = {
    (fmt::emphasis::bold | fmt::fg(fmt::terminal_color::red)), // emerg
    (fmt::emphasis::bold | fmt::fg(fmt::terminal_color::red)), // alert
    (fmt::emphasis::bold | fmt::fg(fmt::terminal_color::red)), // crit
    (fmt::fg(fmt::terminal_color::red)),                       // err
    (fmt::fg(fmt::terminal_color::yellow)),                    // warning
    (fmt::fg(fmt::terminal_color::white)),                     // notice
    (fmt::fg(fmt::terminal_color::white)),                     // info
    (fmt::fg(fmt::terminal_color::blue))                       // debug
};

inline bool
is_loggable(Context::message_type current_level,
            Context::message_type level) noexcept
{
    return static_cast<int>(current_level) >= static_cast<int>(level);
}

namespace detail {

template<typename... Args>
void
log(const Context& ctx,
    Context::message_type level,
    const char* fmt,
    const Args&... args)
{
    if (!is_loggable(ctx.log_priority, level))
        return;

    fmt::print(ctx.cfile_logger,
               irr_message_style[static_cast<int>(level)],
               fmt,
               args...);
}

template<typename... Args>
void
log(const Context& ctx, Context::message_type level, const char* msg)
{
    if (!is_loggable(ctx.log_priority, level))
        return;

    fmt::print(
      ctx.cfile_logger, irr_message_style[static_cast<int>(level)], msg);
}

template<typename... Args>
void
log(Context* ctx,
    Context::message_type level,
    const char* fmt,
    const Args&... args)
{
    if (not is_loggable(ctx->log_priority, level))
        return;

    fmt::print(ctx->cfile_logger,
               irr_message_style[static_cast<int>(level)],
               fmt,
               args...);
}

template<typename... Args>
void
log(Context* ctx, Context::message_type level, const char* msg)
{
    if (not is_loggable(ctx->log_priority, level))
        return;

    fmt::print(
      ctx->cfile_logger, irr_message_style[static_cast<int>(level)], msg);
}

struct sink
{
    template<typename... Args>
    sink(const Args&...)
    {}
};

} // namespace detail

template<typename... Args>
void
emerg(const Context& ctx, const char* fmt, const Args&... args)
{
#ifdef IRRITATOR_ENABLE_LOG
    detail::log(ctx, Context::message_type::emerg, fmt, args...);
#else
    detail::sink(ctx, fmt, args...);
#endif
}

template<typename... Args>
void
alert(const Context& ctx, const char* fmt, const Args&... args)
{
#ifdef IRRITATOR_ENABLE_LOG
    detail::log(ctx, Context::message_type::alert, fmt, args...);
#else
    detail::sink(ctx, fmt, args...);
#endif
}

template<typename... Args>
void
crit(const Context& ctx, const char* fmt, const Args&... args)
{
#ifdef IRRITATOR_ENABLE_LOG
    detail::log(ctx, Context::message_type::crit, fmt, args...);
#else
    detail::sink(ctx, fmt, args...);
#endif
}

template<typename... Args>
void
error(const Context& ctx, const char* fmt, const Args&... args)
{
#ifdef IRRITATOR_ENABLE_LOG
    detail::log(ctx, Context::message_type::err, fmt, args...);
#else
    detail::sink(ctx, fmt, args...);
#endif
}

template<typename... Args>
void
warning(const Context& ctx, const char* fmt, const Args&... args)
{
#ifdef IRRITATOR_ENABLE_LOG
    detail::log(ctx, Context::message_type::warning, fmt, args...);
#else
    detail::sink(ctx, fmt, args...);
#endif
}

template<typename... Args>
void
notice(const Context& ctx, const char* fmt, const Args&... args)
{
#ifdef IRRITATOR_ENABLE_LOG
    detail::log(ctx, Context::message_type::notice, fmt, args...);
#else
    detail::sink(ctx, fmt, args...);
#endif
}

template<typename... Args>
void
info(const Context& ctx, const char* fmt, const Args&... args)
{
#ifdef IRRITATOR_ENABLE_LOG
    detail::log(ctx, Context::message_type::info, fmt, args...);
#else
    detail::sink(ctx, fmt, args...);
#endif
}

template<typename... Args>
void
debug(const Context& ctx, const char* fmt, const Args&... args)
{
#ifdef IRRITATOR_ENABLE_LOG
#ifdef IRRITATOR_ENABLE_DEBUG
    detail::log(ctx, Context::message_type::debug, fmt, args...);
#else
    detail::sink(ctx, fmt, args...);
#endif
#else
    detail::sink(ctx, fmt, args...);
#endif
}

template<typename Arg1, typename... Args>
void
emerg(const Context& ctx,
      const char* fmt,
      const Arg1& arg1,
      const Args&... args)
{
#ifdef IRRITATOR_ENABLE_LOG
    detail::log(ctx, Context::message_type::emerg, fmt, arg1, args...);
#else
    detail::sink(ctx, fmt, arg1, args...);
#endif
}

template<typename Arg1, typename... Args>
void
alert(const Context& ctx,
      const char* fmt,
      const Arg1& arg1,
      const Args&... args)
{
#ifdef IRRITATOR_ENABLE_LOG
    detail::log(ctx, Context::message_type::alert, fmt, arg1, args...);
#else
    detail::sink(ctx, fmt, arg1, args...);
#endif
}

template<typename Arg1, typename... Args>
void
crit(const Context& ctx,
     const char* fmt,
     const Arg1& arg1,
     const Args&... args)
{
#ifdef IRRITATOR_ENABLE_LOG
    detail::log(ctx, Context::message_type::crit, fmt, arg1, args...);
#else
    detail::sink(ctx, fmt, arg1, args...);
#endif
}

template<typename Arg1, typename... Args>
void
error(const Context& ctx,
      const char* fmt,
      const Arg1& arg1,
      const Args&... args)
{
#ifdef IRRITATOR_ENABLE_LOG
    detail::log(ctx, Context::message_type::err, fmt, arg1, args...);
#else
    detail::sink(ctx, fmt, arg1, args...);
#endif
}

template<typename Arg1, typename... Args>
void
warning(const Context& ctx,
        const char* fmt,
        const Arg1& arg1,
        const Args&... args)
{
#ifdef IRRITATOR_ENABLE_LOG
    detail::log(ctx, Context::message_type::warning, fmt, arg1, args...);
#else
    detail::sink(ctx, fmt, arg1, args...);
#endif
}

template<typename Arg1, typename... Args>
void
notice(const Context& ctx,
       const char* fmt,
       const Arg1& arg1,
       const Args&... args)
{
#ifdef IRRITATOR_ENABLE_LOG
    detail::log(ctx, Context::message_type::notice, fmt, arg1, args...);
#else
    detail::sink(ctx, fmt, arg1, args...);
#endif
}

template<typename Arg1, typename... Args>
void
info(const Context& ctx,
     const char* fmt,
     const Arg1& arg1,
     const Args&... args)
{
#ifdef IRRITATOR_ENABLE_LOG
    detail::log(ctx, Context::message_type::info, fmt, arg1, args...);
#else
    detail::sink(ctx, fmt, arg1, args...);
#endif
}

template<typename Arg1, typename... Args>
void
debug(const Context& ctx,
      const char* fmt,
      const Arg1& arg1,
      const Args&... args)
{
#ifdef IRRITATOR_ENABLE_LOG
#ifdef IRRITATOR_ENABLE_DEBUG
    detail::log(ctx, Context::message_type::debug, fmt, arg1, args...);
#else
    detail::sink(ctx, fmt, arg1, args...);
#endif
#else
    detail::sink(ctx, fmt, arg1, args...);
#endif
}

template<typename T>
void
log(const Context& ctx, Context::message_type level, const T& msg)
{
    if (not is_loggable(ctx.log_priority, level))
        return;

    fmt::print(
      ctx.cfile_logger, irr_message_style[static_cast<int>(level)], "{}", msg);
}

template<typename T>
void
log(Context* ctx, Context::message_type level, const T& msg)
{
    if (not is_loggable(ctx->log_priority, level))
        return;

    fmt::print(ctx->cfile_logger,
               irr_message_style[static_cast<int>(level)],
               "{}",
               msg);
}

////////////////////////////////////////////////

template<typename T>
void
emerg(const Context& ctx, const T& msg)
{
#ifdef IRRITATOR_ENABLE_LOG
    log(ctx, Context::message_type::emerg, msg);
#else
    detail::sink(ctx, msg);
#endif
}

template<typename T>
void
alert(const Context& ctx, const T& msg)
{
#ifdef IRRITATOR_ENABLE_LOG
    log(ctx, Context::message_type::alert, msg);
#else
    detail::sink(ctx, msg);
#endif
}

template<typename T>
void
crit(const Context& ctx, const T& msg)
{
#ifdef IRRITATOR_ENABLE_LOG
    log(ctx, Context::message_type::crit, msg);
#else
    detail::sink(ctx, msg);
#endif
}

template<typename T>
void
error(const Context& ctx, const T& msg)
{
#ifdef IRRITATOR_ENABLE_LOG
    log(ctx, Context::message_type::err, msg);
#else
    detail::sink(ctx, msg);
#endif
}

template<typename T>
void
warning(const Context& ctx, const T& msg)
{
#ifdef IRRITATOR_ENABLE_LOG
    log(ctx, Context::message_type::warning, msg);
#else
    detail::sink(ctx, msg);
#endif
}

template<typename T>
void
notice(const Context& ctx, const T& msg)
{
#ifdef IRRITATOR_ENABLE_LOG
    log(ctx, Context::message_type::notice, msg);
#else
    detail::sink(ctx, msg);
#endif
}

template<typename T>
void
info(const Context& ctx, const T& msg)
{
#ifdef IRRITATOR_ENABLE_LOG
    log(ctx, Context::message_type::info, msg);
#else
    detail::sink(ctx, msg);
#endif
}

template<typename T>
void
debug(const Context& ctx, const T& msg)
{
#ifdef IRRITATOR_ENABLE_LOG
#ifndef IRRITATOR_ENABLE_DEBUG
    log(ctx, Context::message_type::debug, msg);
#else
    detail::sink(ctx, msg);
#endif
#else
    detail::sink(ctx, msg);
#endif
}

// In internal API, we prefer abort application when precondition,
// postcondition or assertion fail. For external API, user can select to use
// exception or return with message in terminal.

namespace details {

inline void
print(const char* type,
      const char* cond,
      const char* file,
      const char* line) noexcept
{
    fprintf(stderr, "%s [%s] failure at %s: %s\n", type, cond, file, line);
}

[[noreturn]] inline auto
fail_fast(const char* type,
          const char* cond,
          const char* file,
          const char* line) -> void
{
    print(type, cond, file, line);

#ifndef IRRITATOR_FAIL_FAST
    std::terminate();
#else
    throw std::logic_error(cond);
#endif
}

} // namespace details
} // namespace irr

#define IRR_CONTRACT_FAIL(type)                                               \
    irr::details::fail_fast(                                                  \
      type, irr_stringify(cond), __FILE__, irr_stringify(__LINE__))

#define IRR_CONTRACT_CHECK(type, cond)                                        \
    (irr_likely(cond)                                                         \
       ? static_cast<void>(0)                                                 \
       : irr::details::fail_fast(                                             \
           type, irr_stringify(cond), __FILE__, irr_stringify(__LINE__)))

#define IRR_CONTRACT_CHECK_RETURN_VAL(type, cond, val)                        \
    do {                                                                      \
        if (irr_unlikely(!(cond))) {                                          \
            irr::details::print(                                              \
              type, irr_stringify(cond), __FILE__, irr_stringify(__LINE__));  \
            return val;                                                       \
        }                                                                     \
    } while (0)

#define IRR_CONTRACT_CHECK_RETURN(type, cond)                                 \
    do {                                                                      \
        if (irr_unlikely(!(cond))) {                                          \
            irr::details::print(                                              \
              type, irr_stringify(cond), __FILE__, irr_stringify(__LINE__));  \
            return;                                                           \
        }                                                                     \
    } while (0)

#ifdef IRRITATOR_FULL_OPTIMIZATION
#define irr_expects(cond)
#define irr_ensures(cond)
#define irr_assert(cond)
#else
#define irr_expects(cond) IRR_CONTRACT_CHECK("Precondition", cond)
#define irr_ensures(cond) IRR_CONTRACT_CHECK("Postcondition", cond)
#define irr_assert(cond) IRR_CONTRACT_CHECK("Assertion", cond)
#endif

#define irr_reach() IRR_CONTRACT_FAIL("Reached")

#define irr_return_val_if_fail(cond, val)                                     \
    IRR_CONTRACT_CHECK_RETURN_VAL("Precondition", cond, val)
#define irr_return_if_fail(cond)                                              \
    IRR_CONTRACT_CHECK_RETURN("Precondition", cond)

#endif
