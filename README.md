# SWDevProj

This repo is forked from the one Chase and I worked on together.
The intent is to rework it to offer the same functionality by take fuller advantage of new C++.


### Notes for things to improve:
	strbuff.get()
	used to steal ownership, now I think it copies b/c steal string constructor doesn't really steal.
	consider fixing that
	also, look at strbuff.steal()
	the places I use inner_.clear() and innerBuffer_.clear() maybe should be redesigned. test with wordcout (affects keybuf)
	in general, maybe take another look at the design/uses for these buffer things
--
	diffs worth transferring to new
	b/src/application/thread/nodethread.h
	@@ -45,7 +45,14 @@ class NodeThread : public Thread
			 // Let server know that we're done executing
			 net_->sendMsg(new DoneMsg(app_->this_node(), args.serverIndex));
	+        if (app_->this_node() != args.serverIndex) {
	+            fprintf(stderr, "Node %zu reporting done\n", app_->this_node());
	+            net_->sendMsg(new DoneMsg(app_->this_node(), args.serverIndex));
	+        } else {
	+            net_->handleDoneMsg(new DoneMsg(app_->this_node(), args.serverIndex));
	+        }
			 rt->join();
			 delete rt;
	+        fprintf(stderr, "Nodethread %zu completed\n", app_->this_node());
		 }
--
works well now
replace with unique_ptr instead of Object* then i don't need the loop iterating for deleting.
this change should be separate commit.
didn't do it yet b/c doesn't play well with Object* Array.get(i)
--
also, switch to this (but it's big project). keep old version around b/c may not compile right away
equals(const object other) const { }
--





possibly invalid since changes in this proj
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
  


