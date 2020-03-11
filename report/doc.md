##Introduction: where we give a high-level description of the eau2 system.

The eau2 system is a framework to allow a wide variety of applications to
operate on large datasets distributed across nodes in a network. Each node
has a collection of key-value pairs stored locally as well as the ability to
retrieve values from other nodes over the network, as needed. The system can
be scaled for different use-cases (supports an arbitrary number of nodes).
Most cases will have a different responsibility for each node, for example
one to produce data and another to consume it.

##Architecture: where we describe the various part of eau2 at a high-level.

The eau2 system is made up of three layers of abstraction.

The first and lowest layer is the distributed key-value store. There will be multiple 
key-value stores, each holding some portion of data.
The stored data will be a serialized string 
representation. Each store will be able to serialize
 and deserialize its own data.
If a key-value store requires data that it does not 
have, then it will be able to communicate with other the key-value stores to retrieve this information.
This will be done via a networking abstraction, which will allow key-value stores to behave as nodes on a
 network. In addition to the stores, there will also be a lead node, which will keep track of registered
  stores and notify all connected stores when a new store is added to the network.

The next layer will include abstractions for holding the data from the key-value stores. These abstractions include distributed arrays and dataframes. The distributed array will include a list of keys; a key will define a string that maps to a piece of data, and will include the identifier of the key-value store that the data lives on. The array will also include a cache, which will hold the data for some of the listed keys. However, at any given time, not every key will have its data in the cache. A dataframe will represent the deserialized data that exists in the key-value stores.

The last and highest level is the application layer. In the application, the user will be able to specify what they want to do. Each node will run its own application, with responsibilities distributed across each node. The application can get data from and put data into the key-value store. Distributed arrays can be used to track, organize, and work with specific data across the eau2 system.

##Implementation: where we describe how the system is built, this can include a description of the classes and their API, but only the class you deem relevant and worth describing.(For example: do not describe the utility classes.)

Also, add the serializer class somewhere below

*Store: this class will represent a local key-value store
Fields:
 - map (Str-to-Str Map): the String key will map to a serialized piece of data
 - networkHelper (Client): this network abstraction will allow the store to listen for messages from other nodes and send messages to other stores to request data
 - storeId (size_t): each local store will be represented by a unique numerical identifier. At a higher level, this identifier will help keep track of where the data is stored
 
 Methods:
 - put(Key, String): adds given data to the key-value store specified in the key, not blocking
 - get(Key): request for the data; returns deserialized data from its own store if stored locally, otherwise will make a request to whichever store holds the data
 - getAndWait(Key): same functionality as get but call will block until the data is retrieved
 -getStoreId(): return the id of this store (storeId)

*Key: these are used to define a piece of data at a level higher than the local KV store. Since data can exist in any of the stores, we need multiple attributes to keep track of data
Fields:
 - key (String): maps to a piece of data in a key-value store
 - node_num (size_t): the identifier for the key-value store in which the data is stored

 *NOTE: Keys are immutable*

*Server: This is the "lead node." All nodes will connect and register with this host. Upon registration, the server will broadcast a list of every node that it is connected to. The server will have basic network functionality and a socket in which it listens for node connections. It will also hold connection information for each known node.

*Client: This class is the abstraction that key-value stores will use to communicate with each other. The client will establish a connection and subscribe to the lead node, which tells the client about all other nodes on the system. The client also establishes connections to all other nodes to support two-way communication. Like the server, the client will also include essential networking capabilities, and hold sockets for sending and listening.

*DataFrame: The DataFrame API will be similar to that on previous assignments. It will include operations to store and perform operations on data, such as map. A Schema will be used to describe the Dataframe's column structure. A DataFrame will be immutable, so it will not support operations such as deleting, setting, and modifying columns and rows. The DataFrame will have methods that support converting data types into frames. An example is fromArray; it will take in an array of a certain type, and it will return the DataFrame version. It will also put the serialized data into the given key-value store, with the specified key. These methods will exist for all supported data types (bool, int, floats, strings), and for arrays of these types.

*Serializer: This class will be similar to the one started in Assignment 6. It will be in charge of serializing and deserializing data, and buffering the result. The Serializer will be used in multiple places in this project. All of DataFrame's from... methods will need to initialize a Serializer to serialize the constructed dataframe, to store in the key-value store. The wait and waitAndGet methods within Store will need to intialize a Serializer; the serializer will need to convert the serialized string data into the dataframe that it represents

*DistributedArray
Fields:

* keys (Array): this array will hold all Key values of interest
* cache (Obj-to-Str Map): this map will hold the data for some of the Key objects within this distributed array. The key for this map will be the same Key object stored within the keys array; the value will be the serialized data. The goal of the cache is to improve the performance of data retrieval; before sending a data request to a key-value store, we will first check to see if the data exists in the cache. The cache will be limited in size, so not all the distributed arrays' keys will hold its data there at once. The cache will be refreshed occasionally; at the moment, we are unsure of a strategy to update the cache.

Methods:

* containsKey(Key): does the distributed array's key list contain the given key?
* addKey(Key): add the given key to the distributed array's list of keys
* removeKey(Key): remove the given key from the distributed array's key list
* hasCachedValue(Key): does the cache contain data for the given key?
* getCachedValue(Key): get the cached data for the given key
* removeFromCache(Key): remove the data associated with the given key from the cache
* addToCache(Key, String): add the serialized data, with the given key, to the cache. If cache full, replace an element
* clearCache() - remove all entries from the cache

*Note that the cache methods defined here will delegate to the Cache object, which work on the Cache's map object*

* Cache: class that represents a cache within a distributed array

Fields:

* maxSize: defines a maximum size of the cache; the cache's map cannot hold more than this amount of elements at any given time
* data (Obj-to-String map): this map will hold the data within the cache. The keys will be Key objects, and they will map to the serialized data

Methods:

* containsKey(Key): does the map contain data for the given key?
* getValue(Key): get the mapped data for the given key
* remove(Key): remove the key and data associated with the given key from the map
* put(Key, String): add the serialized data, with the given key, to the map. If map full, 

replace an element

* clear() - remove all entries from the map

*Application: this is the highest level of the program. The Application is what the user interacts with, in which he or she defines operations to perform over a certain set of data
Fields:

* kv (Store): this will be the local kv store for the node that the application is running on. When initialized, it will take in the node id from the Application's constructor and set it as its id

Methods:

* this_node(): returns id of this node; retrieved from the KV store
* run_(): given the node id, perform a certain set of operations. The node id will be 

placed in a switch statement, and each case will contain a different helper for each 
application running (ex.producer to initialize and create the data, summarizer to perform
 some operation on the data). Distributed arrays and dataframes and can be initialized and
  used for these helpers, depending on the purpose of the application

##Use cases: examples of uses of the system. This could be in the form of code like the one above. It is okay to leave this section mostly empty if there is nothing to say. Maybe just an example of creating a dataframe would be enough.

##Open questions: where you list things that you are not sure of and would like the answer to.

##Status: where you describe what has been done and give an estimate of the work that remains.

