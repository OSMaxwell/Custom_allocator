#include <cstdint>
//#include "tensorflow/lite/c/common.h"

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

template <typename T>  // Class T or typename T, the same.
class ArenaBufferAllocator {
  // type definitions
 private:
  T* memory_ptr;
  std::size_t memory_size;
  TfLiteContext* context;
  bool allocated;

 public:
  typedef std::size_t size_type;
  typedef T* pointer;
  typedef T value_type;
  bool allocated_init;  // Init/Prepare = 0 / Eval = 1
  void* raw;

  // Other members for the allocator (optional)
  typedef const T* const_pointer;
  typedef ptrdiff_t difference_type;
  typedef T& reference;
  typedef const T& const_reference;
  typedef void* void_pointer;
  typedef const void* const_void_pointer;

#ifdef _DEBUG
  // Tracing: Contains the tag of each container, incremented on creation.
  static int container_tag = 0;  // -> PLEASE TRANSFORM INTO TEMPLATE USAGE TOO
#endif

  //------------------------For now, not needed----------
  /*   typedef propagate_on_container_copy_assignment = false_type;
    typedef propagate_on_container_move_assignment = true_type;
    typedef propagate_on_container_swap = true_type; */
  //-------------------------------------------

  // Construction and destructors

  // The default constructor to create an allocator object.
  ArenaBufferAllocator() noexcept {
#ifdef _DEBUG
    DBGPRINTF("TAG:%d --- Constructor: Defalut\n");
    this->container_tag++;
#endif
  };  // Needed for initilisation. More testing

  ArenaBufferAllocator(void* ptr, std::size_t memory_size,
                       TfLiteContext* ctx) throw()
      : memory_size(memory_size), context(ctx), allocated_init(true) {
    memory_ptr = static_cast<T*>(ptr);
#ifdef _DEBUG
    this->container_tag++;
    context->ReportError(
        context,
        "TAG:%d --- Constructor: Inside Allocator Init---------%08x %32s\n",
        this, typeid(T).name());
#endif
  };
  ArenaBufferAllocator(int ptr_int, std::size_t memory_size,
                       TfLiteContext* ctx) throw()
      : memory_size(memory_size), context(ctx), allocated_init(false) {
    raw = context->GetScratchBuffer(context, ptr_int);
#ifdef _DEBUG
    this->container_tag++;
    context->ReportError(
        context,
        "TAG:%d --- Constructor: Inside Allocator Eval---------%08x %32s\n",
        this->container_tag, this, typeid(T).name());
#endif
  };

  // Copy contructor
  ArenaBufferAllocator(const ArenaBufferAllocator& other) noexcept {
#ifdef _DEBUG
    this->container_tag++;
    context->ReportError(
        context,
        "TAG:%d --- Copy Constructor:--------------------------%08x %32s\n",
        this->container_tag, this, typeid(T).name());
#endif
  }

  template <class U>
  ArenaBufferAllocator(const ArenaBufferAllocator<U>& other) noexcept {
#ifdef _DEBUG
    this->container_tag++;
    context->ReportError(context,
                         "TAG:%d --- Contrusct T Alloc from X "
                         "Alloc-------------%08x %32s %32s\n",
                         this->container_tag, this, typeid(T).name(),
                         typeid(U).name());
#endif
  }

  // The destructor
  ~ArenaBufferAllocator() {
#ifdef _DEBUG
    this->container_tag++;
    context->ReportError(
        context,
        "TAG:%d --- Destructor --------------------------------%08x %32s\n",
        this->container_tag, this, typeid(T).name());
#endif
  }

  pointer address(reference value) const { return &value; }
  const_pointer address(const_reference value) const { return &value; }

  // For vector allocation: happepns every 2^n elems. We don't want that.
  // Reallocation not allowed if allocated first time.
  // Use limit allowed as max_size
  // No initialization
  pointer allocate(size_type n, const void* hint = 0) {
#ifdef _DEBUG
    context->ReportError(context,
                         "TAG:%d --- In allocate,size(%d) mem_size %d\n",
                         this->container_tag, n, memory_size);
    context->ReportError(context, "TAG:%d --- Init/Prepare?: %d\n",
                         this->container_tag, allocated_init);
#endif
    if (n > memory_size) {
#ifdef _DEBUG
      context->ReportError(
          context, "TAG:%d --- Bad memory size in allocation, got %d\n", n);
#endif
      throw std::bad_array_new_length();
    }
    if (allocated_init == true) {
      return memory_ptr;
    } else if (allocated_init == false) {
      return reinterpret_cast<T*>(raw);
    }

    return memory_ptr;
  }
  // Frees the storage to which ptr refers.
  void deallocate(T* ptr, size_type n) {
#ifdef _DEBUG
    context->ReportError(context, "TAG:%d --- Deallocate ---------------\n",
                         this->container_tag);
#endif
  }  // Do nothing or use TFContext???

  // Initializes the storage of one element to which p refers with value.
  // Equivalent to new( (void*)p)
  void construct(pointer p, const T& value) {
    // std::uninitialized_fill(beg,end,val); //---->Check this out
#ifdef _DEBUG
    context->ReportError(context,
                         "TAG:%d --- CALL FOR ArenaBufferAllocator::construct "
                         "| NO LINE TO EXECUTE.\n",
                         this->container_tag);
#endif
  }

  // Returns the largest value that can be passed to allocate().
  // This max size should be static.
  size_type max_size() const { return memory_size; }

  // A template structure that provides the ability that any allocator may
  // allocate storage of another type indirectly.
  template <typename U>
  struct rebind {
    typedef ArenaBufferAllocator<U> other;
  };

  // Destroys the object to which p refers without deallocating the storage.
  // Simply calls the destructor (~) of the object.
  void destroy(pointer p) {  // Check usage and need?
#ifdef _DEBUG
    context->ReportError(context, "TAG:%d --- destroy %08x\n",
                         this->container_tag, p);
#endif
    p->~T();
  }

  /*   template <class U>
    ArenaBufferAllocator& operator=(const ArenaBufferAllocator<U>&) {
      return *this;
    } */ // Not needed for now
  /*     ArenaBufferAllocator<T> &operator=(const ArenaBufferAllocator &other) {
        return *this;
      } */ //Not needed for now
};

// Returns true if allocators a1 and a2 are interchangeable.
//
// Two allocators are interchangeable if storage allocated from each can be
// deallocated via the other. This function should always return true.
template <typename T, typename U>
bool operator==(const ArenaBufferAllocator<T>& lhs,
                const ArenaBufferAllocator<U>& rhs) noexcept {
  return true;
}

// Returns true if allocators lhs and rhs are not interchangeable. Equivalent
// to
// !(lhs==rhs)
//
// Two allocators are interchangeable if storage allocated from each can be
// deallocated via the other. This function should always return false.
template <typename T, typename U>
bool operator!=(const ArenaBufferAllocator<T>& lhs,
                const ArenaBufferAllocator<U>& rhs) noexcept {
  return false;
}
}  // namespace micro
}  // namespace ops
}  // namespace tflite */
