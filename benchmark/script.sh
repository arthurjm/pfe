#!/bin/bash

#remove the previous result file 
rm result.txt

# default segmentation
./benchmark -p 100 200 300 400 500 600 700 800 900 1000 -s -df
# metrics
./benchmark -p 100 200 300 400 500 600 700 800 900 1000 -s -x -df
./benchmark -p 100 200 300 400 500 600 700 800 900 1000 -s -y -df
./benchmark -p 100 200 300 400 500 600 700 800 900 1000 -s -z -df
./benchmark -p 100 200 300 400 500 600 700 800 900 1000 -s -r -df
# 2 metrics
./benchmark -p 100 200 300 400 500 600 700 800 900 1000 -s -x -y -df
./benchmark -p 100 200 300 400 500 600 700 800 900 1000 -s -x -z -df
./benchmark -p 100 200 300 400 500 600 700 800 900 1000 -s -x -r -df
./benchmark -p 100 200 300 400 500 600 700 800 900 1000 -s -y -z -df
./benchmark -p 100 200 300 400 500 600 700 800 900 1000 -s -y -r -df
./benchmark -p 100 200 300 400 500 600 700 800 900 1000 -s -z -r -df
# 3 metrics
./benchmark -p 100 200 300 400 500 600 700 800 900 1000 -s -x -y -z -df
./benchmark -p 100 200 300 400 500 600 700 800 900 1000 -s -x -y -r -df
./benchmark -p 100 200 300 400 500 600 700 800 900 1000 -s -x -z -r -df
./benchmark -p 100 200 300 400 500 600 700 800 900 1000 -s -y -z -r -df
# 4 metrics 
./benchmark -p 100 200 300 400 500 600 700 800 900 1000 -s -x -y -z -r -df

# create gnuplot graph with default segmentation and different metric
gnuplot -e "set terminal png;
    set output 'metrics.png';
    plot 'result.txt' using 1:2 with lines title 'default',
    '' using 1:3 with lines title 'metrix X',
    '' using 1:4 with lines title 'metrix Y',
    '' using 1:5 with lines title 'metrix Z',
    '' using 1:6 with lines title 'metrix Remission';
    set output;
    set term x11"

# create gnuplot graph with different 2-metrics
gnuplot -e "set terminal png;
    set output '2_metrics.png';
    plot 'result.txt' using 1:7 with lines title 'metrix X-Y',
    '' using 1:8 with lines title 'metrix X-Z',
    '' using 1:9 with lines title 'metrix X-R',
    '' using 1:10 with lines title 'metrix Y-Z',
    '' using 1:11 with lines title 'metrix Y-R',
    '' using 1:12 with lines title 'metrix Z-R';
    set output;
    set term x11"

# create gnuplot graph with different 3/4-metrics
gnuplot -e "set terminal png;
    set output '3_4_metrics.png';
    plot 'result.txt' using 1:13 with lines title 'metrix X-Y-Z',
    '' using 1:14 with lines title 'metrix X-Y-R',
    '' using 1:15 with lines title 'metrix X-Z-R',
    '' using 1:16 with lines title 'metrix Y-Z-R',
    '' using 1:17 with lines title 'metrix X-Y-Z-R';
    set term x11"



