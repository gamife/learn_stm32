1. RCC_CR_HSEON -> 开启 HSE 高速外部时钟, 也就是stm32f103c8t6最小板上的8M晶振, 
2. RCC_CR_HSERDY -> 等待HSE开启完成, 
3. RCC_CFGR_PLLXTPRE -> 设置PREDIV1, 也就是HSE作为PLL的输入的分频,
4. RCC_CFGR_PLLSRC -> 设置锁相环PLL的输入时钟源,
5. RCC_CFGR_PLLMUL -> 设置PLL的倍数, 
6. RCC_CFGR_HPRE -> 设置AHB预分频(输入是SYSCLK), 得到HCLK,
7. RCC_CFGR_PPRE1 和 RCC_CFGR_PPRE2 -> 设置APB1和APB2预分频,
8. RCC_CR_PLLON -> 开启PLL
9. RCC_CR_PLLRDY -> 等待PLL开启成功
10. RCC_CFGR_SW -> 选择 SYSCLK 系统时钟的来源, 设置成PLL
11. RCC_CFGR_SWS ->  等待 SYSCLK 系统时钟切换成设置的来源
## 时钟树
![时钟树](images\时钟树.png)  

* OSC_IN引脚, 接的就是板子上的8M晶振.