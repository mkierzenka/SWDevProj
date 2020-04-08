build: buildTrivial buildDemo
	g++ --std=c++11 -g -pthread tests/generalTests.cpp -o tests/testGeneral
	g++ --std=c++11 -g tests/serializationTests.cpp -o tests/testSerial
	g++ --std=c++11 -g tests/mapTest.cpp -o tests/testMap
	g++ --std=c++11 -g tests/arrayTest.cpp -o tests/testArray
	g++ --std=c++11 -g tests/queueTest.cpp -o tests/testQueue
	g++ --std=c++11 -g tests/cacheTest.cpp -o tests/testCache
	g++ --std=c++11 -g tests/messageTest.cpp -o tests/testMessage
	g++ --std=c++11 -g tests/blockTest.cpp -o tests/testBlock
	#g++ --std=c++11 -g tests/memTest.cpp -o tests/testMemory
	#g++ --std=c++11 -g tests/sorerTest.cpp -o tests/testSorer

test: testTrivial testDemo

ourTests:
	./tests/testSerial
	./tests/testGeneral
	./tests/testMap
	./tests/testArray
	./tests/testQueue
	./tests/testCache
	./tests/testMessage
	./tests/testBlock
	#./tests/testMemory
	#./tests/testSorer data/datafile.sor
	
memory:
	#valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./tests/testSorer data/datafile.sor
	#valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./tests/testTrivial
	#valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./tests/testDemo

buildTrivial:
	g++ --std=c++11 -g -pthread tests/trivialTest.cpp -o tests/testTrivial

testTrivial:
	./tests/testTrivial

buildDemo:
	g++ --std=c++11 -g -pthread tests/demoTest.cpp -o tests/testDemo

testDemo:
	./tests/testDemo

.SILENT: clean
clean:
	rm -f tests/testGeneral tests/testCA tests/testMemory tests/testSorer tests/testMap tests/testArray \
	tests/testQueue tests/testSerial tests/testTrivial tests/testCache tests/testDemo tests/testMessage \
	tests/testBlock
