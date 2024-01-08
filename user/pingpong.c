#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
    int parent2child[2];
    int child2parent[2];

    pipe(parent2child);
    pipe(child2parent);
    int pid = fork();
    if (pid == 0) {
        char buffer;
        close(parent2child[1]);
        read(parent2child[0], &buffer, 1);
        printf("%d: received ping\n", getpid());
        close(child2parent[0]);
        write(child2parent[1], &buffer, 1);
        close(child2parent[1]);
        exit(0);

    } else {
        char buffer = 'x';
        close(parent2child[0]);
        write(parent2child[1], &buffer, 1);
        close(child2parent[1]);
        read(child2parent[0], &buffer, 1);
        printf("%d: received pong\n", getpid());
        close(child2parent[0]);
        
        exit(0);
    }

}