#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"
#include "kernel/fs.h"

int main(int argc, char *argv[]) {
    char buffer[512];
    char* args[MAXARG];
    for(int i = 1; i < argc; i++) {
        args[i - 1] = argv[i];
        // printf("argv[%d]: %s\n", i, argv[i]);
    }
    //We need one space to store the one line command and one space to store '0'. 
    //However, the 'xargs' doesn't put into the list so we only need one space
    if (argc + 1 > 32) {
        printf("too many arguments");
        exit(1);
    }
    //Get the xargs' arguments firstly
    int bytes_read;

    int start = 0;

    //read from the pipe
    while((bytes_read = read(0, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        
        int len = strlen(buffer);
        while(start < len) {
            
            int i = start;
            // printf("i is %d\n", i);

            while(buffer[i] != '\n') {
                i++;
            }
            buffer[i] = '\0';
            
            ++i;
            int copy_len = i - start;
            char * cmd = malloc(copy_len * sizeof(char));
            memcpy(cmd, &buffer[start], copy_len * sizeof(char));
            start = i;
            
            // printf("start is %d\n", start);

            // printf("buffer is %s\n", buffer);
            // printf("cmd is %s\n", cmd);
            //append the commands after the xargs one
            args[argc - 1] = cmd;
            // exec() needs a '0' at the end. Otherwise, it doesn't know where to end
            args[argc] = 0;

            int pid = fork();
            if (pid == 0) { 
                exec(args[0], args);
                free(cmd);
                exit(0);
            } else {
                wait(0);
            }
        }
    }
    exit(0);
}

