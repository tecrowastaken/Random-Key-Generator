

// Copied from numpy
#include "mt19937.h"
/* IT WILL CAUSE PROBLEMS IN WINNT.h !!!*/
#undef N

// XXX: heap corruption in ending have to revert to C++ to fix...

// Cng.lib

#ifdef _WIN32
    #include <windows.h>
    #include <bcrypt.h>
    #include <basetsd.h>
   
    #include <winuser.h>
    #include <stdlib.h>
    #include <stdint.h>
    #include <stdio.h>
    #include <string.h>
    #include <time.h>
    #include <stdbool.h>
   
    #define OPTPARSE_IMPLEMENTATION
    #define OPTPARSE_API static
    #include "optparse.h"

#else 
    #include <stdlib.h>
    #include <stdint.h>
    #include <stdio.h>
    #include <string.h>
    #include <time.h>
    #include <stdbool.h>
    // #error "Windows is the only os supported at the moment"
    #include <sys/time.h>
    #include <optparse.h>
    // If were on linux or not windows...
    #include <getopt.h>
    // Subprocess is the easiest fallback since I haven't seen a 
    // good api for using a clipboard on linux...
    #include "subprocess.h"
    typedef struct subprocess_s subprocess_t;
#endif

#define NON_DEFAULTS(XX) \
    XX(LOWERCASE_CHARS) XX(UPPERCASE_CHARS) XX(NUMBERS) XX(PUNCTUATION)

const uint32_t FLAGS__DEFAULT = 0;
const uint32_t FLAGS__LOWERCASE_CHARS = 1 << 0;
const uint32_t FLAGS__UPPERCASE_CHARS = 1 << 1;
const uint32_t FLAGS__NUMBERS = 1 << 2;
const uint32_t FLAGS__PUNCTUATION = 1 << 3;

const char* RKG_POSSIBILITES[] = {
    /* 0 */
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789",
    /* 1 */
    "abcdefghijklmnopqrstuvwxyz",
    /* 2 */
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
    /* 3 */
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",
    /* 4 */
    "0123456789",
    /* 5 */
    "abcdefghijklmnopqrstuvwxyz0123456789",
    /* 6 */
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789",
    /* 7 */
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789",
    /* 8 */
    "!\"#$%&'()*+,-./:;<=>\?@[]^_`{|}~",
    /* 9 */
    "abcdefghijklmnopqrstuvwxyz!\"#$%&'()*+,-./:;<=>\?@[]^_`{|}~",
    /* 10 */
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ!\"#$%&'()*+,-./:;<=>\?@[]^_`{|}~",
    /* 11 */
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!\"#$%&'()*+,-./:;<=>\?@[]^_`{|}~",
    /* 12 */
    "0123456789!\"#$%&'()*+,-./:;<=>\?@[]^_`{|}~",
    /* 13 */
    "abcdefghijklmnopqrstuvwxyz0123456789!\"#$%&'()*+,-./:;<=>\?@[]^_`{|}~",
    /* 14 */
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!\"#$%&'()*+,-./:;<=>\?@[]^_`{|}~",
    /* 15 */
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!\"#$%&'()*+,-./:;<=>\?@[]^_`{|}~",
};


#define AUTHOR "Tecro"
#define VERSION "0.1.1"

#ifndef _WIN32
const char* xclip_subprocess_cmd[] = {"xclip", "-selection", "c"};
const char* xsel_subprocess_cmd[] = {"xsel", "-b", "-i"};

int subprocess_copy_clipboard(const char* cmd[], char* data, size_t size){
    subprocess_t process;
    if (subprocess_create(xclip_subprocess_cmd, 0, &process) < 0){
        return -1;
    }
    FILE* stdin_file = subprocess_stdin(&process);
    int code;
    fwrite(data, sizeof(char), size, stdin_file);
    if (subprocess_alive(&process)){
        if (subprocess_join(&process, &code) < 0){
            return -1;
        }
    };
    if (subprocess_destroy(&process) < 0){
        return -1;
    }
    return 0;
}
#endif /* _WIN32 */



int copy_to_clipboard(const char* data, size_t data_size){
    #ifdef _WIN32
    // Code Was Referenced from pyperclip's C Calls
        
        #define __RKG__WINDOWS_CHECK_FOR_ERROR(ASSERTION) \
            if (!ASSERTION){ \
                fprintf(stderr, "Error When Calling " #ASSERTION ": [%i]", GetLastError());\
                return -1;\
            }
        
        bool sucess = false;

        for (size_t i = 0; i < 10; i++){
            if (OpenClipboard(NULL)) {
                sucess = true;
                continue;
            }
            break;
        }
        if (!sucess){
            printf("Failed to open clipboard\n");
            return -1;
        }
        __RKG__WINDOWS_CHECK_FOR_ERROR(EmptyClipboard());
   
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, data_size + 1);
        char* handle = (char*)GlobalLock(hMem);
        memcpy(handle, data, data_size);
        handle[data_size + 1] = 0;
        GlobalUnlock(hMem);
        // puts("Success Opening Clipboard");
        __RKG__WINDOWS_CHECK_FOR_ERROR(SetClipboardData(CF_TEXT, handle));
        __RKG__WINDOWS_CHECK_FOR_ERROR(CloseClipboard());
        
        #undef __RKG__WINDOWS_CHECK_FOR_ERROR
        // GlobalFree(hMem);
        return 0;
    #else 
        puts("[WARNING!] There isn't good support for clipboard yet as there isn't good documentation for it, Good luck!\n");
        if (subprocess_copy_clipboard(xclip_subprocess_cmd, data, data_size) == 0){
            return 0;
        };
        // backup...
        if (subprocess_copy_clipboard(xsel_subprocess_cmd, data, data_size) == 0){
            return 0;
        }
        // FAILED!!
        return -1;
    #endif /* _WIN32 */
}




void cryptographic_mt_init(mt19937_state* state){
    #ifdef _WIN32
        uint32_t crypto[624];
        size_t crypto_size = sizeof(crypto);
        memset (crypto, 0, crypto_size);
        // printf("Bcrypt GenRandom\n");
        BCryptGenRandom(NULL, (unsigned char*)crypto, crypto_size, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
        // puts("DONE");
        mt19937_init_by_array(state, crypto, 624);
    #else 
        srand(time(NULL));
        uint32_t crypto[20];
        for (int i = 0; i < 20; i++)
            crypto[i] = (uint32_t)rand();
        mt19937_init_by_array(state, crypto, sizeof(crypto));
    #endif
}

typedef union {
    uint8_t u8[4];
    uint32_t u32;
} rng_state_u;

typedef struct state_s {
    rng_state_u state;
    int index;
} state_t;

void state_init(state_t* s, mt19937_state* mt){
    s->state.u32 = mt19937_next32(mt);
    s->index = 0;
}

uint8_t next_state(state_t* s, mt19937_state* mt){
    if (s->index > 3){
        state_init(s, mt);
    }
    uint8_t nstate = s->state.u8[s->index];
    s->index++;
    return nstate;
}

typedef struct crypto_s {
    mt19937_state mt;
    state_t rng_state;
    // inputbuffer
    const char* ib;
    size_t ib_size;
    // how big are the keys?
    size_t key_size;
} crypto_t;

void cryptographic_init(crypto_t *crypto, uint32_t flag, size_t key_size){
    cryptographic_mt_init(&crypto->mt);
    state_init(&crypto->rng_state, &crypto->mt);

    crypto->ib = RKG_POSSIBILITES[flag]; 
    crypto->ib_size = strlen(crypto->ib);
    crypto->key_size = key_size;
}

char cryptographic_next(crypto_t* crypto){
    return crypto->ib[next_state(&crypto->rng_state, &crypto->mt) % crypto->ib_size];
}



int cryptographic_write(crypto_t* crypto, FILE* f){
    for (size_t i = 0; i < crypto->key_size; i++)
        if (fprintf(f, "%c", cryptographic_next(crypto)) < 0) return -1;
    return 0;
}

void cryptographic_out(crypto_t* crypto, char** out){
    for (size_t i = 0; i < crypto->key_size; i++)
        (*out)[i] = cryptographic_next(crypto);
}




int main(int argc, char const *argv[])
{
    struct optparse_long longopts[] = {
        {"help", 'h', OPTPARSE_NONE},
        {"size", 's', OPTPARSE_REQUIRED},
        {"output", 'o', OPTPARSE_OPTIONAL},
        {"copy", 'c', OPTPARSE_NONE},
        {"lowercase", 'l', OPTPARSE_NONE},
        {"uppercase", 'u', OPTPARSE_NONE},
        {"numbers", 'n', OPTPARSE_NONE},
        {"punctuation", 'p', OPTPARSE_NONE},
        {"version", 'v', OPTPARSE_NONE},
        {"author", 'a', OPTPARSE_NONE},
        {0}
    };

    struct optparse options;
    (void)argc;
    optparse_init(&options, (char**)argv);
    
    int option;
    uint32_t flags = 0;
    size_t size = 40;

    FILE* fp = NULL;
    bool fp_is_dynamic = false;
    bool use_clipboard = false;
    bool output = false;
    
    while ((option = optparse_long(&options, longopts, NULL)) != -1) {
        switch (option){
            case 'h': {
                puts("rkg [options]\n");
                puts("generates a random key for encryptions or password generation");
                puts("--help / -h           Displays help msg");
                puts("--size / -s           set size of generated key [default: 40]");
                puts("--output / -o         Optional place to save newly generated key to [default: stdout]");
                puts("                      Examples include [rkg -o key1.txt key2.txt ...] you can make multiple keys");
                puts("--copy / -c           Copies key to clipboard if -o is not enabled");
                
                puts("Key Generation Options:\n");
                puts("--lowercase / -l      Generate a key with lowercase letters [a-z]");
                puts("--uppercase / -u      Generates a key with uppercase letters [A-Z]");
                puts("--numbers / -n      Generates a key with numbers [0-9]");
                puts("--punctuation / -p    Generates a key with any of these characters [ !\"#$%&\'()*+,-./:;<=>?@[]^_`{|}~ ]");
                puts("--verison / -v        Prints Version and exits");
                puts("--author / -a        Prints Author and exits");
                return 0;
            }
            case 's': {
                size = (size_t)atoi(options.optarg);
                if (size < 8){
                    printf("[Error] Do not make keys that are less than 8 characters it's bad practice\n");
                    return -1;
                }
                break;
            }
            case 'o': {
                output = true;
                break;
            }
            case 'c': {
                use_clipboard = true; 
                break;
            }

            case 'a': {
                puts(AUTHOR);
                return 0;
            }

            case 'v': {
                puts(VERSION);
                return 0;
            }

            /* ==== FLAGS ==== */
            case 'l' : {
                flags |= FLAGS__LOWERCASE_CHARS;
                break;
            }
            
            case 'u' : {
                flags |= FLAGS__UPPERCASE_CHARS;
                break;
            }
            
            case 'n' : {
                flags |= FLAGS__NUMBERS;
                break;
            }

            case 'p' : {
                flags |= FLAGS__PUNCTUATION;
                break; 
            }
            case '?':{
                fprintf(stderr, "%s: %s\n", argv[0], options.errmsg);
                exit(EXIT_FAILURE);
            }
        }
    }
    crypto_t crypto;
    cryptographic_init(&crypto, flags, size);
  

    if (output){
        // puts("Iterating...");
        char* arg;
        while (arg = optparse_arg(&options)){
            printf("%s\n", arg);
            FILE* fp = fopen(arg, "w+");
            if (fp == NULL){
                fprintf(stderr, "Couldn't open %s\n", arg);
                continue;
            }
            if (cryptographic_write(&crypto, fp) < 0){
                fprintf(stderr, "Couldn't write to %s\n", arg);
                fclose(fp);
                continue;
            };
            fclose(fp);
            fprintf(stdout, "New Key Generated To : %s\n", arg);
        }
    } else {
        char* data = (char*)calloc(sizeof(char), size + 1);
        memset(data, 0, size + 1);
        cryptographic_out(&crypto, &data);
        fprintf(stdout, "Key: %s\n", data);
        
        if (use_clipboard){
            if (copy_to_clipboard(data, size) < 0){
                free(data);
                return -1;
            }
            puts("copied key to clipboard");
        }
        free(data);
    }
    return 0;
}
