OBJ = main.o helpers.o circle_q.o

transport: $(OBJ)
	gcc -std=gnu17 -o transport $(OBJ)

main.o: main.c header.h
	gcc -std=gnu17 -c -Og -Wall -Wextra -Werror main.c

helpers.o: helpers.c header.h
	gcc -std=gnu17 -c -Og -Wall -Wextra -Werror helpers.c

circle_q.o: circle_q.c header.h
	gcc -std=gnu17 -c -Og -Wall -Wextra -Werror circle_q.c

clean:
	rm -f $(OBJ)

distclean:
	rm -f transport $(OBJ)
