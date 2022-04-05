#include<stdio.h>
#include<unistd.h>

int main(int argc, char const *argv[])
{
    printf("Hello from run! ");
    printf("Pid from run is: %d\n", getpid());
    printf("\n");
    return 0;
}
