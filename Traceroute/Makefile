OBJ = main.o helpers.o decoding.o

traceroute: $(OBJ)
	gcc -std=gnu17 -o traceroute $(OBJ)

main.o: main.c header.h
	gcc -std=gnu17 -c -Og -Wall -Wextra -Werror main.c

helpers.o: helpers.c header.h
	gcc -std=gnu17 -c -Og -Wall -Wextra -Werror helpers.c

decoding.o: decoding.c header.h
	gcc -std=gnu17 -c -Og -Wall -Wextra -Werror decoding.c

clean:
	rm -f $(OBJ)

distclean:
	rm -f traceroute $(OBJ)
