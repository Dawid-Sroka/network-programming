OBJ = main.o helpers.o

transport: $(OBJ)
	gcc -std=gnu17 -o transport $(OBJ)

main.o: main.c header.h
	gcc -std=gnu17 -c -Og -Wall -Wextra -Werror main.c

helpers.o: helpers.c header.h
	gcc -std=gnu17 -c -Og -Wall -Wextra -Werror helpers.c

clean:
	rm -f $(OBJ)

distclean:
	rm -f transport $(OBJ)
