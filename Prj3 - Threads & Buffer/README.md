# Multithreading and Bounded Buffers

Created a producer-consumer operator to manage access to a bounded buffer storing candy. One group of threads model candy factories 
which generate candy one at a time and insert the candy into the bounded buffer. Another group of threads will model kids which eat 
candy one a time from the bounded buffer.

Created a bounded buffer module which encapsulates access to the bounded buffer. This buffer operates on void* pointers
instead of directly with candy_t structures. This is done so that the buffer does not need to know about the type of information it is storing

Candy-Factory thread functions:
1. Dynamically allocate a new candy item and populate its fields
2. Insert the candy item to the bounded buffer
2. Print a status message such as: "\tFactory 0 ships candy & waits 2s"
3. Sleep for number of seconds identified 

Thread Signaling is done by the main() to end the thread. This is not using Linux signals but rather just a _Bool
global variable (which acts as a flag) that is set to false at the start which is set to true when it's time to end the thread. The factory thread continually checks this _Bool variable to see if it should end.

Kid thread functions:
1. Extract a candy item from the bounded buffer (this will block until there is a candy item to extract)
2. Process the item (for the statistics module that will track what candies have been eaten, and at what time interval)
3. Sleep (the kid threads are canceled from main() using pthread_cancel(). When this occurs, the kid thread will be waiting on the semaphore in the bounded buffer)

Created a statistics module tracking to displaying stats summary:
1. Count the number of candies each factory creates. Called from the candy-factory thread.
2. Count the number of candies that were consumed from each factory.
3. For each factory, the min, max, and average delays for how long it took from the moment the candy was
produced (dynamically allocated) until consumed. Done by the factory thread calling the stats code when a candy is created, and the kid thread calling the stats code when an item is consumed
