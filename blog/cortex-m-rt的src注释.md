- [stm32的启动链接脚本](#stm32的启动链接脚本)
- [设置CCRAM `_stack_start`](#设置ccram-_stack_start)
- [杂项](#杂项)
  - [CCRAM](#ccram)
- [命令](#命令)
  - [查看内存布局 size](#查看内存布局-size)

### [stm32的启动链接脚本](https://www.emoe.xyz/stm32-boot-modeboot-filelinkerscript-analyze/#12_STM32Flash0x0800_0000)
1. 复位后，Cortex-M3内核只能从0x0地址启动，但通过写向量偏移寄存器VTOR（0xE000 ED08），可以在程序执行过程中重定位中断向量表。通过跳线设置启动方式, 就是设置向量偏移寄存器. 
2. 设置向量偏移寄存器，将0x0800_0000地址(FLASH) 映射到 0x00, 内核规定在地址0x0提取的第一个参数为SP(栈起始地址)的初始值，第二个参数（位于0x0000 0004）为复位中断服务程序地址，(程序代码的起始地址, main() 就在这里)。


### 设置CCRAM `_stack_start`
This optional symbol can be used to indicate where the call stack of the program should be
placed. If this symbol is not used then the stack will be placed at the *end* of the `RAM`
region -- the stack grows downwards towards smaller address. This symbol can be used to place
the stack in a different memory region, for example:
```text
/* Linker script for the STM32F303VCT6 */
MEMORY
{
    FLASH : ORIGIN = 0x08000000, LENGTH = 256K
    /* .bss, .data and the heap go in this region */
    RAM : ORIGIN = 0x20000000, LENGTH = 40K
    /* Core coupled (faster) RAM dedicated to hold the stack */
    CCRAM : ORIGIN = 0x10000000, LENGTH = 8K
}
_stack_start = ORIGIN(CCRAM) + LENGTH(CCRAM);
```

### 杂项
#### [CCRAM](https://zhuanlan.zhihu.com/p/306407422)
对于CCRAM，CPU能以最大的系统时钟和最小的等待时间从CCM中读取数据或者代码。官方文档说明了使用CCM的一些优势：比如将频繁读取的数据放到CCM，将中断函数放到CCM，这都能加快程序的执行速度。

### 命令
#### 查看内存布局 size
```bash
$ size -Ax target/thumbv7m-none-eabi/debug/app
target/thumbv7m-none-eabi/debug/app  :
section               size         addr
.vector_table        0x130    0x8000000
.text               0x4820    0x8000130
.rodata              0xf40    0x8004950
.data                  0x0   0x20000000
.bss                  0x1c   0x20000000
.uninit                0x0   0x2000001c
.debug_loc           0x4d5          0x0
.debug_abbrev       0x464e          0x0
.debug_info        0x7563d          0x0
.debug_aranges      0x8a38          0x0
.debug_ranges      0x296a0          0x0
.debug_str         0x9a805          0x0
.debug_pubnames    0x22bb1          0x0
.debug_pubtypes    0x11fb5          0x0
.ARM.attributes       0x32          0x0
.debug_frame       0x1ae38          0x0
.debug_line        0x4ad6f          0x0
.comment              0x13          0x0
Total             0x1e6a3b
```