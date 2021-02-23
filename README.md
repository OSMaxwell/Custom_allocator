# Description 
This is a malloc()/ free() hook used to track memory (de)allocations.

# Components
The goal is to have a simple minimal _.cpp_  and _.h_ files to include in debug mode to evaluate and study lifetime of C++ containers (or any element that uses <memory> melloc).

Currently malloc_hook has two version (different implementations):
- __libc_malloc: (**DEPRECATED**)

    This is currently not working as there is a problem with linking and GCC/G++ to provide this reference. However, we believe that is solved, this should be a more effecient method.

- __malloc_hook:

    Also explained [here](https://www.gnu.org/software/libc/manual/html_node/Hooks-for-Malloc.html). This is a classical way to hook into memory alloactions, however some of the parts are removed, and while compiling some warning might be encountered (and ignored).


Both methods save address and size of memory in an external struct/array for future prints/debugging. Make sure to also compile with '-D _DEBUG' as argument.

# Screenshots
_taken from terminal:_

|Counter|Operation|Pointer|Size|
|-------|----------|--------------|----|
|80     |malloc    |0x56361441c4f0|   4|                   
|81     |malloc    |0x56361441c510|   8|                  
|82     |free      |0x56361441c4f0|    |
|82     |malloc    |0x56361441c4f0|  16|                  
|83     |free      |0x56361441c510|    |                  
|83     |malloc    |0x56361441c7a0|  32|                  
|84     |free      |0x56361441c4f0|    |                  
|84     |malloc    |0x56361441c590|  64|                  
|85     |free      |0x56361441c7a0|    |                
|85     |malloc    |0x56361441c8e0|  64| 


# Compile
To compile, run:
```
g++ -o main.o main.cpp -Wno-deprecated-declarations
```
As the current version is using **__malloc_hook** method, no particular std/lib version is required.

# Missing 
- [x] Fix incremental tag in free_hook.
- [x] \(Not needed) Add system logging with date.
- [ ] Add different test cases with different containers.
- [x] \(Deprecated) Fix __libc_malloc method.