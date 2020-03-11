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


*Distributed KV store

The Distributed Key-Value Store holds Key-Value pairs locally and also
contains the ability to retrieve Values from other Key-Value stores connected
over the network. 


NOTE somewhere, that we store serialized data then deserialize on gets



Also, add the serializer class somewhere below


Key
 - key (String)
 - node_num (size_t)

Value is just a String


Store (for a KV store)
 - map (StrStrMap)
 - networkStuff (Client)
 - num (size_t, this kv's number)
 m put(k, v) - adds to necessary kv store, not blocking
 m get(k) - sends request for the data
 m getAndWait(k) - blocks until data comes back


Server (based on prev hw)
This is the "lead node"
Any new nodes should connect and register here first
Sends list of all known nodes when that changes
 - Networking Utils
 - Socket for listening


Client (based on prev hw)
Should be able to subscribe to lead node, which tells it about all other nodes
Connects to all other nodes to communicate w/ them
 - Networking Utils
 - Socket with connection to Lead Node
 - List of sockets (connections) to other Client nodes

For now, all Clients always connected between eachother
Future improvement could be to only have a subset (cache) of connections at once

*Distributed Arrays and DataFrames

DistributedArray
Seems like this isn't tied to any Node, ie. you make a bunch as you work
	with data. This means no Store in this object
The above means that this object has nothing useful to cache, so the
	cache is for the benefit of the user of this class
	ie. the Application can cache values for things here for faster lookup than network
 - keys (List of Key objects)
 - cache (ObjStrMap, Key objects to Strings)
 m containsKey(k)
 m addKey(k)
 ?? removeKey(k)
 m hasCachedValue(k)
 m getCachedValue(k)
 m removeFromCache(k)
 m addToCache(k, v)
	-> what if cache already full? drop what was added to cache longest ago or maybe just clear it and add new item
 m clearCache()



DataFrame
Mostly stays the same, this is the only place you have actual data (for now?)
Make it immutable (delete sets. delete col/row names. delete/refactor dataframe methods that use col/row names)

assume DataFrame::fromArray treats the created DF as 1 row with the values



*Applications

Application
 - kv (local KV store)
 m this_node()
 m run_()




##Implementation: where we describe how the system is built, this can include a description of the classes and their API, but only the class you deem relevant and worth describing. (For example: do not describe the utility classes.)




Also, add the serializer class somewhere below


3 layers
?global hardcoded constant for number of nodes?


*Distributed KV store
Key
 - key (String)
 - node_num (size_t)

Value is just a String


Store (for a KV store)
 - map (StrStrMap)
 - networkStuff (Client)
 - num (size_t, this kv's number)
 m put(k, v) - adds to necessary kv store, not blocking
 m get(k) - sends request for the data
 m getAndWait(k) - blocks until data comes back


Server (based on prev hw)
This is the "lead node"
Any new nodes should connect and register here first
Sends list of all known nodes when that changes
 - Networking Utils
 - Socket for listening


Client (based on prev hw)
Should be able to subscribe to lead node, which tells it about all other nodes
Connects to all other nodes to communicate w/ them
 - Networking Utils
 - Socket with connection to Lead Node
 - List of sockets (connections) to other Client nodes

For now, all Clients always connected between eachother
Future improvement could be to only have a subset (cache) of connections at once

*Distributed Arrays and DataFrames

DistributedArray
Seems like this isn't tied to any Node, ie. you make a bunch as you work
	with data. This means no Store in this object
The above means that this object has nothing useful to cache, so the
	cache is for the benefit of the user of this class
	ie. the Application can cache values for things here for faster lookup than network
 - keys (List of Key objects)
 - cache (ObjStrMap, Key objects to Strings)
 m containsKey(k)
 m addKey(k)
 ?? removeKey(k)
 m hasCachedValue(k)
 m getCachedValue(k)
 m removeFromCache(k)
 m addToCache(k, v)
	-> what if cache already full? drop what was added to cache longest ago or maybe just clear it and add new item
 m clearCache()



DataFrame
Mostly stays the same, this is the only place you have actual data (for now?)
Make it immutable (delete sets. delete col/row names. delete/refactor dataframe methods that use col/row names)

assume DataFrame::fromArray treats the created DF as 1 row with the values



*Applications

Application
 - kv (local KV store)
 m this_node()
 m run_()



##Use cases: examples of uses of the system. This could be in the form of code like the one above. It is okay to leave this section mostly empty if there is nothing to say. Maybe just an example of creating a dataframe would be enough.

The following is an example of a use case of the eau2 system. The lines of pseudocode are an outline of what each node will run.

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
	// store df in distributed key-value store under key dataA
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
			DataFrame::fromScalar(&dataAFirstDif, &kv, dfA->get(i, j)); // generalize for any type
			DataFrame::fromScalar(&dataBFirstDif, &kv, dfB->get(i, j)); // generalize for any type
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
	p("dataA\[").p(posDF->get(0, 0)).p(", ").p(posDF->get(0, 1)).p("\] = ").p(aDif->get(0));
	p(" , dataB\[").p(posDF->get(0, 0)).p(", ").p(posDF->get(0, 1)).p("\] = ").p(bDif->get(0));
	pln("");
	return;


##Open questions: where you list things that you are not sure of and would like the answer to.

In HW6, we implemented a Server and Clients. It would make sense for each Key-Value Store to have the code from Client, but what about the code for Server?
Having a single Server node makes it easier to register new Clients (add new nodes to the system). We are not sure on the details of implementing this,
should the Server be another node of the network with it's own Key-Value store?
Seems like having a dedicated Server which only handles registering new Clients would be cleaner, but that doesn't seem to fit with
the provided example code. Any advice?


Are we correct in our thinking that the DataFrame largely remains unchanged from how we originally implemented it,
and is the only place actual (deserialized) data is grouped?
(ie. workflow is: make DF of interesting data -> serialize and store in eau2 under some Key)

Any guidance on the cache in the DistributedArray? Seems more usable if it's decided by the Application, but alternative
is that it is an implementation detail of DistributedArray.

Should each Client (KV store) always be connected to every other Client? Are we expecting 100K Client systems?




##Status: where you describe what has been done and give an estimate of the work that remains.

