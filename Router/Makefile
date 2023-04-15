OBJ = main.o

router: $(OBJ)
	gcc -std=gnu17 -o router $(OBJ)

main.o: main.c
	gcc -std=gnu17 -c -Og -Wall -Wextra -Werror main.c

# helpers.o: helpers.c header.h
# 	gcc -std=gnu17 -c -Og -Wall -Wextra -Werror helpers.c

# decoding.o: decoding.c header.h
# 	gcc -std=gnu17 -c -Og -Wall -Wextra -Werror decoding.c

clean:
	rm -f $(OBJ)

distclean:
	rm -f router $(OBJ)
