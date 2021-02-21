// A polymorphic monotonic_buffer_resource-based class to create memory
// allocation pools Use only with GCC9.1, C++17 and above.
#include "pmr_allocator.h"
#include <stdio.h>
#include <memory_resource>  // C++17

// class custom_monotonic_buffer_resource : std::pmr::monotonic_buffer_resource
// {
//   memory_resource *_Resource = std::pmr::get_default_resource();
//   void *do_allocate(const size_t _Bytes, const size_t _Align) override {
//   return _Resource } void do_deallocate(void *, size_t, size_t) override {}
//   // nothing to do
// };

GLOBAL global;

void inline pretty_header() {
  printf("%-7s%-10s%-35s%-20s\n", "Tag", "Operation", "Pointer", "Size");
}

class custom_resource : public std::pmr::memory_resource {
 public:
  explicit custom_resource(
      std::pmr::memory_resource *up = std::pmr::get_default_resource())
      : _upstream{up}, tag(global.tag) {
    printf("%-7i%-10s%-35p\n",tag,"Construct", up);
    global.tag++;
  }

  void *do_allocate(size_t bytes, size_t alignment) override {
    printf("%-7i%-10s%-35p%-20i\n", tag, "allocate",_upstream, bytes);
    return _upstream;
  }

  void do_deallocate(void *ptr, size_t bytes, size_t alignment) override {
    printf("%-7i%-10s%-35p%-20i\n", tag, "deallocate",ptr, bytes);
  }

  bool do_is_equal(const std::pmr::memory_resource &other) const
      noexcept override {
    printf("IS EQUAL\n");
    return this == &other;
  }

 private:
  std::pmr::memory_resource *_upstream;
  int tag;
};