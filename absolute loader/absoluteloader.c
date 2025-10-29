#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(){
    FILE *fp;
    char line[60],name[60],stradd[60];
    int i,j,stradd1;

    fp=fopen("objectcode.txt","r");
    fscanf(fp,"%s",line);

    i = 2; // Start after H^
    j = 0;
    while (line[i] != '^' && i < 8) { // Stop at '^' or after 6 chars (max name length)
    name[j] = line[i];
    i++;
    j++;
    }
    name[j] = '\0'; // Null-terminate the name string exactly where it stopped
    printf("Program Name: %s\n", name);


    do{
        fscanf(fp,"%s",line);
        if(line[0]=='T'){
            for(i=2,j=0;i<8 && j<6;i++,j++){
                stradd[j]=line[i];            
            }
            stradd[j]='\0';
            stradd1=strtol(stradd,NULL,16);

            i=12;
            while(line[i]!='\0'){
                if(line[i]=='^'){
                    i++;
                    continue;
                }
                else{
                    printf("%X : \t%c%c\n",stradd1,line[i],line[i+1]);
                    stradd1++;
                    i+=2;
                }
            }

        }
        else if(line[0]=='E'){
            break;
        }

    }while(!feof(fp));
    fclose(fp);
    return 0;

}