#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>

#include "structDRVCOM.h"

FILE *pIn; 
FILE *pComunic; 
FILE *pOut; 

void ReplaceChrStr (char *buffer, int tam_buffer, char old_chr, char new_chr);
void findCod(char *path, char COD_REDE[8]);
int readComunic(char *path);
int openFiles(char *agrv);
int closeFiles(char *agrv);

int main (int argc, char **argv)
{
  int i;
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

  sleep(20);
  
  argv[1]="C:\\Program Files (x86)\\nxlog\\data\\ListaIFs.txt";

  if (openFiles(argv[1]) != 0) return 1;

  fseek (pIn, 0, SEEK_SET);
  rewind (pIn);
  
  while (strcmp(diaIni, diaAtu) == 0){

    memset(diaAtu, 0, sizeof(diaAtu));
    memset(hora, 0, sizeof(hora));
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strftime(hora,6,"%H:%M", timeinfo);
    strftime(diaAtu,6,"%d", timeinfo);

    if(strcmp(hora, "11:55") == 0) {
      break;
    }
    
    while (!feof(pIn)){
        memset(linha, 0, sizeof(linha));
        fgets(linha, sizeof(linha), pIn);
        ReplaceChrStr(linha, sizeof(linha), 0x0a, 0x00);
        if(strlen(linha) > 1){
          if(readComunic(linha) != 0){
            return 1;
          }
        }
    }

    sleep(5);
    fseek (pIn, 0, SEEK_SET);
    rewind (pIn);
  }


  if (closeFiles(argv[1]) != 0) return 1;

  errno = 0;
  if(remove(argv[1]) != 0) {
    printf("%s\n\n",strerror(errno));
    //system("pause") ;
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
    if (path[i] == '.' && isdigit(path[i+1]) && isdigit(path[i+2]) && isdigit(path[i+3])) {
      i++;
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
  T_REGISTRO registro;
  char COD_REDE[8+1];

  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  freopen (argv,"r",pComunic);
  if (pComunic == NULL){
    return 1;
  }
  findCod(argv, COD_REDE);

  fseek (pComunic, 0, SEEK_END);
  fseek (pOut, 0, SEEK_END);
  lSize = ftell (pComunic);
  rewind (pComunic);
  rewind (pOut);

  memset (&registro, 0x00, sizeof (registro));
  fread (&registro, sizeof (char), lSize, pComunic);

  for (i = 0; i < registro.Comunicacao.QtdeCanaisTotal; i++)
  {
    fprintf(pOut, "%d%02d%02d;%d;%s;;%s;%s;%s;%d;%d;%d;%d;%d;%d;%d;%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, registro.Comunicacao.HorarioAmostragem, "010", COD_REDE, registro.Comunicacao.IdentificacaoAutorizador, registro.Canal[i].IdentificacaoCanal, registro.Canal[i].EstadoConexao, registro.Canal[i].HorarioUltimaConexao, registro.Canal[i].HorarioUltimaQueda, registro.Canal[i].QtdeQuedas, registro.Canal[i].QtdeTx, registro.Canal[i].HorarioUltimoTx, registro.Canal[i].QtdeRx, registro.Canal[i].HorarioUltimoRx);
  }

  return 0;

}

int openFiles(char *argv){

  char linha[80 + 1];

  pIn = fopen (argv, "r");
  if (pIn == NULL){
    return 1;
  }
  pOut = fopen ("ComunicacaoFMT.CSV", "a+");
  if (pOut == NULL){
    return 1;
  }

  fseek (pIn, 0, SEEK_SET);
  rewind (pIn);
  
  while (!feof(pIn)){
    memset(linha, 0, sizeof(linha));
    fgets(linha, sizeof(linha), pIn);
    ReplaceChrStr(linha, sizeof(linha), 0x0a, 0x00);
    if(strlen(linha) > 1){
      pComunic = fopen (linha, "rb");
      if (pComunic == NULL){
        return 1;
      }
      memset(pComunic, 0, sizeof(pComunic));
    }else break;
  }
  return 0;
}

int closeFiles(char *argv){

  char linha[80 + 1];

  fseek (pIn, 0, SEEK_SET);
  rewind (pIn);
  
  while (!feof(pIn)){
    memset(linha, 0, sizeof(linha));
    fgets(linha, sizeof(linha), pIn);
    ReplaceChrStr(linha, sizeof(linha), 0x0a, 0x00);
    if(strlen(linha) > 1){
      freopen (linha,"r",pComunic);
      if (fclose(pComunic)){
        return 1;
      }
      memset(pComunic, 0, sizeof(pComunic));
    }
  }

  if (fclose(pIn)) return 1;
  if (fclose(pOut)) return 1;
  return 0;
}