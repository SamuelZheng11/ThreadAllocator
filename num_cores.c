#include "dispatchQueue.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sysinfo.h>

int main(int argc, char** argv) {
    // create a concurrent dispatch queue
    printf("This machine has %i cores.\n", get_nprocs_conf());
    return 0;
}