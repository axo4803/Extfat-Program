#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

int main()
{
    int fd = open("test.image",O_RDWR);
    lseek(fd,3,SEEK_SET);
    write(fd,"A",1);
    close(fd);
    return 0;
}