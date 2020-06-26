#!/bin/sh
#for ((d=1;d<=5;++d)); do
#for i in {1..5} do
d=1
max=26
while [ $d -lt $max ]
do
	rm -r "cgp$d"
	mkdir "cgp$d"
	cp cgp cgp$d/
	d=$(( d+1 ))
done





