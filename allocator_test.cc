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
class ArenaBufferAllocator {
  // test
/*   ArenaBufferAllocator* copyconstructor = nullptr;
  std::allocator<T>* rebindAllocator = nullptr; */

  // type definitions
 private:
  T* memory_ptr;
  std::size_t memory_size;

  // TfLiteContext* context;
  // bool allocated;

 public:
  typedef std::size_t size_type;
  typedef T* pointer;
  typedef T value_type;
  // bool allocated_init;  // Init/Prepare = 0 / Eval = 1
  // void* raw;

  // Other members for the allocator (optional)
  typedef const T* const_pointer;
  typedef ptrdiff_t difference_type;
  typedef T& reference;
  typedef const T& const_reference;
  typedef void* void_pointer;
  typedef const void* const_void_pointer;

  //------------------------For now, not needed----------
  /*   using propagate_on_container_copy_assignment = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type; */
  //-------------------------------------------

  // A template structure that provides the ability that any allocator may
  // allocate storage of another type indirectly.
  template <typename U>
  struct rebind {
    typedef ArenaBufferAllocator<U, tag> other;
  };

  // Construction and destructors

  // The default constructor to create an allocator object.
  // Only use this after initialising the "global.TagToStaticBuf" map with the
  // associated tag.
  ArenaBufferAllocator() noexcept {
    // Acces global struct
    // printf("TEST : %d\n",global.TagToBufferCounter[tag]);
    auto it = global.TagToStaticBuf.find(tag);
    if (it == global.TagToStaticBuf.end()) {
      printf("%-7dArenaBufferAllocator() Error: Given tag not bound to map.\n",
             tag);
    } else {
      // Set the correct members from global.TagToStaticBuf.
      memory_size = it->second.second;

      memory_ptr = (T*)it->second.first +
                   global.TagToBufferCounter[tag] * memory_size;
      const auto ite = global.TagToBufferCounter.find(tag);
      if (ite != global.TagToBufferCounter.end()) {
        ite->second++;
        // printf("DONE.\n");
      } else {
        printf(
            "%-7dArenaBufferAllocator() Error: Given tag not bound to map.\n",
            tag);
      }
      // global.TagToBufferCounter[tag]++;  // Move this to global??

#ifdef _DEBUG
      DBGPRINTF("%-7d%-35s%-20p%-10d%-5d\n", tag, "ArenaBufferAllocator()",
                memory_ptr, memory_size, global.TagToBufferCounter[tag]);
#endif
    }
  };  // Needed for initilisation. More testing

  // @brief  constructor, usually used for debugging
  ArenaBufferAllocator(T* memory_ptr, std::size_t memory_size)
      : memory_ptr(memory_ptr), memory_size(memory_size) {
    /*      global.TagToStaticBuf.insert(
            {tag, memory_ptr});  */ // Bind correct memory_ptr to tag.
    // global.BufferCounter++;
#ifdef _DEBUG
    DBGPRINTF("%-7d%-35s%-20p%-10d%-5d\n", tag, "ArenaBufferAllocator(..)",
              memory_ptr, memory_size, global.TagToBufferCounter[tag]);
/*     for (auto it = global.TagToStaticBuf.cbegin();
         it != global.TagToStaticBuf.cend(); it++) {
      std::cout << "MAP:" << it->first << " " << it->second << "\n";
    } */
#endif
  }
  // Copy Constructor
  ArenaBufferAllocator(const ArenaBufferAllocator& other) throw()
      : memory_ptr(other.memory_ptr), memory_size(other.memory_size) {
        *this = other;
    /*     const auto ite = global.TagToBufferCounter.find(tag);
        if (ite != global.TagToBufferCounter.end()) {
          ite->second++;
          // printf("DONE.\n");
        } else {
          printf("%-7dArenaBufferAllocator(..& other) Error: Given tag not bound
       to map.\n", tag);
        } */ // <---- CHECK LATER

    // global.BufferCounter++;
    // memory_ptr += global.BufferCounter * memory_size;
    // global.BufferCounter++;
    // other.memory_ptr = memory_ptr
#ifdef _DEBUG
    DBGPRINTF("%-7d%-35s%-20p%-10d%-5d\n", tag,
              "ArenaBufferAllocator(..& other)", memory_ptr, memory_size,
              global.TagToBufferCounter[tag]);
    // printf("\n%d\n", *memory_ptr);
#endif
  };

  template <class U>
  ArenaBufferAllocator(const ArenaBufferAllocator<U, tag>& other) throw() {
    // global.BufferCounter++;
#ifdef _DEBUG
    DBGPRINTF("%-7d%-35s%-20p%-10d%-5d\n", tag,
              "ArenaBufferAllocator(..<U>& other)", memory_ptr, memory_size,
              global.TagToBufferCounter[tag]);
#endif
  };

  // The destructor
  ~ArenaBufferAllocator() throw() {
    // global.BufferCounter--; <------- CHECK LATER
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
    // std::uninitialized_fill(beg,end,val); //---->Check this out
    new ((void*)p) T(value);
#ifdef _DEBUG
    DBGPRINTF("%-7d%-35s%-20p%-10d\n", tag, "***Construct(..)", p, value);
#endif
  }

/*   template<typename U, typename... Args>
  void construct(U* p, Args&&... args) throw(){
    ::new (p) U(std::forward<Args>(args)...);
  } */

  // Returns the largest value that can be passed to allocate().
  // This max size should be static.
  size_type max_size() const { return memory_size; }

  // Destroys the object to which p refers without deallocating the storage.
  // Simply calls the destructor (~) of the object
  void destroy(pointer p) {  // Check usage and need?
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
  }  // Not needed for now
  ArenaBufferAllocator<T, tag>& operator=(const ArenaBufferAllocator& other) {
    return *this;
  }  // Not needed for now
};
/* template <class T, int tag>
int ArenaBufferAllocator<T, tag>::BufferCounter = 0; */

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

template <class T>
class custom_allocator {
 private:
  T* mem_ptr;

 public:
  typedef T value_type;
  custom_allocator(T* value_type_in) noexcept : mem_ptr(value_type_in) {
    printf("CUSTOMALLOC\n");
  }
  template <class U>
  custom_allocator(const custom_allocator<U>&) noexcept {
    printf("CONSTCUSTOMALLOC\n");
  }
  T* allocate(std::size_t n) {
    printf("ALLOC\n");
    return mem_ptr;
  }
  void deallocate(T* p, std::size_t n) { printf("DEALLOC\n"); }
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
}  // namespace micro
}  // namespace ops
}  // namespace tflite */