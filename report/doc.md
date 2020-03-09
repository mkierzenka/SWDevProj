##Introduction: where we give a high-level description of the eau2 system.



##Architecture: where we describe the various part of eau2 at a high-level.

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





##Implementation: where we describe how the system is built, this can include a description of the classes and their API, but only the class you deem relevant and worth describing. (For example: do not describe the utility classes.)


##Use cases: examples of uses of the system. This could be in the form of code like the one above. It is okay to leave this section mostly empty if there is nothing to say. Maybe just an example of creating a dataframe would be enough.


##Open questions: where you list things that you are not sure of and would like the answer to.


##Status: where you describe what has been done and give an estimate of the work that remains.

