## 深入理解链接器

什么是链接器（Linker）： **链接器**是一个将编译器产生的目标文件打包成可执行文件，或者库文件，或者目标文件的程序。 

-   链接器本质上也是一个程序，本质上和我们经常使用的普通程序没什么不同；
-   链接器的输入是编译器编译好的目标文件；
-   链接器在将目标文件打包处理后，生成或者可执行文件，或者库，或者目标文件。

链接器就像压缩软件一样，将一堆目标文件打包为一个文件。

所有的应用程序都是链接器将所需要的一个个简单的目标文件汇集起来形成的， 链接器的工作过程如下：

-    链接器对给定的目标文件或库的集合进行**符号决议**以确保模块间的依赖是正确的；
-    链接器将给定的目标文件集合进行**拼接打包**成需要的库或最终可执行文件 ；
-    链接器对链接好的库或可执行文件进行**重定位**。

### 符号决议

符号决议过程中， 链接器需要做的工作就是**确保所有目标文件中的符号引用都有唯一的定义**。首先我们要清楚典型的 c 文件中有哪些。

源文件的变量包括：

-   全局变量： 只要程序没有结束运行，全局变量都可以随时使用。  用 static 修饰的全局变量，其生命周期也等同于程序的运行周期，只是这种全局变量只能在所被定义的文件当中使用，对其它文件不可见。 
-   局部变量： 局部变量只能在相应的函数内部使用，当函数调用完成后该函数中的局部变量也就无法使用了。局部变量只存在与函数的栈帧中，函数调用后相应的栈帧会被回收。

源文件编译后形成对应的目标文件，其本质包括两部分：

-   代码部分： 这里的代码指的是计算机可以执行的机器指令，也就是源文件中定义的所有函数。
-    数据部分 ： 源文件中定义的全局变量。如果是已经初始化后的全局变量，该全局变量的值也存在于数据部分。

局部变量是函数私有的，局部变量只能在该函数内部使用，而全局变量没有这个限制的。所以函数私有的**局部变量被放在了代码段中**，作为机器指令的操作数。 

编译器在遇到外部定义的全局变量或函数时，只要在当前文件中找到其声明，编译器就认为编译正确，而寻找变量定义的工作交给了链接器。其中链接器的一项工作就是确定所**使用的变量要有唯一的定义**。

**符号表**

虽然编译器留给了链接器一项任务，但编译器为了链接器工作轻松一点，制作了符号表（ Symbol table ）。

符号表中保存的信息有两部分：

-   该目标文件中引用的全局变量以及函数
-   该目标文件中定义的全局变量以及函数

 编译器在编译过程中每次遇到一个全局变量或者函数名都会在符号表中添加一项，最终编译器会统计出如下所示的一张符号表： 

|    名字     |     类型     | 是否被外部引用 |  区域  |
| :---------: | :----------: | :------------: | :----: |
| global_int  | 引用，未定义 |                |        |
| extern_func | 引用，未定义 |                |        |
| static_int  |     定义     |       否       | 数据段 |
| static_func |     定义     |       否       | 代码段 |
|  local_int  |     定义     |       否       | 数据段 |

其中 `global_int` 和 `extern_func` 是外部定义的全局变量和函数，当前文件中只是声明，编译器并没有找到其定义。剩余的变量编译器都可以在当前文件中找到其定义。 

`static_func` 为当前文件定义的局部函数，因此在代码段；剩余的符号都是全局变量，因此在数据段。

在C语言中**经static修饰**过的函数以及变量都是当前**文件私有**的，对外部不可见 。

有了符号表，编译生成的目标函数就由以下三部分构成：

![](D:\study\backendAscensionProject\note\c_cpp\pics\objectfile.png)

有了符号表，链接器就可以进行符号决议了。 

**符号决议的过程**

符号表给链接器提供了两种信息，一个是当前目标文件可以提供给其它目标文件使用的符号，另一个其它目标文件需要提供给当前目标文件使用的符号。有了这些信息链接器就可以进行符号决议了。 

假设链接器需要链接三个目标文件 ，链接器会依次扫描每一个给定的目标文件，同时链接器还维护了两个集合，一个是已定义符号集合D，另一个是未定义符合集合U，下面是**链接器进行符合决议**的过程： 

-   对于当前目标文件，查找其符号表，并将已定义的符号并添加到已定义符号集合D中。 
-   对于当前目标文件，查找其符号表，将每一个**当前目标文件引用的符号与已定义符号集合D进行对比**，如果该符号不在集合D中则将其添加到未定义符合集合U中。 
-   当所有文件都扫描完成后，如果为定义符号集合U不为空，则说明当前输入的目标文件集合中有未定义错误，链接器报错，整个编译过程终止。 

 一句话概括就是只要每个目标文件所引用变量都能在其它目标文件中找到唯一的定义，整个链接过程就是正确的 。

假设 math.c 的数字计算程序，其中定义了一个 add 函数，该函数在 main.c 中被引用到。 我们只需要在 main.c 中 include 写好的 math.h 头文件就可以使用add函数了，如图所示：

![](D:\study\backendAscensionProject\note\c_cpp\pics\reference.png)

将这里 math.c 文件中的代码注释掉之后再重新编译，就会出现 undefined reference to 'add(int, int)' 错误，如图所示： 

![](D:\study\backendAscensionProject\note\c_cpp\pics\undefine.png)

该错误产生的过程如下：

-   链接器通过检查符号表发现代码 main.o 中引用了外部定义的 add 函数， 所以链接器开始查找add函数到底是在哪里定义的。
-   链接器去 math.o 的目标文件符号表中查找，没有找到add函数的定义。
-   链接器转而去其它目标文件符号表中查找，同样没有找到add函数的定义 。
-   链接器在所有的目标文件符号表中都没有查找到 add 函数， 因此链接器停止工作并报出错误 undefined reference to `add(int, int)'。

综上所述，链接器在符号决议中会进行如下的排查工作：

-   函数的函数名 add 有没有写正确。
-   链接命令中有没有包含 math.o，如果没有添加上该目标文件。
-   如果链接命令没有问题，查看 math.c 中定义的 add 函数定义是否有问题。 
-   如果是 C 和 C++ 混合编程时，确保相应的位置添加了 extern "C"。 
