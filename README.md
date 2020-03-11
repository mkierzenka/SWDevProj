# SWDevProj


For part 2
*Example test case:
	Sorer to read in a dataframe (use someone else's)
	Run some test cases on the dataframe



### Notes for things to improve from before:

in pmap
each thread gets r.clone
also, this thread (where pmap is called) can be used to do some of the work
then do r.join_delete(threads\[i\]) looping over all threads

don't we miss a row, 3threads and 100rows
memory leak? Should go through array and delete actual dataframethread elements?

rewrite lengthrower to use fielder
fielder takes idx and row ref

are the strings provided in A4P1 immutable? - probably should be for this project
^^ Do we really need to clone?
\*we should test the df.filter method also implement it

accept could call start/finish on fielder
Row allowed to copy the data?

write that string stuff takes ownership and makes copies in comments and readme

put report into git dir (A5)
add join_delete to dataframe

we updated DF code in A5, add those changes into A4P1 / A4P2
(also maybe changed in A6, too)

Clean up client and server, try to get more than two clients to connect successfully

Check serializer for correctness

Things we changed in A6
- cleaned up map.h to remove memory leaks (was in A2) - kind of
- future iprovements, use map instead of array

Reduce the number of switch statements all over dataframe (esp. in helpers)
Improvements to networking layer (connect to >2 other clients at once)

