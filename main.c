#include <stdio.h>
#include "encoding.c"

int main()
{
    FILE *menotti;
    int i;
    char c;
    char *filename = "curriculos/curriculo.xml";
    char *file_contents = read_file(filename);
    printf("%s", ISO8859ToUTF8(file_contents));
    /*if (menotti != NULL){
        //Faz a leitura dos 10 primeiros caracteres do arquivo
        for (i =0; i < 1000;i++){
            c = fgetc(menotti);
            printf("%c",c);
        }
        return 0;
    }*/
    return 0;
}