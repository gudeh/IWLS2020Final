#!/bin/bash

sh clean_all.sh
python make_benchmarks_4.py
rm -rf ../../Benchmarks_4 
mv Benchmarks_4 ../../
python make_new_benchmarks.py
python make_c50_files.py
python train_trees.py