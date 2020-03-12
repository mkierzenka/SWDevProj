build:
	g++ --std=c++11 -g tests/main.cpp -o tests/testDF
	g++ --std=c++11 -g tests/colArrTests.cpp -o tests/testCA

test:
	./tests/testDF
	./tests/testCA

.SILENT: clean
clean:
	rm -f tests/testDF tests/testCA
