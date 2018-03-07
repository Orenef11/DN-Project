#!/bin/bash

#create raft.so file
echo 'compile C code'
gcc -shared -o ./raft.so -fPIC ./*.c ./C/Code/*.c 1>/dev/null

#run redis
echo 'open Redis server'
sudo docker run -d -p 7777:6379 redis 1>/dev/null

#add our module to PATH
PATH="$PATH:/home/ubuntu/workspace/raft_project/DN-Project/C/Python"
#run python
echo 'run raft...'
python3 ./Python/run.py

#close redis
sudo redis-cli shutdown 1>/dev/null
echo 'close Redis server'
echo 'Exit raft succsessfully'
