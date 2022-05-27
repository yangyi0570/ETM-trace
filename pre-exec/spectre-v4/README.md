# 结论

测试环境为：Linux hikey970 4.9.78-147538-g244928755bbe #1 SMP PREEMPT Thu May 31 10:58:02 CST 2018 aarch64 aarch64 aarch64 GNU/Linux。

成功泄露数据。

# 使用

1. 为了使用PMU counter进行计数，需要先使用onlinecpu.sh下线其他核，只保留其中一个，该脚本接受一个参数，参数为1表示上线0-6核，如果参数为0，则表示下线其他核
2. 编译PMU使能内核模块，然后将其装载
3. 编译运行spectre_v4

# 注意

在编译spectre_v4时，选项中一定要加入 -Os 优化



