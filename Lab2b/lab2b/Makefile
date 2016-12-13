default: lab2_list.c SortedList.h SortedList.c Makefile
	gcc -g  -lrt -lpthread SortedList.c SortedList.h lab2_list.c -o lab2_list
build: lab2_list.c SortedList.h SortedList.c Makefile
	gcc -g  -lrt -lpthread SortedList.c SortedList.h lab2_list.c -o lab2_list
clean:
	rm -f lab2_list
tests:
	./lab2_add --threads=1 --iterations=10000 --sync=m >> lab_2b1_list.csv
	./lab2_add --threads=2	--iterations=10000 --sync=m >> lab_2b1_list.csv
	./lab2_add --threads=4	--iterations=10000 --sync=m >> lab_2b1_list.csv
	./lab2_add --threads=8	--iterations=10000 --sync=m >> lab_2b1_list.csv
	./lab2_add --threads=12 --iterations=10000 --sync=m >> lab_2b1_list.csv
	./lab2_add --threads=1	--iterations=10000 --sync=s >> lab_2b1_list.csv
	./lab2_add --threads=2 --iterations=10000 --sync=s >> lab_2b1_list.csv
	./lab2_add --threads=4 --iterations=10000 --sync=s >> lab_2b1_list.csv
	./lab2_add --threads=8 --iterations=10000 --sync=s >> lab_2b1_list.csv
	./lab2_add --threads=12 --iterations=10000 --sync=s >> lab_2b1_list.csv
	./lab2_list --threads=1	--iterations=1000 --sync=m >> lab_2b1_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=m >> lab_2b1_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=m >> lab_2b1_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=m >> lab_2b1_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m >> lab_2b1_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=m >> lab_2b1_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=m >> lab_2b1_list.csv
	./lab2_list --threads=1 --iterations=1000 --sync=s >> lab_2b1_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=s >> lab_2b1_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=s >> lab_2b1_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=s >> lab_2b1_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s >> lab_2b1_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=s >> lab_2b1_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=s >> lab_2b1_list.csv
	-./lab2_list --threads=1 --iterations=1 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=1 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=1 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=1 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=1 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=2 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=2 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=2 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=2 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=2 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=4 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=4 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=4 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=4 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=4 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=8 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=8 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=8 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=8 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=8 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=16 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=16 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=16 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=16 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=16 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=10 --sync=m  --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=10 --sync=m --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=10 --sync=m --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=10 --sync=m --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=10 --sync=m --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=20 --sync=m --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=20 --sync=m --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=20 --sync=m --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=20 --sync=m --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=20 --sync=m --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=40 --sync=m --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=40 --sync=m --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=40 --sync=m --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=40 --sync=m --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=40 --sync=m --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=80 --sync=m --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=80 --sync=m --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=80 --sync=m --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=80 --sync=m --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=80 --sync=m --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=10 --sync=s  --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=10 --sync=s --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=10 --sync=s --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=10 --sync=s --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=10 --sync=s --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=20 --sync=s --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=20 --sync=s --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=20 --sync=s --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=20 --sync=s --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=20 --sync=s --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=40 --sync=s --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=40 --sync=s --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=40 --sync=s --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=40 --sync=s --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=40 --sync=s --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=80 --sync=s --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=80 --sync=s --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=80 --sync=s --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=80 --sync=s --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=80 --sync=s --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=1 --lists=1 --sync=m >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=2 --lists=1 --sync=m >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=4 --lists=1 --sync=m >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=8 --lists=1 --sync=m >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=12 --lists=1 --sync=m >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=1 --lists=4 --sync=m >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=2 --lists=4 --sync=m >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=4 --lists=4 --sync=m >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=8 --lists=4 --sync=m >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=12 --lists=4 --sync=m >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=1 --lists=8 --sync=m >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=2 --lists=8 --sync=m >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=4 --lists=8 --sync=m >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=8 --lists=8 --sync=m >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=12 --lists=8 --sync=m >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=1 --lists=16 --sync=m >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=2 --lists=16 --sync=m >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=4 --lists=16 --sync=m >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=8 --lists=16 --sync=m >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=12 --lists=16 --sync=m >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=1 --lists=1 --sync=s >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=2 --lists=1 --sync=s >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=4 --lists=1 --sync=s >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=8 --lists=1 --sync=s >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=12 --lists=1 --sync=s >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=1 --lists=4 --sync=s >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=2 --lists=4 --sync=s >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=4 --lists=4 --sync=s >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=8 --lists=4 --sync=s >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=12 --lists=4 --sync=s >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=1 --lists=8 --sync=s >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=2 --lists=8 --sync=s >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=4 --lists=8 --sync=s >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=8 --lists=8 --sync=s >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=12 --lists=8 --sync=s >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=1 --lists=16 --sync=s >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=2 --lists=16 --sync=s >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=4 --lists=16 --sync=s >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=8 --lists=16 --sync=s >> lab_2b_list.csv
	-./lab2_list --iterations=1000 --threads=12 --lists=16 --sync=s >> lab_2b_list.csv 


graphs:
	/u/cs/grad/zhou/iloveos/gnuplot lab_2b_list.gp
profile: 
	LD_PRELOAD=./lib/libprofiler.so.0 CPUPROFILE=raw.perf ./lab2_list --threads=12 --iterations=1000 --sync=s
	./bin/pprof --text lab2_list raw.perf >> list.prof
	./bin/pprof --list="SortedList_insert" lab2_list raw.perf >> list.prof
	./bin/pprof --list="SortedList_lookup" lab2_list raw.perf >> list.prof
	LD_PRELOAD=./lib/libprofiler.so.0 CPUPROFILE=raw.perf ./lab2_list --threads=12 --iterations=1000 --sync=m
	./bin/pprof --text lab2_list raw.perf >> list.prof
	./bin/pprof --list="SortedList_insert" lab2_list raw.perf >> list.prof
	./bin/pprof --list="SortedList_lookup" lab2_list raw.perf >> list.prof
tarball:
	tar -czvf lab2b-004632787.tar.gz SortedList.h SortedList.c raw.perf list.prof lab2_list.c Makefile lab_2b_list.csv lab_2b1_list.csv lab_2b_list.gp README.txt lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png
