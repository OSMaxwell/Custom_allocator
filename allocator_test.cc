#include <cstdint>
//#include "tensorflow/lite/c/common.h"
#include <map>
#include <scoped_allocator>
#include "allocator_test.h"

#ifdef _DEBUG
#include <stdio.h>
#define DBGPRINTF(format, ...) printf(format, ##__VA_ARGS__)
#include <memory>
#include <typeinfo>
#endif

// Encapsulates strategies for access/addressing, allocation/deallocation and
// construction/destruction of objects. Using C++11 Implementation for
// simplicity and backwords comp.
//      -> Using allocator_traits
// Goal : minimal allocator to use TFLM Scratch Buffer.

namespace tflite {
namespace ops {
namespace micro {

GLOBAL global;  // The struct to "connect" with TFLite. DO NOT USE DIRECTLY

// @brief Big class for Arena Buffer Allocator /Memory allocator.
// For more info check under:
// https://en.cppreference.com/w/cpp/memory/allocator
// @tparam T for type
// @tparam tag for tracking and mapping
template <typename T>
class
    ArenaBufferAllocator {  // C++03 Variant
                            // https://en.cppreference.com/w/cpp/named_req/Allocator
 private:
  T* memory_ptr;
  std::size_t memory_size;
  int _tag;

 public:
  typedef std::size_t size_type;
  typedef T* pointer;
  typedef T value_type;
  typedef const T* const_pointer;
  typedef ptrdiff_t difference_type;
  typedef T& reference;
  typedef const T& const_reference;
  typedef void* void_pointer;
  typedef const void* const_void_pointer;

  // A template structure that provides the ability that any allocator may
  // allocate storage of another type indirectly.
  template <typename U>
  struct rebind {
    typedef ArenaBufferAllocator<U> other;
  };

  // The default constructor to create an allocator object.
  // Only use this after initialising the "global.TagToStaticBuf" map with the
  // associated tag.
  ArenaBufferAllocator() noexcept {
    auto it = global.TagToStaticBuf.begin();
    while (it != global.TagToStaticBuf.end()) {
      if (it->second._counter > 0) {
        it++;
        continue;
      } else {
        _tag = it->first;
        memory_size = it->second._size; /* + counter* memory_size =??*/
        memory_ptr = (T*)it->second._static_buffer;
        it->second._counter++;
        break;
      }
    }
#ifdef _DEBUG
    DBGPRINTF("%-7d%-35s%-20p%-10d%-5d\n", _tag, "ArenaBufferAllocator()",
              memory_ptr, memory_size, global.TagToStaticBuf[_tag]._counter);
#endif
  };
  // Specific allocator for nested containers.
  // Only use this after initialising the "global.TagToStaticBuf" map with the
  // associated tag.
  // @param tag : The tag to look for in the map.
  ArenaBufferAllocator(int tag) noexcept {
    auto it = global.TagToStaticBuf.find(tag);
    if (it == global.TagToStaticBuf.end()) {
#ifdef _DEBUG
      DBGPRINTF(
          "%-7dArenaBufferAllocator() Error: Given tag not bound to map.\n",
          tag);
#endif
    } else {
      memory_size = it->second._size; /* + counter* memory_size =??*/
      memory_ptr = (T*)it->second._static_buffer;
      it->second._counter++;
#ifdef _DEBUG
      DBGPRINTF("%-7d%-35s%-20p%-10d%-5d\n", _tag, "ArenaBufferAllocator(..)",
                memory_ptr, memory_size, global.TagToStaticBuf[_tag]._counter);
#endif
    }
  }

  // Copy Constructor
  ArenaBufferAllocator(const ArenaBufferAllocator& other) noexcept
      : memory_ptr(other.memory_ptr),
        memory_size(other.memory_size),
        _tag(other._tag) {
#ifdef _DEBUG
    DBGPRINTF("%-7d%-35s%-20p%-10d%-5d\n", _tag,
              "ArenaBufferAllocator(..& other)", memory_ptr, memory_size,
              global.TagToStaticBuf[_tag]._counter);
#endif
  };

  template <class U>
  ArenaBufferAllocator(const ArenaBufferAllocator<U>& other) noexcept
      : memory_ptr(other.memory_ptr),
        memory_size(other.memory_size),
        _tag(other._tag) {
    {
#ifdef _DEBUG
      DBGPRINTF("%-7d%-35s%-20p%-10d%-5d\n", _tag,
                "ArenaBufferAllocator(..<U>& other)", memory_ptr, memory_size,
                global.TagToStaticBuf[_tag]._counter);
#endif
    };

    // The destructor
    ~ArenaBufferAllocator(){
#ifdef _DEBUG
      DBGPRINTF("%-7d%-35s%-20p%-10s%-5d\n", _tag, "~ArenaBufferAllocator()",
                memory_ptr, "", global.TagToStaticBuf[_tag]._counter);
#endif
    };

    pointer address(reference value) const { return &value; }

    const_pointer address(const_reference value) const { return &value; }

    // For vector allocation for e.g: happepns every 2^n elems. We don't want
    // that. Reallocation not allowed if allocated first time. Use limit
    // allowed as max_size No initialization. Allow TfLite to control this
    // outside of the allocator. (Refer to the struct)
    pointer allocate(size_type n, const void* hint = 0) throw() {
#ifdef _DEBUG
      DBGPRINTF("%-7d%-35s%-20p%-10d\n", _tag, "allocate(..)", memory_ptr, n);
#endif
      if (n > memory_size) {
#ifdef _DEBUG
        DBGPRINTF("n > memory_size\t Tag: %d\n", _tag);
#endif
        throw std::bad_array_new_length();
      }
      return memory_ptr;
    }

    // Frees the storage to which ptr refers.
    // Also not allowed. Until futher user intervention (Inside TFLite)
    void deallocate(T * ptr, size_type n) {
#ifdef _DEBUG
      DBGPRINTF("%-7d%-35s%-20p%-10d\n", _tag, "deallocate(..)", ptr, n);
#endif
    }  // Do nothing or use TFContext???

    // Initializes the storage of one element to which p refers with value.
    // Equivalent to new( (void*)p)
    void construct(pointer p, const T& value) throw() {
      new ((void*)p) T(value);  // Only for testing, to be deleted afterwards.
                                // NO DYN ALLOC!
#ifdef _DEBUG
      DBGPRINTF("%-7d%-35s%-20p%-10d\n", _tag, "***Construct(..)", p, value);
#endif
    }

    // Returns the largest value that can be passed to allocate().
    // This max size should be static.
    size_type max_size() const { return memory_size; }

    // Destroys the object to which p refers without deallocating the storage.
    // Simply calls the destructor (~) of the object
    void destroy(pointer p) {
#ifdef _DEBUG
      DBGPRINTF("%-7d%-35s%-20p%-10s\n", _tag, "destroy(..)", p, "");
#endif
      p->~T();
    }
    template <typename U>
    void destroy(U * p) {
#ifdef _DEBUG
      DBGPRINTF("%-7d%-35s%-20p%-10s\n", _tag, "destroy(<U>..)", p, "");
#endif
      p->~U();
    }

    template <typename U>
    ArenaBufferAllocator& operator=(const ArenaBufferAllocator<U>&) {
      return *this;
    }
    ArenaBufferAllocator<T>& operator=(const ArenaBufferAllocator& other) {
      return *this;
    }
  };

  // Returns true if allocators a1 and a2 are interchangeable.
  //
  // Two allocators are interchangeable if storage allocated from each can be
  // deallocated via the other. This function should always return true.
  template <class T, class U>
  constexpr bool operator==(const ArenaBufferAllocator<T>&,
                            const ArenaBufferAllocator<U>&) noexcept {
    return true;
  }

  // Returns true if allocators lhs and rhs are not interchangeable.
  // Equivalent to
  // !(lhs==rhs)
  //
  // Two allocators are interchangeable if storage allocated from each can be
  // deallocated via the other. This function should always return false.
  template <class T, class U>
  constexpr bool operator!=(const ArenaBufferAllocator<T>&,
                            const ArenaBufferAllocator<U>&) noexcept {
    return false;
  }

  // @brief Initialises the mapping of each static buffer to an auto
  // incrementing "tag" AreanaBufferAllocator<T>.
  // The vector construction start from the beginning of the map. The first Init
  // would be for the FIRST vector to be created.
  // @param buff_ptr : The pointer to an already allocated buffer using
  // TfLite.
  // @param size : The expected max size of the allocated buffer.
  template <typename T>
  void InitTagToStaticBuf(T* buff_ptr, size_t size) {
    global.TagToStaticBuf.insert({global.tag, tag_id(buffer_ptr, size)});
    global.tag++;
  }
  // ----------------------------------------------------------
  // ---------------- No tag included in these structs---------
  // ----------------------------------------------------------

  // Basic default Allocator. (C++14 Variant)
  // https://en.cppreference.com/w/cpp/memory/allocator
  // Minimal
  // @tparam T Type of object to allocate.
  template <class T>
  struct ArenaAllocator {
    using value_type = T;
    using size_type = std::size_t;
    using propagate_on_container_move_assignment = std::true_type;
    using is_awlays_equal = std::false_type;

    ArenaAllocator() { /* do_stuff Params passing through GLOBAL*/
    }
    ArenaAllocator(const ArenaAllocator&) noexcept;
    template <class U>
    ArenaAllocator(const ArenaAllocator<U>&) noexcept { /* do_stuff */
    }
    ~ArenaAllocator() { /* do_stuff */
    }
    size_type max_size() const noexcept {
      return std::numeric_limits<size_type>::max() /
             sizeof(value_type);  // Fix this!
    }
    T* allocate(std::size_t n) { /* Do_something and return */
    }
    T* allocate(std::size_t n,
                const void* hint) { /*Do same thing as allocate above*/
    }
    void deallocate(T* p, std::size_t n) { /* do_something */
    }
  };

  template <class T, class U>
  constexpr bool operator!=(const ArenaAllocator<T>&,
                            const ArenaAllocator<U>&) noexcept {
    return false;
  }

  template <class T, class U>
  constexpr bool operator!=(const ArenaAllocator<T>&,
                            const ArenaAllocator<U>&) noexcept {
    return false;
  }

  // Allocator traits to use the basic allocator and handle complicated
  // allocations and lateral propagations.
  template <class T>
  struct ArenaAllocator_traits {
    using allocator_type = T;
    using value_type = T::value_type;
    using pointer = value_type*;
    using size_type = std::size_t;

    using propagate_on_container_copy_assignment = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;

    template <class X>
    using rebind_alloc = T::rebind<X>::other /*finish_this?*/;
    template <class X>
    using rebind_traits = ArenaAllocator_traits<rebind_alloc<X>>;

    static T* allocatote(T& a, size_type n) { return a.allocate(n); }
    static T* allocate(T& a, size_type n, const_void_pointer hint) {
      return a.allocate(n, hint);
    }
    static void deallocate(T& a, pointer p, size_type n) { a.deallocate(p, n); }

    template <class X, class... Args>
    static void construct(T& a, X* p, Args&&... args) {
      a.construct(p, std::forward<Args>(args)...);
    }
    template <class X>
    static void destroy(T& a, X* p) {
      p->~X();
    }
    static size_type max_size(T const& a) noexcept { return a.max_size(); }
    static T select_on_container_copy_construction(T const& a) {
      return a;  // Check this
    }
  };

  // Standardized way to access certain properties of pointer-like types
  template <class T>
  struct ArenaPointer_traits;
  template <class T>
  struct ArenaPointer_traits<T*> {
    using pointer = T*;
    using element_type = T;
    using difference_type = std::ptrdiff_t;

    template <class U>
    using rebind = U*;
    // Constructs a dereferenceable pointer or pointer-like object ("fancy
    // pointer") to its argument.
    static pointer pointer_to(element_type& r) noexcept {
      return std::addressof(r);
    }
  };

  /*Created from: https://en.cppreference.com/w/cpp/header/scoped_allocator
  Used in scoped containers ex vector of lists of maps.*/
  template <class OuterAlloc, class... InnerAllocs>
  class ArenaScoped_allocator_adaptor : public OuterAlloc {
   private:
    using OuterTraits = ArenaAllocator_traits<OuterAlloc>;
    ArenaScoped_allocator_adaptor<InnerAllocs...> inner;

   public:
    using outer_allocator_type = OuterAlloc;
    if (sizeof...(InnerAllocs) == 0) {
      using inner_allocator_type = ArenaScoped_allocator_adaptor<OuterAlloc>;
    } else {
      using inner_allocator_type =
          ArenaScoped_allocator_adaptor<InnerAllocs...>;
    }  // Check.

    using value_type = typename OuterTraits::value_type;
    using size_type = typename OuterTraits::size_type;
    using difference_type = typename OuterTraits::difference_type;
    using pointer = typename OuterTraits::pointer;
    using const_pointer = typename OuterTraits::const_pointer;
    using void_pointer = typename OuterTraits::void_pointer;
    using const_void_pointer = typename OuterTraits::const_void_pointer;

    using propagate_on_container_copy_assignment = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;
    // using is_always_equal = /* Maybe not needed */;

    template <class Tp>
    struct rebind {
      using other =
          ArenaScoped_allocator_adaptor<OuterTraits::template rebind_alloc<Tp>,
                                        InnerAllocs...>;
    };
    ArenaScoped_allocator_adaptor();
    template <class OuterA2>
    ArenaScoped_allocator_adaptor(OuterA2&& outerAlloc,
                                  const InnerAllocs&... innerAllocs) noexcept;

    ArenaScoped_allocator_adaptor(
        const ArenaScoped_allocator_adaptor& other) noexcept;
    ArenaScoped_allocator_adaptor(
        ArenaScoped_allocator_adaptor&& other) noexcept;

    template <class OuterA2>
    ArenaScoped_allocator_adaptor(
        const ArenaScoped_allocator_adaptor<OuterA2, InnerAllocs...>&
            other) noexcept;
    template <class OuterA2>
    ArenaScoped_allocator_adaptor(
        ArenaScoped_allocator_adaptor<OuterA2, InnerAllocs...>&&
            other) noexcept;

    ArenaScoped_allocator_adaptor& operator=(
        const ArenaScoped_allocator_adaptor&) = default;
    ArenaScoped_allocator_adaptor& operator=(ArenaScoped_allocator_adaptor&&) =
        default;

    ~ArenaScoped_allocator_adaptor();

    inner_allocator_type& inner_allocator() noexcept;
    const inner_allocator_type& inner_allocator() const noexcept;
    outer_allocator_type& outer_allocator() noexcept;
    const outer_allocator_type& outer_allocator() const noexcept;
    pointer allocate(size_type n);
    pointer allocate(size_type n, const_void_pointer hint);
    void deallocate(pointer p, size_type n);
    size_type max_size() const;
    template <class T, class... Args>
    void construct(T* p, Args&&... args);

    template <class T>
    void destroy(T* p);

    ArenaScoped_allocator_adaptor select_on_container_copy_construction() const;
  };
  template <class OuterAlloc, class... InnerAllocs>
  ArenaScoped_allocator_adaptor(OuterAlloc, InnerAllocs...)
      ->ArenaScoped_allocator_adaptor<OuterAlloc, InnerAllocs...>;

  // scoped allocator operators
  template <class OuterA1, class OuterA2, class... InnerAllocs>
  bool operator==(
      const ArenaScoped_allocator_adaptor<OuterA1, InnerAllocs...>& a,
      const ArenaScoped_allocator_adaptor<OuterA2, InnerAllocs...>& b) noexcept;

}  // namespace micro
}  // namespace micro
}  // namespace ops