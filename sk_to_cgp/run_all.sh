#!/bin/bash

sh clean_all.sh
python make_new_benchmarks.py
python make_c50_files.py
python train_trees.py
