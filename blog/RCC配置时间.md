### RCC初始化流程
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
#### 时钟树
![时钟树](images\时钟树.png)  

* OSC_IN引脚, 接的就是板子上的8M晶振.


### TIM计数

1. RCC_APB2ENR_TIM1EN -> 使能TIM1时钟,
2. RCC_APB2RSTR_TIM1RST -> 先置一复位TIM1时钟, 然后回0, 
3. TIMx_CR1_CEN -> 失能计数器,
4. TIMx_PSC, TIMx_ARR -> 预分频和自动重装载
5. TIMx_RCR -> 高级定时器, 还有个重复计数寄存器, 就是设置计数器重装多少次产生一个事件, 本来是计数器每次重装产生一个事件的.
6. TIMx_CR1_DIR -> 设置计数模式(向上,向下, 中央)
7. TIMx_EGR_UG -> 软件置一产生更新事件, 硬件自动清零, 会重新初始化计数器CNT, 
8. TIMx_SR_UIF -> 因为软件生成了更新事件, 需要清除一下 `更新中断标记`
9. TIMx_CR1_CEN -> 使能计数器

### 中断设置
1. SCB_AIRCR_PRIGROUP 设置NVIC优先级分组, 优先级有4个bit, 但是优先级有两种(1.嵌套中断,也就是中断打断中断 2.中断排队), 所以4个bit中几个bit作为嵌套中断优先级就是由这里设置的
2. NVIC_IPRx 用来设置中断通道的优先级, 4个bit有效, 但是参数是u8, 所以其实只有[7:4] 是有效的
3. NVIC_ISERx 使能对应的中断通道