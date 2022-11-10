# Tema 1 PSO: _Biblioteca stdio_
_Prelipcean Marius_<br/>
_Grupa: C113D_

## Obiectivele temei
* Familiarizarea cu modul de funcționare al bibliotecii standard input/​output (stdio)
* Aprofundarea conceptelor de:
  * I/O buffering
  * Crearea de procese și rularea de fișiere executabile
  * Redirectearea intrărilor și ieșirilor standard
  * Generarea de biblioteci dinamice

## Enunt
Să se realizeze o implementare minimală a bibliotecii stdio, care să permită lucrul cu fișiere. Biblioteca va trebui să implementeze structura SO_FILE (similar cu FILE din biblioteca standard C), împreună cu funcțiile de citire/scriere. De asemenea, va trebui să ofere funcționalitatea de buffering. <br/>
Rezolvarea temei va trebui să genereze o bibliotecă dinamică numită libso_stdio.so/so_stdio.dll care implementează funcțiile declarate în header-ul [so_stdio.h](https://github.com/systems-cs-pub-ro/so/blob/master/assignments/2-stdio/util/so_stdio.h).

## [Checker](https://github.com/systems-cs-pub-ro/so/tree/master/assignments/2-stdio)
Aceasta tema a fost verificata folosind checker-ul din link-ul de mai sus! (80/95)

## Compilare si rulare
#### Makefile
```Makefile
main: main.c libso_stdio.so path
	gcc -g -Wall main.c -o main libso_stdio.so

path:
	export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
	
libso_stdio.so: so_stdio.c
	gcc -g -fPIC $? -shared -o libso_stdio.so

clean:
	rm -f *.o *~ main *.so
```
#### Rulare
```bash
make clean
```
```bash
make main
```
```bash
./main
```

## Rezolvare
### Elemente necesare:
#### so_stdio.h
Contine definitiile functiilor 
```c++
#ifndef SO_STDIO_H
#define SO_STDIO_H

#if defined(__linux__)
#define FUNC_DECL_PREFIX
#elif defined(_WIN32)
#include <Windows.h>

#ifdef DLL_EXPORTS
#define FUNC_DECL_PREFIX __declspec(dllexport)
#else
#define FUNC_DECL_PREFIX __declspec(dllimport)
#endif

#else
#error "Unknown platform"
#endif

#include <stdlib.h>

#define SEEK_SET	0	/* Seek from beginning of file.  */
#define SEEK_CUR	1	/* Seek from current position.  */
#define SEEK_END	2	/* Seek from end of file.  */

#define SO_EOF (-1)

struct _so_file;

typedef struct _so_file SO_FILE;

FUNC_DECL_PREFIX SO_FILE *so_fopen(const char *pathname, const char *mode);         //da
FUNC_DECL_PREFIX int so_fclose(SO_FILE *stream);                                    //da

#if defined(__linux__)
FUNC_DECL_PREFIX int so_fileno(SO_FILE *stream);                                    //da
#elif defined(_WIN32)
FUNC_DECL_PREFIX HANDLE so_fileno(SO_FILE *stream);                                 
#else
#error "Unknown platform"
#endif


FUNC_DECL_PREFIX int so_fflush(SO_FILE *stream);                                    //da

FUNC_DECL_PREFIX int so_fseek(SO_FILE *stream, long offset, int whence);            //da
FUNC_DECL_PREFIX long so_ftell(SO_FILE *stream);                                    //da    

FUNC_DECL_PREFIX
size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream);             //da

FUNC_DECL_PREFIX
size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream);

FUNC_DECL_PREFIX int so_fgetc(SO_FILE *stream);                                     //da
FUNC_DECL_PREFIX int so_fputc(int c, SO_FILE *stream);

FUNC_DECL_PREFIX int so_feof(SO_FILE *stream);                                      //da
FUNC_DECL_PREFIX int so_ferror(SO_FILE *stream);                                    //da

FUNC_DECL_PREFIX SO_FILE *so_popen(const char *command, const char *type);
FUNC_DECL_PREFIX int so_pclose(SO_FILE *stream);

#endif /* SO_STDIO_H */
```
#### so_tools.h
Contine macro-uri, handle/file descriptor, structura _so_file<br/>
```c++
#define _CRT_SECURE_NO_WARNINGS
#if defined(__linux__)
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
typedef int SO_HANDLE;

#elif defined(_WIN32)
#include <fileapi.h>
typedef HANDLE SO_HANDLE

#else
#error "Unknown platform"
#endif

#include <string.h>

//definire valori true/false
#define SO_TRUE 1
#define SO_FALSE 0

//definire buffer
#define BUFF_SIZE 4096

//definire access modes
typedef enum {
	R, R_PLUS, W, W_PLUS, A, A_PLUS
} FILE_ACCESS;

//definire operatii
typedef enum {
	READ_OP, WRITE_OP, DIFF_OP, SEEK_OP
} PREV_OP;

//handle-ul fisierului pe care dorim sa-l deschidem are o structura anume
//see README.md section Structura handle
struct _so_file {
#if defined(__linux__)
#elif defined(_WIN32)
    SO_HANDLE fHandle;
#else
#error "Unknown platform"
#endif
    SO_HANDLE fd;                       
	FILE_ACCESS access_type;
	PREV_OP prev;
	unsigned char buff[BUFF_SIZE];
	int buff_pos;
	int current_buffer_size;
	long current_pos;
	int is_error;
	int is_eof;
	int prev_read_bytes;
	int prev_written_bytes;
	int pid;
};
```

## Surse
### System calls
* https://en.wikipedia.org/wiki/System_call
* https://wiki.mta.ro/c/3/pso/lab/02
* https://0xax.gitbooks.io/linux-insides/content/SysCall/
* https://www.tutorialspoint.com/system-calls-in-unix-and-windows#:~:text=System%20calls%20in%20Windows%20are,system%20using%20the%20system%20calls.
* https://www.geeksforgeeks.org/input-output-system-calls-c-create-open-close-read-write/
* https://wiki.mta.ro/c/3/pso/lab/02

### LINUX 
* Structura
  * https://stackoverflow.com/questions/33536061/file-descriptors-and-file-handles-and-c#:~:text=File%20handle%20is%20a%20type,is%20the%20file%20descriptor)%3F
  * https://en.wikipedia.org/wiki/File_descriptor
  * [feof() flag](https://stackoverflow.com/questions/12337614/how-feof-works-in-c)
  * [ferror() flag](https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-ferror-test-read-write-errors)
  * https://github.com/rkibistu/Own-implementation-stdio.h

* so_fopen() -> open() syscall
  * https://0xax.gitbooks.io/linux-insides/content/SysCall/linux-syscall-5.html
  * https://github.com/torvalds/linux/blob/master/fs/open.c
  * https://www.thinkage.ca/gcos/expl/c/lib/open.html
  * https://github.com/TritonDataCenter/syslinux/blob/master/com32/lib/fopen.c

* so_fclose() -> close() syscall
  * http://codewiki.wikidot.com/c:system-calls:close
  * https://scholarsoul.com/fclose-in-c/

* so_fileno()
  * fileno - map a stream pointer to a file descriptor
  * https://pubs.opengroup.org/onlinepubs/009604599/functions/fileno.html

* so_fflush()
  * fflush() is typically used for output stream only. Its purpose is to clear (or flush) the output buffer and move the buffered data to console (in case of stdout) or disk (in case of file output stream)
  * https://www.geeksforgeeks.org/use-fflushstdin-c/
  * https://www.reddit.com/r/learnprogramming/comments/3br04y/c_can_someone_please_explain_fflushstdin_to_me/

* so_fwrite() -> write() syscall
  * https://chromium.googlesource.com/chromiumos/third_party/glibc/+/cvs/libc-970720/stdio/fwrite.c

### WINDOWS
  * https://www.cs.rpi.edu/academics/courses/fall01/os/
