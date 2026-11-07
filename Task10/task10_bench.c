#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <spawn.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/resource.h>

extern char **environ;
#define FILE_SIZE (4L * 1024 * 1024)

static double now(void){ struct timespec t; clock_gettime(CLOCK_MONOTONIC,&t); return t.tv_sec+t.tv_nsec/1e9; }
static double tv(struct timeval t){ return t.tv_sec+t.tv_usec/1e6; }

typedef struct { double wall,user,sys; } T;
static struct rusage r0; static double w0;
static void start(void){ getrusage(RUSAGE_SELF,&r0); w0=now(); }
static T stop(void){ struct rusage r1; getrusage(RUSAGE_SELF,&r1);
    T t={now()-w0, tv(r1.ru_utime)-tv(r0.ru_utime), tv(r1.ru_stime)-tv(r0.ru_stime)}; return t; }

static void make_file(void){
    FILE *f=fopen("big.dat","w"); for(long i=0;i<FILE_SIZE;i++) fputc('A'+i%26,f); fclose(f); }

static long copy_rw(size_t b){
    char *buf=malloc(b); int in=open("big.dat",O_RDONLY);
    int out=open("out.dat",O_WRONLY|O_CREAT|O_TRUNC,0644);
    long calls=0; ssize_t n;
    while((n=read(in,buf,b))>0){ calls++; if(write(out,buf,n)!=n) break; }
    calls++; close(in); close(out); free(buf); return calls; }

static void copy_stdio(void){
    FILE *in=fopen("big.dat","rb"),*out=fopen("out.dat","wb"); char c[1];
    while(fread(c,1,1,in)==1) fwrite(c,1,1,out);   /* 1 byte per call, library buffers */
    fclose(in); fclose(out); }

static void copy_mmap(void){
    int in=open("big.dat",O_RDONLY); int out=open("out.dat",O_RDWR|O_CREAT|O_TRUNC,0644);
    ftruncate(out,FILE_SIZE);
    char *s=mmap(NULL,FILE_SIZE,PROT_READ,MAP_PRIVATE,in,0);
    char *d=mmap(NULL,FILE_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,out,0);
    memcpy(d,s,FILE_SIZE); msync(d,FILE_SIZE,MS_SYNC);
    munmap(s,FILE_SIZE); munmap(d,FILE_SIZE); close(in); close(out); }

static void *thr(void *a){ (void)a; return NULL; }

int main(void){
    T t; make_file();
    printf("== Copy of %ld bytes with read()/write() ==\n",FILE_SIZE);
    printf("%-10s %-12s %8s %8s %8s\n","buffer","read() calls","wall(s)","user(s)","sys(s)");
    size_t sz[]={1,16,64,512,4096,32768,65536,1048576};
    for(int i=0;i<8;i++){ start(); long c=copy_rw(sz[i]); t=stop();
        printf("%-10zu %-12ld %8.3f %8.3f %8.3f\n",sz[i],c,t.wall,t.user,t.sys); }
    printf("\n== Other methods (same file) ==\n");
    start(); copy_stdio(); t=stop();
    printf("fread/fwrite 1 byte at a time : wall %.3f user %.3f sys %.3f\n",t.wall,t.user,t.sys);
    start(); copy_mmap(); t=stop();
    printf("mmap + memcpy                 : wall %.3f user %.3f sys %.3f\n",t.wall,t.user,t.sys);

    printf("\n== Creating 1000 execution units ==\n");
    start();
    for(int i=0;i<1000;i++){ pid_t p=fork(); if(p==0) _exit(0); waitpid(p,NULL,0); }
    t=stop(); printf("fork()+wait x1000        : wall %.3f user %.3f sys %.3f\n",t.wall,t.user,t.sys);
    start();
    for(int i=0;i<1000;i++){ pthread_t th; pthread_create(&th,NULL,thr,NULL); pthread_join(th,NULL); }
    t=stop(); printf("pthread_create+join x1000: wall %.3f user %.3f sys %.3f\n",t.wall,t.user,t.sys);
    start();
    for(int i=0;i<1000;i++){ pid_t p; char *argv[]={"true",NULL}; posix_spawn(&p,"/bin/true",NULL,NULL,argv,environ); waitpid(p,NULL,0); }
    t=stop(); printf("posix_spawn /bin/true x1000: wall %.3f user %.3f sys %.3f\n",t.wall,t.user,t.sys);
    start();
    for(int i=0;i<1000;i++){ pid_t p=fork(); if(p==0){ execlp("true","true",(char*)NULL); _exit(127);} waitpid(p,NULL,0); }
    t=stop(); printf("fork()+exec /bin/true x1000: wall %.3f user %.3f sys %.3f\n",t.wall,t.user,t.sys);
    unlink("big.dat"); unlink("out.dat"); return 0; }
