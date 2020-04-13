# SWDevProj


### Memory/Ownership Notes
* Array, Map, Queue all steal ownership of what's passed into them
* Test files without memory leaks: array, queue, map, message, cache, blocks, serial, store, schema
* Tests with memory leaks/unchecked
  * Distributed array (test not developed)
  * Network
  * Trivial
  * Demo
  * Sorer (test not working yet)



### ReceiverThread Info
Receives WaitAndGet message it can't currently respond to -> add to MessageQueue msgsInProg_
Think of this queue as "linking" to KVStore's receivedMsgs_ (they are the same object for a KVStore-ReceiverThread pair)
The field receivedMsgs_ is how ReceiverThread sends info to KVStore
When new data is added to a KVStore (RT calls kv_->put()), KVStore adds it locally
  then checks the receivedMsgs_ Queue to see if anyone else was looking for the data that just came in
It sends the responses if available


### Notes for things to improve from before:

* Cleanup and free more memory in pseudonetworking stuff -> receiverthread, nodethread, etc.

* demoTest.cpp does not delete anything, memory leaks here

* DataFrame needs equals and hash (probably other classes do too...)

* See if there are other places where Blocks can be cleared instead of repeatedly
	created and deleted. (like deserializing for get methods, etc.) Saves memory usage

* Move most of logic (like loop) from Column.add_all into Distributed Array. Remove block imports from Column

* Delete stringarray, update message classes that use it

* Delete unused message types

* Update schema to use types enum

* Implement push_back for columns

* Add KVstore to DF constructor with schema and key, remove one that takes in dataframe

* Sorer code- make the arrays use our array instead of re-implementing functionality

* Sorer code- merge Types enum and our col type enum

* Changing Sorer/reading in file to go bottom up instead of top down (create the blocks then add those to arrays)

* Methods w/o an implementation: add assert(false) and comment

* Add unit tests: distributed array

* doc.md: create diagram for architecture section

* Add underscore to all private fields (KVStore)

* Distribute data across nodes

* Figure out how to use port numbers

* Test network from different machines