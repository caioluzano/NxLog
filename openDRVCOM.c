#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <Windows.h>

#include "structDRVCOM.h"

void ReplaceChrStr (char *buffer, int tam_buffer, char old_chr, char new_chr);
void findCod(char *path, char COD_REDE[8]);
int readComunic(char *path);

int main (int argc, char **argv)
{
  int i;
  FILE *pIN;
  char linha[80 + 1];
  char diaIni[2];
  char diaAtu[2];
  char hora[6];

  time_t rawtime;
  struct tm * timeinfo;

  time (&rawtime);
  timeinfo = localtime (&rawtime);
  strftime(diaIni,6,"%d", timeinfo);
  strftime(diaAtu,6,"%d", timeinfo);
  

  Sleep(10000);

  //argv[1]="\\\\192.168.54.106\\conf\\ListaIFs.txt";

  if(argc < 2){
    printf("Necessario pelo menos 1 argumento.\n");;
    return 0;
  }

  pIN = fopen (argv[1], "r");
  if (pIN == NULL){
    printf ("Falha ao abrir arquivo %s!\n", argv[1]);
    return 1;
  }

  fseek (pIN, 0, SEEK_SET);
  rewind (pIN);
  
  while (strcmp(diaIni, diaAtu) == 0){

    memset(diaAtu, 0, sizeof(diaAtu));
    memset(hora, 0, sizeof(hora));
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strftime(hora,6,"%H:%M", timeinfo);
    strftime(diaAtu,6,"%d", timeinfo);

    if(strcmp(hora, "23:59") == 0) {
      break;
    }
    
    while (!feof(pIN)){
        memset(linha, 0, sizeof(linha));
        fgets(linha, sizeof(linha), pIN);
        ReplaceChrStr(linha, sizeof(linha), 0x0a, 0x00);
        if(strlen(linha) > 1){
          if(readComunic(linha) != 0){
            return 1;
          }
        }
    }

    Sleep(5000);
    fseek (pIN, 0, SEEK_SET);
    rewind (pIN);
  }

  //freopen (linha,"r",pComunic);
  fclose (pIN);
  errno = 0;
  if(remove(argv[1]) != 0) {
    printf("%s\n\n",strerror(errno));
    return 1;
  }
  return 0;
}

/* Esta função foi uma contribuição do Fernando Akira, todos os direitos estão reservados ao mesmo. */
void ReplaceChrStr (char *buffer, int tam_buffer, char old_chr, char new_chr)
{
  int i;
  char *ptr = buffer;

  for (i = 0; i < tam_buffer; i++)
  {
    if (*ptr == old_chr)
      *ptr = new_chr;

    ptr++;
  }
}

void findCod(char *path, char COD_REDE[8]){
  int i, j;
  long size = strlen(path);
  for(i=0; i<size; i++){
    if (path[i-1] == 'o' && path[i] == '.' && isdigit(path[i+1]) && isdigit(path[i+2]) && isdigit(path[i+3])) {
      i++;
      memset(COD_REDE, 0, sizeof(COD_REDE));
      for(j=0; i<size; j++){
        COD_REDE[j] = path[i];
        i++;
      }
    }
  }
}

int readComunic(char *argv){

  int i;
  long lSize;
  FILE *pFile;
  FILE *pOut;
  T_REGISTRO registro;
  char COD_REDE[8+1];

  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  errno = 0;
  pFile = fopen (argv, "rb");
  pOut = fopen ("ComunicacaoFMT.CSV", "a+");

  if (pFile == NULL){
    return 1;
  }else if(pOut == NULL){
    return 1;
  }

  findCod(argv, COD_REDE);

  fseek (pFile, 0, SEEK_END);
  fseek (pOut, 0, SEEK_END);
  lSize = ftell (pFile);
  rewind (pFile);
  rewind (pOut);

  memset (&registro, 0x00, sizeof (registro));
  fread (&registro, sizeof (char), lSize, pFile);

  for (i = 0; i < registro.Comunicacao.QtdeCanaisTotal; i++)
  {

    fprintf(pOut, "%d%02d%02d;%d;%s;;%s;%s;%s;%d;%d;%d;%d;%d;%d;%d;%d\n", 
      tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, registro.Comunicacao.HorarioAmostragem, "010", COD_REDE, registro.Comunicacao.IdentificacaoAutorizador, registro.Canal[i].IdentificacaoCanal, registro.Canal[i].EstadoConexao, registro.Canal[i].HorarioUltimaConexao, registro.Canal[i].HorarioUltimaQueda, registro.Canal[i].QtdeQuedas, registro.Canal[i].QtdeTx, registro.Canal[i].HorarioUltimoTx, registro.Canal[i].QtdeRx, registro.Canal[i].HorarioUltimoRx);

  }

  fclose (pFile);
  fclose (pOut);
  return 0;

}