// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ORG_VLEPROJECT_IRRITATOR_STRING_HPP
#define ORG_VLEPROJECT_IRRITATOR_STRING_HPP

#include <array>

#include <cstring>

namespace irr {

template<unsigned N>
class string
{
public:
    using this_type = string<N>;
    using container_type = std::array<char, N>;
    using value_type = typename container_type::value_type;
    using pointer = typename container_type::pointer;
    using const_pointer = typename container_type::const_pointer;
    using reference = typename container_type::reference;
    using const_reference = typename container_type::const_reference;
    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;
    using size_type = typename container_type::size_type;
    using difference_type = typename container_type::difference_type;
    using reverse_iterator = typename container_type::reverse_iterator;
    using const_reverse_iterator =
      typename container_type::const_reverse_iterator;

private:
    container_type buffer;

public:
    constexpr string() noexcept
    {
        clear();
    }

    constexpr string(const char* buff) noexcept
    {
        if (buff == nullptr)
            clear();
        else {
            std::strncpy(buffer.data(), buff, N - 1);
            buffer[N - 1] = '\0';
        }
    }

    constexpr string(const string& other) noexcept
    {
        std::strncpy(buffer.data(), other.buffer.data(), N - 1);
        buffer[N - 1] = '\0';
    }

    constexpr string(string&& other) noexcept
    {
        std::strncpy(buffer.data(), other.buffer.data(), N - 1);
        buffer[N - 1] = '\0';
        other.buffer[0] = '\0';
    }

    constexpr string& operator=(const string& other) noexcept
    {
        if (this != &other) {
            string cpy(other);

            std::strncpy(other.buffer.data(), buffer.data(), N - 1);
            other.buffer[N - 1] = '\0';

            std::strncpy(buffer.data(), cpy.buffer.data(), N - 1);
            buffer[N - 1] = '\0';
        }

        return *this;
    }

    constexpr string& operator=(const char* other) noexcept
    {
        if (other == nullptr)
            clear();
        else {
            std::strncpy(buffer.data(), other, N - 1);
            buffer[N - 1] = '\0';
        }

        return *this;
    }

    constexpr string& operator=(string&& other) noexcept
    {
        if (this != &other) {
            std::strncpy(buffer.data(), other.buffer.data(), N - 1);
            buffer[N - 1] = '\0';
            other.buffer[0] = '\0';
        }

        return *this;
    }

    constexpr void clear() noexcept
    {
        buffer[0] = '\0';
    }

    constexpr void fill(char value) noexcept
    {
        std::fill_n(begin(), N - 1, value);
    }

    constexpr bool empty() const noexcept
    {
        return buffer[0] == '\0';
    }

    constexpr size_t size() const noexcept
    {
        return strlen(data());
    }

    constexpr size_t max_size() const noexcept
    {
        return N - 1;
    }

    constexpr reference operator[](size_t n) noexcept
    {
        return buffer[n];
    }

    constexpr const_reference operator[](size_t n) const noexcept
    {
        return buffer[n];
    }

    constexpr pointer data() noexcept
    {
        return buffer.data();
    }

    constexpr const_pointer data() const noexcept
    {
        return buffer.data();
    }

    constexpr void swap(this_type& other) noexcept
    {
        std::swap_ranges(begin(), end(), other.begin());
    }

    constexpr iterator begin() noexcept
    {
        return buffer.begin();
    }

    constexpr const_iterator begin() const noexcept
    {
        return buffer.begin();
    }

    constexpr iterator end() noexcept
    {
        return buffer.end();
    }

    constexpr const_iterator end() const noexcept
    {
        return buffer.end();
    }

    constexpr reverse_iterator rbegin() noexcept
    {
        return buffer.rbegin();
    }

    constexpr const_reverse_iterator rbegin() const noexcept
    {
        return buffer.rbegin();
    }

    constexpr reverse_iterator rend() noexcept
    {
        return buffer.rend();
    }

    constexpr const_reverse_iterator rend() const noexcept
    {
        return buffer.rend();
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return buffer.cbegin();
    }

    constexpr const_iterator cend() const noexcept
    {
        return buffer.cend();
    }

    constexpr const_reverse_iterator crbegin() const noexcept
    {
        return buffer.crbegin();
    }

    constexpr const_reverse_iterator crend() const noexcept
    {
        return buffer.crend();
    }

    constexpr friend bool operator==(const this_type& lhs,
                                     const this_type& rhs) noexcept
    {
        return std::strncmp(lhs.buffer.data(), rhs.buffer.data(), N) == 0;
    }

    constexpr friend bool operator!=(const this_type& lhs,
                                     const this_type& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    constexpr friend bool operator<(const this_type& lhs,
                                    const this_type& rhs) noexcept
    {
        return std::strncmp(lhs.buffer.data(), rhs.buffer.data(), N) < 0;
    }

    constexpr friend bool operator>(const this_type& lhs,
                                    const this_type& rhs) noexcept
    {
        return std::strncmp(lhs.buffer.data(), rhs.buffer.data(), N) > 0;
    }

    constexpr friend bool operator<=(const this_type& lhs,
                                     const this_type& rhs) noexcept
    {
        return !(lhs > rhs);
    }

    constexpr friend bool operator>=(const this_type& lhs,
                                     const this_type& rhs) noexcept
    {
        return !(lhs < rhs);
    }

    constexpr friend bool operator==(const this_type& lhs,
                                     const char* rhs) noexcept
    {
        return std::strncmp(lhs.buffer.data(), rhs, N) == 0;
    }

    constexpr friend bool operator!=(const this_type& lhs,
                                     const char* rhs) noexcept
    {
        return !(lhs == rhs);
    }

    constexpr friend bool operator<(const this_type& lhs,
                                    const char* rhs) noexcept
    {
        return std::strncmp(lhs.buffer.data(), rhs, N) < 0;
    }

    constexpr friend bool operator>(const this_type& lhs,
                                    const char* rhs) noexcept
    {
        return std::strncmp(lhs.buffer.data(), rhs, N) > 0;
    }

    constexpr friend bool operator<=(const this_type& lhs,
                                     const char* rhs) noexcept
    {
        return !(lhs > rhs);
    }

    constexpr friend bool operator>=(const this_type& lhs,
                                     const char* rhs) noexcept
    {
        return !(lhs < rhs);
    }
};

} // namespace irr

#endif