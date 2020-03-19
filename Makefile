build:
	g++ --std=c++11 -g -pthread tests/generalTests.cpp -o tests/testGeneral
	g++ --std=c++11 -g tests/colArrTests.cpp -o tests/testCA
	g++ --std=c++11 -g tests/memTest.cpp -o tests/testMemory
	g++ --std=c++11 -g tests/sorerTest.cpp -o tests/testSorer
	g++ --std=c++11 -g tests/mapTest.cpp -o tests/testMap

test:
	./tests/testSorer data/datafile.sor

testGeneral: test
	./tests/testGeneral
	./tests/testCA
	./tests/testMemory
	./tests/testMap
	
memory:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./tests/testSorer data/datafile.sor

.SILENT: clean
clean:
	rm -f tests/testGeneral tests/testCA tests/testMemory tests/testSorer tests/testMap
