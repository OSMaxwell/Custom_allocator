# Description 
This is a malloc()/ free() hook used to track memory (de)allocations.

# Components
The goal is to have a simple minimal _.cpp_  and _.h_ files to include in debug mode to evaluate and study lifetime of C++ containers (or any element that uses <memory> melloc).

Currently malloc_hook has two version (different implementations):
- __libc_malloc:

    This is currently not working as there is a problem with linking and GCC/G++ to provide this reference. However, we believe that is solved, this should be a more effecient method.

- __malloc_hook:

    Also explained [here](https://www.gnu.org/software/libc/manual/html_node/Hooks-for-Malloc.html). This is a classical way to hook into memory alloactions, however some of the parts are removed, and while compiling some warning might be encountered (and ignored).


Both methods save address and size of memory in an external struct/array for future prints/debugging. Make sure to also compile with '-D _DEBUG' as argument.