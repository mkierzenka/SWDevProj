build:
	g++ --std=c++11 -g tests/generalTests.cpp -o tests/testGeneral
	g++ --std=c++11 -g tests/colArrTests.cpp -o tests/testCA

test:
	.tests/testGeneral
	./tests/testCA

.SILENT: clean
clean:
	rm -f tests/testGeneral tests/testCA
