#include "utils.h"
#include "libbst.h"
#include "liblist.h"
#include "libstack.h"

/*
Retorna uma lista de strings
Cada string eh o caminho a um determinado curriculo
*/
char **list_filename(char *dirname, int *list_size)
{
  // Vetor em que serao armazenados as strings
  char **filenames;
  filenames = malloc(sizeof(char *) * (N_ARCHS));

  DIR *cur_dir;
  struct dirent *entry;
  cur_dir = opendir(dirname);
  if (!cur_dir)
  {
    perror("ERRO ao abrir o diretório");
    exit(1);
  }
  // I vai contar quantas vezes passamos pelo loop abaixo
  int i = 0;

  // Loop sobre todos os arquivos de um diretorio
  while ((entry = readdir(cur_dir)))
  {
    // Ignorando as entradas dos diretórios . e ..
    if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
    {
      filenames[i] = malloc(sizeof(char) * LINESIZE);
      strcpy(filenames[i], dirname);
      strcat(filenames[i], "/");
      strcat(filenames[i], entry->d_name);
      i++;
    }
  }
  // Para nos informar o tamanho da stack criada
  *list_size = i;

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

/* Retorna o conteudo entre as aspas*/
char *get_inside_lable(char *string)
{
  char *temp = string;
  int len = 0;
  int i = 0;
  temp++;
  string++;
  while (temp[0] != '\"')
  {
    temp++;
    len++;
  }
  char *data = malloc(sizeof(char) * (len + 1));
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
char *get_data(char *string, char *lable, char **pointer)
{
  char *text;
  char *data;
  /* Procurando pela lable */
  text = strstr(string, lable);

  /* Testa se foi achado uma lable restante */
  if (text == NULL)
  {
    *pointer = NULL;
    return NULL;
  }

  /* Movendo o ponteiro ate as aspas */
  text = strstr(text, "\"");
  /* Pegando o conteudo entre aspas */
  data = get_inside_lable(text);
  /* Mantendo track do endereco em que parei a leitura */
  *pointer = text;
  return data; // Lembrar de dar free
}

void process_data(char *string)
{
  char *date;
  char *name;
  char *pointer;
  date = get_data(string, "ANO-DO-ARTIGO=", &pointer);
  while (pointer != NULL)
  {
    name = get_data(pointer, "TITULO-DO-PERIODICO-OU-REVISTA=", &pointer);
    printf("%s : %s \n", name, date);
    free(date);
    free(name);
    date = get_data(pointer, "ANO-DO-ARTIGO=", &pointer);
  }
}

char *get_quali(char *line)
{
  char *ini = line;
  line++;
  while (line[0] != '\0')
  {
    line++;
    ini++;
  }
  return ini;
}

void find_quali(char *file_path, char *name)
{
  FILE *arq;
  char line[LINESIZE];

  arq = fopen(file_path, "r");
  if (!arq)
  {
    perror("Erro ao abrir arquivo");
    exit(1);
  }
  /* Tenta ler uma linha */
  fgets(line, LINESIZE, arq);
  printf("%s \n", get_quali(line));
  fclose(arq);
}