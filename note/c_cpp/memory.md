# C/C++ 内存管理

Linux 系统在装载 elf 格式的程序文件时，会调用 loader 把可执行文件中的各个段依次载入到从某一地址开始的空间中。

用户程序可以直接使用系统调用来管理 heap 和mmap 映射区域，但更多的时候程序都是使用 C 语言提供的 malloc()和 free()函数来动态的分配和释放内存。stack区域是唯一不需要映射，用户却可以访问的内存区域。

## 进程内存布局

计算机系统分为32位和64位，而32位和64位的进程布局是不一样的，即使是同为32位系统，其布局依赖于内核版本，也是不同的。以x86平台Linux进程内存分布为例，内存空间从低地址到高地址依次为：代码段、（初始化）数据段、（未初始化）数据段（BSS）、堆、mmap、栈、命令行参数和环境变量。其中堆的内存空间向高地址生长，栈的内存空间向低地址生长。

C语言中几种变量类型：全局变量、局部变量和静态变量。C函数默认为全局函数，在其他“.c”文件中需要使用关键字 `extern` 修饰声明，如果需要定义为静态函数只作用于本文件中，需要使用关键字 `static` 修饰声明。

- 栈区（Stack）— 存储程序执行期间的本地变量和函数的参数，从高地址向低地址生长
- 堆区（Heap）动态内存分配区域，通过 malloc、new、free 和 delete 等函数管理
- 未初始化变量区（BSS）— 存储未被初始化的全局变量和静态变量
- 数据区（Data）— 存储在源代码中有预定义值的全局变量和静态变量
- 代码区（Text）— 存储只读的程序执行代码，即机器指令

用户程序可以直接使用系统调用来管理 heap 和mmap 映射区域，但更多的时候程序都是使用 C 语言提供的 malloc()和 free()函数来动态的分配和释放内存。stack区域是唯一不需要映射，用户却可以访问的内存区域。

操作系统提供了相关的系统调用来完成内存分配工作。

- 对于heap的操作，操作系统提供了brk()函数，c运行时库(glibc)提供了sbrk()函数。
- 对于mmap映射区域的操作，操作系统提供了mmap()和munmap()函数。

sbrk()，brk() 或者 mmap() 都可以用来向我们的进程添加额外的虚拟内存。而glibc就是使用这些函数来向操作系统申请虚拟内存，以完成内存分配的。

> 内存的延迟分配，只有在真正访问一个地址的时候才建立这个地址的物理映射，这是 Linux 内存管理的基本思想之一。Linux 内核在用户申请内存的时候，只是给它分配了一个线性区（也就是虚拟内存），并没有分配实际物理内存；只有当用户使用这块内存的时候，内核才会分配具体的物理页面给用户，这时候才占用宝贵的物理内存。内核释放物理页面是通过释放线性区，找到其所对应的物理页面，将其全部释放的过程。

## 内存分配和管理

C 语言为内存的分配和管理提供了几个函数，声明在 <stdlib.h> 头文件中。

|  函数   | 描述  |
|  ----  | ----  |
| void *calloc(int num, int size);  | 在内存中动态地分配 num 个长度为 size 的连续空间，并将每一个字节都初始化为 0。所以它的结果是分配了 num*size 个字节长度的内存空间，并且每个字节的值都是0。 |
| void free(void *address);  | 该函数释放 address 所指向的内存块,释放的是动态分配的内存空间。 |
| void *malloc(int num);  | 在堆区分配一块指定大小的内存空间，用来存放数据。这块内存空间在函数执行完成后不会被初始化，它们的值是未知的。 |
| void *realloc(void *address, int newsize);  | 该函数重新分配内存，把内存扩展到 newsize。 |