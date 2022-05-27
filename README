使用coresight的etm进行追踪
前提步骤
1. 安装mount_cs_device模块（在mount_cs_device文件夹中，安装后可以多次使用，不用每次追踪都安装再卸载）

运行
2. 运行要检测的程序demo_helper
3. 设置demo_helper的优先级为-20（需要sudo权限）
4. 安装demo.ko模块（使能etm对CPU的指令流追踪）

结束
5. 移除demo.ko模块（关闭etm对CPU的指令流追踪）
6. 结束demo_helper进程

分析
7. sudo dd if=/dev/cs_device of=output_file 将数据从虚拟的字符设备中读取
8. ptm2human -e -i output_file > decoded_file