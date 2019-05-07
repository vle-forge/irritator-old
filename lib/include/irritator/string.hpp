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
    string() noexcept
      : buffer{ '\0' }
    {}

    string(const char* buff) noexcept
      : buffer{ '\0' }
    {
        if (buff) {
            std::strncpy(buffer.data(), buff, N - 1);
            buffer[N - 1] = '\0';
        }
    }

    string(string&& other) noexcept = delete;
    string& operator=(string&& other) noexcept = delete;

    string(const string& other) noexcept
    {
        std::strncpy(buffer.data(), other.buffer.data(), N - 1);
        buffer[N - 1] = '\0';
    }

    string& operator=(const string& other) noexcept
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

    string& operator=(const char* other) noexcept
    {
        if (other == nullptr)
            clear();
        else {
            std::strncpy(buffer.data(), other, N - 1);
            buffer[N - 1] = '\0';
        }

        return *this;
    }

    void clear() noexcept
    {
        buffer[0] = '\0';
    }

    void fill(char value) noexcept
    {
        std::fill_n(begin(), N - 1, value);
    }

    bool empty() const noexcept
    {
        return buffer[0] == '\0';
    }

    size_t size() const noexcept
    {
        return strlen(data());
    }

    size_t max_size() const noexcept
    {
        return N - 1;
    }

    reference operator[](size_t n) noexcept
    {
        return buffer[n];
    }

    const_reference operator[](size_t n) const noexcept
    {
        return buffer[n];
    }

    pointer data() noexcept
    {
        return buffer.data();
    }

    const_pointer data() const noexcept
    {
        return buffer.data();
    }

    void swap(this_type& other) noexcept
    {
        std::swap_ranges(begin(), end(), other.begin());
    }

    iterator begin() noexcept
    {
        return buffer.begin();
    }

    const_iterator begin() const noexcept
    {
        return buffer.begin();
    }

    iterator end() noexcept
    {
        return buffer.end();
    }

    const_iterator end() const noexcept
    {
        return buffer.end();
    }

    reverse_iterator rbegin() noexcept
    {
        return buffer.rbegin();
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return buffer.rbegin();
    }

    reverse_iterator rend() noexcept
    {
        return buffer.rend();
    }

    const_reverse_iterator rend() const noexcept
    {
        return buffer.rend();
    }

    const_iterator cbegin() const noexcept
    {
        return buffer.cbegin();
    }

    const_iterator cend() const noexcept
    {
        return buffer.cend();
    }

    const_reverse_iterator crbegin() const noexcept
    {
        return buffer.crbegin();
    }

    const_reverse_iterator crend() const noexcept
    {
        return buffer.crend();
    }

    friend bool operator==(const this_type& lhs, const this_type& rhs) noexcept
    {
        return std::strncmp(lhs.buffer.data(), rhs.buffer.data(), N) == 0;
    }

    friend bool operator!=(const this_type& lhs, const this_type& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    friend bool operator<(const this_type& lhs, const this_type& rhs) noexcept
    {
        return std::strncmp(lhs.buffer.data(), rhs.buffer.data(), N) < 0;
    }

    friend bool operator>(const this_type& lhs, const this_type& rhs) noexcept
    {
        return std::strncmp(lhs.buffer.data(), rhs.buffer.data(), N) > 0;
    }

    friend bool operator<=(const this_type& lhs, const this_type& rhs) noexcept
    {
        return !(lhs > rhs);
    }

    friend bool operator>=(const this_type& lhs, const this_type& rhs) noexcept
    {
        return !(lhs < rhs);
    }

    friend bool operator==(const this_type& lhs, const char* rhs) noexcept
    {
        return std::strncmp(lhs.buffer.data(), rhs, N) == 0;
    }

    friend bool operator!=(const this_type& lhs, const char* rhs) noexcept
    {
        return !(lhs == rhs);
    }

    friend bool operator<(const this_type& lhs, const char* rhs) noexcept
    {
        return std::strncmp(lhs.buffer.data(), rhs, N) < 0;
    }

    friend bool operator>(const this_type& lhs, const char* rhs) noexcept
    {
        return std::strncmp(lhs.buffer.data(), rhs, N) > 0;
    }

    friend bool operator<=(const this_type& lhs, const char* rhs) noexcept
    {
        return !(lhs > rhs);
    }

    friend bool operator>=(const this_type& lhs, const char* rhs) noexcept
    {
        return !(lhs < rhs);
    }
};

} // namespace irr

#endif
