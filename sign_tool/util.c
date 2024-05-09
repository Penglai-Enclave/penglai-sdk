#include "util.h"
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

void printHex(unsigned char *c, int n)
{
	int i;
	for (i = 0; i < n; i++) {
		printf("0x%02X, ", c[i]);
		if ((i%4) == 3)
		    printf(" ");

		if ((i%16) == 15)
		    printf("\n");
	}
	if ((i%16) != 0)
		printf("\n");
}

int get_file_size(const char *filename)
{
    struct stat st; 

    if (stat(filename, &st) == 0)
        return st.st_size;

    return -1; 
}

int read_file_to_buf(const char *filename, unsigned char *buffer, size_t bsize, long offset)
{
    FILE *fd = fopen(filename, "rb");
	if(fd == NULL){
		printf("open file failed\n");
		return -1;
	}
	fseek(fd, offset, 0);
	int count = fread(buffer, 1, bsize, fd);
    fclose(fd);
	if(count != bsize){
		printf("read byte number is wrong: num: %d\n", count);
		return -1;
	}
    return 0;
}

int write_data_to_file(const char *filename, const char *modes, unsigned char *buf, size_t bsize, long offset)
{
    FILE *fd = fopen(filename, modes);
	if(fd == NULL){
		printf("open file failed\n");
		return -1;
	}
	fseek(fd, offset, 0);
	int count = fwrite(buf, 1, bsize, fd);
    fclose(fd);
	if(count != bsize){
		printf("write byte number is wrong: num: %d\n", count);
		return -1;
	}
    return 0;
}

int copy_file(const char *source_path, const char *dest_path)
{
    int fd_to, fd_from;
    char buf[4096];
    ssize_t nread;
    int saved_errno;

    fd_from = open(source_path, O_RDONLY);
    if (fd_from < 0)
        return -1;

    fd_to = open(dest_path, O_WRONLY | O_CREAT | O_EXCL, 0666);
    if (fd_to < 0)
        goto out_error;

    while (nread = read(fd_from, buf, sizeof buf), nread > 0)
    {
        char *out_ptr = buf;
        ssize_t nwritten;

        do {
            nwritten = write(fd_to, out_ptr, nread);

            if (nwritten >= 0)
            {
                nread -= nwritten;
                out_ptr += nwritten;
            }
            else if (errno != EINTR)
            {
                goto out_error;
            }
        } while (nread > 0);
    }

    if (nread == 0)
    {
        if (close(fd_to) < 0)
        {
            fd_to = -1;
            goto out_error;
        }
        close(fd_from);

        /* Success! */
        return 0;
    }

  out_error:
    saved_errno = errno;

    close(fd_from);
    if (fd_to >= 0)
        close(fd_to);

    errno = saved_errno;
    return -1;
}