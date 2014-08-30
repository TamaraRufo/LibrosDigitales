#include "milibreria.h"
void proceso2(char servidor[20]);
MEMO *pn;//Memoria compartida
int espera=1; //Espera de 10 minutos
void minutos(int s)
{
	espera=0;
}
/* Parametros de entrada: argc,argv para darle la ip del servidor
 * Función: principal
 * Parametro de salida: 0
 * */
int main(int argc, char *argv[])
{
	int shmid,msgid,proc2,bucle=0,vez=0;
	msgid=msgget(KEYC,IPC_CREAT|0666);//creamos una COLA
	shmid=shmget(KEYM,sizeof(MEMO),IPC_CREAT | 0666); //creamos MEMORIA COMPARTIDA
	pn=(MEMO *)shmat(shmid,0,0);
	signal(SIGUSR1,minutos);
	printf("PUNTO 1\n");
	if(argc<2)printf("Error en parametros\n");
	else
	{
		proc2=fork();
		switch(proc2)
		{
			case -1:
					printf("Error en proceso lector\n");
					break;
			case 0:
			
					while(bucle==0)
					{
						if (espera==0)
						{
							vez++;
							printf("Entra proceso. Comprobación nº: %d\n",vez);
							printf("El total de libros enviados al servidor es: %d\n",pn->enviados);
							sleep(2);
							proceso2(argv[1]);
							espera=1;
						}
					}
					break;
			default:
					while (bucle==0)
					{
						kill(proc2,SIGUSR1);
						sleep(20);//Espera 20 segundos						
					}
		}
	}
	close(msgid);//cerramos cola 
	close(shmid);//cerramos memoria
	return 0;
}
/* Parametros de entrada: servidor.Para darle la ip del servidor.
 * Función: Esta funcion nos recoge los mensajes de la cola y se los 
 * manda al servidor mediante socket.
 * Parametro de salida: 0
 * */
void proceso2(char servidor[20]) //recoge de la cola
{
	int msgid;
	COMANDOLIBROPROV clibro;
	//variables socket
	struct hostent *h;
	struct sockaddr_in d;
	//struct in_addr dir;
	int n,s;
	msgid=msgget(KEYC,0666);//nos enganchamos a la cola
	//leemos de la cola
	if(-1==msgrcv(msgid,&clibro,sizeof(COMANDOLIBROPROV),1,IPC_NOWAIT))
	{
		printf("No hay ningun mensaje\n");
	}
	else
	{
	
		printf("Hay mensajes en la cola.\n");
		h=gethostbyname(servidor);
		if(h)
		{
			bzero(&d,sizeof(d));
			d.sin_family=AF_INET;
			d.sin_port=htons(1041);
			bcopy(h->h_addr,&d.sin_addr,4);
			s=socket(AF_INET,SOCK_STREAM,0);
			if(s<0)
			{
				printf("Error en el socket\n");
				exit(1);
			}
			if(connect(s,(struct sockaddr *)&d,sizeof(d)))
			{
				printf("Error al conectar\n");
			}
			else
			{
				
				n=send(s,&clibro,sizeof(COMANDOLIBROPROV),0); //enviamos mensaje
			}
			close(s);
		}
	}
}
