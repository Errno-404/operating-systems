#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>


typedef struct WcBlock WcBlock;

struct WcBlock
{
    char ** blocks;
    size_t max_block_count;
    size_t current_block_count;
};



WcBlock *init_blocks(size_t max_block_count){
    if(max_block_count <= 0){
        printf("Cannot initialize block of size of 0 or less!");
        return NULL;
    }

    WcBlock *block = malloc(sizeof(*block));
    if(block == NULL){
        printf("Malloc error!");
        return NULL;
    }


    block->max_block_count = max_block_count;
    block->current_block_count = 0;

    block->blocks = (char **) calloc(max_block_count, sizeof(char *));

    if(block->blocks == NULL){
        printf("Calloc error!");
        free(block);
        return NULL;
    }

    return block;
}

void read_wc_to_block(WcBlock *blocks, char * path){

    // incorrect arguments:
    if(blocks == NULL){
        printf("You have to initialize blocks first!\n");
        return;
    }
    else if(blocks->current_block_count >= blocks->max_block_count){
        printf("Reached maximum block count!\n");
        return;
    }
    else if(access(path, F_OK) != 0){
        printf("%s does not exist!\n", path);
        return;
    }
    

    // command building
   
    char * wc = "wc ";
    char * flow = " > ";
    char temp_filename[] = "/tmp/wcStats.XXXXXX";

    size_t commandLength = strlen(wc) + strlen(path) + strlen(flow) +strlen(temp_filename);

    char * command = (char *) malloc(sizeof(char) * (commandLength + 1));
    if(command == NULL){
        printf("Malloc failure!\n");
        return;
    }


    // handling tempfile
    int fd = mkstemp(temp_filename);
    
    strcpy(command, wc);
    strcat(command, path);
    strcat(command, flow);
    strcat(command, temp_filename);

    
    int ret = system(command);
    free(command);
    if(ret != 0){
        printf("No such file or directory!\n");
        return;
    }


    off_t size_of_tempfile = lseek(fd, 0, SEEK_END);

    int last_block = blocks->current_block_count;
    blocks->blocks[last_block] = calloc(size_of_tempfile + sizeof(char), sizeof(char));
    

    lseek(fd, 0, SEEK_SET);
    if(read(fd, blocks->blocks[last_block], size_of_tempfile) == -1){
        printf("Error while reading file\n");
        return;
    }


    unlink(temp_filename);
    
    blocks->current_block_count ++;
    close(fd); // tak dla pewności
}

char * get_block(WcBlock *block, int index){
    if(block == NULL){
        printf("Block is NULL!\n");
        return NULL;
    }
    else if(index >= block->current_block_count){
        printf("Given index is too big!\n");
        return NULL;
    }
    else{
        return block->blocks[index];
    }


}


void delete_block(WcBlock *block, int index){
    if(block == NULL){
        printf("block was not malloced!\n");
        return;
    }
    else if(index > block->current_block_count){
        printf("index out of range!\n");
        return;
    }
    

    memmove((block->blocks + index), (block->blocks + index + 1), sizeof(char *) * (block->current_block_count - index - 1));
    
    block->blocks[--(block->current_block_count)] = NULL;

}

void free_blocks(WcBlock *block){
    if(block == NULL){
        printf("block is null\n");
        return;
    }

    for(int i = 0; i< block->current_block_count; i++){
        free(block->blocks[i]);
    }
    free(block);
}

