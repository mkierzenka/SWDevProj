## Usage:
* "make build" - compiles all tests
* "make test" - runs sorer test
* "make testGeneral" - runs sorer test and a bunch of others testing different things
* "make memory" - runs sorer test through valgrind
* "make clean" - deletes compiled files


## Introduction

The eau2 system is a framework to allow a wide variety of applications to
operate on large datasets distributed across nodes in a network. Each node
has a collection of key-value pairs stored locally as well as the ability to
retrieve values from other nodes over the network, as needed. The system can
be scaled for different use-cases (supports an arbitrary number of nodes).
Most cases will have a different responsibility for each node, for example
one to produce data and another to consume it.

## Architecture

The eau2 system is made up of three layers of abstraction.

The first and lowest layer is the distributed key-value store. There will be 
multiple key-value stores, each holding some portion of data. Each key will map to
a piece of serialized data or a dataframe. Each store will be able to serialize and
deserialize its own data. If a key-value store requires data that it does not have, then 
it will be able to communicate with the other key-value stores to retrieve this 
information. This will be done via a networking abstraction, which will allow key-value 
stores to behave as nodes on a network. In addition to the stores, there will also be a lead
node, which will keep track of registered stores and notify all connected stores when a new 
store is added to the network.

The next layer will include abstractions for holding the data from the key-value stores. 
These abstractions include distributed arrays and dataframes. A dataframe allows the user
to access a set of data across multiple nodes. It supports data storage and retrieval 
without exposing the details of how and where the data is stored. A dataframe holds a 
collection of columns. Each column is a distributed array. A distributed array contains a 
bunch of keys as well as a cache, which will support short-term storage for some of the 
keys' data. The distributed array will also have a key-value store; therefore even if data
does not live in the cache, the array will be able to look up the corresponding data for 
all of its keys. 

The last and highest level is the application layer. In the application, the user 
will be able to specify what they want to do. Each node will run its own application, 
with responsibilities distributed across each node. The application can get data 
from and put data into the key-value store. Distributed arrays can be used to track, 
organize, and work with specific data across the eau2 system.

## Implementation

For the critical classes that we will implement for this system, we included a run-down
of their fields and methods. For components that are re-used from previous assignments,
we provided descriptions of how they'll be used.

* KVStore: this class will represent a local key-value store

Fields:
 * map (Str-to-Obj Map): the String key will map to a serialized piece of data
 * networkHelper (Client): this network abstraction will allow the store to listen for 
 messages from other nodes and send messages to other stores to request data
 * storeId (size_t): each local store will be represented by a unique numerical identifier. 
 At a higher level, this identifier will help keep track of where the data is stored
 
 Methods:
 * put(Key, String): adds given data to the key-value store specified in the key, 
 not blocking
 * get(Key): request for the data; returns deserialized data from its own store if 
 stored locall
 * getAndWait(Key): retrieves data with the given Key from the Key's node
 * getStoreId(): return the id of this store (storeId)
 * getValue(Key): return the value that the given key maps to; this method will not do
 any serialization or deserialization, rather will return the result exactly as it is stored

* Key: these are used to define a piece of data at a level higher than the local KV store. 
Since data can exist in any of the stores, we need multiple attributes to keep track of data
Fields:
 - key (String): maps to a piece of data in a key-value store
 - node_num (size_t): the identifier for the key-value store in which the data is stored

 *NOTE: Keys are immutable. There will be methods to retrieve these values but not modify them*

* Value: represents the serialized data that a Key maps to. It will include a character pointer 
that holds the serialized data and a size_t that describes the maximum amount of bytes that can
be stored in this pointer.

* Server: This is the "lead node." All nodes will connect and register with this host. 
Upon registration, the server will broadcast a list of every node that it is connected to. 
The server will have basic network functionality and a socket in which it listens for node 
connections. It will also hold connection information for each known node.

* Client: This class is the abstraction that key-value stores will use to communicate with 
each other. The client will establish a connection and subscribe to the lead node, which 
tells the client about all other nodes on the system. The client also establishes 
connections to all other nodes to support two-way communication. Like the server, the 
client will also include essential networking capabilities, and hold sockets for sending 
and listening.

* DataFrame: The DataFrame API will be similar to that on previous assignments. It will 
include operations to store and perform operations on data, such as map. A Schema will 
be used to describe the Dataframe's column structure. A DataFrame will be immutable, so 
it will not support operations such as deleting, setting, and modifying columns and rows. 

DataFrame's data storage will change. In previous assignments, DataFrames held all of their
data in columns and rows. However we now want to make the data storage distributed. Instead 
of storing the actual data, Columns will now be distributed. Like before, the DataFrame will 
have an array of columns. However the Column class will change, as it will now contain a 
Distributed Array of keys. Each key will map to a "block" of data that is held in the store. 
With this change, now that Columns no longer store their own data, we will be able to eliminate 
the duplicate Column classes for each type. Instead, we can just have a field in Column that 
describes the type.

The DataFrame will hold a KVStore object so that it can look up data as requested by the 
user.

The DataFrame will have methods that support converting data types into frames. An example 
is fromArray; it will take in an array of a certain type, and it will return the DataFrame 
version (add the values in one new column). It will break up the array into chunks; each chunk 
will be assigned a key to be stored in the distributed KV system. The key will then be added to 
the column's distributed array. Once the input array is completely broken up and stored, the 
DataFrame will be serialized and stored under its key in the distributed system.

Fields:
* DistributedArray (to hold columns)
* KVStore: to look up data using the keys in the DistributedArray
* Schema: keeps track of the DataFrame's column structure
* Key: key for this dataframe in the store; will be used to create the keys for the
frame's columns' chunks


Methods (new ones we anticipate):
* all the methods that create a DataFrame from a data type (ex. fromArray, fromScalar, etc.)
* local_map: perform an operation on all data that is only held on a certain node
* since a dataframe is immutable, we can remove any that modify the dataframe


* Column: A column now stores data in the distributed KV Store, instead of locally. A Column will be a DistributedArray where each Key points to a Value containing a fixed sized number of elements that belong in this Column. Since we are no longer storing elements
directly in the column, we only need one column class now.

Fields:
* DistributedArray (to hold blocks of values)
* size: number of elements in the column (!= number of keys in array)
* KVStore: will be used by column to retrieve its own data when needed and add keys to the store
* ColType(enum): what type of column is this? String, boolean, int, or double
* Key: key of the dataframe that the column belongs to; will be used to build up block keys

Methods:
* most methods from before, for getting data from the column. not 'set' methods (or anything else that modifies the column). Might include the push_back methods
* add_all: one for each data type; adds all given elements of that datatype to the array; breaks data up into chunks, creates keys for those chunks, and adds them to the KVStore


* Serializer: This class will be similar to the one started in Assignment 6. It will be 
in charge of serializing and deserializing data, and buffering the result. The Serializer 
will be used in multiple places in this project. All of DataFrame's from... methods will 
need to initialize a Serializer to serialize the constructed dataframe, to store in the 
key-value store. The wait and waitAndGet methods within Store will need to initalize a 
Serializer; the serializer will need to convert the serialized string data into the dataframe 
that it represents

* DistributedArray: this class will hold reference information about data throughout the 
system. The purpose of this class is to bridge data that lives in different areas of the 
system. This class also holds a KVStore reference, so that it can look up data not
stored locally.

Fields:

* store (KVStore): to get data from the distributed system
* keys (Array): this array will hold all Key values of interest
* cache (Obj-to-Obj Map): this map will hold the data for some of the Key objects within 
this distributed array. The key for this map will be the same Key object stored within the 
keys array; the value will be the serialized data. The goal of the cache is to improve the 
performance of data retrieval; before sending a data request to a key-value store, we will 
first check to see if the data exists in the cache. The cache will be limited in size, so 
not all the distributed arrays' keys will hold its data there at once. The cache will be 
refreshed occasionally; at the moment, we are unsure of a strategy to update the cache.

Methods:

* containsKey(Key): does the distributed array's key list contain the given key?
* addKey(Key): add the given key to the distributed array's list of keys
* get(Key): get the value for the specified key from the distributed array. Check the cache first and if the cache
does not contain the data, then make a call too the KV
store
* get(size_t): get key at specified index in array, then return data for that key
* getKeyAtIndex(size_t): return key at the given index in the array

*NOTE that the cache methods defined here will delegate to the Cache object, which work 
on the Cache's map object*


* Cache: class that represents a cache within a distributed array. Holds the data of
some of the array's keys

Fields:

* maxSize: defines a maximum size of the cache; the cache's map cannot hold more than 
this amount of elements at any given time
* data (Obj-to-Obj map): this map will hold the data within the cache. The keys will 
be Key objects, and they will map to the serialized data
* keyOrder (Queue): this queue will keep the order in which keys are added to the cache. This will help us update our cache by adding and removing elements on a FIFO basis

Methods:

* containsKey(Key): does the map contain data for the given key?
* getValue(Key): get the mapped data for the given key
* put(Key, String): add the serialized data, with the given key, to the map. If map 
full, replace an element
* clear() - remove all entries from the map


*Application: this is the highest level of the program. The Application is what the 
user interacts with, in which he or she defines operations to perform over a certain 
set of data

Fields:

* kv (Store): this will be the local kv store for the node that the application is 
running on. When initialized, it will take in the node id from the Application's 
 and set it as its id

Methods:

* this_node(): returns id of this node; retrieved from the KV store
* run_(): given the node id, perform a certain set of operations. The node id will be 
placed in a switch statement, and each case will contain a different helper for each 
application running (ex.producer to initialize and create the data, summarizer to 
perform some operation on the data). Distributed arrays and dataframes and can be 
initialized and used for these helpers, depending on the purpose of the application

## Use cases

The following is an example of a use case of the eau2 system. The lines of pseudocode 
are an outline of what each node will run.

Application:

 dataA = Key("dataA", 0);
 dataB = Key("dataB", 1);
 firstDifPos = Key("firstDifPos", 2);
 dataAFirstDif = Key("dataAFirstDif", 2);
 dataBFirstDif = Key("dataBFirstDif", 2);
 same = Key("areSame", 2);
 sameSize = Key("sameSize", 2);

 * ProducerA (node_num = 0):
	Sorer s = new Sorer();
	DataFrame\* df = s.read("fileA");
	// store df in distributed key-value store under key dataA
	return;

 * ProducerB (node_num = 1):
	Sorer s = new Sorer();
	DataFrame\* df = s.read("fileB");
	// store df in distributed key-value store under key dataB
	return;

 * Comparer (node_num = 2):
	DataFrame\* dfA = waitAndGet(dataA);
	DataFrame\* dfB = waitAndGet(dataB);
	int areSame = 0;
	if (size of dfA != size of dfB) {
		DataFrame::fromScalar(&sameSize, &kv, 0);
		DataFrame::fromScalar(&same, &kv, 0);
		return;
	}
	DataFrame::fromScalar(&sameSize, &kv, 1);
	// loop through elements
		if (dfA->get(i, j) not equal dfB->get(i, j)) {
			int\* pos = new int\[2\];
			pos\[0\] = i;
			pos\[1\] = j;
			DataFrame::fromArray(&firstDifPos, &kv, 2, pos);
			// generalize for any type
			DataFrame::fromScalar(&dataAFirstDif, &kv, dfA->get(i, j)); 
			// generalize for any type
			DataFrame::fromScalar(&dataBFirstDif, &kv, dfB->get(i, j)); 
		}
	DataFrame::fromScalar(&same, &kv, 1);
	return;

 * Reporter (node_num = 3):
	DataFrame\* sameDF = waitAndGet(same);
	if (sameDF->get(0, 0)) {
		pln("Same Data! :)");
		return;
	}
	pln("Different Data.");
	DataFrame\* sizeDF = waitAndGet(sameSize);
	if (sizeDF->get(0, 0)) {
		pln("Non-Equal Size");
		return;
	}
	DataFrame\* posDF = waitAndGet(firstDifPos);
	DataFrame\* aDif = waitAndGet(dataAFirstDif);
	DataFrame\* bDif = waitAndGet(dataBFirstDif);
	pln("Different Data!");
	p("Example: ");
	p("dataA\[").p(posDF->get(0, 0)).p(", ").p(posDF->get(0, 1)).p("\] = ")
	.p(aDif->get(0));
	p(" , dataB\[").p(posDF->get(0, 0)).p(", ").p(posDF->get(0, 1)).p("\] = ")
	.p(bDif->get(0));
	pln("");
	return;


## Open questions

In HW6, we implemented a Server and Clients. It would make sense for each Key-Value 
Store to have the code from Client, but what about the code for Server?
Having a single Server node makes it easier to register new Clients (add new nodes 
to the system). We are not sure on the details of implementing this; should the Server 
be another node of the network with it's own Key-Value store? Seems like having a dedicated 
Server which only handles registering new Clients would be cleaner, but that doesn't seem 
to fit with the provided example code. Any advice?


Should each Client (KV store) always be connected to every other Client? Are we expecting 
100K Client systems?


How exactly will creating dataframes from data types work. For example, if we convert an 
array into a dataframe, will these values all be stored in a single column? Would we want
multiple columns? How exactly would we decide this...


Cyclic dependency: if the DataFrame has a KVStore (which it seems like it needs), wouldn't
it be possible for the DataFrame to technically access the deserialized form of itself?
Could this cause issues?


Should we serialize and deserialize KV stores?

## Status
We have decided to use another group's Sorer implementation, as ours was written in Python and 
not fully correct.
The group was chosen based on the results of our testing in Assignment 5, Part 1 -> 
https://github.com/yth/CS4500A1P1
As part of incorporating their Sorer into our codebase, we discovered a bug that we fixed 
locally and notified the team so they could fix their codebase.

We succeeded in creating an adapter to use their Sorer with our DataFrame classes, a test 
demonstrating this can be found in tests/sorerTest.cpp.
This test highlighted some memory leaks in our implementation, which have now all been fixed.
There is a little bit of small bug-fixing left for our DataFrame implementation.

The networks portion implemented for previous assignments is a good start for what will be 
required for this project.
One current problem with it is that it only supports two clients and a server. Working on a
fix.
Most of the networking functionality required for this project is set up.

Remaining tasks for M2
-from methods
-Test serialization
-Create Application level: fairly straightforward, mainly uses classes from bottom two layers
-Jan feedback from M1
