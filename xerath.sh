#!/bin/bash

for i in 1000000000
do
for j in {1..3}
do
./mont $i >> datos.txt
done
done
