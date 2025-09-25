#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define LINE_SIZE 1000
#define MAX_LINES 30000      
#define MAX_SYMBOLS 4096

int next_var_addr = 16;

struct table {
    char id[MAX_SYMBOLS][LINE_SIZE];
    int  value[MAX_SYMBOLS];
    int  count;
};
struct jump_table{
    char id[8][5];    // Changed from 4 to 5 to fit "null\0"
    char value[8][4]; // Changed from 3 to 4 to fit "000\0"
};

struct dest_table{
    char id[8][5];    // Changed from 4 to 5 to fit "null\0" 
    char value[8][4]; // Changed from 3 to 4 to fit "000\0"
};

struct comp_table{
    char id[36][4];
    char value[36][8];
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

char* find_jmp(char id[]){
    int c = 0;
    while(c<8){
        if(strcmp(jump.id[c],id) == 0){
            return jump.value[c];
        }
        c++;
    }
    return NULL;
}


struct dest_table dest = {
    .id = {"null","M","D","MD","A","AM","AD","AMD"},
    .value = {"000","001","010","011","100","101","110","111"},
};


char* find_des(char id[]){
    int c = 0;
    while(c<8){
        if(strcmp(dest.id[c],id) == 0){
            return dest.value[c];
        }
        c++;
    }
    return NULL;
}

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


char* find_cmp(char id[]){
    int c = 0;
    while(c<28){
        if(strcmp(comp.id[c],id) == 0){
            return comp.value[c];
        }
        c++;
    }
    return NULL;
}

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

    // Print file contents for debugging
    // printf("File contents:\n");
    // rewind(file);
    // char temp_buffer[LINE_SIZE];
    // int temp_line = 0;
    // while (fgets(temp_buffer, sizeof(temp_buffer), file) != NULL) {
    //     printf("Line %d: %s", temp_line++, temp_buffer);
    // }
    // rewind(file);
    // printf("\n");
    int line_count = 0;
    char buffer[LINE_SIZE];
    char ch;
    char (*file_buffer)[LINE_SIZE] = malloc(sizeof(char[LINE_SIZE]) * MAX_LINES);
    if (!file_buffer) {
        fprintf(stderr, "Allocation failed\n");
        fclose(file);
        return 1;
    }
    
    int ll =0;

    while (fgets(buffer, sizeof(buffer), file) != NULL && line_count < MAX_LINES)
    {
        // printf("%s %d",buffer,ll);  // Remove this line - it's flooding output
        ll++;
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
            // printf("%s\n",symb);
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
    
    // printf("\nSymbol Table:\n");
    // for(int i = 0; i < symbol_table.count; i++) {
    //     printf("%s: %d\n", symbol_table.id[i], symbol_table.value[i]);
    // }
    // printf("\nFile Buffer:\n");
    // for(int i = 0; i < line_count; i++) {
    //     printf("Line %d: %s", i, file_buffer[i]);
    // }

    printf("first pass done");

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
            char *bin_str = tobin(f);
            char fbin[17] = "0";  // Increased size
            strcat(fbin, bin_str);
            free(bin_str);  // Add this line
            strcpy(file_data[i], fbin);
        }else{
            int desti = 0, compi = 0, jumpi = 0; 
            char des[10]="null\0";
            char jmp[10]="null\0";
            char cmp[10]="null\0";

            int tmpind = 0;
            char tempdata[10];
            for(int j = 0; buffer[j] != '\0' && buffer[j] != '\n'; j++) {
                if(buffer[j]=='='){
                    tempdata[tmpind]='\0';
                    strcpy(des,tempdata);
                    desti = 1;
                    tmpind = 0;
                    j++;
                }else if(buffer[j]==';'){
                    tempdata[tmpind]='\0';
                    strcpy(cmp,tempdata);
                    compi = 1;
                    tmpind = 0;
                    j++;
                }
                tempdata[tmpind]=buffer[j];
                tmpind++;
            }
            if((desti == 1)&&(compi==0)){
                    tempdata[tmpind]='\0';
                    strcpy(cmp,tempdata);
                    compi = 1;
                    tmpind = 0;
            }else if((compi==1)&&(jumpi==0)){
                tempdata[tmpind]='\0';
                strcpy(jmp,tempdata);
                jumpi = 1;
                tmpind = 0;
            }
            
            // printf("%s %s %s\n",des,cmp,jmp);
            char cbin[17] = "111";  // Need 17 chars: "111" + 7 + 3 + 3 + '\0'
            char desbit[4];
            char jmpbit[4];
            char cmpbit[8];

            // Add this debugging instead:
            // printf("Parsing line %d: '%s'\n", i, buffer);
            printf("des:'%s' cmp:'%s' jmp:'%s'\n", des, cmp, jmp);

            // Strip whitespace from cmp, jmp, and des
            char *start, *end;

            // Strip cmp
            start = cmp;
            while(isspace(*start)) start++;
            end = start + strlen(start) - 1;
            while(end > start && isspace(*end)) end--;
            *(end + 1) = '\0';
            memmove(cmp, start, end - start + 2);

            // Strip jmp
            start = jmp;
            while(isspace(*start)) start++;
            end = start + strlen(start) - 1;
            while(end > start && isspace(*end)) end--;
            *(end + 1) = '\0';
            memmove(jmp, start, end - start + 2);

            // Strip des
            start = des;
            while(isspace(*start)) start++;
            end = start + strlen(start) - 1;
            while(end > start && isspace(*end)) end--;
            *(end + 1) = '\0';
            memmove(des, start, end - start + 2);

            char *cmp_result = find_cmp(cmp);
            char *jmp_result = find_jmp(jmp);
            char *des_result = find_des(des);

            if (!cmp_result) printf("FAILED to find cmp: '%s'\n", cmp);
            if (!jmp_result) printf("FAILED to find jmp: '%s'\n", jmp);
            if (!des_result) printf("FAILED to find des: '%s'\n", des);

            if (!cmp_result || !jmp_result || !des_result) {
                printf("skipping\n");
                continue;  // Skip this line, don't exit
            }

            strcpy(cmpbit, cmp_result);
            strcpy(jmpbit, jmp_result);
            strcpy(desbit, des_result);
                strcat(cbin,cmpbit);
                strcat(cbin,desbit);
                strcat(cbin,jmpbit);
                cbin[16]='\0';  // This is correct now
                // printf("%s\n",cbin);
                strcpy(file_data[i],cbin);
        }
        
    }

    char output_filename[256];
    strcpy(output_filename, argv[1]);
    char *dot = strrchr(output_filename, '.');
    if (dot && strcmp(dot, ".asm") == 0) {
        strcpy(dot, ".hack");
    } else {
        strcat(output_filename, ".hack");
    }

    FILE *output_file = fopen(output_filename, "w");
    if (!output_file) {
        perror("Error creating output file");
        free(file_buffer);
        free(file_data);
        return 1;
    }

    for (int i = 0; i < line_count; i++) {
        fprintf(output_file, "%s\n", file_data[i]);
    }
    fclose(output_file);
    free(file_buffer);
    free(file_data);

    printf("Binary output written to %s\n", output_filename);
    return 0;
}