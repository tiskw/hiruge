# Makefile

.PHONY: check count clean

# Define the software name.
SOFTWARE := hiruge

# Define the compiler to be used.
ifeq ($(compiler),clang)
	CC := clang++ -stdlib=libc++ -std=c++20 -Wall -Wextra -O3 -mtune=native -march=native
else
	CC := g++ -std=c++20 -Wall -Wextra -O3 -mtune=native -march=native
endif

# Define the compiler options.
CFLG := -Isrc -Iexternal -I/usr/include/freetype2
LIBS := -lX11 -lXft

$(SOFTWARE): external/toml.hpp objs objs/complete.o objs/config.o objs/main.o objs/window.o
	$(CC) -o $(@) $(CFLG) objs/*.o $(LIBS)
	strip $(@)

external/toml.hpp:
	mkdir -p external
	wget -q -O external/toml.hpp https://raw.githubusercontent.com/marzer/tomlplusplus/master/toml.hpp --no-check-certificate

objs:
	mkdir -p objs

objs/complete.o: src/complete.cxx src/complete.hxx
	$(CC) -c -o $(@) $(CFLG) $(<)

objs/config.o: src/config.cxx src/config.hxx
	$(CC) -c -o $(@) $(CFLG) $(<)

objs/main.o: src/main.cxx
	$(CC) -c -o $(@) $(CFLG) $(<)

objs/window.o: src/window.cxx src/window.hxx
	$(CC) -c -o $(@) $(CFLG) $(<)

check:
	cppcheck --enable=all --suppress=missingIncludeSystem $(C_SOURCE)

count:
	cloc --by-file $(C_SOURCE) $(H_SOURCE) Makefile

clean:
	rm -f $(SOFTWARE)
	rm -rf objs

# vim: noexpandtab tabstop=4 shiftwidth=4 fdm=marker
