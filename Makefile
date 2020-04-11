build: buildTrivial buildDemo
	g++ --std=c++11 -g -pthread tests/generalTests.cpp -o tests/testGeneral
	g++ --std=c++11 -g tests/serializationTests.cpp -o tests/testSerial
	g++ --std=c++11 -g tests/mapTest.cpp -o tests/testMap
	g++ --std=c++11 -g tests/arrayTest.cpp -o tests/testArray
	g++ --std=c++11 -g tests/queueTest.cpp -o tests/testQueue
	g++ --std=c++11 -g tests/cacheTest.cpp -o tests/testCache
	g++ --std=c++11 -g tests/messageTest.cpp -o tests/testMessage
	g++ --std=c++11 -g tests/blockTest.cpp -o tests/testBlock
	g++ --std=c++11 -g tests/storeTest.cpp -o tests/testStore
	g++ --std=c++11 -g tests/storeIntegrationTests.cpp -o tests/testIntegrationStore
	g++ --std=c++11 -g tests/schemaTest.cpp -o tests/testSchema
	g++ --std=c++11 -g tests/keyTest.cpp -o tests/testKey
	g++ --std=c++11 -g tests/valueTest.cpp -o tests/testValue
	g++ --std=c++11 -g tests/columnTest.cpp -o tests/testColumn
	#g++ --std=c++11 -g tests/memTest.cpp -o tests/testMemory
	#g++ --std=c++11 -g tests/sorerTest.cpp -o tests/testSorer

test: testTrivial testDemo

ourTests:
	./tests/testSerial
	./tests/testMap
	./tests/testArray
	./tests/testQueue
	./tests/testCache
	./tests/testMessage
	./tests/testBlock
	./tests/testStore
	./tests/testIntegrationStore
	./tests/testSchema
	./tests/testKey
	./tests/testValue
	./tests/testColumn
	./tests/testGeneral
	#./tests/testMemory
	#./tests/testSorer data/datafile.sor
	
memory:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./tests/testDemo


buildTrivial:
	g++ --std=c++11 -g -pthread tests/trivialTest.cpp -o tests/testTrivial

testTrivial:
	./tests/testTrivial

buildDemo:
	g++ --std=c++11 -g -pthread tests/demoTest.cpp -o tests/testDemo

testDemo:
	./tests/testDemo

buildWordcount:
	g++ --std=c++11 -g -pthread tests/wordcountTest.cpp -o tests/testWordcount

testWordcount:
	./tests/testWordcount

buildGeneral:
	g++ --std=c++11 -g tests/generalTests.cpp -o tests/testGeneral

testGeneral:
	./tests/testGeneral

buildColumnArray:
	g++ --std=c++11 -g tests/columnArrayTest.cpp -o tests/testColumnArray

testColumnArray:
	./tests/testColumnArray

#./eau2 -app (appname) -num_nodes N -i (mynodenum) -ip (myipaddr) -port N -serverIp (addr) -serverPort N -pseudo
buildeau2:
	g++ --std=c++11 -g -pthread entry.cpp -o eau2

runeau2:
	./eau2 -app trivial -num_nodes 1 -i 0 -ip 127.0.0.1 -port 8080 -serverIp 127.0.0.1 -serverPort 8080


.SILENT: clean
clean:
	rm -f tests/testGeneral tests/testCA tests/testMemory tests/testSorer tests/testMap tests/testArray \
	tests/testQueue tests/testSerial tests/testTrivial tests/testCache tests/testDemo tests/testMessage \
	tests/testBlock tests/testColumnArray tests/testStore tests/testIntegrationStore tests/testSchema \
	tests/testKey tests/testValue tests/testColumn eau2
