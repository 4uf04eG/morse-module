#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <unistd.h>

static void print_help(int status) {
    printf("Usage:");
    printf("\n\tbeep-morse [--external] <MESSAGE>");
    printf("\n\tbeep-morse [--dot --dash]");
    printf("\n\tbeep-morse [--help]");
    printf("\nOptions:");
    printf("\n\t--external");
    printf("\n\t\tmake sound through external speakers\n");
    printf("\n\t--dot");
    printf("\n\t\tmake dot sound through external speakers\n");
    printf("\n\t--dash");
    printf("\n\t\tmake dash sound through external speakers\n");
    printf("\n\t--help");
    printf("\n\t\tshow this message\n");
    exit(status);
}

void play_dot_external(void) {
    system("ffplay -f lavfi -i 'sine=frequency=800:duration=0.05' -autoexit -nodisp >/dev/null 2>&1");
}

void play_dash_external(void) {
    system("ffplay -f lavfi -i 'sine=frequency=800:duration=0.15' -autoexit -nodisp >/dev/null 2>&1");
}

void write_to_proc(int external_status, char* message) {
    int size = strlen(message);
    char write_msg[1024] = "";   

    if (external_status == 1) 
        strcpy(write_msg, "--external ");
    
    strcat(write_msg, message);
    
    if (access("/proc/morse", W_OK) != -1) {
    	FILE* file  = fopen("/proc/morse",  "w");
        fwrite(write_msg, sizeof(char), sizeof(write_msg), file);
    } else {
        printf("Please load morse-module\n");
        exit(-1);
    }
}

int main(int argc, char **argv) {
    if (argc == 1)
        print_help(-1);

    int external_status = 0;
    char* message = (char*)malloc(1024 * sizeof(char));

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--dash") == 0)
            play_dash_external();
        else if (strcmp(argv[i], "--dot") == 0)
            play_dot_external();
        else if (strcmp(argv[i], "--external") == 0)
            external_status = 1;
        else if (strcmp(argv[i], "--help") == 0)
            print_help(0);
        else
            sprintf(message, "%s %s", message, argv[i]);
    }

    if (message != NULL)
        write_to_proc(external_status, message);
    else if (external_status == 1)
        print_help(-1);
}
