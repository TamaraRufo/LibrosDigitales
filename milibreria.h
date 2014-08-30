#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<dirent.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/wait.h>
#include<signal.h>
#include<time.h>
#include<sys/types.h>
#include<fcntl.h>

#include<curses.h>
#include<sys/msg.h>
#include<sys/socket.h>
#include<netinet/in.h> 
#include<arpa/inet.h>
#include<strings.h>
#include<netdb.h>

#define KEYM 3454354 //memoria compartida
#define KEYC 3424565 //cola 
typedef struct{
	char nombre[20];
	char contra[10];
}USUARIO;
typedef struct{
	int valor;
}RESPUESTA;
typedef struct{
	char proveedor[20];
	char contra[10];
	char nombre[20];
	char contrau[10];
}PROV;
typedef struct{
	int enviados;
}MEMO;
typedef struct{
	char comando[20];
	char nombre[20];
	char contrau[10];
}COMANDOUSU;
typedef struct{
	char comando[20];
	char proveedor[20];
	char contra[10];
	char nombre[20];
	char contrau[10];
}COMANDOPROV;


typedef struct{
	char titulo[50];
	char autor[50];
	char adq[10];
	char prov[20];
	char tema[50];
	int pag;
	int isbn;
	int desc;
}DESC;
typedef struct{
	char texto[40];
}LIBRO;
typedef struct {
	long tipo;
	char comando[20];
	char titulo[50];
	char autor[50];
	char adq[10];
	char prov[20];
	char tema[50];
	int pag;
	int isbn;
	int desc;
	char texto[40];
}COMANDOLIBROPROV;
typedef struct {
	char comando[20];
	char lista[500];
}COMANDOLISTALIBROUSU;
typedef struct {
	char comando[20];
	char titulo[50];
	char autor[50];
	char adq[10];
	char prov[20];
	char tema[50];
	int pag;
	int isbn;
	int desc;
	char texto[40];
}COMANDOLIBROUSU;
