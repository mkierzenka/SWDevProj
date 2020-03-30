# SWDevProj

demoTest.cpp has latest waitAndGet

### Notes for things to improve from before:

* waitAndGet has an empty while loop. This should probably be replaced with a lock or other means
	to notify (instead of burning CPU cycles)

* getFromNetwork_ has a while loop. This should probably be replaced with a lock or other means
	to notify (instead of burning CPU cycles)

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
