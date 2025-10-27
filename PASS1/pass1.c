#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<stdlib.h>

int main(){
    FILE *source,*intermediate,*optab,*symtab,*length;
    char label[20],opcode[20],operand[20];
    char symtab_data[20],optab_data[20];

    int locctr,starting_address,program_length;
    bool symbol_found,opcode_found;

    source=fopen("source.txt","r");
    intermediate=fopen("intermediate.txt","w");
    optab=fopen("optab.txt","r");
    symtab=fopen("symtab.txt","w+");
    length=fopen("length.txt","w");

    if(!source || !intermediate || !optab || !symtab || !length){
        printf("Error in file opening");
        return 1;
    }

    fscanf(source,"%s%s%s",label,opcode,operand);
    if(strcmp(opcode,"START")==0){
        starting_address=(int)strtol(operand,NULL,16);
        locctr=starting_address;
        fprintf(intermediate,"0000\t%s\t%s\t%s\n",label,opcode,operand);
    
    }
    fscanf(source,"%s%s%s",label,opcode,operand);
    while(strcmp(opcode,"END")!=0){
        if(strcmp(label,"**")!=0){
            symbol_found=false;
            rewind(symtab);

            while(fscanf(symtab,"%s",symtab_data)==1){
                if(strcmp(label,symtab_data)==0){
                    printf("Error:Duplicate");
                    symbol_found=true;
                    break;
                }
            }
            if(!symbol_found){
                fprintf(symtab,"%s\t%04x\n",label,locctr);
            }
        }
        opcode_found=false;
        rewind(optab);
        while(fscanf(optab,"%s",optab_data)==1){
            if(strcmp(opcode,optab_data)==0){
                opcode_found=true;
                break;
            }

        }
        fprintf(intermediate,"%04x\t",locctr);

        if(opcode_found){
            locctr+=3;

        }
        else if(strcmp(opcode,"WORD")==0){
            locctr+=3;
        }
        else if(strcmp(opcode,"RESW")==0){
            locctr+=3*atoi(operand);

        }
        else if(strcmp(opcode,"RESB")==0){
            locctr+=atoi(operand);
        }
        else if(strcmp(opcode,"BYTE")==0){
            if(operand[0]=='C'){
                locctr+=(int)strlen(operand)-3;
            }
            else if(operand[0]=='X'){
                locctr+=((int)strlen(operand)-3)/2;
            }
        }
        else{
            printf("Error");
        }
        fprintf(intermediate,"%-8s%-8s%s\n",label,opcode,operand);

        fscanf(source,"%s%s%s",label,opcode,operand);
    }
    fprintf(intermediate,"%04x\t%-8s%-8s%s\n",locctr,label,opcode,operand);
    program_length=locctr-starting_address;

    fprintf(length,"%d",program_length);

    fclose(source);
    fclose(intermediate);
    fclose(optab);
    fclose(symtab);
    fclose(length);
    return 0;
}