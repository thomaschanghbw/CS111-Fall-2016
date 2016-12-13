default: lab4_2.c lab4_1.c
	gcc -lmraa -lm -o lab4_1 lab4_1.c
	gcc -lmraa -lm -lpthread -o lab4_2 lab4_2.c
build: lab4_1.c lab4_2.c
	gcc -lmraa -lm -o lab4_1 lab4_1.c
	gcc -lmraa -lm -lpthread -o lab4_2 lab4_2.c
dist:
	tar -czvf lab4-004632787.tar.gz lab4_1.c lab4_2.c lab4_1_temperature.log lab4_2_temperature.log Makefile README
