#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
//#include "ptm2human/ptm2human.h"

int main(){
    int fd = open("/dev/cs_trace_buffer", O_RDONLY);
    char* buffer = (char*)malloc(64*1024*sizeof(char));
    while(1){
        memset(buffer, 0, 64 * 1024);
        int bytes_read = read(fd, buffer, 64 * 1024);      // 第三个参数为buffer最多能读的字节数
        printf("Detecter: %d bytes are read.\n", bytes_read);
        // parse trace data
        //ptm2human(buffer, bytes_read);
        // detect attack in parsed data
        sleep(1);
    }
    
    return 0;
}