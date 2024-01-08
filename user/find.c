#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include <stdbool.h>
#include <stddef.h>

bool compare(char * target, char* filename){
    if (strcmp(target, filename) == 0) {
        return true;
    }
    return false;
}


void dealFiles(char* target, char* filename, char* dir) {
    // printf("files : %s\n", filename);
    if (compare(filename, target) == true) {
        printf("%s/%s\n", dir, filename);
    }
}

void findFiles(char *path, char* target) {
    // printf("find: dir: %s, the target is %s\n", path, target);
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
        printf("find: path too long\n");
        exit(1);
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';

    while(read(fd, &de, sizeof(de)) == sizeof(de)){
        // printf("enter de : %s\n", de.name);
        if(de.inum == 0 || compare(de.name, ".")|| compare(de.name, ".."))
            continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if(stat(buf, &st) < 0){
            printf("find: cannot stat %s\n", buf);
            continue;
        } 
        if (st.type == T_DIR)   {
            findFiles(buf, target);
        } else {
            dealFiles(target, de.name, path);
        }
    }
    close(fd);  
}


//find all the files in a directory tree with a specific name. 

//find all the files like 'ls'
//strcmp to cmp str
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: find <dir> <file>\n");
    }
    char* path = argv[1];
    char* filename = argv[2];
    findFiles(path, filename);
    exit(0);
}

