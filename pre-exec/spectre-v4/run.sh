
sudo ./pmu/onlinecpu.sh 0

sudo insmod ./pmu/kernel_mod/enable_arm_pmu.ko

# sudo ./pmu/onlinecpu.sh 1

taskset -c 7 ./v4_tmp 

# sudo rmmod enable_arm_pmu

# sudo ./pmu/onlinecpu.sh 1
