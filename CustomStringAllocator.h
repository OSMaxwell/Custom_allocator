template <typename T>
class CustomStringAllocator
{
private:
    T *memory_ptr;
    std::size_t memory_size;

public:
    typedef std::size_t size_type;
    typedef T *pointer;
    typedef const T *const_pointer;
    typedef T &reference;
    typedef const T &const_reference;
    typedef T value_type;

    CustomStringAllocator(T *memory_ptr, std::size_t memory_size) : memory_ptr(memory_ptr), memory_size(memory_size) {}
    ~CustomStringAllocator() {}

    template <class U>
    struct rebind
    {
        typedef CustomStringAllocator<U> other;
    };
    template <class U>
    CustomStringAllocator(const CustomStringAllocator<U> &) {}

    pointer address(reference x) const { return &x; }
    const_pointer address(const_reference x) const { return &x; }
    size_type max_size() const throw() { return memory_size; }

    pointer allocate(size_type n, const void *hint = 0)
    {
        // do not allow growing for strings
        return memory_ptr;
    }

    void deallocate(pointer p, size_type n)
    {
    }

    void construct(pointer p, const T &val)
    {
        new (static_cast<void *>(p)) T(val);
    }

    void construct(pointer p)
    {
        new (static_cast<void *>(p)) T();
    }

    void destroy(pointer p)
    {
        p->~T();
    }
};
