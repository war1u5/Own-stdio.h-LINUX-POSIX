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

## Comenzi pentru rulare
```bash 
make clean
nake main
export LD_LIBRARY_PATH=.:D_LIBRARY_PATH
./main
```

## Rezolvare
### Elemente necesare:
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

### LINUX 
* Structura handle
  * https://stackoverflow.com/questions/33536061/file-descriptors-and-file-handles-and-c#:~:text=File%20handle%20is%20a%20type,is%20the%20file%20descriptor)%3F
  * https://en.wikipedia.org/wiki/File_descriptor
  * [feof() flag](https://stackoverflow.com/questions/12337614/how-feof-works-in-c)
  * [ferror() flag](https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-ferror-test-read-write-errors)
  * 

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
  * 

* so_fwrite()
  * https://chromium.googlesource.com/chromiumos/third_party/glibc/+/cvs/libc-970720/stdio/fwrite.c
  * 

### WINDOWS
  * https://www.cs.rpi.edu/academics/courses/fall01/os/