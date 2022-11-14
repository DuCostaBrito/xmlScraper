#include "liblist.h"


lista_t *lista_cria()
{
    lista_t *l;
    /* aloca espaco para a cabeca da lista_t */
    if (!(l = (lista_t *)malloc(sizeof(lista_t))))
        return NULL;
    
    l->size = 0;
    l->total = 0;
    return l;
}

void insert(lista_t *l, char *name, char *quali)
{
    int i;

    for (i = 0; i < l->size; i++)
    {
        if (strcmp(l->nodes[i].name, name) == 0)
        {
            l->nodes[i].n_participation = l->nodes[i].n_participation + 1;
            l->total++;
            return;
        }
    }

    strcpy(l->nodes[l->size].quali, quali);
    strcpy(l->nodes[l->size].name, name);
    l->nodes[l->size].n_participation = 1;
    l->size = l->size + 1;
    l->total = l->total + 1;
    return;
}

char *return_quali(int i)
{
    if (i == 0)
        return "A1";
    else if (i == 1)
        return "A2";
    else if (i == 2)
        return "A3";
    else if (i == 3)
        return "A4";
    else if (i == 4)
        return "B1";
    else if (i == 5)
        return "B2";
    else if (i == 6)
        return "B3";
    else if (i == 7)
        return "B4";
    else if (i == 8)
        return " C";
    else
        return "NC";
}
void lista_imprime(lista_t *l, int i)
{
    int j;
    char quali[3];
    strcpy(quali, return_quali(i));
    if (l->size == 0)
        return;
    printf("Estrato %s: \n", quali);
    for (j = 0; j < l->size; j++)
    { 
        if (strcmp(quali, l->nodes[j].quali) == 0)
            printf("%s: %d \n", l->nodes[j].name, l->nodes[j].n_participation);
    }
    printf("\n");
    return;
}