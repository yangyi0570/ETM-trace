#include <iostream>
#include <cstdio>
using namespace std;

int main()
{
    FILE *p_file = fopen("trace.dat", "w+");
    if(p_file == NULL)
    {
        printf("This file is not exited");
    }

    uint32_t val;
    while(cin >> val)
    {
        cout<< val;
        fwrite(&val, sizeof(val), 1, p_file);
    }

    fclose(p_file);
    p_file = NULL;
}
