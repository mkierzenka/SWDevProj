# SWDevProj


### Memory/Ownership Notes
* Array, Map, Queue all steal ownership of what's passed into them
* Test files without memory leaks:
  * array, queue, map
  * serialization
  * message, cache, blocks, distributed array
  * schema
  * store, storeIntegration
  * trivial test (all 4 trials)
  * demo test
  * Sorer
  * Linus

* Tests with memory leaks/unchecked
  * Wordcount (1 leak - 10 bytes)



### ReceiverThread Info
Receives WaitAndGet message it can't currently respond to -> add to KVStore's msgsCache_ (map).
When new data is added to a KVStore (RT calls kv_->put()), KVStore adds it locally, 
then checks the msgCache_ map to see if anyone else was looking for the data that just came in.
It sends the responses if available

### Additional notes

* Don't use args unless it's specifically for network; pseudo doesn't use all of them
* Key stored for each block (DF is DataFrame's key): Key('DF-colIdx-blockIdx', blockIdx % numNodes)
* Real Network sends a Message as the serialized msg type prepended to the serialized message.
  
### Notes for things to improve from before:

* See if there are other places where Blocks can be cleared instead of repeatedly
	created and deleted. (like deserializing for get methods, etc.) Saves memory usage

* Sorer code- make the arrays use our array instead of re-implementing functionality (Field object)

* Move reader and writer parent classes into new directory

* Move sorer into sorwriter

* Check command line arguments for valid input

* Move from methods from dataframe.h to own file