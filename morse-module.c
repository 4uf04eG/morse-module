#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/kmod.h>
#include <linux/delay.h>
#include <asm/uaccess.h>


MODULE_AUTHOR("Ilya Kurin, 4uf04eg@gmail.com");
MODULE_DESCRIPTION("Beeps message using Morse code");
MODULE_LICENSE("GPL");

#define DOT dot();
#define DASH dash();
#define INTRA_CHARACTER_SPACE msleep(50);
#define INTER_CHARACTER_SPACE msleep(150);
#define WORD_SPACE msleep(350);

const char* MORSE_TABLE[36] = {
    "01",   //A
    "1000", //B  
    "1010", //C
    "100",  //D
    "0",    //E
    "0010", //F
    "110",  //G
    "0000", //H
    "00",   //I
    "0111", //J
    "101",  //K
    "0100", //L
    "11",   //M
    "10",   //N
    "111",  //O
    "0110", //P
    "1101", //Q
    "010",  //R
    "000",  //S
    "1",    //T
    "001",  //U
    "0001", //V
    "011",  //W
    "1001", //X
    "1011", //Y
    "1100", //Z
    "11111",//0
    "01111",//1
    "00111",//2
    "00011",//3
    "00001",//4
    "00000",//5
    "10000",//6
    "11000",//7
    "11100",//8
    "11110",//9
};

static struct proc_dir_entry *entry;
static enum sound_type { INTERNAL, EXTERNAL } type;

static void dot(void) {
    char * envp[] = { "HOME=/", NULL };

    if (type == EXTERNAL) {
        char * argv[] = {
            "/usr/local/bin/beep-morse", "--dot", NULL
        };
        call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);
    }
    else {
        char * argv[] = { 
            "/usr/bin/env", 
            "-i", "/usr/bin/beep",
            "-f","800","-l","50", NULL
        };
        call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);
    }
}

static void dash(void) {
    char * envp[] = {"HOME=/", NULL };

    if (type == EXTERNAL) {
        char * argv[] = {
            "/usr/local/bin/beep-morse", "--dash", NULL
        };
        call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);   
    }
    else {
        char * argv[] = { 
            "/usr/bin/env", 
            "-i", "/usr/bin/beep",
            "-f","800","-l","150", NULL
        };
        call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);
    }
}

static void make_sound(const char* morse_code) {
    int i;

    for (i = 0; i < strlen(morse_code); i++) {
        if (morse_code[i] == '1') DASH;
        if (morse_code[i] == '0') DOT;

        INTRA_CHARACTER_SPACE;
    } 
}

static void letter_to_code(char letter) {
    int lowerPos = letter - 'a';
    int upperPos = letter - 'A';
    int digitPos = 26 + letter - '0';
    int size = sizeof(MORSE_TABLE) / sizeof(MORSE_TABLE[0]);

    if (lowerPos >= 0 && lowerPos < size)
        make_sound(MORSE_TABLE[lowerPos]);
    else if (upperPos >= 0 && upperPos < size)
        make_sound(MORSE_TABLE[upperPos]);
    else if (digitPos >= 0 && digitPos < size)
        make_sound(MORSE_TABLE[digitPos]);

    INTER_CHARACTER_SPACE;
}

static void proceed_message(char* message, int start_point) {
    int i = start_point; 

    for (; i < strlen(message); i++){
        if (message[i] == ' ') {
            WORD_SPACE;
            continue;
        }

        letter_to_code(message[i]);
    }  
}

//Returns start of real message
static int proceed_params(char* message) {
    if (strncmp(message, "--external ", 11) == 0) {
        type = EXTERNAL;  
        return 11;
    }

    return 0;
}

static ssize_t handle_write(
    struct file *file, 
    const char __user *ubuf,
    size_t count, 
    loff_t *ppos) {
    char procfs_buffer[1024];

    if (raw_copy_from_user(procfs_buffer, ubuf, count))
	return -EFAULT;

    proceed_message(procfs_buffer, proceed_params(procfs_buffer));
	
    return count;
}
 
static struct file_operations operations = {
    .owner = THIS_MODULE,
    .write = handle_write,
};

static int __init morse_init(void) {
    entry = proc_create("morse", 0222, NULL, &operations);
    return 0;
}

static void __exit morse_exit(void) {
    proc_remove(entry);
}

module_init(morse_init);
module_exit(morse_exit);
