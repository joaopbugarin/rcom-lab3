make:
	rm -f src/trans.o
	gcc -o src/trans src/trans.c
	rm -f src/rec.o
	gcc -o src/rec src/rec.c