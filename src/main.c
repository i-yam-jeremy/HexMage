#include <stdio.h>

#include <sys/time.h>

#include "hexmage.h"

int main(int argc, char** argv) {
    struct timeval start, end;
    gettimeofday(&start, 0);
    
    int result = HexMage_compile("test_files/test01.hm");
    
    gettimeofday(&end, 0);
    long long elapsed_ms = ((end.tv_sec-start.tv_sec)*1000000LL + end.tv_usec-start.tv_usec)/1000;
    printf("Compile time: %lld ms\n", elapsed_ms);
    
    return result;
}