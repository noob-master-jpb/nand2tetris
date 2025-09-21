#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define LINE_SIZE 300
#define MAX_LINES 50000      
#define MAX_SYMBOLS 512

int next_var_addr = 16;

struct table {
    char id[MAX_SYMBOLS][LINE_SIZE];
    int  value[MAX_SYMBOLS];
    int  count;
};
struct jump_table{
    char id[8][4];
    char value[8][3];
};

struct dest_table{
    char id[8][4];
    char value[8][3];
};

struct comp_table{
    char id[36][3];
    char value[36][7];
};

struct table symbol_table = {
    .id = {"R0","R1","R2","R3","R4","R5","R6","R7","R8","R9","R10","R11","R12","R13","R14","R15",
        "SCREEN","KBD",
        "SP","LCL","ARG","THIS","THAT",},
    .value = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
        16384,24576,
        0,1,2,3,4},
    .count = 23
};


int find_sym(char id[]){
    int c = 0;
    while(c<symbol_table.count){
        if(strcmp(symbol_table.id[c],id) == 0){
            return symbol_table.value[c];
        }
        c++;
    }
    return -1;
}

void add_sym(char id[],int value){
    int curr = symbol_table.count;
    strcpy(symbol_table.id[curr],id);
    symbol_table.value[curr]=value;
    symbol_table.count++;
}




struct jump_table jump = {
    .id = {"null","JGT", "JEQ", "JGE", "JLT", "JNE", "JLE", "JMP",},
    .value = {"000","001","010","011","100","101","110","111"},
};

struct dest_table dest = {
    .id = {"null","M","D","MD","A","AM","AD","AMD"},
    .value = {"000","001","010","011","100","101","110","111"},
};

struct comp_table comp = {
    .id = {"0","1","-1","D","A","!D","!A","-D","-A","D+1","A+1","D-1","A-1",""},
};


char * tobin(int num){
    char *bin = malloc(16);
    int m =0;
    for(int i=14; i>=0; i--){
        if((num >>i) & 1){
            bin[m] = '1';
            m++;
            continue;
        }
        bin[m] = '0';
        m++;
    }
    bin[16] = '\0';
    return bin;
}
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <inputfile>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    char (*file_data)[LINE_SIZE] = malloc(sizeof(char[LINE_SIZE]) * MAX_LINES);
    if (!file_data) {
        fprintf(stderr, "Allocation failed\n");
        fclose(file);
        return 1;
    }
    int line_count = 0;
    char buffer[LINE_SIZE];
    char ch;
    char (*file_buffer)[LINE_SIZE] = malloc(sizeof(char[LINE_SIZE]) * MAX_LINES);
    if (!file_buffer) {
        fprintf(stderr, "Allocation failed\n");
        fclose(file);
        return 1;
    }
    // printf("%p",file_buffer);
    while (fgets(buffer, sizeof(buffer), file) != NULL && line_count < MAX_LINES)
    {
        int shift = 0;
        while(buffer[shift] == ' ' || buffer[shift] == '\t') {
            shift++;
        }
        if (shift > 0) {
            int i;
            for (i = 0; i < strlen(buffer) - shift; i++) {
                buffer[i] = buffer[i + shift];
                
            }
            buffer[i] = '\0';
        }
        
        if ((buffer[0] == '/' && buffer[1] == '/') || (buffer[0] == '\n')) {
            continue; 
        }

        if(buffer[0]=='('){
            int j = 1;
            
            char symb[LINE_SIZE-2];
            int symind = 0;
            while((buffer[j]!='\n') && (buffer[j]!=')') ){
                symb[symind++] = buffer[j++];
            }
            symb[symind] = '\0';
            printf("%s\n",symb);


            continue;
        }else{
            strcpy(file_buffer[line_count], buffer);
        }
        
        // else if( buffer[0] == '@'){
        //     char temp[LINE_SIZE];
        //     int k = 1;
        //     while(0);
        //     printf("%d A -: %s",line_count, buffer); 
        // }
        // else{
        //     printf("%d C -: %s",line_count, buffer); 
        // }
        
        line_count++;
        // memset(buffer, 0, sizeof(buffer));

    }
    

    return 0;
}