# stm8s-atomthreads-sdcc
STM8S105K4
DHT22
LED BLINK
E-INK

2019年3月14日13点13分
重新修复串口抽风问题，printf发送串口信息时候，会发生死机问题，改用sprintf后再发送的方式，DHT22采样后输出打印正常。

2019-3-13 19:56:19
修复uart.c 串口发送时，怀疑被系统时钟中断打断数据传输，导致运行一段时间后，会出现无法输出串口信息的BUG。