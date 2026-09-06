#!/bin/bash

func_dir="functions"

func_arg=""

for entry in "$func_dir"/*
do
    func_arg+=" $entry"
done 

gcc main.c $func_arg -Wall -Wextra -o shell