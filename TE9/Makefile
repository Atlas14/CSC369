readimage :  readimage.o
	gcc -Wall -g -o readimage $^

%.o : %.c ext2.h
	gcc -Wall -g -c $<

clean : 
	rm -f *.o readimage *~
