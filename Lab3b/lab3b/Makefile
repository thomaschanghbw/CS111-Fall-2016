default: lab3b.cpp
	g++ -static-libstdc++ lab3b.cpp -o lab3b
run: 
	./lab3b >> lab3b_check.txt
clean:
	rm -f lab3b lab3b_check.txt
dist:
	tar -czvf lab3b-004632787.tar.gz lab3b.cpp Makefile README
