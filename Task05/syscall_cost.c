#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/syscall.h>
static double now(void){ struct timespec t; clock_gettime(CLOCK_MONOTONIC,&t); return t.tv_sec+t.tv_nsec/1e9; }
__attribute__((noinline)) int plain(int x){ __asm__ volatile(""); return x+1; }
int main(void){
    const long N = 5000000; volatile long s = 0; double t0;
    t0 = now(); for(long i=0;i<N;i++) s += plain((int)i);
    double f = (now()-t0)/N*1e9;
    t0 = now(); for(long i=0;i<N;i++) s += syscall(SYS_getppid);
    double c = (now()-t0)/N*1e9;
    printf("ordinary function call : %7.1f ns\nsystem call (getppid)  : %7.1f ns\nratio                  : %7.1f x\n", f, c, c/f);
    return 0; }
