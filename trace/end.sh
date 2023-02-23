sudo rmmod demo

echo "remove demo.ko successfully!"
# kill demo_helper processes
ps -ef | grep demo_helper | grep -v grep | awk '{print $2}' | xargs kill -9
