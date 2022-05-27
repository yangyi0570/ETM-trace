# sudo ./pre-exec/spectre-v4/pmu/onlinecpu.sh 0

taskset -c 7 ./demo_helper_simple_branch & 1>/dev/null

# set demo_helper priority
sudo bash -c "ps -ef | grep demo_helper | grep -v grep | awk '{print $2}' | renice -n -20 -p"

sudo insmod demo.ko

echo "install demo.ko successfully!"

# taskset -c 7 ./pre-exec/v1 &

# sudo rmmod demo

# echo "remove demo.ko successfully!"
