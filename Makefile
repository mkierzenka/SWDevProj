build:
	g++ --std=c++11 -g tests/generalTests.cpp -o tests/testGeneral
	g++ --std=c++11 -g tests/colArrTests.cpp -o tests/testCA
	g++ --std=c++11 -g tests/main.cpp -o tests/testMemory

test:
	./tests/testGeneral
	./tests/testCA
	./tests/testMemory

.SILENT: clean
clean:
	rm -f tests/testGeneral tests/testCA tests/testMemory
