# replacement-algorithms

To run this program, go in to the src folder, use the `make` command and it will compile all the cpp files.

Then run the following commands for the following policies:
- For FIFO or LRU:
    - ./memsim __tracefile__ __nframes__ _policy_ __mode__
        - where _policy_ is either the string "fifo" or "lru"

- For VMS
    - ./memsim __tracefile__ __nframes__ "vms" __p__ __mode__
        - where __p__ is the percentage and it will be used to calculate the size of both buffers

## Key

- __tracefile__: there are two tracefiles provided, "bzip.trace" and "sixpack.trace"

- __nFrames__: Any integer > 0

- __mode__: either the string "quiet" or "debug"; The quiet mode will only print once the program is done, whereas the debug mode will print as the policy goes through its operations.