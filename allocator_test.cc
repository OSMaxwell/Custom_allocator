#include <cstdint>
//#include "tensorflow/lite/c/common.h"
#include <map>
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
template <typename T, int tag>
class
    ArenaBufferAllocator {  // C++03 Variant
                            // https://en.cppreference.com/w/cpp/named_req/Allocator
 private:
  T* memory_ptr;
  std::size_t memory_size;

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
  /*   typedef std::false_type propagate_on_container_move_assignment;
    typedef std::false_type propagate_on_container_copy_assignment;
    typedef std::true_type propagate_on_container_swap; */

  // typedef std::true_type is_always_equal;
  // A template structure that provides the ability that any allocator may
  // allocate storage of another type indirectly.
  template <typename U>
  struct rebind {
    typedef ArenaBufferAllocator<U, tag> other;
  };

  // The default constructor to create an allocator object.
  // Only use this after initialising the "global.TagToStaticBuf" map with the
  // associated tag.
  ArenaBufferAllocator() noexcept {
    auto it = global.TagToStaticBuf.find(tag);
    if (it == global.TagToStaticBuf.end()) {
      printf("%-7dArenaBufferAllocator() Error: Given tag not bound to map.\n",
             tag);
    } else {
      memory_size = it->second.second;

      memory_ptr =
          (T*)it->second.first + global.TagToBufferCounter[tag] * memory_size;
      const auto ite = global.TagToBufferCounter.find(tag);
      if (ite != global.TagToBufferCounter.end()) {
        ite->second++;
      } else {
        printf(
            "%-7dArenaBufferAllocator() Error: Given tag not bound to map.\n",
            tag);
      }
#ifdef _DEBUG
      DBGPRINTF("%-7d%-35s%-20p%-10d%-5d\n", tag, "ArenaBufferAllocator()",
                memory_ptr, memory_size, global.TagToBufferCounter[tag]);
#endif
    }
  };

  // Copy Constructor
  ArenaBufferAllocator(const ArenaBufferAllocator& other) noexcept
      : memory_ptr(other.memory_ptr), memory_size(other.memory_size) {
#ifdef _DEBUG
    DBGPRINTF("%-7d%-35s%-20p%-10d%-5d\n", tag,
              "ArenaBufferAllocator(..& other)", memory_ptr, memory_size,
              global.TagToBufferCounter[tag]);
#endif
  };

  template <class U>
  ArenaBufferAllocator(const ArenaBufferAllocator<U, tag>& other) throw() {
#ifdef _DEBUG
    DBGPRINTF("%-7d%-35s%-20p%-10d%-5d\n", tag,
              "ArenaBufferAllocator(..<U>& other)", memory_ptr, memory_size,
              global.TagToBufferCounter[tag]);
#endif
  };

  // The destructor
  ~ArenaBufferAllocator() throw() {
#ifdef _DEBUG
    DBGPRINTF("%-7d%-35s%-20p%-10s%-5d\n", tag, "~ArenaBufferAllocator()",
              memory_ptr, "", global.TagToBufferCounter[tag]);
#endif
  };

  pointer address(reference value) const { return &value; }

  const_pointer address(const_reference value) const { return &value; }

  // For vector allocation for e.g: happepns every 2^n elems. We don't want
  // that. Reallocation not allowed if allocated first time. Use limit allowed
  // as max_size No initialization. Allow TfLite to control this outside of the
  // allocator. (Refer to the struct)
  pointer allocate(size_type n, const void* hint = 0) throw() {
#ifdef _DEBUG
    DBGPRINTF("%-7d%-35s%-20p%-10d\n", tag, "allocate(..)", memory_ptr, n);
#endif
    if (n > memory_size) {
#ifdef _DEBUG
      DBGPRINTF("n > memory_size\t Tag: %d\n", tag);
#endif
      throw std::bad_array_new_length();
    }
    return memory_ptr;
  }

  // Frees the storage to which ptr refers.
  // Also not allowed. Until futher user intervention (Inside TFLite)
  void deallocate(T* ptr, size_type n) {
#ifdef _DEBUG
    DBGPRINTF("%-7d%-35s%-20p%-10d\n", tag, "deallocate(..)", ptr, n);
#endif
  }  // Do nothing or use TFContext???

  // Initializes the storage of one element to which p refers with value.
  // Equivalent to new( (void*)p)
  void construct(pointer p, const T& value) throw() {
    new ((void*)p)
        T(value);  // Only for testing, to be deleted afterwards. NO DYN ALLOC!
#ifdef _DEBUG
    DBGPRINTF("%-7d%-35s%-20p%-10d\n", tag, "***Construct(..)", p, value);
#endif
  }

  // Returns the largest value that can be passed to allocate().
  // This max size should be static.
  size_type max_size() const { return memory_size; }

  // Destroys the object to which p refers without deallocating the storage.
  // Simply calls the destructor (~) of the object
  void destroy(pointer p) {
#ifdef _DEBUG
    DBGPRINTF("%-7d%-35s%-20p%-10s\n", tag, "destroy(..)", p, "");
#endif
    p->~T();
  }
  template <typename U>
  void destroy(U* p) {
#ifdef _DEBUG
    DBGPRINTF("%-7d%-35s%-20p%-10s\n", tag, "destroy(<U>..)", p, "");
#endif
    p->~U();
  }

  template <typename U>
  ArenaBufferAllocator& operator=(const ArenaBufferAllocator<U, tag>&) {
    return *this;
  }
  ArenaBufferAllocator<T, tag>& operator=(const ArenaBufferAllocator& other) {
    return *this;
  }
};

// Returns true if allocators a1 and a2 are interchangeable.
//
// Two allocators are interchangeable if storage allocated from each can be
// deallocated via the other. This function should always return true.
template <class T, class U, int tag>
constexpr bool operator==(const ArenaBufferAllocator<T, tag>&,
                          const ArenaBufferAllocator<U, tag>&) noexcept {
  return true;
}

// Returns true if allocators lhs and rhs are not interchangeable. Equivalent
// to
// !(lhs==rhs)
//
// Two allocators are interchangeable if storage allocated from each can be
// deallocated via the other. This function should always return false.
template <class T, class U, int tag>
constexpr bool operator!=(const ArenaBufferAllocator<T, tag>&,
                          const ArenaBufferAllocator<U, tag>&) noexcept {
  return false;
}

// @brief Initialises the mapping of each static buffer to its corresponding
// tag.
// @param tag : Tag of the allocator used, can also be seen in
// AreanaBufferAllocator<T,tag>
// @param buff_ptr : The pointer to an already allocated buffer using TfLite.
// @param size : The expected max size of the allocated buffer.
template <typename T>
void InitTagToStaticBuf(int tag, T* buff_ptr, size_t size) {
  global.TagToStaticBuf.insert({tag, {buff_ptr, size}});
  global.TagToBufferCounter.insert({tag, 0});
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
struct pointer_traits;
template <class T>
struct pointer_traits<T*> {
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

/*Create for this https://en.cppreference.com/w/cpp/header/scoped_allocator*/

}  // namespace micro
}  // namespace ops
}  // namespace tflite