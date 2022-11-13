#include "utils.h"
/*
  Edit distance function.
  Retirada de: https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C
*/
int levenshtein(char *s1, char *s2)
{
  unsigned int s1len, s2len, x, y, lastdiag, olddiag;
  s1len = strlen(s1);
  s2len = strlen(s2);
  unsigned int column[s1len + 1];
  for (y = 1; y <= s1len; y++)
    column[y] = y;
  for (x = 1; x <= s2len; x++)
  {
    column[0] = x;
    for (y = 1, lastdiag = x - 1; y <= s1len; y++)
    {
      olddiag = column[y];
      column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + (s1[y - 1] == s2[x - 1] ? 0 : 1));
      lastdiag = olddiag;
    }
  }
  return column[s1len];
}

/* Devolve o numero de arquivos contidos no diretorio*/
int count_arqs(DIR *dirname)
{
  struct dirent *entry;
  // count vai contar quantas vezes passamos pelo loop abaixo
  int count = 0;

  // Loop sobre todos os arquivos de um diretorio
  while ((entry = readdir(dirname)))
  {
    // Ignorando as entradas dos diretórios . e ..
    if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
      count++;
  }
  rewinddir(dirname);
  return count;
}

/*
Retorna uma lista de strings
Cada string eh o caminho a um determinado curriculo
*/
char **list_filename(char *dirname, int *num_arq)
{
  int i = 0;
  char **filenames;
  DIR *cur_dir;
  struct dirent *entry;
  cur_dir = opendir(dirname);
  if (!cur_dir)
  {
    perror("ERRO ao abrir o diretório");
    exit(1);
  }

  *num_arq = count_arqs(cur_dir);
  // Vetor em que serao armazenados as strings

  filenames = malloc(sizeof(char *) * (*num_arq));

  // Loop sobre todos os arquivos de um diretorio
  while ((entry = readdir(cur_dir)))
  {
    // Ignorando as entradas dos diretórios . e ..
    if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
    {
      /* Dinifindo as entradas como "diretorio/xxxx.xml" */
      filenames[i] = malloc(sizeof(char) * LINESIZE);
      strcpy(filenames[i], dirname);
      strcat(filenames[i], "/");
      strcat(filenames[i], entry->d_name);
      i++;
    }
  }

  closedir(cur_dir);
  return filenames;
}

void free_list_filenames(char **list, int size)
{
  int i;
  for (i = 0; i < size; i++)
  {
    free(list[i]);
  }
  free(list);
}

/*
Funcao que le todo o arquivo xml e o aloca seu conteudo em uma string
*/
unsigned char *read_file(FILE *file)
{
  // Se a leitura falha, retorna NULL
  if (file == NULL)
    return NULL;

  // Move o ponteiro para o final do texto
  fseek(file, 0, SEEK_END);

  // ftell() devolve a posicao atual do ponteiro que esta no fim do texto
  int length = ftell(file);

  // Move o ponteiro de volta para o inicio
  fseek(file, 0, SEEK_SET);

  // Aloca dinamicamente o espaco no qual sera salvo todo o conteudo do texto
  unsigned char *string = malloc(sizeof(char) * (length + 1));

  char c;
  int i = 0;

  // Continua lendo caractere por caractere ate o fim
  while ((c = fgetc(file)) != EOF)
  {
    string[i] = c;
    i++;
  }

  // Coloca o caractere nulo no fim da string
  string[i] = '\0';

  fclose(file);

  return string;
}

/* Retorna o conteudo entre as aspas */
char *get_inside_lable(char *string, char *data)
{
  int i = 0;
  string++;
  while (string[0] != '\"')
  {
    data[i] = string[0];
    string++;
    i++;
  }
  data[i] = '\0';
  return data;
}

/* Retorna o conteudo de uma lable no arquivo xml */
void get_data(char *string, char *lable, char *data, char **pointer)
{
  char *text;
  /* Procurando pela lable */
  text = strstr(string, lable);

  /* Testa se foi achado uma lable restante */
  if (text == NULL)
  {
    *pointer = NULL;
    return;
  }

  /* Movendo o ponteiro ate as aspas */
  text = strstr(text, "\"");
  /* Pegando o conteudo entre aspas */
  get_inside_lable(text, data);
  /* Mantendo track do endereco em que parei a leitura */
  *pointer = text;
  return; // Lembrar de dar free
}

/* Acha a index da qualificacao no vetor Estrato*/
int estrato_index(char *quali)
{
  if (strcmp(quali, "A1") == 0)
    return 0;
  else if (strcmp(quali, "A2") == 0)
    return 1;
  else if (strcmp(quali, "A3") == 0)
    return 2;
  else if (strcmp(quali, "A4") == 0)
    return 3;
  else if (strcmp(quali, "B1") == 0)
    return 4;
  else if (strcmp(quali, "B2") == 0)
    return 5;
  else if (strcmp(quali, "B3") == 0)
    return 6;
  else if (strcmp(quali, "B4") == 0)
    return 7;
  else if (strcmp(quali, " C") == 0)
    return 8;
  else
    return 9;
}

/*
Compara o nome do evento com cada linha.
Retorna a qualificacao se exite, ou "NC", caso nao exista. (POR REFERENCIA)
*/
void comparing(FILE *arq, char *name, char *quali, fpos_t *ini)
{
  char line[LINESIZE];
  char ch;
  int i;

  // Lendo cada linha
  ch = fgetc(arq);
  while (ch != EOF)
  {
    i = 0;
    while (ch != '\n')
    {
      line[i] = ch;
      i++;
      ch = fgetc(arq);
    }
    line[i] = '\0';
    // LINHA COMPLETA

    line[i - 3] = '\0'; // Deixando so o nome na linha

    // Comparando cada linha com o nome do evento
    if (levenshtein(line, name) <= 10)
    {
      // Pegando a qualificacao
      quali[0] = line[i - 2];
      quali[1] = line[i - 1];
      quali[2] = '\0';
      name = line;
      fsetpos(arq, ini); // Voltando o ponteiro para o inicio
      return;
    }

    ch = fgetc(arq);
  }
  fsetpos(arq, ini); // Voltando o ponteiro para o inicio
  return;            // NAO CONTEM
}

/*
Insere na lista de estrato todas as informacoes uteis do artigo.
Tais como: ano, nome, estrato.
*/
void get_lattes_data(char *string, int n, char *lable[], lista_t *estrato[], int array[])
{
  FILE *arq;
  fpos_t pos;          // Para guardar a posicao do inicio do arquivo
  char date[5];        // Para guardar o ano
  char quali[3];       // Para guardar a qualificacao
  char name[LINESIZE]; // Para guardar o nome do evento
  char *pointer;
  int index;

  // Abrindo o arquivo
  arq = fopen(lable[3], "r");
  if (!arq)
  {
    perror("Erro ao abrir arquivo");
    exit(1);
  }
  fgetpos(arq, &pos);

  /* Procurando pela data */
  get_data(string, lable[1], date, &pointer);
  while (pointer != NULL)
  {
    strcpy(quali, "NC");
    /* Procurando o nome do evento*/
    get_data(pointer, lable[2], name, &pointer);

    /* Procurando pela qualificacao */
    comparing(arq, name, quali, &pos);

    /* Inserindo na lista correspondente */
    index = estrato_index(quali);
    array[index + (n * 10)]++;
    lista_insere_ordenado(estrato[index], name, atoi(date));

    /* Procuranodo pela data de novo */
    get_data(pointer, lable[1], date, &pointer);
  }
  fclose(arq);
  return;
}

/* Realiza todo o processo, inserindo nas listas as producoes de todos os pesquisadores, e incrementando os vetores*/
void process_wrapper(char **filenames, int num_files, char *per_path, char *conf_path, lista_t *Periodicos[], lista_t *Conferencias[], int vetor_per[], int vetor_conf[])
{
  FILE *cur_file;
  int i;
  unsigned char *lattes;
  /* Vetor de strings contendo as lables a serem procuradas */
  char *conf_lable[] = {"CONFERENCIAS", "ANO-DO-TRABALHO=", "NOME-DO-EVENTO=", conf_path};
  char *per_lable[] = {"PERIODICOS", "ANO-DO-ARTIGO=", "TITULO-DO-PERIODICO-OU-REVISTA=", per_path};

  for (i = 0; i < num_files; i++)
  {
    cur_file = fopen(filenames[i], "r");
    lattes = read_file(cur_file);
    /* Le o lattes, procurando primeiro pelas conferencias e depois pelos periodicos*/
    get_lattes_data((char *)lattes, i, conf_lable, Conferencias, vetor_conf);
    get_lattes_data((char *)lattes, i, per_lable, Periodicos, vetor_per);
    free(lattes);
  }

  free_list_filenames(filenames, num_files);

  return;
}

void print_years(lista_t *Periodicos[], lista_t *Conferencias[])
{
  int i;
  int year = Periodicos[0]->ini->year;
  
  for (i = 0; i < 10; i++)
  {
    printf();
  }
}

/* Apenas uma funcao para organizar*/
void display_menu()
{
  printf("SELECIONE UMA DAS OPCOES\n");
  printf("0) Encerrar o programa\n");
  printf("1) Apresentar a produção sumarizada do grupo por ordem de periódicos discriminando os estratos\n");
  printf("2) Apresentar a produção sumarizada do grupo por ordem de conferências discriminando os estratos\n");
  printf("3) Apresentar a produção dos pesquisadores do grupo por ordem de autoria discriminando os estratos; Em periódicos. Em conferências\n");
  printf("4) Apresentar a produção sumarizada do grupo por ano discriminando os estratos; Em periódicos; Em conferências.\n");
  printf("Opcao: ");

  return;
}
