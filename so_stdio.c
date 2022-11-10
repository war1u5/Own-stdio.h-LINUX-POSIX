#define _CRT_SECURE_NO_WARNINGS
#include "so_stdio.h"
#include "so_tools.h"
#define _DEBUG_

ssize_t xread(int fd, void *buf, size_t count)
{
	size_t bytes_read = 0;
	ssize_t bytes_read_now;

	while (bytes_read < count) {
		bytes_read_now = read(fd, buf + bytes_read, count - bytes_read);

		if (bytes_read_now == 0){
            return bytes_read;
        }
		else if (bytes_read_now < 0){
            return SO_EOF;
        }
		bytes_read = bytes_read + bytes_read_now;
	}

	return bytes_read;
}

ssize_t xwrite(int fd, const void *buf, size_t count)
{
	size_t bytes_written = 0, bytes_written_now;

	while (bytes_written < count) {
		bytes_written_now = write(fd, buf + bytes_written, count - bytes_written);

		if (bytes_written_now <= 0){
            return SO_EOF;
        }
		bytes_written = bytes_written + bytes_written_now;
	}

	return bytes_written;
}

//functie pentru instantierea strcuturii doar atunci cand e nevoie de ea (memory friendly)
SO_FILE *get_struct(int my_fd, FILE_ACCESS acc, int pid)
{
	SO_FILE *file_descr = malloc(sizeof(SO_FILE));

	if (!file_descr){
        return NULL;
    }
		
	file_descr->fd = my_fd;
	file_descr->access_type = acc;
	file_descr->prev = DIFF_OP;
	memset(file_descr->buff, 0, BUFF_SIZE);
	file_descr->buff_pos = 0;
	file_descr->current_pos = 0;
	file_descr->current_buffer_size = 0;
	file_descr->is_error = 0;
	file_descr->is_eof = 0;
	file_descr->prev_read_bytes = 0;
	file_descr->prev_written_bytes = 0;
	file_descr->pid = pid;

	return file_descr;
}

SO_FILE *so_fopen(const char *pathname, const char *mode)
{
	int my_fd;

	if (!strncmp(mode, "r", 1) && strncmp(mode, "r+", 2)) {
		my_fd = open(pathname, O_RDONLY);

		if (my_fd < 0){
            return NULL;
        }
			
		return get_struct(my_fd, R, -1);
	} 
    else if (!strncmp(mode, "r+", 2)) {
		my_fd = open(pathname, O_RDWR);

		if (my_fd < 0){
            return NULL;
        }
			
		return get_struct(my_fd, R_PLUS, -1);
	} 
    else if (!strncmp(mode, "w", 1) && strncmp(mode, "w+", 2)) {
		my_fd = open(pathname, O_WRONLY | O_CREAT | O_TRUNC, 0644);

		if (my_fd < 0){
            return NULL;
        }
			
		return get_struct(my_fd, W, -1);
	} 
    else if (!strncmp(mode, "w+", 2)) {
		my_fd = open(pathname, O_RDWR | O_CREAT | O_TRUNC, 0644);
		if (my_fd < 0){
            return NULL;
        }
			
		return get_struct(my_fd, W_PLUS, -1);
	} 
    else if (!strncmp(mode, "a", 1) && strncmp(mode, "a+", 2)) {
		my_fd = open(pathname, O_APPEND | O_CREAT | O_WRONLY, 0644);
			if (my_fd < 0){
                return NULL;
            }
				
			return get_struct(my_fd, A, -1);
	} 
    else if (!strncmp(mode, "a+", 2)) {
		my_fd = open(pathname, O_APPEND | O_CREAT | O_RDWR, 0644);
		if (my_fd < 0){
            return NULL;
        }
			
		return get_struct(my_fd, A_PLUS, -1);
	}

	return NULL;
}

int so_fclose(SO_FILE *stream)
{
	if (so_fflush(stream)) {
		stream->is_error = 1;
		free(stream);
		return SO_EOF;
	}

	if (!close(stream->fd)) {
		free(stream);
		return 0;
	}

	stream->is_error = 1;
	free(stream);
	return SO_EOF;
}

int so_fflush(SO_FILE *stream)
{
	SO_HANDLE fd;
	ssize_t write_chr;

	fd = stream->fd;
	if (stream->prev == WRITE_OP) {
		write_chr = xwrite(fd, stream->buff,
			stream->current_buffer_size);
	if (write_chr == -1) {
		stream->is_error = 1;
		return SO_EOF;
	}
		stream->buff_pos = 0;
		stream->current_buffer_size = 0;
		memset(stream->buff, 0, BUFF_SIZE);
		stream->current_pos += write_chr;
	}
	return 0;
}

int so_fseek(SO_FILE *stream, long offset, int whence)
{
	SO_HANDLE fd;
	__off_t off; //signed 32-bit integer

	if (stream->prev == READ_OP) {
		memset(stream->buff, 0, BUFF_SIZE);
		stream->buff_pos = 0;
		stream->current_buffer_size = 0;
	} 
    else if (stream->prev == WRITE_OP) {
		so_fflush(stream);
		if (stream->is_error){
            return SO_EOF;
        }
	}

	fd = stream->fd;
	off = lseek(fd, offset, whence);
	if (off < 0) {
		stream->is_error = 1;
		return SO_EOF;
	}

	if (whence == SEEK_SET){
        stream->current_pos = off;
    }
	else if (whence == SEEK_CUR){
        stream->current_pos = stream->current_pos + offset;
    }	
	else if (whence == SEEK_END){
        stream->current_pos = off;
    }
		
	stream->prev = SEEK_OP;
	return 0;
}

long so_ftell(SO_FILE *stream)
{
	return stream->current_pos;
}

int so_fileno(SO_FILE *stream)
{
	int fd = stream->fd;
    
	return fd;
}

int so_feof(SO_FILE *stream)
{
	if (stream->is_eof){
        return SO_EOF;
    }

	return 0;
}

int so_ferror(SO_FILE *stream)
{
	if (stream->is_error){
        return SO_EOF;
    }
		
	return 0;
}


int so_fgetc(SO_FILE *stream)
{
	int fd, read_chr;
	unsigned char cchr;

	if (stream->access_type == A ||
		stream->access_type == W)
		return SO_EOF;

	if (stream->prev == WRITE_OP)
		so_fflush(stream);

	fd = stream->fd;
	stream->prev = READ_OP;

	if (stream->buff_pos == stream->current_buffer_size) {
		read_chr = read(fd, stream->buff, BUFF_SIZE);
		if (read_chr == -1) {
			stream->is_error = 1;
			return SO_EOF;
		} 
		else if (read_chr == 0) {
			stream->is_eof = 1;
			stream->current_pos++;
			return SO_EOF;
		}
		stream->current_buffer_size = read_chr;
		stream->buff_pos = 0;
	}

	cchr = stream->buff[stream->buff_pos];
	stream->buff_pos++;
	stream->current_pos++;

	return (int)cchr;
}

int so_fputc(int c, SO_FILE *stream)
{
	SO_HANDLE fd;
	int write_chr;

	if (stream->prev == READ_OP) {
		stream->current_pos +=
			(stream->current_buffer_size - stream->buff_pos + 1);
		stream->buff_pos = 0;
		stream->current_buffer_size = 0;
		memset(stream->buff, 0, BUFF_SIZE);
	}

	if (stream->access_type == R)
		return SO_EOF;

	fd = stream->fd;
	stream->prev = WRITE_OP;

	if (stream->buff_pos == BUFF_SIZE) {
		write_chr = xwrite(fd, stream->buff, stream->buff_pos);
		if (write_chr == -1) {
			stream->is_error = 1;
			return SO_EOF;
		} 
		else if (write_chr == 0) {
			stream->is_eof = 1;
			stream->current_pos++;
			return SO_EOF;
		}
		stream->current_buffer_size = 0;
		stream->buff_pos = 0;
		memset(stream->buff, 0, BUFF_SIZE);
	}

	stream->buff[stream->buff_pos] = (unsigned char)c;
	stream->buff_pos++;
	stream->current_buffer_size++;
	stream->current_pos++;
	return c;
}

size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
	int i, read_chr, total_bytes = size * nmemb;
	unsigned char *final;

	//verificam daca op prev a fost cu update mode
	if (stream->prev == WRITE_OP && !stream->is_error && (stream->access_type == R_PLUS || stream->access_type == W_PLUS ||stream->access_type == A_PLUS)) {
		so_fflush(stream);
	}

	final = malloc((total_bytes + 2) * sizeof(unsigned char));
	if (!final) {
		stream->is_error = 1;
		return SO_EOF;
	}

	memset(final, '\0', total_bytes + 2);
	stream->prev = READ_OP;

    //citim char by char pana ajungem la eof sau pana ajungem la nr de char necesare
	i = 0;
	while (i < total_bytes) {
		read_chr = so_fgetc(stream);

		if (stream->is_error) {
			memcpy(ptr, final, i);
			free(final);
			return 0;
		} 
        else if (stream->is_eof) {
			memcpy(ptr, final, i);
			free(final);
			stream->prev_read_bytes = i / size;
			return (i / size);
		}
			final[i] = (unsigned char)read_chr;
			i++;
	}

	memcpy(ptr, final, i);
	free(final);
	stream->prev_read_bytes = nmemb;

	return nmemb;
}

size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
	int i, total_bytes = size * nmemb;
	unsigned char *src;

	if (stream->prev == READ_OP) {
		stream->current_pos = stream->current_pos + (stream->current_buffer_size - stream->buff_pos + 1);
		stream->buff_pos = 0;
		stream->current_buffer_size = 0;
		memset(stream->buff, 0, BUFF_SIZE);
	}

	src = malloc((total_bytes + 2) * sizeof(unsigned char));
	if (!src) {
		stream->is_error = 1;
		return SO_EOF;
	}

	memcpy(src, ptr, total_bytes);

	stream->prev = WRITE_OP;

     //scriem char by char pana ajungem la eof sau pana ajungem la nr de char necesare
	for (i = 0 ; i < total_bytes; ++i) {
		so_fputc((int)src[i], stream);
		if (stream->is_error) {
			free(src);
			return 0;
		} 
        else if (stream->is_eof) {
			free(src);
			stream->prev_written_bytes = i / size;
			return (i / size);
		}
	}

	free(src);
	stream->prev_written_bytes = nmemb;

	return nmemb;
}

//todo...
SO_FILE *so_popen(const char *command, const char *type)
{

	return NULL;
}

int so_pclose(SO_FILE *stream)
{
	return NULL;
}