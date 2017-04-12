#!/bin/bash

echo -n > ./mapping_data
for i in {1..1000000}; do
  echo -e "$RANDOM\t$RANDOM" >> mapping_data
done
