Implementations go in order of Main.h, Generic.h, Generic_allocator.h

Main.h is the bais of known size vector and matricies 
Generic.h includes templates so that you can have different sized vector and matricies
Generic_allocator.h allignes everything to 16 byte memory, so that i can use SSE instructions. This is based on a 4x4 matrix or vector.
