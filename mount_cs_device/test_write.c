#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int f = open("/dev/cs_device",O_RDWR);
    int val = 65;
    char* c = (char*)(&val);
    write(f,c,4);
    val = 66;
    write(f,c,4);
    val = 67;
    write(f,c,4);
    val = 67;
    write(f,c,4);
    val = 0xffffffff;
    write(f,c,4);
    val = 68;
    write(f,c,4);
    close(f);
    return 0;
}