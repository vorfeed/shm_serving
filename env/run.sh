#!/bin/bash

cd ~/shm_serving
./bin/serving --daemon
sleep 2
./bin/visitor
ps -elf | grep 'bin/serving' | awk '{print $4}' | xargs kill -9 > /dev/null
./bin/shared_memory_remover
