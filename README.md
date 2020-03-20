# SWDevProj



### Notes for things to improve from before:

* Get rid of commandline arg stuff in Sorer main file, or at least refactor into
	another class to cleanup main. (Per 35:00 of swdev-v1)

* Piazza #925_f2

* In Schema, do we use a ton of memory to store the col types because we alloc big
	cols/blocks just to store the one element? Probably just make it a char array,
	saves lots of data -> piazza #939

* Clean up client and server, try to get more than two clients to connect successfully

* Check serializer for correctness


* we should test the df.filter method also implement it

* Consider:are the strings provided in A4P1 immutable? - probably should be for this project
^^ Do we really need to clone?


* Consider:Row allowed to copy the data?

* document which methods using String\* take ownership and makes copies in comments

* Maybe refactor to have 2 stores: blobstore and kvstore
	one has just the dataframes, and is where the Application pulls "get" from
	the other has all the actual blocks of data that is used by a dataframe to return
	actual values (get_double(i,j))

* Change all NULL references to nullptr