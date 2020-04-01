# SWDevProj

demoTest.cpp has latest waitAndGet


Current ReceiverThread idea:
WaitAndGet messages it can't currently respond to go on MessageQueue msgsInProg_
Think of this queue as "linking" to KVStore's receivedMsgs_ (they are the same object for a KVStore-ReceiverThread pair)
The field receivedMsgs_ is how ReceiverThread sends info to KVStore
When a ReceiverThread receives a PutData message, it adds it to kvstore (addLocal)
And then checks the msgsInProg_ Queue to see if anyone else was looking for the data that just came in
It sends the responses if available



Room for improvement:
KVStore.put sends a Put message (from itself to itself), even if we are trying to put data locally.
This causes the ReceiverThread to receive that message, add it to the local store,
and then (this is the key) run through the msgsInProg_ Queue to see if anyone is looking for that new data
There should be a way to accomplish this without have to send a message to yourself - perhaps using the shared queue in reverse
The KVStore could maybe check the queue when it wants to add a new key locally, and send back new responses if possible
But when I tried I did not get it to work consistently (Demo would hang, though possibly from an issue elsewhere)



### Notes for things to improve from before:


* waitandgetmsg.h could use comments and maybe cleanup, parallel to getdatamsg

* Put pragma once everywhere

* demoTest.cpp does not delete anything, memory leaks here

* Does columnarray need push_back? ColArr has it too. Do we even use it?

* Clean up client and server, try to get more than two clients to connect successfully

* Fix generalTests to use Column and check all 4 types

* Do we want DataFrame.add_row() and Column.push_back()? If not, delete memTest

* Sorer code- make the arrays use our array instead of re-implementing functionality

* Sorer code- merge Types enum and our col type enum

* Fix old tests to compile/run/pass

* DataFrame needs equals and hash (probably other classes do too...)

* See if there are other places where Blocks can be cleared instead of repeatedly
	created and deleted. (like deserializing for get methods, etc.) Saves memory usage

* Move most of logic (like loop) from Column.add_all into Distributed Array. Remove block imports from Column

* Changing Sorer/reading in file to go bottom up instead of top down (create the blocks then add those to arrays)

* fromScalar for int, float, string

* Condense block subclasses into one class -> Block (union type and blocktype)

* Delete code for printing throughout

* Add KVstore to DF constructor with schema and key, remove one that takes in dataframe

* Create application for sorer

* Put all messages in same file

* Delete unused message types
