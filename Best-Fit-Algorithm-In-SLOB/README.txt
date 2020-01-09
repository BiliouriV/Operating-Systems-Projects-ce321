Viktoria Biliouri AEM:2138 mail:vbiliouri@inf.uth.gr
Dimitris Christodoulou AEM:2113 mail:dchristodoulou@inf.uth.gr
Gerasimos Sideris AEM:2275 mail:gsideris@inf.uth.gr

We faced some obstacles during the implementation of the Best_Fit algorithm. What we achieved in our implementation was that we found the best block but we couldn't find the best page. To find the best page we implement a while loop to find the smallest free space in order to have a succesful allocation. However, the OS didn't boot so we were forced to deactivate slob alloc.
From the (half) implementation we understood that the Best_Fit algorithm is a better way to allocate memory because in the end it leaves less free space than the Next_Fit algorithm. (there are not big free pieces of page_memory)

For the compile: gcc -Wall -g -c -o libmemory.o memory.c
ar rcs libmemory.a libmemory.o
gcc -Wall -g -c test.c -o test.o
gcc -g -o test_new test.o -L. libmemory.a
./test_new
