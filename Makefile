build: buildeau2
	g++ --std=c++11 -g -pthread tests/generalTests.cpp -o tests/testGeneral
	g++ --std=c++11 -g tests/serializationTests.cpp -o tests/testSerial
	g++ --std=c++11 -g tests/mapTest.cpp -o tests/testMap
	g++ --std=c++11 -g tests/arrayTest.cpp -o tests/testArray
	g++ --std=c++11 -g tests/queueTest.cpp -o tests/testQueue
	g++ --std=c++11 -g tests/cacheTest.cpp -o tests/testCache
	g++ --std=c++11 -g tests/nodeInfoTest.cpp -o tests/testNodeInfo
	g++ --std=c++11 -g tests/messageTest.cpp -o tests/testMessage
	g++ --std=c++11 -g tests/blockTest.cpp -o tests/testBlock
	g++ --std=c++11 -g tests/storeTest.cpp -o tests/testStore
	g++ --std=c++11 -g tests/storeIntegrationTests.cpp -o tests/testIntegrationStore
	g++ --std=c++11 -g tests/schemaTest.cpp -o tests/testSchema
	g++ --std=c++11 -g tests/keyTest.cpp -o tests/testKey
	g++ --std=c++11 -g tests/valueTest.cpp -o tests/testValue
	g++ --std=c++11 -g tests/columnTest.cpp -o tests/testColumn
	g++ --std=c++11 -g tests/sorerTest.cpp -o tests/testSorer
	g++ --std=c++11 -g tests/distributedArrayTest.cpp -o tests/testDistributedArray

test: testTrivial testDemo testWordcount

ourTests:
	./tests/testGeneral
	./tests/testSerial
	./tests/testMap
	./tests/testArray
	./tests/testQueue
	./tests/testCache
	./tests/testMessage
	./tests/testNodeInfo
	./tests/testBlock
	./tests/testStore
	./tests/testIntegrationStore
	./tests/testSchema
	./tests/testKey
	./tests/testValue
	./tests/testColumn
	./tests/testDistributedArray
	./tests/testSorer data/datafile.sor

memory:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./eau2 -app demo -num_nodes 1 -pseudo

#use this target if you're trying to debug a specific test in the suite
buildDebug:
	g++ --std=c++11 -g tests/columnTest.cpp -o tests/testColumn

testDebug:
	./tests/testColumn

buildeau2:
	g++ --std=c++11 -g -pthread entry.cpp -o eau2

#./eau2 -app (appname) -num_nodes N -i (mynodenum) -ip (myipaddr) -port N -serverIp (addr) -serverPort N -pseudo
runeau2:
	./eau2 -app demo -num_nodes 3 -pseudo

testTrivial:
	./eau2 -app trivial -num_nodes 1 -i 0 -ip 127.0.0.1 -port 8080 -serverIp 127.0.0.1 -serverPort 8080
	#./eau2 -app trivial -num_nodes 1 -pseudo

testDemo:
	./eau2 -app demo -num_nodes 3 -i 0 -ip 127.0.0.1 -port 8080 -serverIp 127.0.0.1 -serverPort 8080 &
	./eau2 -app demo -num_nodes 3 -i 1 -ip 127.0.0.2 -port 8080 -serverIp 127.0.0.1 -serverPort 8080 &
	./eau2 -app demo -num_nodes 3 -i 2 -ip 127.0.0.3 -port 8080 -serverIp 127.0.0.1 -serverPort 8080
	#./eau2 -app demo -num_nodes 3 -pseudo

testWordcount:
	./eau2 -app wordcount -num_nodes 1 -i 0 -ip 127.0.0.1 -port 8080 -serverIp 127.0.0.1 -serverPort 8080
	#./eau2 -app wordcount -num_nodes 1 -pseudo

testLinus:
	./eau2 -app linus -num_nodes 1 -i 0 -ip 127.0.0.1 -port 8080 -serverIp 127.0.0.1 -serverPort 8080
	#./eau2 -app demo -num_nodes 1 -pseudo

.SILENT: clean
clean:
	rm -f tests/testGeneral tests/testCA tests/testMemory tests/testSorer tests/testMap tests/testArray \
	tests/testQueue tests/testSerial tests/testCache tests/testMessage tests/testBlock tests/testColumnArray \
	tests/testStore tests/testIntegrationStore tests/testSchema tests/testKey tests/testValue \
	tests/testColumn tests/testDistributedArray tests/testNodeInfo eau2
