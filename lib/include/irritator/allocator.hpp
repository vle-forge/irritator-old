// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ORG_VLEPROJECT_IRRITATOR_ALLOCATOR_HPP
#define ORG_VLEPROJECT_IRRITATOR_ALLOCATOR_HPP

#include <memory>

#include <cstdint>
#include <cstdlib>

namespace irr {

class allocator_malloc
{
public:
    allocator_malloc() = default;
    allocator_malloc(const allocator_malloc&) = default;
    allocator_malloc& operator=(const allocator_malloc&) = default;

    bool operator==(const allocator_malloc&)
    {
        return true;
    }

    bool operator!=(const allocator_malloc&)
    {
        return false;
    }

    void* allocate(size_t n, int /*flags*/ = 0)
    {
        return malloc(n);
    }

    void* allocate(size_t n, size_t alignment)
    {
        return aligned_alloc(alignment, n);
    }

    void deallocate(void* p, size_t /*n*/)
    {
        free(p);
    }
};

template<class T, std::size_t capacity = 1024>
class fixed_memory_allocator
{
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

private:
    union block
    {
        block* next;
        typename std::aligned_storage<sizeof(T), alignof(T)>::type storage;
    };

    class buffer
    {
        static constexpr std::size_t block_size = sizeof(T) > sizeof(block)
                                                    ? sizeof(T)
                                                    : sizeof(block);
        std::uint8_t data[block_size * capacity];
    };

    buffer data;
    int free_head = -1;
    int size = 0;

public:
    fixed_memory_allocator() = default;
    fixed_memory_allocator(fixed_memory_allocator&&) = delete;
    fixed_memory_allocator(const fixed_memory_allocator&) = delete;
    fixed_memory_allocator operator=(fixed_memory_allocator&&) = delete;
    fixed_memory_allocator operator=(const fixed_memory_allocator&) = delete;
    ~fixed_memory_allocator() = default;

    T* allocate()
    {
        if (size >= capacity)
            throw std::bad_alloc();

        block* new_alloc;
        if (free_head >= 0) {
            new_alloc = free_head;
            free_head = data[free_head]->next;
        } else
            new_alloc = reinterpret_cast<block*>(&data[size++]);

        return reinterpret_cast<T*>(new_alloc);
    }

    void deallocate(T* pointer) noexcept
    {
        block* blk = reinterpret_cast<block*>(pointer);
        blk->next = free_head;
        free_head = blk;
    }
};

template<class T, std::size_t capacity = 1024>
class block_memory_allocator
{
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

private:
    union block
    {
        block* next;
        typename std::aligned_storage<sizeof(T), alignof(T)>::type storage;
    };

    class buffer
    {
        static constexpr std::size_t block_size = sizeof(T) > sizeof(block)
                                                    ? sizeof(T)
                                                    : sizeof(block);

        std::uint8_t data[block_size * capacity];

    public:
        const buffer* next;

        buffer(buffer* next_) noexcept
          : next(next_)
        {}

        T* get_block(int index) noexcept
        {
            return reinterpret_cast<T*>(&data[block_size * index]);
        }
    };

    block* free_head = nullptr;
    buffer* first_buffer = nullptr;
    std::size_t size = capacity;

public:
    block_memory_allocator() noexcept = default;
    block_memory_allocator(const block_memory_allocator&) noexcept = default;
    block_memory_allocator& operator=(const block_memory_allocator&) noexcept =
      default;

    block_memory_allocator(block_memory_allocator&& other) noexcept
      : free_head(other.free_head)
      , first_buffer(other.first_buffer)
      , size(other.size)
    {
        other.free_head = nullptr;
        other.first_buffer = nullptr;
        other.size = { 0 };
    }

    block_memory_allocator& operator=(block_memory_allocator&& other) noexcept
    {
        if (this != &other) {
            while (first_buffer) {
                auto* buffer = first_buffer;
                first_buffer = buffer->next;
                delete buffer;
            }

            free_head = other.free_head;
            first_buffer = other.first_buffer;
            size = other.size;
            other.free_head = nullptr;
            other.first_buffer = nullptr;
            other.size = { 0 };
        }

        return *this;
    }

    ~block_memory_allocator() noexcept
    {
        while (first_buffer) {
            auto* buffer = first_buffer;
            first_buffer = buffer->next;
            delete buffer;
        }
    }

    T* allocate()
    {
        if (free_head) {
            block* block = free_head;
            free_head = block->next;
            return reinterpret_cast<T*>(block);
        }

        if (size >= capacity) {
            first_buffer = new buffer(first_buffer);
            size = 0;
        }

        return first_buffer->getBlock(size++);
    }

    void deallocate(T* pointer) noexcept
    {
        block* blk = reinterpret_cast<block*>(pointer);
        blk->next = free_head;
        free_head = blk;
    }
};

template<class T, std::size_t capacity = 1024>
class fixed_node_allocator : private fixed_memory_allocator<T, capacity>
{
public:
    using value_type = T;
    using size_type = std::size_t;
    using propagate_on_container_move_assignment = std::true_type;
    using difference_type = std::ptrdiff_t;

    T* allocate(size_type n, const void* hint = 0)
    {
        if (n != 1 || hint)
            throw std::bad_alloc();

        return fixed_node_allocator<T, capacity>::allocate();
    }

    void deallocate(T* p, size_type n)
    {
        fixed_node_allocator<T, capacity>::deallocate(p);
    }

    void construct(T* p, const T& val)
    {
        new (p) T(val);
    }

    void destroy(T* p)
    {
        p->~T();
    }
};

template<class T, std::size_t capacity = 1024>
class block_node_allocator : private block_memory_allocator<T, capacity>
{
public:
    using value_type = T;
    using size_type = std::size_t;
    using propagate_on_container_move_assignment = std::true_type;
    using difference_type = std::ptrdiff_t;

    T* allocate(size_type n, const void* hint = 0)
    {
        if (n != 1 || hint)
            throw std::bad_alloc();

        return block_node_allocator<T, capacity>::allocate();
    }

    void deallocate(T* p, size_type n)
    {
        block_node_allocator<T, capacity>::deallocate(p);
    }

    void construct(T* p, const T& val)
    {
        new (p) T(val);
    }

    void destroy(T* p)
    {
        p->~T();
    }
};

} // irr

#endif // ORG_VLEPROJECT_IRRITATOR_ALLOCATOR_HPP
