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
    .id = {"0","1","-1",
        "D","A","!D",
        "!A","-D","-A",
        "D+1","A+1","D-1",
        "A-1","D+A","D-A",
        "A-D","D&A","D|A",
        "M","!M","-M",
        "M+1","M-1","D+M",
        "D-M","M-D","D&M",
        "D|M"},
    .value={
        "0101010","0111111","0111010",
        "0001100","0110000","0001101",
        "0110001","0001111","0110011",
        "0011111","0110111","0001110",
        "0110010","0000010","0010011",
        "0000111","0000000","0010101",
        "1110000","1110001","1110011",
        "1110111","1110010","1000010",
        "1010011","1000111","1000000",
        "1010101"
    }
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
    bin[15] = '\0';
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
            if(find_sym(symb)==-1){
                add_sym(symb,line_count);
            }

            continue;
        }else{
            strcpy(file_buffer[line_count], buffer);
        }
        line_count++;
    }
    fclose(file);
    
    printf("\nSymbol Table:\n");
    for(int i = 0; i < symbol_table.count; i++) {
        printf("%s: %d\n", symbol_table.id[i], symbol_table.value[i]);
    }
    printf("\nFile Buffer:\n");
    // for(int i = 0; i < line_count; i++) {
    //     printf("Line %d: %s", i, file_buffer[i]);
    // }

    char (*file_data)[LINE_SIZE] = malloc(sizeof(char[LINE_SIZE]) * MAX_LINES);
    if (!file_data) {
        fprintf(stderr, "Allocation failed\n");
        fclose(file);
        return 1;
    }
    for(int i = 0; i < line_count; i++) {
        strcpy(buffer, file_buffer[i]);
        if(buffer[0]=='@'){
            // printf("%s",buffer);
            int h =1;
            char tmpsymb[LINE_SIZE];
            int isdig = 1;

            while ((buffer[h]!='\0')&&(buffer[h]!='\n'))
            {
                if(!(isdigit(buffer[h]))){
                    isdig = 0;
                }

                tmpsymb[h-1] = buffer[h];
                h++;
            }
            tmpsymb[h-1]='\0';
            int f;
            if(isdig){
                f = atoi(tmpsymb);
            }else{
                f = find_sym(tmpsymb);
                if(f==-1){
                    add_sym(tmpsymb,next_var_addr);
                    f = next_var_addr;
                    next_var_addr++;
                }
            }
            char fbin[16] = "0";
            strcat(fbin,tobin(f));
            // printf("%s\n",fbin);
            // printf("@%d\n",f);
            strcpy(file_data[i],fbin);
        }else{
            // printf("%s",buffer);
            int desti = 0, compi = 0, jumpi = 0; 
            char des[4]="null";
            char jmp[4]="null";
            char cmp[4]="null";
            int h =0;
            int tmpind = 0;
            while((buffer[h]!='\0')&&(buffer[h]!='\n')){
                char tempdata[50];
                if(buffer[h]=='='){
                    for(int ij=tmpind; ij<h; ij++){
                        tempdata[ij-tmpind]=buffer[ij];  // ✅ Correct: subtract tmpind
                    }
                    tempdata[h-tmpind]='\0';             // ✅ Correct: null terminate at proper position
                    strcpy(des,tempdata);
                    desti = 1;
                    tmpind=h+1;
                    printf("DEST - %s\n",des);
                }else if(buffer[h]==';'){
                    for(int ij=tmpind; ij<h; ij++){
                        tempdata[ij-tmpind]=buffer[ij];    // ✅ Correct indexing
                    }
                    tempdata[h-tmpind]='\0';               // ✅ Correct null terminator
                    strcpy(cmp,tempdata);
                    printf("COMP - %s\n",cmp);    
                    compi = 1;
                    tmpind=h+1;  // Move this before jump extraction
                    
                    // Extract jump part
                    int jmp_len = 0;
                    for(int ij=tmpind; ij<strlen(buffer); ij++){
                        if(buffer[ij]!='\n' && buffer[ij]!='\0'){
                            tempdata[jmp_len]=buffer[ij];    // ✅ Use jmp_len for indexing
                            jmp_len++;
                        }
                    }
                    tempdata[jmp_len]='\0';                // ✅ Correct null terminator
                    strcpy(jmp,tempdata);
                    jumpi = 1;
                    printf("JUMP - %s\n",jmp);
                }else if((desti==1)&&(compi==0)){
                    
                }
                
                h++;
            
            }
        }
        
    }

    // printf("\nFile Data (Binary):\n");
    // for(int i = 0; i < line_count; i++) {
    //     printf("Line %d: %s\n", i, file_data[i]);
    // }
    return 0;
}