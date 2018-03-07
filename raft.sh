#!/bin/bash

#create raft.so file
echo 'compile C code'
gcc -shared -o ./raft.so -fPIC ./*.c ./C/Code/*.c

#run redis
echo 'open Redis server'
sudo docker run -d -p 7777:6379 redis

#add our module to PATH
PATH="$PATH:/home/ubuntu/workspace/raft_project/DN-Project/C/Python"
echo $PATH
#run python
echo 'run raft...'
python3 ./Python/run.py

#close redis
sudo redis-cli shutdown
echo 'close Redis server'
echo 'Exit raft succsessfully'
