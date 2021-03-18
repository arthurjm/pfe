#!/bin/bash

#remove the previous result file 
#rm result.txt

# default segmentation
#./benchmark -p 50 100 150 200 250 300 350 400 450 500 550 600 650 700 750 800 850 900 950 1000 -s -df
#./benchmark -p 50 100 150 200 250 300 350 400 450 500 550 600 650 700 750 800 850 900 950 1000 -s -df -x -y -z
#./benchmark -p 50 100 150 200 250 300 350 400 450 500 550 600 650 700 750 800 850 900 950 1000 -h -df
#./benchmark -p 50 100 150 200 250 300 350 400 450 500 550 600 650 700 750 800 850 900 950 1000 -h -df -x -y -z
# create gnuplot graph with default segmentation and different metric
gnuplot -e "set terminal pdf;
    set output 'compare.pdf';
    set key right bottom;
    set xlabel 'number of superpixels';
    set ylabel 'accuracy %';
    plot 'result.txt' using 1:2 with lines title 'segmentation default',
    '' using 1:3 with lines title 'segmentation metrics',
    '' using 1:4 with lines title 'hierarchy default',
    '' using 1:5 with lines title 'hierarchy metrics',
    '' using 1:2 with points pt 1 ps 0.5 lc 8 notitle,
    '' using 1:3 with points pt 1 ps 0.5 lc 8 notitle,
    '' using 1:4 with points pt 1 ps 0.5 lc 8 notitle,
    '' using 1:5 with points pt 1 ps 0.5 lc 8 notitle;
    set output;
    set term x11"




