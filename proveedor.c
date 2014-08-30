#include "milibreria.h"

void menu (char servidor[20]); //menu principal
void accederp(char servidor[20]); //acceder como proveedor
void accederuprov(char servidor[20],char proveedor[20],char contra[10]);//acceder como usuario de proveedor
int comandoservidorp(char servidor[20],char comando[20],char proveedor[20],char contra[10],char usuario[20],char contrau[10]);
void submenu(char servidor[20],char prov[20],char contra[10]);//menu de provee dor
void altaprov(char servidor[20]);//alta de proveedor
void altauprov(char servidor[20],char prov[20], char contra[10]);//alta de usuario
void eliminarprov(char servidor[20]);//eliminar proveedor
void eliminaruprov(char servidor[20],char proveedor[20],char contra[10]);//eliminar usuario
void borrarDir(char ruta[256]);//borrar cualquier directorio
void enviar(void);//enviar libro al servidor
void intro(char prov[20],char usuario[20]);//introducir libro
MEMO *pn;//Memoria compartida

/* Parametros de entrada: argc,argv para darle la ip del servidor
 * Función: principal
 * Parametro de salida: 0
 * */
int main(int argc, char *argv[])
{
	int shmid,msgid;
	msgid=msgget(KEYC,IPC_CREAT|0666);//creamos una COLA
	shmid=shmget(KEYM,sizeof(MEMO),IPC_CREAT | 0666); //creamos MEMORIA COMPARTIDA
	pn=(MEMO *)shmat(shmid,0,0);
	pn->enviados=0;
	mkdir("./Proveedores",0700);
	if(argc<2)printf("Error en parametros\n");
	else
	{
		menu(argv[1]);
	}
	close(msgid);//cerramos cola 
	close(shmid);//cerramos memoria
	return 0;
}
/* Parametros de entrada: servidor, para pasarle la ip
 * Función: esta función nos muestra el menu principal
 * Parametro de salida: void
 * */
void menu (char servidor[20]) //menu principal
{
	char op;
	do
		{
			system("clear");
			printf("PROVEEDORES\n");
			printf("===========\n");
			printf("A. Acceder\n");
			printf("B. Dar de alta un proveedor\n");
			printf("C. Eliminar proveedor\n");
			printf("S. Cerrar\n");
			printf("\n");
			printf("Introduce una opcion: ");
			scanf(" %c",&op);
			
			switch(op)
			{
				case 'A':
				case 'a':
						 system("clear");
						 accederp(servidor);//acceder proveedor						 
						 break;
				case 'B':
				case 'b':
						 system("clear");
						 altaprov(servidor); //alta proveedor			 
						 break;
				case 'C':
				case 'c':
						 system("clear");
						 eliminarprov(servidor); //eliminar proveedor
						 break;
				case 'S':
				case 's':
						 system("clear");
						 printf("Hasta la proxima...");
						 sleep(2);
						 break;
				default:
						 printf("La opcion escogida no es correcta...\n");
			}
		}
	while(op!='S' || op!='s');
}
/* Parametros de entrada: servidor, para pasarle la ip
 * Función: esta función accede al servidor con el nombre y contraseña de un proveedor
 * Parametro de salida: void
 * */
void accederp(char servidor[20]) //acceder proveedor
{
	COMANDOPROV cprov;
	int res;
	printf("ACCEDER\n");
	printf("=======\n");
	printf("\n");
	printf("Introduce el nombre de proveedor: ");
	scanf("%s",cprov.proveedor);
	printf("\nIntroduce la contraseña de proveedor: ");
	scanf("%s",cprov.contra);
	strcpy(cprov.comando,"LOGINP");
	res=comandoservidorp(servidor,cprov.comando,cprov.proveedor,cprov.contra,"","");
	if(res==0)
	{
		printf("\n");
		printf("Bienvenido al sistema proveedor: %s\n",cprov.proveedor);
		sleep(2);
		submenu(servidor,cprov.proveedor,cprov.contra); //menu para el usuario del proveedor
	}
	else
	{
		printf("\n");
		printf("Lo sentimos, compruebe su usuario o contraseña, no son correctos\n");
		printf("Recuerde que si no esta dado de alta puede hacerlo.\n");
		sleep(2);
	}
}
/* Parametros de entrada: servidor,proveedor,contra. Para pasarle la ip
 * del servidor, y el nombre y contraseña del proveedor al que pertenece
 * Función: esta función nos muestra el menu principal
 * Parametro de salida: void
 * */
void accederuprov(char servidor[20],char proveedor[20],char contra[10])
{
	COMANDOPROV cprov;
	int res;
	printf("ACCEDER\n");
	printf("=======\n");
	printf("\n");
	printf("Introduce el nombre de usuario: ");
	scanf("%s",cprov.nombre);
	printf("\nIntroduce la contraseña de usuario: ");
	scanf("%s",cprov.contrau);
	strcpy(cprov.comando,"LOGINU");
	res=comandoservidorp(servidor,cprov.comando,proveedor,contra,cprov.nombre,cprov.contrau);
	if(res==0)
	{
		printf("\n");
		printf("Bienvenido al sistema usuario: %s\n",cprov.nombre);
		intro(proveedor,cprov.nombre); //menu para el usuario del proveedor
	}
	else
	{
		printf("\n");
		printf("Lo sentimos, compruebe su usuario o contraseña, no son correctos\n");
		printf("Recuerde que si no esta dado de alta puede hacerlo.\n");
		sleep(2);
	}
}
/* Parametros de entrada: servidor,comando,proveedor,contra,usuario,contrau. 
 * Para pasarle la ip del servidor, el comando a ejecutar, el nombre y 
 * contraseña del proveedor al que pertenece y el usuario y contraseña del usuario.
 * Función: esta función manda por socket un mensaje al servidor con los datos.
 * parametro de salida: 0 si no existe, 1 si existe. 
 * */
int comandoservidorp(char servidor[20],char comando[20],char proveedor[20],char contra[10],char usuario[20],char contrau[10])
{
	COMANDOPROV cprov;
	RESPUESTA v;
	struct hostent *h;
	struct sockaddr_in d;
	int s;
	
	strcpy(cprov.comando,comando);
	strcpy(cprov.proveedor,proveedor);
	strcpy(cprov.contra,contra);
	strcpy(cprov.nombre,usuario);
	strcpy(cprov.contrau,contrau);
	
	h=gethostbyname(servidor);
	if(h)
	{
		bzero(&d,sizeof(d));
		d.sin_family=AF_INET;
		d.sin_port=htons(1040);
		bcopy(h->h_addr,&d.sin_addr,4);
		
		s=socket(AF_INET,SOCK_STREAM,0);
		if(s<0)
		{
			printf("Error en el socket\n");
			return 1;
		}
		if(connect(s,(struct sockaddr *)&d,sizeof(d)))
		{
			printf("Error al conectar\n");
			return 1;
		}
		else
		{
			send(s,&cprov,sizeof(COMANDOPROV),0); //enviamos mensaje
			recv(s,&v,sizeof(RESPUESTA),MSG_WAITALL);//recibimos respuesta
			//printf("Mensaje de respuesta recibido. v.valor=%d\n",v.valor);
			if(v.valor==0)
			{
				return 0;
			}
			else
			{
				return 1;
			} 
			close(s);
		}
		
	}
	else return 1;
}
/* Parametros de entrada: proveedor,contra. Para pasarle el nombre y 
 * contraseña del proveedor .
 * Función: esta función comprueba si el proveedor esta dado de alta.
 * Parametro de salida: 0 si no existe, 1 si existe. 
 * */
int comprueba(char usuario[20],char contra[20]) //comprueba si el usuario existe
{
	PROV usu;
	int d;
	d=open("./Servidor/registroprov",O_RDONLY);
	while(read(d,&usu,sizeof(USUARIO))!=EOF || usu.nombre==usuario)
	{
		if(usu.nombre==usuario && usu.contra==contra)
		{
			return 0;
		}
		else return 1;
	}
	close(d);
	if(d==0)return 0;
	else return 1;
}
/* Parametros de entrada: servidor,proveedor,contra. 
 * Para pasarle la ip del servidor, y el nombre y contraseña del proveedor.
 * Función: esta función que nos muestra un menu del proveedor.
 * Parametro de salida: void.
 * */
void submenu(char servidor[20],char prov[20],char contra[10])
{
	char op;
	do
		{
			system("clear");
			printf("PROVEEDOR: %s\n",prov);
			printf("===========================\n");
			printf("A. Acceder\n");
			printf("B. Crear usuario\n");
			printf("C. Borrar usuario\n");
			printf("S. Salir\n");
			printf("\n");
			printf("Introduce una opcion: ");
			scanf(" %c",&op);
			switch(op)
			{
				case 'A':
				case 'a':
							system("clear");
							accederuprov(servidor,prov,contra); //alta de usuario
							break;
				case 'B':
				case 'b':
							system("clear");
							altauprov(servidor,prov,contra); //alta de usuario
							break;
				case 'C':
				case 'c':
							system("clear");
							eliminaruprov(servidor,prov,contra); //eliminar usuario
							break;
				case 'S':
				case 's':
							system("clear");
							printf("Hasta la proxima...");
						 	sleep(2);
							break;
				default:
							printf("La opcion escogida no es correcta...\n");
			}
		}
	while(op!='S' || op!='s');
}
/* Parametros de entrada: prov,usuario,contra. 
 * Para pasarle el nombre del proveedor al que pertenece y el usuario 
 * y contraseña del usuario.
 * Función: esta función comprueba si el usuario esta dado de alta.
 * Parametro de salida: 0 si no existe, 1 si existe. 
 * */
int compruebausu(char prov[20],char usuario[20],char contra[20])
{
	PROV usu;
	int d;
	d=open("./Servidor/registroprov",O_RDONLY);
	while(read(d,&usu,sizeof(PROV))!=EOF || usu.nombre==usuario)
	{
		if(usu.proveedor==prov && usu.nombre==usuario && usu.contrau==contra)
		{
			return 0;
		}
		else return 1;
	}
	close(d);
	if(d==0)return 0;
	else return 1;
}
/* Parametros de entrada: servidor. Para pasarle la ip del servidor.
 * Función: esta función recoge los datos del proveedor que queremos 
 * dar de alta.
 * Parametro de salida: void. 
 * */
void altaprov(char servidor[20])
{
	char ruta[256],proveedor[20],contra[10],comando[20];
	int res;
	printf("DAR DE ALTA\n");
	printf("===========\n");
	printf("\n");
	printf("Introduce el proveedor: ");
	scanf(" %s",proveedor);
	printf("\nIntroduce la contraseña del proveedor: ");
	scanf(" %s",contra);
	strcpy(comando,"ALTAP");
	printf("\nLlama a comandoservidorp\n");
	res=comandoservidorp(servidor,comando,proveedor,contra,"","");//comprueba el proveedor
	printf("valor de res en Altaup %d\n",res);
	if(res==0)
	{
		strcpy(ruta,"./Proveedores/");
		strcat(ruta,proveedor);
		mkdir(ruta,0777);
		printf("¡¡Felicidades!!\n");
		printf("El proveedor se ha creado con exito.\n");
		sleep(2);
	}
	else 
	{
		printf("Entra en respuesta 1\n");
		printf("El proveedor no ha podido ser creado.\n");
		sleep(2);
	}
}
/* Parametros de entrada: servidor,proveedor,contra. Para recoger los
 * datos del usuario que queremos dar de alta.
 * Función: esta función que recoge los datos del usuario que queremos 
 * dar de alta.
 * Parametro de salida: void. 
 * */
void altauprov(char servidor[20],char proveedor[20],char contra[20])
{
	char ruta[256],nombre[20],contrau[10],comando[20];
	int res;
	printf("DAR DE ALTA\n");
	printf("===========\n");
	printf("\n");
	printf("Introduce el usuario: ");
	scanf("%s",nombre);
	printf("\nIntroduce la contraseña: ");
	scanf("%s",contrau);
	strcpy(comando,"ALTAUP");
	res=comandoservidorp(servidor,comando,proveedor,contra,nombre,contrau);//comprueba el proveedor
	printf("valor de res en Altaup %d\n",res);
	if(res==0)
	{
		strcpy(ruta,"./Proveedores/");
		strcat(ruta,proveedor);
		strcat(ruta,"/");
		strcat(ruta,nombre);
		mkdir(ruta,0777);
		printf("¡¡Felicidades!!\n");
		printf("El usuario se ha creado con exito.\n");
		sleep(2);
	}
	else 
	{
		printf("El proveedor no ha podido ser creado.\n");
		sleep(2);
	}
}
/* Parametros de entrada: servidor. Para pasarle la ip del servidor.
 * Función: esta función que recoge los datos del proveedor a eliminar 
 * y los comprueba en el servidor.
 * parametro de salida: void. 
 * */
void eliminarprov(char servidor[20])
{
	char ruta[256],comando[20];
	int res;
	PROV prov;
	printf("BORRAR PROVEEDOR\n");
	printf("================\n");
	printf("\n");
	printf("Introduce el proveedor a borrar: ");
	scanf(" %s",prov.proveedor);
	printf("\nIntroduce la contraseña del proveedor a borrar: ");
	scanf(" %s",prov.contra);
	strcpy(comando,"BAJAP");
	res = comandoservidorp(servidor,comando,prov.proveedor,prov.contra,"","");//comprueba usuario y contraseña
	if(res==0)
	{
		strcpy(ruta,"./Proveedores/");
		strcat(ruta,prov.proveedor);
		borrarDir(ruta);//borra sus carpetas
		printf("El proveedor ha sido eliminado\n");
		sleep(2);
	}
	else 
	{
		printf("El proveedor no ha podido ser eliminado\n");
		sleep(2);
	}
}
/* Parametros de entrada: servidor,proveedor,contra. Para pasarle la ip
 * del servidor, y el nombre y contraseña del usuario.
 * Función: esta función recoge los datos del usuario que queremos 
 * eliminar y lo comprueba en el servidor.
 * parametro de salida: void. 
 * */
void eliminaruprov(char servidor[20],char proveedor[20], char contra[10])
{
	char ruta[256],comando[20];
	int res;
	PROV prov;
	strcpy(prov.proveedor,proveedor);
	strcpy(prov.contra,contra);
	printf("BORRAR USUARIO DE PROVEEDOR\n");
	printf("===========================\n");
	printf("\n");
	printf("Introduce el usuario a borrar: ");
	scanf(" %s",prov.nombre);
	printf("\nIntroduce la contraseña del usuario a borrar: ");
	scanf(" %s",prov.contrau);
	strcpy(comando,"BAJAP");
	res = comandoservidorp(servidor,comando,prov.proveedor,prov.contra,prov.nombre,prov.contrau);//comprueba usuario y contraseña
	if(res==0)
	{
		strcpy(ruta,"./Proveedores/");
		strcat(ruta,prov.proveedor);
		strcat(ruta,"/");
		strcat(ruta,prov.nombre);
		borrarDir(ruta);//borra sus carpetas
		printf("El usuario ha sido eliminado\n");
		sleep(2);
	}
	else 
	{
		printf("El usuario no ha podido ser eliminado\n");
		sleep(2);
	}
}
/* Parametros de entrada: ruta.
 * Función: esta función elimina recursivamente cualquier directorio.
 * parametro de salida: void. 
 * */
void borrarDir(char ruta[256])
{	
	struct dirent *entrada;
	DIR *dir;
	char tmp[200];
	dir=opendir(ruta);
	if (dir==NULL)
	    printf("Error en directorio %s\n",ruta);
	else
	while((entrada=readdir(dir)))
	{
		if((entrada->d_type)!=DT_DIR)
		{
			strcpy(tmp,ruta);
			strcat(tmp,entrada->d_name);
			printf("Borrando fichero %s\n",tmp);
			remove(tmp);
		}
		else
		{
			if((strcmp(entrada->d_name,".")!=0)&&(strcmp(entrada->d_name,"..")!=0))
			{
				strcpy(tmp,ruta);
				
				strcat(tmp,entrada->d_name);
				strcat(tmp,"/");
				printf("Entrando en directorio %s\n",tmp);
				borrarDir(tmp);
				printf("Borrando directorio %s\n",tmp);
				rmdir(tmp);
			}
		}
	}
	closedir(dir);
	rmdir(ruta);
}
/* Parametros de entrada: prov,usuario. Para pasarle el nombre del 
 * proveedor y el del usuario.
 * Función: esta función recoge los datos del libro y los envia a una 
 * cola de mensajes.
 * parametro de salida:void. 
 * */
void intro(char prov[20],char usuario[20])
{
	int msgid;
	COMANDOLIBROPROV clibro;
	
	msgid=msgget(KEYC,0666);//nos enganchamos a la cola

	//introduccimos los datos del libro
	printf("INTRODUCIR LIBRO\n");
	printf("================\n");
	fflush(stdin);
	printf("\nIntroduce el titulo: ");
	scanf("%s",clibro.titulo);
	fflush(stdin);
	printf("\nIntroduce el autor: ");
	scanf("%s",clibro.autor);
	fflush(stdin);
	clibro.tipo=1;
	printf("\nIntroduce el numero de paginas: ");
	scanf(" %d",&clibro.pag);
	fflush(stdin);
	printf("\nIntroduce el ISBN: ");
	scanf(" %d",&clibro.isbn);
	fflush(stdin);
	printf("\nIntroduce el fecha de adquisicion: ");
	scanf("%s",clibro.adq);
	fflush(stdin);
	printf("\nIntroduce el proveedor: ");
	scanf("%s",clibro.prov);
	fflush(stdin);
	printf("\nIntroduce el tema: ");
	scanf("%s",clibro.tema);
	fflush(stdin);
	printf("\nIntroduce el texto del libro: \n");
	scanf("%s",clibro.texto);
	fflush(stdin);
	msgsnd(msgid,&clibro,sizeof(COMANDOLIBROPROV),0);
	pn->enviados++;
}
