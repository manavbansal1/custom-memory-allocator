# Custom Memory Allocator  

## Overview  
This project implements a custom memory allocator in C, similar in spirit to `malloc()` and `free()`. The allocator manages heap memory using a linked list of blocks, supports multiple allocation strategies, and provides runtime statistics about memory usage.  

The implementation is entirely built from scratch using `sbrk()` for heap extension, without relying on standard memory allocation functions such as `malloc()` or `free()`.  

## Features  
- **Custom Allocation (`alloc`)**  
  Allocates memory blocks from the heap. If sufficient memory is not available, the allocator extends the heap in fixed increments.  

- **Custom Deallocation (`dealloc`)**  
  Frees memory blocks and inserts them back into the free list. Adjacent free blocks are automatically coalesced to avoid fragmentation.  

- **Allocation Strategies (`allocopt`)**  
  Supports three memory allocation algorithms:  
  - **First Fit** – Selects the first available block that is large enough.  
  - **Best Fit** – Selects the smallest block that satisfies the request.  
  - **Worst Fit** – Selects the largest block available.  

- **Heap Growth Control**  
  The allocator extends the heap only in fixed increments (`INCREMENT`), respecting a user-specified limit.  

- **Memory Statistics (`allocinfo`)**  
  Provides runtime statistics including:  
  - Total free memory size  
  - Number of free chunks  
  - Largest free chunk size  
  - Smallest free chunk size  

## Design Details  
- **Heap Extension:**  
  Memory is requested from the system using `sbrk()` in multiples of a predefined increment.  

- **Free List Management:**  
  Free blocks are maintained in a linked list using a custom `header` structure stored directly on the heap. New free blocks are added to the head of the list.  

- **Block Splitting & Coalescing:**  
  - Large free blocks can be split to satisfy smaller allocation requests.  
  - Contiguous free blocks are coalesced during deallocation to reduce fragmentation.  

- **Reset Behavior:**  
  Calling `allocopt()` resets the allocator to its initial state, including restoring the program break to the original position.  

## File Structure  
```
.
├── CMakeLists.txt    # Build configuration
├── README.md         # Project documentation
├── include/
│   └── alloc.h       # Data structures and function prototypes
└── src/
    ├── alloc.c       # Implementation of allocator
    └── main.c        # Test driver
```

## Building & Running  
### Prerequisites  
- C compiler (e.g., `gcc`)  
- CMake (>= 3.10)  

### Build Instructions  
```bash
mkdir build
cd build
cmake ..
make
```

### Run  
```bash
./main
```

This will run the included test driver (`main.c`) that exercises the allocator and outputs test results.  

## Example Usage  
```c
#include "alloc.h"

int main() {
    // Set allocator to FIRST_FIT with 1024-byte limit
    allocopt(FIRST_FIT, 1024);

    // Allocate and free memory
    void *p = alloc(64);
    dealloc(p);

    // Get allocator statistics
    struct allocinfo info = allocinfo();
    write(1, &info.free_size, sizeof(info.free_size)); // Using write instead of printf
}
```

## Debugging  
Debugging was performed using **cgdb**, which allows direct inspection of the heap layout while stepping through allocations and deallocations. Since logging is limited when memory functions are restricted, cgdb was essential for verifying heap consistency.  

## Lessons Learned  
- Implementing memory management requires careful handling of linked list operations and boundary conditions.  
- Strategies like **coalescing** are critical to prevent memory fragmentation.  
- Debugging at the heap level provides deeper insights into system programming compared to typical application-level debugging.  

## Motivation  
I built this project to deepen my understanding of low-level memory management and operating system concepts. Modern applications rely heavily on allocators, but we rarely see how they work under the hood. By implementing one from scratch, I gained hands-on experience with:  
- Heap extension using `sbrk()`  
- Free list management  
- Allocation algorithms (first-fit, best-fit, worst-fit)  
- Trade-offs in performance vs. fragmentation  
- Debugging at the systems level  

This project provided valuable insights into how real-world allocators (like `malloc`/`free`) operate behind the scenes.


## Acknowledgment  
The project requirements and initial specifications were adapted from the [SFU CMPT-201 Assignment 9 repository](https://github.com/SFU-CMPT-201/a9-pub).
