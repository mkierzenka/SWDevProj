## Usage:
* "make build" - compiles all tests
* "make test" - runs tests supplied to us (eg. trivial, demo tests)
* "make ourTests" - runs sorer test and a bunch of others testing different parts of our program
* "make memory" - runs selected test through valgrind
* "make clean" - deletes compiled files
* there are also targets for specific programs we are focusing on (ex. buildDemo and testDemo for M3)


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
multiple key-value stores, one per node; each will hold some portion of data. Each key 
will map to a chunk of serialized data or a dataframe. Each store will be able to serialize 
and deserialize its own data. If a key-value store requires data that it does not have, then 
it will be able to communicate with the other key-value stores to retrieve this 
information. This will be done via a networking abstraction, which will allow key-value 
stores to behave as nodes on a network and send messages to each other. In addition to the stores, there will also be a lead
node, which will keep track of registered stores and notify all connected stores when a new 
store is added to the network.

The next layer will include abstractions for holding and representing the data from the key-value stores. 
These abstractions include distributed arrays and dataframes. A dataframe allows the user
to access a set of data across multiple nodes. It supports data storage and retrieval 
without exposing the details of how and where the data is stored. A dataframe holds a 
collection of columns. Each column is a distributed array. A distributed array contains a 
collection of keys as well as a cache, which will support short-term storage for some of the 
keys' deserialized data. The distributed array will also have a key-value store; therefore even if data
does not live in the cache, the array will be able to look up the corresponding data for 
all of its keys. 

The last and highest level is the application layer. In the application, the user 
will be able to specify what they want to do. Each node will run its own application, 
with responsibilities distributed across each node. The application can get data 
from and put data into the key-value store. Distributed arrays can be used to track, 
organize, and work with specific data across the eau2 system.

## Implementation

For the critical classes that we will implement for this system, we included a run-down
of their fields and public methods. For components that are re-used from previous assignments,
we provided descriptions of how they'll be used.

* KVStore: this class will represent a local key-value store

  * Fields:
    * kvMap_ (MapStrObj): the String key will map to a serialized piece of data (Value)
    * node_ (INetwork): this network abstraction will allow the store to listen for 
    messages from other nodes and send messages to other stores to request data or put data in another node's store
    * storeId_ (size_t): each local store will be represented by a unique numerical identifier. 
    At a higher level, this identifier will help keep track of where the data is stored
    * msgCache_ (Map): this map will hold the messages that the KVStore cannot handle right away. For example, if another node requests data from this store that is guaranteed to eventually be in this store, the store will hold that message in the queue until the data is present. The key will be an actual Key object and the value will be the message that the key corresponds to. Right now only two types of messages will exist in this cache: WaitAndGetMsg and ReplyDataMsg
    * msgCacheLock_ (Lock): used when accessing the cache, to avoid another thread from reading from/writing to the cache at the same time
 
  * Methods:
    * put(Key, Value): adds given data to the key-value store specified in the key, 
    not blocking; sends Put message if Key's node id doesn't correspond to this store's id
    * get(Key): request for the data; returns deserialized data from its own store or from the network if data for the key exists, otherwise returns nullptr
    * waitAndGet(Key): retrieves data with the given Key from the Key's node or from the network if not local; blocking, so won't return until data exists in store specified by key
    * getValue(Key): return the value that the given key maps to; this method will not do
    any serialization or deserialization, rather will return the result exactly as it is stored; goes to the network if data not stored locally
    * addMsgWaitingOn(WaitAndGetMsg): add wait and get message to cache; this kvstore currently doesn't have the data to send a reply data, so need to wait until the data put into the store
    * addReply(ReplyDataMsg): adds reply data message to the cache

* Key: these are used to define a piece of data at a level higher than the local KV store. 
Since data can exist in any of the stores, we need multiple attributes to keep track of data
  * Fields:
    * kStr_ (String): maps to a piece of data in a key-value store
    * homeNode_ (size_t): the identifier for the key-value store in which the data is stored

 *NOTE: Keys are immutable. There will be methods to retrieve these values but not modify them*

* Value: represents the serialized data that a Key maps to. It will include a character pointer 
that holds the serialized data and a size_t that describes the maximum amount of bytes that can
be stored in this pointer.

* PseudoNetwork: for Milestone 3, we created a fake networking layer to aid with debugging
  for our distributed KV store. Similarly to the videos, we set up a PseudoNetwork to have
  an array of message queues; each queue keeps track of the messages received and pending for
  each node. The pseudonetwork supports sending and receiving messages. Send pushes the given
  message to the target node's queue, while receive pops from the node's queue. Due to this 
  setup with a single message queue array, each node and thread needs access to the same 
  message queues. Therefore we had to initialize a PseudoNetwork in our .cpp test file and pass
  it in to numerous objects throughout the program, including the Application and the KVStore. When we incorporate the actual network layer, each node will be able to have its own network object

* Network: the actual networking layer that we incorporated into our program for M4. It includes messages for initializing the servers and clients. This kicks off and completes the handshake process. For the client, it connects to the server, sends a register message to the server, and then receives a directory message from the server. For the server (first node per program), it accepts connections from clients, receives register messages from clients, and keeps track of the clients. Once all nodes are registered, it broadcasts a directory message to all of its connected nodes.

* INetwork: We created an INetwork that both Network and PseudoNetwork inherited. This was so that our entire program could use an INetwork, allowing for easy swapping between our pseudo and real networks in our tests (see the .cpp test files for more context). The interfaces includes six methods. The first two are for sending and receiving. The last four are only used by the actual network: server_init, client_init, handleRegisterMsg, and handleDirectoryMsg. These are not needed for the pseudonetwork, so they were able to be left blank in the INetwork class for PseudoNetwork to inherit

* Messages: messages are what nodes use to communicate each other. Data can be placed inside messages. Messages are serialized by the sending node, then sent, then deserialized and read by the receiving node. Each message type has a different purposes. The following outlines the current message types sent between nodes in our program:
  * Get: this is a request for data. It is not blocking, so it will return a nullptr if the data doesn't exist in the requested node's store
  * WaitAndGet: this is a request for data and is sent to a specific node that holds this data. This message is blocking; a response won't be delivered to the sender until the data exists in the requested node's store
  * ReplyData: this is a response to a data request. The node who received this message will send serialized data back to the node that requested it
  * PutMsg: this message puts data, both a key and a value, in another node's store
  * RegisterMsg: allows a client to register with a main, or server, node that keeps track of connections
  * DirectoryMsg: broadcasts a list of connections to each clients; necessary for clients to be able to send messages between each other
  * Other message types that may be used in the future include Ack, Directory, Register, Kill, and Status messages


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

  The DataFrame will hold a KVStore object that contains the frame's actual data.
  Not only does the DataFrame need this object, but also any other objects within
  the frame that need to store and request data

  The DataFrame will have methods that support converting data types into frames. An example 
  is fromArray; it will take in an array of a certain type, and it will return the DataFrame 
  version (add the values in one new column). It will break up the array into chunks; each chunk 
  will be assigned a key to be stored in the distributed KV system. The key will then be added to 
  the column's distributed array. Once the input array is completely broken up and stored, the 
  DataFrame will be serialized and stored under its key in the distributed system.

  * Fields:
    * columns_ (ColumnArray): holds column information
    * store_ (KVStore): dataframe passes store to classes that need it for data lookup; static initialization 
    * schema_ (Schema): keeps track of the DataFrame's column structure
    * key_ (Key): key for this dataframe in the store; will be used to create the keys for the
frame's columns' chunks


  * Methods (new ones we anticipate):
    * all the methods that create a DataFrame from a data type (ex. fromArray, fromScalar, fromVisitor, etc.) and return it
    * void add_array(size_t, *type*): adds array into the DataFrame as a column; one for each type
    * void map(Rower): perform an operation on all data in the DataFrame
    * Schema& get_schema(): return schema of DataFrame
    * void add_column(Column): add column to DataFrame
    * get: get and return certain value from DataFrame's columns; one for each type
    * void add_row (Row): add a row to the DataFrame (currently not fully implemented)
    * size_t nrows(): return number of rows in the df
    * size_t ncols(): returns number of columns in the df


* Column: A column now stores data in the distributed KV Store, instead of locally. A Column will be a DistributedArray where each Key points to a Value containing a fixed sized number of elements that belong to this Column. Since we are no longer storing elements directly in the column, we do not need separate column implementations for each type

  * Fields:
    * blocks_ (DistributedArray): holds blocks of column data
    * size_ (size_t): number of elements in the column (!= number of keys in array)
    * store_ (KVStore): will be used by column to retrieve its own data when needed and add keys to the store
    * type_ (ColType: enum): what type of column is this? String, boolean, int, or double
    * baseKey_ (Key): key of the dataframe that the column belongs to; will be used to build up block keys

  * Methods:
    * most methods from before, for getting data from the column. not 'set' methods (or anything else that modifies the column). Might include the push_back methods
    * void add_all(size_t, *type*): one for each data type; adds all given elements of that datatype to the column; breaks data up into chunks, creates keys for those chunks, and adds them to the KVStore
    * void setStore(KVStore): pass a KVStore to the dataframe; will initially start in the dataframe, which will be passed to the column array and eventually the column
    * char getCharType(): get column's data type in character form ('I', 'B', 'S', 'F')
    * void push_back(val): add value to end of column; currently debating whether or not this implementation is necessary, would have one for each type
    * size_t size(): return size of column
    * get: get element from column; one method for each type


* Serializer: This class will be similar to the one started in Assignment 6. It supports writing 
primitive types, character pointers (strings), and message types to its buffer, and can also read
these when deserializing. It will be 
in charge of serializing and deserializing data, and buffering the result. The Serializer 
will be used in multiple places in this project. All of DataFrame's from... methods need to 
initialize a Serializer to serialize the constructed dataframe, to store in the 
key-value store. The wait and waitAndGet methods within Store need to initialize a 
Serializer; the serializer will need to convert the serialized string data into the dataframe 
that it represents. Serialization is also needed whenever attempting to retrieve
data from the DataFrame, as all the data is serialized in the KVStore

* DistributedArray: this class will hold reference information about data throughout the 
system. The purpose of this class is to bridge data that lives in different areas of the 
system. This class also holds a KVStore reference, so that it can look up data not
stored locally.

  * Fields:
    * store_ (KVStore): to get data from the distributed system
    * keyList_ (KeyArr): this array will hold all Key values of interest
    * cache_ (Cache): this object will hold the data for some of the Keys within 
this distributed array. Before sending a data request to a key-value store, we will 
first check to see if the data exists in the cache.

  * Methods:
    * bool containsKey(Key): does the distributed array's key list contain the given key?
    * void addKey(Key): add the given key to the distributed array's list of keys
    * get(Key, size_t): get the value for the specified key from the distributed array. Check the cache first and if the cache does not contain the data, then make a call too the KV
  store; one for each type
    * Key getKeyAtIndex(size_t): return key at the given index in the array
    * void setStore(KVStore): this method will be used to pass a store into the distributed array; the store will come from the dataframe initially and move down the hierarchy (dataframe -> columnarray -> column -> distributed array)
    * Serialization and deserialization methods

* Cache: class that represents a cache within a distributed array. Holds the data of
some of the array's keys.
keys array; the value will be the deserialized data.Holds the actual, deserialized data for improved retrieval
efficiency. The cache will be limited in size and will be 
refreshed occasionally. We currently use a FIFO strategy to refresh the cache; if we want to add a new item to the cache but it is full, then we remove the element that has been in there the longest


  * Fields:
    * maxSize_ (size_t): defines a maximum size of the cache; the cache's map cannot hold more than 
this amount of elements at any given time
    * data_ (Map): this map will hold the data within the cache. The keys will 
be Key objects, and they will map to the serialized data.
    * keyOrder_ (Queue): this queue will keep the order in which keys are added to the cache. This will help us update our cache by adding and removing elements on a FIFO basis

  * Methods:
    * bool containsKey(Key): does the map contain data for the given key?
    * Block getBlock(Key): get the mapped deserialized data for the given key
    * void put(Key, Block): add the Block, with the given key, to the map. If map 
    full, replace an element
    * void clear() - remove all entries from the map


* Application: this is the highest level of the program. The Application is what the 
user interacts with, in which he or she defines operations to perform over a certain 
set of data. An application will run on each node. To specify operations, Application can be subclassed

  * Fields:
    * kv_ (KVStore): this will be the local kv store for the node that the application is 
    running on. When initialized, it will take in the node id from the Application's 
    and set it as its id
    * idx_ (size_t): node index that this application is running on
    * net_ (INetwork): network node for this application; passed into constructor but steals ownership

  * Methods:
    * size_t this_node(): returns id of this node
    * KVStore getStore(): retrieve KVStore from application
    * INetwork getNetwork(): retrieve network from application
    * run_(): given the node id, perform a certain set of operations. The node id will be 
    placed in a switch statement, and each case will contain a different helper for each 
    application running (ex.producer to initialize and create the data, summarizer to 
    perform some operation on the data). Distributed arrays and dataframes and can be 
    initialized and used for these helpers, depending on the purpose of the application


* Utilities: We have a variety of common classes used throughout the code base. These include Object, String, Array, Map, Queue, and Thread
  
  
## Use cases

The following is an example of a use case of the eau2 system. The lines of pseudocode 
are an outline of what each node will run.

```
Application:

    dataA = Key("dataA", 0);
    dataB = Key("dataB", 1);
    firstDifPos = Key("firstDifPos", 2);
    dataAFirstDif = Key("dataAFirstDif", 2);
    dataBFirstDif = Key("dataBFirstDif", 2);
    same = Key("areSame", 2);
    sameSize = Key("sameSize", 2);

    ProducerA (node_num = 0):
        Sorer s = new Sorer();
        DataFrame* df = s.read("fileA");
        // store df in distributed key-value store under key dataA
    return;

    ProducerB (node_num = 1):
        Sorer s = new Sorer();
        DataFrame* df = s.read("fileB");
        // store df in distributed key-value store under key dataB
    return;

    Comparer (node_num = 2):
        DataFrame* dfA = waitAndGet(dataA);
        DataFrame* dfB = waitAndGet(dataB);
        int areSame = 0;
        if (size of dfA != size of dfB) {
            DataFrame::fromScalar(&sameSize, &kv, 0);
            DataFrame::fromScalar(&same, &kv, 0);
            return;
        }
        DataFrame::fromScalar(&sameSize, &kv, 1);
        // loop through elements
    	if (dfA->get(i, j) not equal dfB->get(i, j)) {
    		int* pos = new int[2];
    		pos[0] = i;
    		pos[1] = j;
    		DataFrame::fromArray(&firstDifPos, &kv, 2, pos);
    		// generalize for any type
    		DataFrame::fromScalar(&dataAFirstDif, &kv, dfA->get(i, j)); 
    		// generalize for any type
    		DataFrame::fromScalar(&dataBFirstDif, &kv, dfB->get(i, j)); 
        }
        DataFrame::fromScalar(&same, &kv, 1);
    return;

    Reporter (node_num = 3):
        DataFrame* sameDF = waitAndGet(same);
        if (sameDF->get(0, 0)) {
    	    pln("Same Data! :)");
    	    return;
        }
        pln("Different Data.");
        DataFrame* sizeDF = waitAndGet(sameSize);
        if (sizeDF->get(0, 0)) {
    	    pln("Non-Equal Size");
    	    return;
        }
        DataFrame* posDF = waitAndGet(firstDifPos);
        DataFrame* aDif = waitAndGet(dataAFirstDif);
        DataFrame* bDif = waitAndGet(dataBFirstDif);
        pln("Different Data!");
        p("Example: ");
        p("dataA[").p(posDF->get(0, 0)).p(", ").p(posDF->get(0, 1)).p("] = ")
            .p(aDif->get(0));
        p(" , dataB[").p(posDF->get(0, 0)).p(", ").p(posDF->get(0, 1)).p("] = ")
            .p(bDif->get(0));
        pln("");
    return;
```

## Open questions

How do we decide what to make the buffer size for receiving messages? Do we need to be able to fragment messages? We are getting segfaults when we make the demo test too large, not sure if this is related to our buffer

## Status
We have decided to use another group's Sorer implementation, as ours was written in Python and 
not fully correct.
The group was chosen based on the results of our testing in Assignment 5, Part 1 -> 
https://github.com/yth/CS4500A1P1
As part of incorporating their Sorer into our codebase, we discovered a bug that we fixed 
locally and notified the team so they could fix their codebase.

We succeeded in creating an adapter to use their Sorer with our DataFrame classes, a test 
demonstrating this can be found in tests/sorerTest.cpp.

We have implemented serialization for all of the necessary classes and message, and are able to use the key-value store for data storage and retrieval. We also have a completely working pseudonetwork and a somewhat functional actual network, allowing us to distribute our data. However we currently store our blocks on the same node as our dataframe. We would like to come up with some sort of mechanism for effectively distributing data across nodes.

Our Trivial test works with both our pseudo and actual network. Our Demo test works fully with our pseudonetwork, but only partially with our actual network. In the demo case, we had to drastically reduce the amount of elements to 100. If the dataframe became too large, the program would either segfault or result in corrupted memory. We are currently unsure of why this issue exists, but believe it may be related to how we are reading in messages.

There are additional features we need to add to our network. For example, we currently hard-code the number of nodes, server node number, and server ip as constants. Rather than having to hard-set those, we would like to pass those in as command line arguments. When we run the trivial test, we have to manually set the number of nodes to 1, then set it back to 3 when we run the demo test.

Our file reading is a work in-progress. We have a target for the test; it compiles, but we are currently getting seg fault issues. In particular, we need to further investigate and test our fromVisitor.

We also have some general code cleanup to do, including remove unused code and reduce our memory leaks.


## Remaining tasks from M3:
  
* Clean up leaks from tests


## Remaining tasks for M4:

* Improve networking/demo code: avoid segfaults for large data
* Work on file reading