#! /usr/bin/gnuplot
#
# purpose:
#        generate data reduction graphs for the multi-threaded list project
#
# input: lab_2b_list.csv
#       1. test name
#       2. # threads
#       3. # iterations per thread
#       4. # lists
#       5. # operations performed (threads x iterations x (ins + lookup + delete))
#       6. run time (ns)
#       7. run time per operation (ns)
#
set terminal png
set datafile separator ","

set title "List-1: Throughput vs Number of Threads"
set xlabel "Threads"
set logscale x 2
set ylabel "Throughput (Ops per second)"
set logscale y 10
set output 'lab2b_1.png'
set key left top
plot \
     "< grep 'add-m,[0-9]*,10000,' lab_2b1_list.csv" using ($2):(1000000000)/($6) \
     	title 'mutex, 10000 iterations' with linespoints lc rgb 'green', \
     "< grep 'add-s,[0-9]*,10000,' lab_2b1_list.csv" using ($2):(1000000000)/($6) \
     	title 'spin, 10000 iterations' with linespoints lc rgb 'purple', \
     "< grep 'list-none-m,[0-9]*,1000,' lab_2b1_list.csv" using ($2):(1000000000)/($7) \
     	title 'mutex, 1000 iterations' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,[0-9]*,1000,' lab_2b1_list.csv" using ($2):(1000000000)/($7) \
     	title 'spin, 1000 iterations' with linespoints lc rgb 'blue'

set title "List-2: Wait Time for Lock vs Number of Threads"
set logscale x 2
set logscale y 100
set ylabel "Wait Time"
set output 'lab2b_2.png'
plot \
     "< grep 'list-none-m,[0-9]*,1000,' lab_2b1_list.csv" using ($2):($8) \
     	title 'mutex, 10000 iterations' with linespoints lc rgb 'green'

set title "List-3: Lists Iterations that Run Without Failure"
set logscale x 2
set xrange [0.75:]
set xlabel "Threads"
set logscale y 10
set ylabel "Successful Iterations"
set output 'lab2b_3.png'
plot \
     "< grep 'list-id-none' lab_2b_list.csv" using ($2):($3) \
     	title 'yield=id, lists=4' with points lc rgb 'red', \
	"< grep 'list-id-s' lab_2b_list.csv" using ($2):($3) \
     	title 'yield=id, lists=4, spin' with points lc rgb 'blue', \
	"< grep 'list-id-m' lab_2b_list.csv" using ($2):($3) \
     	title 'yield=id, lists=4, mutex' with points lc rgb 'green'

set title "List-5: Throughput for Spin Locks w/ Multiple Lists"
set xlabel "Threads"
set ylabel "Throughput (Ops per sec)"
set logscale y
set output 'lab2b_5.png'
plot \
     "< grep 'list-none-s,[0-9]*,1000,1,' lab_2b_list.csv" using ($2):(1000000000)/($6) \
     	title 'lists=1' with linespoints lc rgb 'blue', \
     "< grep 'list-none-s,[0-9]*,1000,4' lab_2b_list.csv" using ($2):(1000000000)/($6) \
     	title 'lists=1' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,[0-9]*,1000,8' lab_2b_list.csv" using ($2):(1000000000)/($6) \
     	title 'lists=1' with linespoints lc rgb 'orange', \
     "< grep 'list-none-s,[0-9]*,1000,16' lab_2b_list.csv" using ($2):(1000000000)/($6) \
     	title 'lists=1' with linespoints lc rgb 'green'

set title "List-4: Throughput for Mutex Locks w/ Multiple Lists"
set xlabel "Threads"
set ylabel "Throughput (Ops per sec)"
set output 'lab2b_4.png'
plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab_2b_list.csv" using ($2):(1000000000)/($6) \
     	title 'lists=1' with linespoints lc rgb 'blue', \
     "< grep 'list-none-m,[0-9]*,1000*,4' lab_2b_list.csv" using ($2):(1000000000)/($6) \
     	title 'lists=4' with linespoints lc rgb 'red', \
     "< grep 'list-none-m,[0-9]*,1000*,8' lab_2b_list.csv" using ($2):(1000000000)/($6) \
     	title 'lists=8' with linespoints lc rgb 'orange', \
     "< grep 'list-none-m,[0-9]*,1000*,16' lab_2b_list.csv" using ($2):(1000000000)/($6) \
     	title 'lists=16' with linespoints lc rgb 'green'