#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


void pipeline(int p[2]) {
    int prime;
    close(p[1]);
    if (read(p[0], &prime , sizeof(int)) == 0) {
        close(p[0]);
        return ;
    }
    printf("prime %d\n", prime);
    int newp[2];
    pipe(newp);
    int pid = fork();
    
    if (pid == 0) {    
        //close the read end for the child
        close(newp[0]);
        int others;
       
        while (read(p[0], &others , sizeof(int)) != 0) {
            if (others % prime != 0) {
                // printf("what's new : %d\n", others);
                write(newp[1], &others, sizeof(int));
            }
        }
        //close the read end for the last one
        close(p[0]);
        //close the write end for the child
        close(newp[1]);
        exit(0);
    } else {
        close(p[0]);
        wait(0);
        pipeline(newp);
        // close(p[0]);
        close(newp[1]);
    }

}     

int main() {
    int num = 2;
    int p[2];
    pipe(p);
    int pid = fork();
    if (pid == -1) {
        printf("pipe failed");
        exit(1);
    }
    if (pid == 0) {
        close(p[0]);
        while(num < 36) {
            write(p[1], &num, sizeof(int));
            num++;
        }
        close(p[1]);
        exit(0);
    } else {
        wait(0);
        pipeline(p);
        close(p[0]);
        close(p[1]);
        exit(0);
    }
}
