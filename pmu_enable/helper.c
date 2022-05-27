#include <stdio.h>

int main(){

    int flag = 1;

    while (flag) {
        // do nothing
    }

    printf("enable PMU successfully!\n");
    
    int temp;

    asm("mrs %0, PMCCNTR_EL0 \n\t":"=r"(temp));

    printf("%d\n", temp);

    while (flag)
    {
        /* code */
    }
    

    return 0;
}