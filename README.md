# stm8s-atomthreads-sdcc
STM8S105K4
DHT22
LED BLINK
E-INK

2019-3-13 19:56:19
修复uart.c 串口发送时，怀疑被系统时钟中断打断数据传输，导致运行一段时间后，会出现无法输出串口信息的BUG。