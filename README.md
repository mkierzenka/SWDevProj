# SWDevProj



### Notes for things to improve from before:


* Clean up client and server, try to get more than two clients to connect successfully

* (Marcin) Rename float references to double

* Do we need both ColumnArray and ColArr?

* Fix old tests to compile/run/pass

* Serializer needs to have adjustable memory size

* DataFrame needs equals and hash (probably other classes do too...)

* Add basic sanity tests for Key, Value

* See if there are other places where Blocks can be cleared instead of repeatedly
	created and deleted. (like deserializing for get methods, etc.) Saves memory usage

* ??Maybe cache should store deserialized data. Might as well deserialize it first,
	so we aren't repeatedly deserializing the things in the cache.

* ??Should Column.add_all live more in Distributed Array? Do we need the block imports in Column?

* Test Cache, especially what happens when we try to add to it when full

* fromScalar

