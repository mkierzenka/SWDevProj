# SWDevProj



valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./a.out


For part 2
*Example test case:
	Sorer to read in a dataframe (use someone else's)
	Run some test cases on the dataframe



### Notes for things to improve from before:

write a rower that actually uses join_delete

in pmap
each thread gets r.clone
also, this thread (where pmap is called) can be used to do some of the work
then do r.join_delete(threads\[i\]) looping over all threads

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


Clean up client and server, try to get more than two clients to connect successfully

Check serializer for correctness

Things we changed in A6
- future iprovements, use map instead of array

Reduce the number of switch statements all over dataframe (esp. in helpers)
Improvements to networking layer (connect to >2 other clients at once)

