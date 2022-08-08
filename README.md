# juno r2基于coresight的跟踪系统
本项目模块利用coresight中的etm组件对处理器核进行跟踪，在用户空间通过虚拟字符设备将跟踪数据读出。
## 项目组成
1. 使能coresight的内核模块。
2. 挂载虚拟字符设备cs_device的内核模块。（mount_cs_device目录下）
## 当前进度
1. 实现coresight组件链的使能。juno r2的组件链为etm->funnel->etf。
2. 实现了读出跟踪数据：挂载虚拟字符设备，由本模块从etf中读取数据并写入到虚拟字符设备中，再从用户空间读字符设备获取跟踪数据。
3. PMU可以输出到跟踪流中。并简单验证了其正确性。
## 实现细节
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
## TODO
- 挂载虚拟字符设备的模块，其开辟的内存空间过小，考虑修改为vmalloc，而不是现有的kmalloc。
- 跟踪出来的信息是否足够呢？目前来看，对于单核而言，最多可以跟踪到4个PMU事件。那么多核又是怎样的呢？以及，能否跟踪出多于4个的PMU事件数呢？
- 能不能对目前可复现的侧信道做一次监测，看看效果。
- 通过内核模块的使能方式仍然需要考察。其是否高效呢？如果真的在PMI中开关使能模块是否合理呢。
## 问题
1. 20220524/1050.bin为使能pmu事件导出到etm后，跟踪得到的数据。ptm2human无法解析。所以，需要修改ptm2human，并检查跟踪数据是否有pmu事件的格式出现。
2. 当前版本在除去pmu输入到etm后，能够跟踪出数据。但有如下两个问题：
    * 只有在通过/sys下的coresight接口使能一次后，本模块才可以正常使能或关闭。
    * 跟踪出来的数据过少，虽然虚拟字符设备里存储跟踪数据的空间只有64kB，但是在被跟踪核上运行死循环一秒，跟踪数据只有3000多byte显然有点少了。
## 使用方式
hikey 970上的使用过程，和juno r2并不完全一致。在juno r2上，运行过程只需要将程序绑定至被跟踪核即可，不需要调整优先级。
### 前提步骤
1. 安装mount_cs_device模块（在mount_cs_device文件夹中，安装后可以多次使用，不用每次追踪都安装再卸载）
### 运行
2. 运行要检测的程序demo_helper
3. 设置demo_helper的优先级为-20（需要sudo权限）
4. 安装demo.ko模块（使能etm对CPU的指令流追踪）
### 结束
5. 移除demo.ko模块（关闭etm对CPU的指令流追踪）
6. 结束demo_helper进程
### 分析
7. sudo dd if=/dev/cs_device of=output_file 将数据从虚拟的字符设备中读取
8. ptm2human -e -i output_file > decoded_file

## 通过/sys接口使能的方式
root@junor2:/sys/bus/coresight/devices# echo "1">20010000.etf/enable_sink 
root@junor2:/sys/bus/coresight/devices# echo "1">22140000.etm/enable_source 
root@junor2:/sys/bus/coresight/devices# echo "0">22140000.etm/enable_source 
root@junor2:/sys/bus/coresight/devices# echo "0">20010000.etf/enable_sink

## 参考

