# SWDevProj



### Notes for things to improve from before:

* Does columnarray need push_back? ColArr has it too. Do we even use it?

* Clean up client and server, try to get more than two clients to connect successfully

* Do we need both ColumnArray and ColArr?

* Fix generalTests to use Column and check all 4 types

* Do we want DataFrame.add_row() and Column.push_back()? If not, delete memTest

* Sorer code- make the arrays use our array instead of reimplementing functionality

* Sorer code- merge Types enum and our col type enum

* Fix old tests to compile/run/pass

* DataFrame needs equals and hash (probably other classes do too...)

* Add basic sanity tests for Key, Value

* See if there are other places where Blocks can be cleared instead of repeatedly
	created and deleted. (like deserializing for get methods, etc.) Saves memory usage

* ??Maybe cache should store deserialized data. Might as well deserialize it first,
	so we aren't repeatedly deserializing the things in the cache.

* Move most of logic (like loop) from Column.add_all into Distributed Array. Remove block imports from Column

* fromScalar

* ??Delete code for printing thoughout