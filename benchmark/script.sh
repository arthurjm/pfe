#!/bin/bash

#remove the previous result file 
#rm result.txt

# default segmentation
#./benchmark -p 50 100 150 200 250 300 350 400 450 500 550 600 650 700 750 800 850 900 950 1000 -s -df
# metrics
#./benchmark -p 50 100 150 200 250 300 350 400 450 500 550 600 650 700 750 800 850 900 950 1000 -s -x -df
#./benchmark -p 50 100 150 200 250 300 350 400 450 500 550 600 650 700 750 800 850 900 950 1000 -s -y -df
#./benchmark -p 50 100 150 200 250 300 350 400 450 500 550 600 650 700 750 800 850 900 950 1000 -s -z -df
#./benchmark -p 50 100 150 200 250 300 350 400 450 500 550 600 650 700 750 800 850 900 950 1000 -s -r -df
# 2 metrics
#./benchmark -p 50 100 150 200 250 300 350 400 450 500 550 600 650 700 750 800 850 900 950 1000 -s -x -y -df
#./benchmark -p 50 100 150 200 250 300 350 400 450 500 550 600 650 700 750 800 850 900 950 1000 -s -x -z -df
#./benchmark -p 50 100 150 200 250 300 350 400 450 500 550 600 650 700 750 800 850 900 950 1000 -s -x -r -df
#./benchmark -p 50 100 150 200 250 300 350 400 450 500 550 600 650 700 750 800 850 900 950 1000 -s -y -z -df
#./benchmark -p 50 100 150 200 250 300 350 400 450 500 550 600 650 700 750 800 850 900 950 1000 -s -y -r -df
#./benchmark -p 50 100 150 200 250 300 350 400 450 500 550 600 650 700 750 800 850 900 950 1000 -s -z -r -df
# 3 metrics
#./benchmark -p 50 100 150 200 250 300 350 400 450 500 550 600 650 700 750 800 850 900 950 1000 -s -x -y -z -df
#./benchmark -p 50 100 150 200 250 300 350 400 450 500 550 600 650 700 750 800 850 900 950 1000 -s -x -y -r -df
#./benchmark -p 50 100 150 200 250 300 350 400 450 500 550 600 650 700 750 800 850 900 950 1000 -s -x -z -r -df
#./benchmark -p 50 100 150 200 250 300 350 400 450 500 550 600 650 700 750 800 850 900 950 1000 -s -y -z -r -df
# 4 metrics 
#./benchmark -p 50 100 150 200 250 300 350 400 450 500 550 600 650 700 750 800 850 900 950 1000 -s -x -y -z -r -df

# create gnuplot graph with default segmentation and different metric
gnuplot -e "set terminal pdf;
    set output 'metrics.pdf';
    set key right bottom;
    set xlabel 'number of superpixels';
    set ylabel 'accuracy (%)';
    plot 'result.txt' using 1:2 with lines title 'default',
    '' using 1:3 with lines title 'metric X',
    '' using 1:4 with lines title 'metric Y',
    '' using 1:5 with lines title 'metric Z',
    '' using 1:6 with lines title 'metric Remission',
    '' using 1:2 with points pt 1 ps 0.5 lc 8 notitle,
    '' using 1:3 with points pt 1 ps 0.5 lc 8 notitle,
    '' using 1:4 with points pt 1 ps 0.5 lc 8 notitle,
    '' using 1:5 with points pt 1 ps 0.5 lc 8 notitle,
    '' using 1:6 with points pt 1 ps 0.5 lc 8 notitle;
    set output;
    set term x11"

# create gnuplot graph with different 2-metrics
gnuplot -e "set terminal pdf;
    set output '2_metrics.pdf';
    set key right bottom;
    set xlabel 'number of superpixels';
    set ylabel 'accuracy (%)';
    plot 'result.txt' using 1:7 with lines title 'metric X-Y',
    '' using 1:8 with lines title 'metric X-Z',
    '' using 1:9 with lines title 'metric X-R',
    '' using 1:10 with lines title 'metric Y-Z',
    '' using 1:11 with lines title 'metric Y-R',
    '' using 1:12 with lines title 'metric Z-R',
    '' using 1:7 with points pt 1 ps 0.5 lc 8 notitle,
    '' using 1:8 with points pt 1 ps 0.5 lc 8 notitle,
    '' using 1:9 with points pt 1 ps 0.5 lc 8 notitle,
    '' using 1:10 with points pt 1 ps 0.5 lc 8 notitle,
    '' using 1:11 with points pt 1 ps 0.5 lc 8 notitle,
    '' using 1:12 with points pt 1 ps 0.5 lc 8 notitle;
    set output;
    set term x11"

# create gnuplot graph with different 3/4-metrics
gnuplot -e "set terminal pdf;
    set output '3_4_metrics.pdf';
    set key right bottom;
    set xlabel 'number of superpixels';
    set ylabel 'accuracy (%)';
    plot 'result.txt' using 1:13 with lines title 'metric X-Y-Z',
    '' using 1:14 with lines title 'metric X-Y-R',
    '' using 1:15 with lines title 'metric X-Z-R',
    '' using 1:16 with lines title 'metric Y-Z-R',
    '' using 1:17 with lines title 'metric X-Y-Z-R',
    '' using 1:13 with points pt 1 ps 0.5 lc 8 notitle,
    '' using 1:14 with points pt 1 ps 0.5 lc 8 notitle,
    '' using 1:15 with points pt 1 ps 0.5 lc 8 notitle,
    '' using 1:16 with points pt 1 ps 0.5 lc 8 notitle,
    '' using 1:17 with points pt 1 ps 0.5 lc 8 notitle;;
    set term x11"



