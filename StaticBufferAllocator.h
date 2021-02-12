#include <cstdint>
#include <iterator>
#include <vector>
#include <iostream>

template <typename T>
class StaticBufferAllocator
{
private:
    T *memory_ptr;
    std::size_t memory_size;

public:
    typedef std::size_t size_type;
    typedef T *pointer;
    typedef T value_type;

    StaticBufferAllocator(T *memory_ptr, std::size_t memory_size) : memory_ptr(memory_ptr), memory_size(memory_size) {}

    StaticBufferAllocator(const StaticBufferAllocator &other) throw() : memory_ptr(other.memory_ptr), memory_size(other.memory_size){};

    template <typename U>
    StaticBufferAllocator(const StaticBufferAllocator<U> &other) throw() : memory_ptr(other.memory_ptr), memory_size(other.memory_size){};

    template <typename U>
    StaticBufferAllocator &operator=(const StaticBufferAllocator<U> &other) { return *this; }
    StaticBufferAllocator<T> &operator=(const StaticBufferAllocator &other) { return *this; }
    ~StaticBufferAllocator() {}

    pointer allocate(size_type n, const void *hint = 0) { return memory_ptr; } // when allocate return the buffer
    void deallocate(T *ptr, size_type n) {} // empty cause the deallocation is buffer creator's responsability

    size_type max_size() const { return memory_size; }
};
