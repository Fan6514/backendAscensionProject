## 系统上电启动

### 操作系统引导程序

当按下开机键的那一刻，在主板上提前写死的固件程序 **BIOS** 会将硬盘中**启动区的 512 字节**的数据（主引导记录），原封不动复制到**内存中的 0x7c00** 这个位置，并跳转到那个位置进行执行。至于为何是这个地址，可以参考文章[《为什么主引导记录的内存地址是0x7C00？》](http://www.ruanyifeng.com/blog/2015/09/0x7c00.html)。

![](/note/os/pics/boost.png)

**启动区的定义**非常简单，只要硬盘中的 0 盘 0 道 1 扇区的 512 个字节的最后两个字节分别是 **0x55** 和 **0xaa**，那么 BIOS 就会认为它是个启动区。 

此时的 BIOS 仅仅就是个代码搬运工，把 512 字节的二进制数据从硬盘搬运到了内存中而已。**所以作为操作系统的开发人员，仅仅需要把操作系统最开始的那段代码，编译并存储在硬盘的 0 盘 0 道 1 扇区即可**。之后 BIOS 会帮我们把它放到内存里，并且跳过去执行。

Linux 最开始的代码，在源码文件中用汇编语言写的 **bootsect.s**，位于 **boot** 文件夹下。 通过编译，这个 bootsect.s 会被编译成二进制文件，存放在启动区的第一扇区。

```c
#include <asm/boot.h>

SETUPSECTS	= 4				/* setup在磁盘的从第二个扇区开始的4个扇区 */
BOOTSEG		= 0x07C0		/* bootsect会被BIOS加载到0x7c00的位置 */
INITSEG		= DEF_INITSEG	/* 将bootsect位置移到位置0x90000-避开系统模块占用处 */
SETUPSEG	= DEF_SETUPSEG	/* setup程序从0x90200处开始 */
SYSSEG		= DEF_SYSSEG	/* system模块加载到0x10000(64KB)处 */
SYSSIZE		= DEF_SYSSIZE	/* system模块的长度 */
					/* to be loaded */
ROOT_DEV	= 0 			/* ROOT_DEV is now written by "build" */
SWAP_DEV	= 0			/* SWAP_DEV is now written by "build" */
    
.global _start
_start:

	# Normalize the start address
	jmpl	$BOOTSEG, $start2

start2:
	/* 对段寄存器ds, es, ss进行初始化, 使之等于cs*/
	movw	%cs, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	movw	$0x7c00, %sp     /* 初始化栈指针 */
	sti						/* 设置中断 */
	cld						/* 清除方向标志 */

	movw	$bugger_off_msg, %si  /* 寄存器si指向要显示的字符 */
```

我们知道，BIOS 会将 bootsect 加载到  0x7C00 处，在执行完 `jpml` 指令后会跳转到 start2 标号处的代码。也就是后面所有的代码都在以 0x7C00 为基址的内存中寻址。

由于 x86 为了让自己在 16 位这个实模式下能访问到 20 位的地址线这个历史因素，所以段基址要先左移四位。**那 0x07c0 左移四位就是 0x7c00**，那这就刚好和这段代码被 BIOS 加载到的内存地址 0x7c00 一样了。

