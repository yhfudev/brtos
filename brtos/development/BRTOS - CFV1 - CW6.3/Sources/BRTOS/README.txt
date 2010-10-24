== BRTOS 1.1 Changelog ==

- Improved scheduler 	       - New sucessive aproximation scheduler
- Priority information changed - Now the highest priority is 31 or 15, depending on the NUMBER_OF_PRIORITIES define
- Improved Post functions      - Now, post function executes the ChangeContext() if it is called from a task
- Improved Pend functions      - Erase the time to wait value from the task context when no timeout occurs
- Improved code for porting to another architecture
- Minor bugs corrections


== BRTOS 1.2 Changelog ==

- Improved code for porting to another architecture
- Improved semaphore / mutex / mailbox / queue services
- Support for selecting nesting/no nesting interrupt through define in the HAL for Coldfire V1 port
- New OS Trace feature (permit to collect data on how your application is behaving)
- Minor bugs corrections


== BRTOS 1.3 Changelog ==

- Improved block / unblock task functions
- Improved mutex - Now one mutex has its own priority
- Improved queue functions - Know there is no calls to malloc / calloc functions. Queue heap size must be defined in the BRTOS.h


== BRTOS 1.4 Changelog ==

- Improved semaphore / mutex / mailbox / queue services
- Improved code for speed - the VERBOSE define can be used to avoid saving unnecessary task data
- Now it is possible to disable the compute of the CPU load
- Improved queue function calls - No more need to pass the queue buffer in the Pend/Post functions


== BRTOS 1.45 Changelog ==

- Mutex bug correction - Now it is possible to call multiple mutex in a task to acquire more than one resource
- Improved serial driver - The TX polling was removed. Now TX uses interrupt.
- Improved include and remove of tasks from the ReadyList (better for processors of 8 and 16 bits)
