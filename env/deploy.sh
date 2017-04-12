#!/bin/bash

rm -rf ~/shm_serving
mkdir -p ~/shm_serving/bin ~/shm_serving/glog
cp -r conf data ~/shm_serving
cp ../build/shm_serving/serving/serving ~/shm_serving/bin
cp ../build/tools/shared_memory_remover ~/shm_serving/bin
cp ../build/example/visitor ~/shm_serving/bin
