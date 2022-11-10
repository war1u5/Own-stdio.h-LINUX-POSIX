main: main.c libso_stdio.so path
	gcc -g -Wall main.c -o main libso_stdio.so

path:
	export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
	
libso_stdio.so: so_stdio.c
	gcc -g -fPIC $? -shared -o libso_stdio.so

clean:
	rm -f *.o *~ main *.so