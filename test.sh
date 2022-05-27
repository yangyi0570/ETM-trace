ps -ef | grep demo_helper | grep -v grep | awk '{print $2}' | xargs renice -n -20 -p
