build:
	g++ --std=c++11 -g -pthread tests/generalTests.cpp -o tests/testGeneral
	g++ --std=c++11 -g tests/serializationTests.cpp -o tests/testSerial
	g++ --std=c++11 -g tests/trivialTest.cpp -o tests/testTrivial
	#g++ --std=c++11 -g tests/mapTest.cpp -o tests/testMap
	g++ --std=c++11 -g tests/arrayTest.cpp -o tests/testArray
	g++ --std=c++11 -g tests/queueTest.cpp -o tests/testQueue
	#g++ --std=c++11 -g tests/memTest.cpp -o tests/testMemory
	#g++ --std=c++11 -g tests/sorerTest.cpp -o tests/testSorer

test:
	./tests/testTrivial

ourTests:
	./tests/testSerial
	./tests/testGeneral
	#./tests/testMap
	./tests/testArray
	./tests/testQueue
	#./tests/testMemory
	#./tests/testSorer data/datafile.sor
	
memory:
	#valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./tests/testSorer data/datafile.sor
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./tests/testTrivial

.SILENT: clean
clean:
	rm -f tests/testGeneral tests/testCA tests/testMemory tests/testSorer tests/testMap tests/testArray \
	tests/testQueue tests/testSerial tests/testTrivial tests/testCache

buildCache:
	g++ -std=c++11 -g tests/cacheTest.cpp -o tests/testCache

testCache:
	./tests/testCache

buildTrivial:
	g++ -std=c++11 -g tests/trivialTest.cpp -o tests/testTrivial

testTrivial:
	./tests/testTrivial

buildDemo:
	g++ -std=c++11 -pthread -g tests/demoTest.cpp -o tests/testDemo

testDemo:
	./tests/testDemo