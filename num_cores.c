#include "dispatchQueue.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sysinfo.h>

int getNumberOfCores(){
    return get_nprocs_conf();
}

int main(int argc, char** argv) {
    // create a concurrent dispatch queue
    int item = getNumberOfCores();
    printf("%i\n", item);
    return 0;
}