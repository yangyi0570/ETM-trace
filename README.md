# 基于coresight的跟踪系统
本项目模块利用coresight中的etm组件对处理器核进行跟踪，在用户空间通过虚拟字符设备将跟踪数据读出。

## 项目组成
1. 使能coresight的内核模块。
2. 挂载虚拟字符设备cs_device的内核模块。（mount_cs_device目录下）

## 当前进度
1. 实现coresight组件链的使能。juno r2的组件链为etm->funnel->etf。
2. 实现了读出跟踪数据：挂载虚拟字符设备，由本模块从etf中读取数据并写入到虚拟字符设备中，再从用户空间读字符设备获取跟踪数据。
3. PMU可以输出到跟踪流中。并简单验证了其正确性。

## 实现细节
### 跨平台实现
在etmtrace 0.3后，将hikey970与juno r2的使能模块集成到一起。根据makefile的宏定义来判断编译哪些代码，主要集中在register.h和connect.h文件上。**在trace.c和connect.c中，需要自己硬编码对当前平台需要使能的coresight部件。并且PMU事件跟踪在etm.c中，需要注意对于不同平台它的PMU事件号不同，也需要重新写。**
### PMU事件跟踪
首先需要明确，PMU事件是作为外部事件导入到ETM中。在其内部通过数据总线连接两者。
所以工作分为两部分：
1. 对于PMU而言，它需要通过置位PMU的寄存器PMCR_EL0的PMCR_X位，来使能PMU的输出。
2. 配置ETM将PMU作为外部事件接收。
    - TRCEXTINSELR内部可以填入外部事件号。PMU的事件号需查对于具体某个处理器型号的技术手册。比如对于A72而言，其手册中，PMU event bus指明了各事件在导入到trace时对应的事件号。例如，cycle的事件号为16。但实际写入到事件字段时，需要+4。因为前四个外部事件事件号是CTI的。
    - TRCRSCTLRn寄存器用于选择资源。这里资源的定义是广泛的，可以是地址过滤器也可以是计数器等。这里选择外部事件。因为在TRCEXTINSELR中已经写入了想要跟踪的PMU事件的外部事件号。由TRCRSCTLRn选中外部事件号，作为被跟踪的资源。
    - TRCEVENTCTL0R选中TRCRSCTLRn来选择不同的资源，作为输出的对象。
    - TRCEVENTCTL1R通过置位使得对应的资源能够输出跟踪元素。
### ETB
对于juno r2而言，通过tmc_info_etf函数可以查到其size为64KB。
### funnel
对于funnel的配置需要注意的是，由于funnel前序有数据流入，所以这里需要指定哪几个上游端口打开。
### 过滤
由于本项目是针对EL0级侧信道攻击的防御，所以可以通过设置内存地址范围过滤或者异常等级过滤来减少数据量的产生。

## TODO
- 挂载虚拟字符设备的模块，其开辟的内存空间过小，考虑修改为vmalloc，而不是现有的kmalloc。
- 对于hikey而言，需要重写etr的导出，目前还没有写入cs_device中，因为用的还是kmalloc，容量太小。
- 跟踪出来的信息是否足够呢？目前来看，对于单核而言，最多可以跟踪到4个PMU事件。那么多核又是怎样的呢？以及，能否跟踪出多于4个的PMU事件数呢？
- 能不能对目前可复现的侧信道做一次监测，看看效果。
- 通过内核模块的使能方式仍然需要考察。其是否高效呢？如果真的在PMI中开关使能模块是否合理呢。（可以通过sys接口来控制
- 过滤？

## 问题
1. 在etmtrace 0.1版本中出现的PMU与执行流跟踪的问题，在etmtrace0.2中解决。

## 使用方式
1. 安装mount_cs_device模块（在mount_cs_device文件夹中，安装后可以多次使用，不用每次追踪都安装再卸载）
    > sudo insmod mount_cs_device.ko
2. 安装demo.ko模块（使能etm对CPU的指令流追踪）
    > sudo insmod demo.ko
3. 移除demo.ko模块（关闭etm对CPU的指令流追踪）
    > sudo rmmod demo.ko
4. 将数据从虚拟的字符设备中读取
        > sudo dd if=/dev/cs_device of=output_file
5. 使用ptm2human进行解码
    > ptm2human -e -i output_file > decoded_file

## 注意事项
1. hikey上使用etm时，4核是不可以的，其他核还没测试，不太清楚原因。7核是没问题的。

## 通过/sys接口使能的方式
> root@junor2:/sys/bus/coresight/devices# echo "1">20010000.etf/enable_sink 
root@junor2:/sys/bus/coresight/devices# echo "1">22140000.etm/enable_source 
root@junor2:/sys/bus/coresight/devices# echo "0">22140000.etm/enable_source 
root@junor2:/sys/bus/coresight/devices# echo "0">20010000.etf/enable_sink

## 参考

