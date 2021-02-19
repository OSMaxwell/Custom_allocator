/*prototypes*/
// static void cus_init_hook(void);
// static void *cus_malloc_hook(size_t, const void *);
// static void cus_free_hook(void *, const void *);

// /*original hooks*/
// static void *(*old_malloc_hook)(size_t, const void *);
// static void (*old_free_hook)(void *, const void *);

// // Use this to replace hooks.
// static void cus_init_hook(void) {
//   old_malloc_hook = __malloc_hook;
//   old_free_hook = __free_hook;
//   __malloc_hook = cus_malloc_hook;
//   __free_hook = cus_free_hook;
// }

// // A custom hook for malloc
// static void *cus_malloc_hook(size_t size, const void *caller) {
//   void *result;
//   /* Restore all old hooks */
//   __malloc_hook = old_malloc_hook;
//   __free_hook = old_free_hook;
//   /* Call recursively */
//   result = malloc(size);
//   /* Save underlying hooks */
//   old_malloc_hook = __malloc_hook;
//   old_free_hook = __free_hook;
//   /* printf might call malloc, so protect it too. */
//   printf("Malloc (%i) returns %p\n", size, result);
//   /* Restore our own hooks */
//   __malloc_hook = cus_malloc_hook;
//   __free_hook = cus_free_hook;
//   // global.TagToStaticBuf.insert({result,size}); <- This call calls for more
//   // malloc which calls for map elements!!!
//   global.address_table[global.tag] = result;
//   global.size_table[global.tag] = size;
//   global.tag++;
//   return result;
// }
// // A custom hook for free
// static void cus_free_hook(void *ptr, const void *caller) {
//   /* Restore all old hooks */
//   __malloc_hook = old_malloc_hook;
//   __free_hook = old_free_hook;
//   /* Call recursively */
//   free(ptr);
//   /* Save underlying hooks */
//   old_malloc_hook = __malloc_hook;
//   old_free_hook = __free_hook;
//   /* printf might call free, so protect it too. */
//   printf("freed pointer %p\n", ptr);
//   /* Restore our own hooks */
//   __malloc_hook = cus_malloc_hook;
//   __free_hook = cus_free_hook;
// }
