#ifndef TESTE_H_GUARD
#define TESTE_H_GUARD

#define NUMERO_MAXIMO_CANAIS 8

#pragma pack(1)

// --------------------------------------

struct HEADER
{
  unsigned short TamanhoHeader;
  unsigned short VersaoArquivo;
  unsigned char Reservado [12];
  unsigned char Md5 [16];
};
typedef struct HEADER T_HEADER;

// --------------------------------------

struct COMUNICACAO
{
  unsigned short TamanhoRegistro;
  unsigned short VersaoRegistro;
  char IdentificacaoAutorizador [32 + 1];
  unsigned int HorarioAmostragem;
  unsigned short QtdeCanaisTotal;
  unsigned short QtdeCanaisAtivos;
  unsigned short QtdeCanaisInativos;
};
typedef struct COMUNICACAO T_COMUNICACAO;

// --------------------------------------

struct CANAL
{
  int TipoComunicacao;
  char IdentificacaoCanal [64 + 1];
  unsigned char EstadoConexao;
  unsigned int HorarioUltimaConexao;
  unsigned int HorarioUltimaQueda;
  unsigned int QtdeQuedas;
  unsigned int HorarioUltimoTx;
  unsigned int QtdeTx;
  unsigned int HorarioUltimoRx;
  unsigned int QtdeRx;
};
typedef struct CANAL T_CANAL;

// --------------------------------------

struct REGISTRO
{
  T_HEADER Header;
  T_COMUNICACAO Comunicacao;
  T_CANAL Canal [NUMERO_MAXIMO_CANAIS];
};
typedef struct REGISTRO T_REGISTRO;

// --------------------------------------

#pragma pack()

#endif // TESTE_H_GUARD
