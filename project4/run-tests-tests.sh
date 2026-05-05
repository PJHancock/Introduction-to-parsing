#!/bin/bash

# script for project 4 example tests

program="project4"
numbers="40 44 45 46 47 48 49 61 62"
testdir="project4-tests"
diffopts=" -a -i -b -w -B "  # ignore whitespace

g++ -Wall -std=c++17 -g *.cpp -o $program

for number in $numbers ; do
 
    echo "Running input" $number

    inputfile=$testdir/in$number.txt
    answerfile=$testdir/out$number.txt
    outputfile=actual$number.txt

    ./$program $inputfile > $outputfile

    diff $diffopts $answerfile $outputfile || echo "diff failed on test" $number

    rm $outputfile

done

rm $program

