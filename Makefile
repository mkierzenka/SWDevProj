build:
	g++ --std=c++11 tests/main.cpp -o test

test:
	./test

.SILENT: clean
clean:
	rm -f tests/test
