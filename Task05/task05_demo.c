#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

/* Tiny "patient record" program: write a record, read it back with the
   raw system calls, then do the same with the C library (fopen/fread).
   Run:  strace -o raw.txt ./task05_demo   and read raw.txt */
int main(void)
{
    const char *rec = "patient=1042;hr=72;spo2=98\n";
    char buf[64];

    int fd = open("record.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644); /* file mgmt  */
    write(fd, rec, strlen(rec));                                      /* file mgmt  */
    close(fd);

    fd = open("record.txt", O_RDONLY);
    ssize_t n = read(fd, buf, sizeof buf - 1);                        /* read()     */
    close(fd);
    buf[n] = '\0';
    printf("raw read() got %zd bytes: %s", n, buf);

    FILE *f = fopen("record.txt", "r");                               /* library    */
    size_t m = fread(buf, 1, sizeof buf - 1, f);
    fclose(f);
    printf("fread() got %zu bytes\n", m);

    printf("pid=%d\n", (int)getpid());                                /* info maint */
    unlink("record.txt");
    return 0;
}
