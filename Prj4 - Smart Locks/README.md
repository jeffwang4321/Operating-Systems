# Project 4: SmartLock for Deadlock Prevention

The project involved implementing a smart locking library that prevents deadlocks. Deadlocks are prevented by ensuring that a circular wait never occurs. The library internally relies on pthread mutex locks. 

SmartLock internally holds a pthread_mutex_t instance for correct locking and unlocking mechanism. Implemented init_lock(), lock(), unlock() and cleanup() functions to incorporate the deadlock prevention logic in SmartLock. The lock() function returns an int value, either 0 or 1 where 1 indicates that the lock got acquired and 0 indicates that the lock was not acquired (in order to prevent deadlocks). If the program calls lock, it is not always guaranteed to acquire the lock and it must first check the return value before proceeding.

Deadlock prevention is performed by ensuring that lock() does not result in a circular wait. If the allocation cannot be satisfied, the calling process is either blocked or 0 is returned back to the calling process so that the process does not block.

In order to perform cycle detection, Smart Locks represents each thread and each lock with a unique id which achieved by using the pthread_self() function for the threads and by assigning unique integer id for each SmartLock variable. A resource allocation graph is implemented to ensure that the resouces are allocated collectly. Assignment edges and request edges must be set correctly, so if a cycle is detected, then SmartLock will reassign accordingly.

The cleanup() function will releases any dynamically allocated memory of SmartLock (such as the internal resource allocation graph) to avoid memory leaks. 
