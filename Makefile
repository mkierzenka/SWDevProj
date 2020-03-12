build:
	g++ --std=c++11 -g tests/main.cpp -o tests/testDF

test:
	./tests/testDF

.SILENT: clean
clean:
	rm -f tests/testDF
