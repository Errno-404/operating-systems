#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <unistd.h>

#include <sys/times.h>
#include <dlfcn.h>

typedef struct WcBlock WcBlock;

static void pr_times(clock_t real, struct tms *tmsstart, struct tms *tmsend);

int main(int argc, char *argv[])
{

    void *handle = dlopen("libwclib.so", RTLD_LAZY);
    if (!handle)
    {
        printf("There is no such library\n");
        return 1;
    }

    WcBlock *(*init_blocks)(size_t);
    init_blocks = dlsym(handle, "init_blocks");

    void (*read_wc_to_block)(WcBlock *, char *);
    read_wc_to_block = dlsym(handle, "read_wc_to_block");

    char *(*get_block)(WcBlock *, int);
    get_block = dlsym(handle, "get_block");

    void (*delete_block)(WcBlock *, int);
    delete_block = dlsym(handle, "delete_block");

    void (*free_blocks)(WcBlock *);
    free_blocks = dlsym(handle, "free_blocks");

    char input[1024] = "";
    WcBlock *block = NULL;
    int init_flag = 0;

    struct tms tmsstart, tmsend;
    clock_t start, end;

    while(1){
        fputs("> ", stdout);
        if(fgets(input, 1024 ,stdin) == NULL){
            fputs("Error", stdout);
            return 1;
        }
        fputs(input, stdout);
        

        // pozbywamy się nowej linii
        input[strcspn(input, "\n")] = 0;

        start = times(&tmsstart);

        if(strncmp(input, "init", 4) == 0){
            if(init_flag != 0){
                fputs("Already initialized!\n", stdout);
                continue;
            }

            char * size_c = input + 5;

            int length = strlen(size_c);
            for(int i = 0; i < length; i++){
                if(isdigit(size_c[i]) == 0){
                    fputs("Given size is not a number!\n", stdout);
                }
            }

            int size_i = atoi(size_c);

            //time measurement

            block = init_blocks(size_i);

            if(block != NULL) init_flag = 1;

        }
        else if(strncmp(input, "count", 5) == 0){
            char * file_name = input + 6;

            read_wc_to_block(block, file_name);

        }
        else if(strncmp(input, "show", 4) == 0){
            char * index_c = input + 5;

            // ulepszyć np o flagę i potem na jej podstawie zadecydować o contiune
            int length = strlen(index_c);
            for(int i = 0; i < length; i++){
                if(isdigit(index_c[i]) == 0){
                    fputs("Given index is not a number!\n", stdout);

                }
            }

            int index_i = atoi(index_c);

            char * ret = get_block(block, index_i);

            if(ret != NULL){
                fputs(ret, stdout);
            }

        }
        else if(strncmp(input, "delete", 6) == 0){
            char * index_c = input + 7;

            // ulepszyć np o flagę i potem na jej podstawie zadecydować o contiune
            int length = strlen(index_c);
            for(int i = 0; i < length; i++){
                if(isdigit(index_c[i]) == 0){
                    fputs("Given index is not a number!\n", stdout);

                }
            }

            int index_i = atoi(index_c);

            delete_block(block, index_i);

        }
        else if(strncmp(input, "destroy", 7) == 0){
            if(init_flag != 0){

                free_blocks(block);

                block = NULL;
                init_flag = 0;
            }
            else{
                fputs("There is no block initialized!\n", stdout);
            }

        }
        else if(strncmp(input, "exit", 4) == 0){
            return 0;
        }
        else{
            fputs("Unknown command!\n", stdout);
        }

        end = times(&tmsend);

        pr_times(end - start, &tmsstart, &tmsend);

    }

    dlclose(handle);
}

static void pr_times(clock_t real, struct tms *tmsstart, struct tms *tmsend)
{
    static long clktck = 0;
    if (clktck == 0)
    {
        clktck = sysconf(_SC_CLK_TCK);
    }

    fprintf(stdout, "==================\n");
    fprintf(stdout, " real: %10.8f\n", real / (double) clktck);
    fprintf(stdout, " user: %10.8f\n", (tmsend->tms_utime - tmsstart->tms_utime)/ (double) clktck);
    fprintf(stdout, " sys: %11.8f\n", (tmsend->tms_stime - tmsstart->tms_stime) / (double) clktck);
    fprintf(stdout, "==================\n");
}