#include "milibreria.h"
void menu(void);
void crearToS(void);
void creartema(char ruta[256]);
void crearsubtema(char ruta[256]);
void borrarToS(void);
void borrartema(char ruta[256]);
void borrarsubtema(char ruta[256]);
void blibro(void);
void buscaYborra(char tema[20], char ruta[255]);
void funcionsocketP(void);
void funcionsocketU(void);
void funcionsocketLibrosU(void);
void funcionsocketLibrosP(void);
int compruebausu(char usuario[20],char contra[10]);
int compruebauprov(char proveedor[20],char contra[10],char usuario[20],char contrau[10]);
int compruebaprov(char proveedor[20],char contra[10]);
int listar(char ruta[256]);
void crear (char ruta[256]);
void borrar(char ruta[256]);
void borrarusu(char usuario[20],char contra[10]);
void borraruprov(char proveedor[20],char contra[10],char usuario[20],char contrau[10]);
void borrarprov(char proveedor[20],char contra[10]);
void guardaLibro(COMANDOLIBROPROV clibro);
void partir(char *original, char * parte1, char * parte2);
int crearListalibros (char lista[500],char ruta[255]);
COMANDOLIBROUSU buscaLibro(char libro[50],char ruta[255]);
char* substr (char* cadena, int comienzo, int longitud);
void listaservidor (char ruta[255]);

/* Parametros de entrada: void.
 * Función: principal
 * Parametro de salida: 0
 * */
int main()
{
	int i=1,j=1,k=1,l=1,fu,fp;
	mkdir("./Servidor",0700);
	mkdir("./Servidor/Temas",0700);
	fu=open("./Servidor/registrousu",O_CREAT|O_RDWR|O_APPEND,0777);
	fp=open("./Servidor/registroprov",O_CREAT|O_RDWR|O_APPEND,0777);
	close(fu);
	close(fp);
	i=fork();
	switch(i)
	{
		case -1:
				printf("Error al hacer el fork\n");
				sleep(2);
				break;
		case 0:
				funcionsocketP();//conexion del socket proveedor
				break;
		default:
				if(i!=0)
				{
					j=fork();
					switch(j)
					{
						case -1:
								printf("Error");
								break;
						case 0:
								funcionsocketU();//socket comprobacion de usuarios
								break;
						default:
								if(j!=0)
								{
									k=fork();
									switch(k)
									{
										case -1:
												printf("Error");
												break;
										case 0:
												funcionsocketLibrosP();//socket comprobacion de usuarios
												break;
										default:
											if(k!=0)
											{
												l=fork();
												switch(l)
												{
													case -1:
														printf("Error");
														break;
													case 0:
														funcionsocketLibrosU();
														sleep(2);
														break;
													default:
														menu();//menu principal
												}
											}
												
									}
								}
			}
		}
	}
	return 0;
}
/* Parametros de entrada: void
 * Función: Recibe los mensajes del proveedor y guarda el libro en local
 * Parametro de salida: void
 * */
void funcionsocketLibrosP(void)
{
	COMANDOLIBROPROV clibro;
	struct sockaddr_in dir, dircli;
	int sd,ss;
	int lon;
	
	sd=socket(AF_INET,SOCK_STREAM,0);
	if(sd<0)
	{
		printf("Error en el socket\n");
		exit(1);
	}
	//inicializamos la estructura
	bzero(&dir,sizeof(dir));
	dir.sin_family=AF_INET;
	dir.sin_port=htons(1041);
	dir.sin_addr.s_addr=INADDR_ANY;
	
	if(bind(sd,(struct sockaddr *)&dir,sizeof(dir))<0)
	{
		printf("Error en el bin socketLibrosP\n");
		exit(1);
	}
	
	listen(sd,3);
	while(1)
	{
		lon=sizeof(dircli);
		ss=accept(sd,(struct sockaddr *)&dircli,(socklen_t *)&lon);
		if(ss==-1)
		{
			printf("Error en la conexion\n");
		}
		else
		{
			recv(ss,&clibro,sizeof(COMANDOLIBROPROV),MSG_WAITALL);//recibimos un mensaje
			//printf("Libro recibido\n");
			guardaLibro(clibro);
			close(ss);
		}
	}
}
/* Parametros de entrada: void
 * Función: Recibe los datos del usuario, comprueba la accion que 
 * quiere realizar:LOGIN,ALTA,BAJA.
 * Parametro de salida: 0 si el resultado de la comprobacion es correcto,
 * 1 si es erroneo.
 * */
void funcionsocketU(void)
{
	COMANDOUSU cusu;
	USUARIO usu;
	RESPUESTA v;
	struct sockaddr_in dir, dircli;
	int sd,ss,d;
	int lon,res;
	
	sd=socket(AF_INET,SOCK_STREAM,0);
	if(sd<0)
	{
		printf("Error en el socket\n");
		exit(1);
	}
	//inicializamos la estructura
	bzero(&dir,sizeof(dir));
	dir.sin_family=AF_INET;
	dir.sin_port=htons(1050);
	dir.sin_addr.s_addr=INADDR_ANY;
	
	if(bind(sd,(struct sockaddr *)&dir,sizeof(dir))<0)
	{
		printf("Error en el bin socketU\n");
		exit(1);
	}
	
	listen(sd,3);
	while(1)
	{
		lon=sizeof(dircli);
		ss=accept(sd,(struct sockaddr *)&dircli,(socklen_t *)&lon);
		if(ss==-1)
		{
			printf("Error en la conexion\n");
		}
		else
		{
			recv(ss,&cusu,sizeof(COMANDOUSU),MSG_WAITALL);
			//printf("Mensaje recibido -> Analizando comando:%s\n",cusu.comando);
			if (strcmp(cusu.comando,"LOGIN")==0)
			{
				res=compruebausu(cusu.nombre,cusu.contrau);
				if(res==0)
				{
					v.valor=0;
					send(ss,&v,sizeof(RESPUESTA),0);
				} 
				else
				{
					v.valor=1;
					send(ss,&v,sizeof(RESPUESTA),0);
				}
				close(ss);
			}
			else if (strcmp(cusu.comando,"ALTA")==0)
			{
				res=compruebausu(cusu.nombre,cusu.contrau);
				//printf("res=%d\n",res);
				if(res==0)
				{
					v.valor=1;
					send(ss,&v,sizeof(RESPUESTA),0);
				} 
				else
				{
					strcpy(usu.nombre,cusu.nombre);
					strcpy(usu.contra,cusu.contrau);
					d=open("./Servidor/registrousu",O_WRONLY|O_APPEND);
					write(d,&usu,sizeof(USUARIO));
					close(d);
					v.valor=0;
					send(ss,&v,sizeof(RESPUESTA),0);
				}
			}
			else if (strcmp(cusu.comando,"BAJA")==0)
			{
				res=compruebausu(cusu.nombre,cusu.contrau);
				if(res==0)
				{
					borrarusu(cusu.nombre,cusu.contrau);
					v.valor=0;
					send(ss,&v,sizeof(RESPUESTA),0);
				} 
				else
				{
					v.valor=1;
					send(ss,&v,sizeof(RESPUESTA),0);
				}
			}
			else
			{
				v.valor=1;
				send(ss,&v,sizeof(RESPUESTA),0);
			}
		}
	}
}
/* Parametros de entrada: void
 * Función: Recibe los datos del proveedor, y del usuario del proveedor
 * comprueba la accion que quiere realizar:LOGIN,ALTA,BAJA,LOGINU,ALTAU,BAJAU.
 * Parametro de salida: 0 si el resultado de la comprobacion es correcto,
 * 1 si es erroneo.
 * */
void funcionsocketP(void)
{
	COMANDOPROV cprov;
	PROV prov;
	RESPUESTA v;
	struct sockaddr_in dir, dircli;
	int sd,ss,d;
	int lon,res;
	
	sd=socket(AF_INET,SOCK_STREAM,0);
	if(sd<0)
	{
		printf("Error en el socket\n");
		exit(1);
	}
	//inicializamos la estructura
	bzero(&dir,sizeof(dir));
	dir.sin_family=AF_INET;
	dir.sin_port=htons(1040);
	dir.sin_addr.s_addr=INADDR_ANY;
	
	if(bind(sd,(struct sockaddr *)&dir,sizeof(dir))<0)
	{
		printf("Error en el bind socketP\n");
		exit(1);
	}
	
	listen(sd,3);
	while(1)
	{
		lon=sizeof(dircli);
		ss=accept(sd,(struct sockaddr *)&dircli,(socklen_t *)&lon);
		if(ss==-1)
		{
			printf("Error en la conexion\n");
		}
		else
		{
			recv(ss,&cprov,sizeof(COMANDOPROV),MSG_WAITALL);
			//printf("Mensaje recibido -> Analizando comando:%s\n",cprov.comando);
			
			if ((strcmp(cprov.comando,"LOGINP"))==0)
			{
				//printf("COMANDO: LOGINP \n");
				res=compruebaprov(cprov.proveedor,cprov.contra);
				if(res==0)
				{
					v.valor=0;
					send(ss,&v,sizeof(RESPUESTA),0);
				} 
				else
				{
					v.valor=1;
					send(ss,&v,sizeof(RESPUESTA),0);
				}
				close(ss);
			}
			else if ((strcmp(cprov.comando,"LOGINU"))==0)
			{
				//printf("COMANDO: LOGINU \n");
				res=compruebauprov(cprov.proveedor,cprov.contra,cprov.nombre,cprov.contrau);			
				if(res==0)
				{
					v.valor=0;
					send(ss,&v,sizeof(RESPUESTA),0);
				} 
				else
				{
					v.valor=1;
					send(ss,&v,sizeof(RESPUESTA),0);
				}
				close(ss);
			}
			else if ((strcmp(cprov.comando,"ALTAP"))==0)
			{
				//printf("COMANDO: ALTAP \n");
				res=compruebaprov(cprov.proveedor,cprov.contra);
				//printf("valor de res en Altap %d\n",res);
				if(res==0)
				{
					v.valor=1;
					send(ss,&v,sizeof(RESPUESTA),0);
				} 
				else
				{
					strcpy(prov.proveedor,cprov.proveedor);
					strcpy(prov.contra,cprov.contra);
					d=open("./Servidor/registroprov",O_WRONLY|O_APPEND);
					write(d,&prov,sizeof(PROV));
					close(d);
					v.valor=0;
					send(ss,&v,sizeof(RESPUESTA),0);
				}
			}
			else if ((strcmp(cprov.comando,"BAJAP"))==0)
			{
				//printf("COMANDO: BAJAP \n");
				res=compruebaprov(cprov.proveedor,cprov.contra);
				if(res==0)
				{
					borrarprov(cprov.proveedor,cprov.contra);
					v.valor=0;
					send(ss,&v,sizeof(RESPUESTA),0);
				} 
				else
				{
					v.valor=1;
					send(ss,&v,sizeof(RESPUESTA),0);
				}
			}
			else if ((strcmp(cprov.comando,"ALTAUP"))==0)
			{
				//printf("COMANDO: ALTAUP \n");
				res=compruebauprov(cprov.proveedor,cprov.contra,cprov.nombre,cprov.contrau);
				//printf("valor de res en Altaup %d\n",res);
				if(res==0)
				{
					v.valor=1;
					send(ss,&v,sizeof(RESPUESTA),0);
				} 
				else
				{
					strcpy(prov.proveedor,cprov.proveedor);
					strcpy(prov.contra,cprov.contra);
					strcpy(prov.nombre,cprov.nombre);
					strcpy(prov.contrau,cprov.contrau);
					d=open("./Servidor/registroprov",O_WRONLY|O_APPEND);
					write(d,&prov,sizeof(PROV));
					close(d);
					v.valor=0;
					send(ss,&v,sizeof(RESPUESTA),0);
				}
			}
			else if ((strcmp(cprov.comando,"BAJAUP"))==0)
			{
				//printf("COMANDO: BAJAUP \n");
				res=compruebauprov(cprov.proveedor,cprov.contra,cprov.nombre,cprov.contrau);
				if(res==0)
				{
					borraruprov(cprov.proveedor,cprov.contra,cprov.nombre,cprov.contrau);
					v.valor=0;
					send(ss,&v,sizeof(RESPUESTA),0);
				} 
				else
				{
					v.valor=1;
					send(ss,&v,sizeof(RESPUESTA),0);
				}
			}			
			else
			{
				//printf("COMANDO NO RECONOCIDO. COMANDO=%s\n",cprov.comando);
				v.valor=1;
				send(ss,&v,sizeof(RESPUESTA),0);
			}
		}
	}
}
/* Parametros de entrada: usuario,contra. 
 * Para pasarle el nombre el usuario y contraseña del usuario.
 * Función: esta función comprueba si el usuario esta dado de alta.
 * Parametro de salida: 0 si no existe, 1 si existe. 
 * */
int compruebausu(char usuario[20],char contra[10])
{
	USUARIO usu;
	int d;
	d=open("./Servidor/registrousu",O_RDONLY);
	while(read(d,&usu,sizeof(USUARIO))!=0)
	{
		if((strcmp(usu.nombre,usuario)==0) && (strcmp(usu.contra,contra)==0))
		{
			printf("El usuario existe en el servidor.");
			close(d);
			return 0;
		}
	}
	printf("El usuario no existe en el servidor.");
	close(d);
	return 1;
}
/* Parametros de entrada: proveedor,contra,usuario,contrau. 
 * Para pasarle el nombre del proveedor y contraseña, al que pertenece 
 * y el usuario y contraseña del usuario.
 * Función: esta función comprueba si el usuario proveedor esta dado de alta.
 * Parametro de salida: 0 si no existe, 1 si existe. 
 * */
int compruebauprov(char proveedor[20],char contra[10],char usuario[20],char contrau[10])
{
	PROV prov;
	int d;
	d=open("./Servidor/registroprov",O_RDONLY);
	while(read(d,&prov,sizeof(PROV))!=0)
	{
		if ((strcmp(prov.proveedor,proveedor)==0) && (strcmp(prov.contra,contra)==0) && (strcmp(prov.nombre,usuario)==0) && (strcmp(prov.contrau,contrau)==0))
		{
			//printf("El usuario existe en el servidor para este proveedor.");
			close(d);
			return 0;
		}
	}
	//printf("El usuario no existe en el servidor para este proveedor.");
	close(d);
	return 1;
}
/* Parametros de entrada: proveedor,contra. 
 * Para pasarle el nombre del proveedor y contraseña.
 * Función: esta función comprueba si el proveedor esta dado de alta.
 * Parametro de salida: 0 si no existe, 1 si existe. 
 * */
int compruebaprov(char proveedor[20],char contra[10])
{
	PROV prov;
	int d;
	d=open("./Servidor/registroprov",O_RDONLY);
	while(read(d,&prov,sizeof(PROV))!=0)
	{
		if ((strcmp(prov.proveedor,proveedor)==0) && (strcmp(prov.contra,contra)==0))
		{
			//printf("Existe el proveedor en el servidor.\n");
			close(d);
			return 0;
		}
	}
	//printf("No existe el proveedor en el servidor.\n");
	close(d);
	return 1;
}
/* Parametros de entrada: void
 * Función: esta función nos muestra el menu principal
 * Parametro de salida: void
 * */
void menu (void)
{
	char op;
	do
		{
			system("clear");
			printf("SERVIDOR\n");
			printf("===========\n");
			printf("A. Crear tema o subtema\n");
			printf("B. Borrar tema o subtema\n");
			printf("C. Borrar libro\n");
			printf("S. Cerrar servidor\n");
			printf("\n");
			printf("Introduce una opcion: ");
			scanf(" %c",&op);
			switch(op)
			{
				case 'A':
				case 'a':
						 system("clear");
						 crearToS();
						 break;
				case 'B':
				case 'b':
						 system("clear");
						 borrarToS();
						 break;
				case 'C':
				case 'c':
						 system("clear");
						 blibro();
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
/* Parametros de entrada: void
 * Función: esta función nos muestra el menu para elegir si queremos 
 * crear un tema o subtema.
 * Parametro de salida: void
 * */
void crearToS (void)
{
	char ruta[256];
	int op;
	strcpy(ruta,"./Servidor/Temas/");
	printf("CREAR TEMA O SUBTEMA\n");
	printf("====================\n");
	printf("\n");
	printf("¿Que quieres crear?\n");
	printf("1. Tema\n");
	printf("2. Subtema\n");
	printf("\n");
	printf("Introduce una opcion: ");
	scanf(" %d",&op);
	
	switch(op)
	{
		case 1:
				system("clear");
				creartema(ruta);
				break;
		case 2:
				system("clear");
				crearsubtema(ruta);
				break;
		default:
				printf("Opcion incorrecta;");
		
	} 
}
/* Parametros de entrada: ruta. Para indicarle su situación.
 * Función: esta función nos crea un tema.
 * Parametro de salida: void
 * */
void creartema(char ruta[256])
{
	char tema[20];
	int l;
	printf("CREAR TEMA\n");
	printf("==========\n");
	printf("\n");
	printf("Los temas existentes son:\n");
	l=listar(ruta);
	if(l==1)printf("No se encontraron temas disponibles\n");
	printf("\n");
	printf("Introduce el nombre del tema: ");
	scanf("%s",tema);
	strcat(ruta,tema);
	crear(ruta);
}
/* Parametros de entrada: ruta. Para indicarle su situación.
 * Función: esta función nos crea un subtema.
 * Parametro de salida: void
 * */
void crearsubtema(char ruta[256])
{
	char tema[20],subtema[20];
	int op,l;
	printf("CREAR SUBTEMA\n");
	printf("=============\n");
	printf("\n");
	printf("Los temas existentes son:\n");
	l=listar(ruta);
	if(l==1)
	{
		printf("No se encontraron temas disponibles\n");
		printf("Introduce el nombre del subtema: ");
		scanf(" %s",tema);
		strcat(ruta,tema);
		crear(ruta);
	}
	else
	{
		printf("\n");
		printf("Introduce el nombre del tema principal: ");
		scanf(" %s",tema);
		system("clear");
		printf("CREAR SUBTEMA\n");
		printf("=============\n");
		printf("\n");
		printf("Los Subtemas existentes son:\n");
		strcat(ruta,tema);
		strcat(ruta,"/");
		l=listar(ruta);
		if(l==1)
		{
			printf("No se encontraron subtemas disponibles\n");
			printf("Introduce el nombre del subtema: ");
			scanf("%s",tema);
			strcat(ruta,tema);
			crear(ruta);
		}
		else
		{
			printf("¿Quieres crear un subtema '1' o un Sub-subtema '2' ?: ");
			scanf(" %d",&op);
			if(op==2) crearsubtema(ruta);
			else
			{
				printf("\n");
				printf("Introduce el nombre del subtema: ");
				scanf(" %s",subtema);
				strcat(ruta,subtema);
				strcat(ruta,"/");
				crear(ruta);
			}
		}
	}
}
/* Parametros de entrada: void
 * Función: esta función nos muestra el menu para elegir si queremos 
 * borrar un tema o subtema.
 * Parametro de salida: void
 * */
void borrarToS(void)
{
	char ruta[256];
	int op;
	strcpy(ruta,"./Servidor/Temas/");
	printf("BORRAR TEMA O SUBTEMA\n");
	printf("=====================\n");
	printf("\n");
	printf("¿Que quieres borrar?\n");
	printf("1. Tema\n");
	printf("2. Subtema\n");
	printf("\n");
	printf("Introduce una opcion: ");
	scanf(" %d",&op);
	switch(op)
	{
		case 1:
				borrartema(ruta);
				break;
		case 2:
				borrarsubtema(ruta);
				break;
		default:
				printf("Opcion incorrecta;");
	}
}
/* Parametros de entrada: ruta. Para indicarle su situación.
 * Función: esta función nos borra un tema.
 * Parametro de salida: void
 * */
void borrartema(char ruta[256])
{
	int l;
	char tema[20];
	printf("BORRAR TEMA\n");
	printf("===========\n");
	printf("\n");
	printf("Los temas existentes son:\n");
	l=listar(ruta);
	if(l==1)printf("No hay temas que borrar\n");
	else
	{
		printf("Introduce el nombre del tema a eliminar: ");
		scanf(" %s",tema);
		strcat(ruta,tema);
		strcat(ruta,"/");
		borrar(ruta);
	}
}
/* Parametros de entrada: ruta. Para indicarle su situación.
 * Función: esta función nos borra un subtema.
 * Parametro de salida: void
 * */
void borrarsubtema(char ruta[256])
{
	int l,op;
	char tema[20],subtema[20];
	printf("BORRAR SUBTEMA\n");
	printf("==============\n");
	printf("\n");
	printf("Los temas existentes son:\n");
	l=listar(ruta);
	if(l==1)
	{
		printf("No se encontraron temas disponibles\n");
	}
	else
	{
		printf("\n");
		printf("Introduce el nombre del tema principal: ");
		scanf(" %s",tema);
		system("clear");
		printf("BORRAR SUBTEMA\n");
		printf("==============\n");
		printf("\n");
		printf("Los Subtemas existentes son:\n");
		strcat(ruta,tema);
		strcat(ruta,"/");
		l=listar(ruta);
		if(l==1)
		{
			printf("No se encontraron subtemas disponibles\n");
		}
		else
		{
			printf("¿Quieres borrar un subtema '1' o un Sub-subtema '2' ?: ");
			scanf(" %d",&op);
			if(op==2) borrarsubtema(ruta);
			else
			{
				printf("\n");
				printf("Introduce el nombre del subtema: ");
				scanf(" %s",subtema);
				strcat(ruta,subtema);
				strcat(ruta,"/");
				borrar(ruta);
			}
		}
	}
}
/* Parametros de entrada: ruta. Para indicarle su situación.
 * Función: esta función nos mostrará un listado de los temas que 
 * tenemos disponibles en el servidor.
 * Parametro de salida: 0 si ha encontrado directorios, 1 si no.
 * */
int listar (char ruta[256])
{
	int cont=1;
	DIR *d;
	struct dirent *entrada;
	d=opendir(ruta);
	while((entrada=readdir(d))!=NULL)
		{
			if(entrada->d_type==DT_DIR)
				{
					if(entrada->d_name[0]!='.')
					{
						printf("%d. %s\n",cont,entrada->d_name);
						cont++;
					}
				}
		}
	closedir(d);
	if(cont==1)return 1;
	else return 0;
}
/* Parametros de entrada: void.
 * Función: esta función nos muestra los libros existentes en el 
 * servidor nos pide el nombre del que queremos eliminar y lo elimina.
 * Parametro de salida: void.
 * */
void blibro(void) 
{
	char tema[20],ruta[255];
	
	printf("BORRAR LIBRO\n");
	printf("============\n");
	printf("Los libros existentes en el servidor son:\n");
	printf("\n");
	strcpy(ruta,"./Servidor/Temas/");
	listaservidor(ruta);
	printf("\n");
	printf("Introduce el nombre del libro: ");
	scanf(" %s",tema);
	strcpy(ruta,"./Servidor/Temas/");
	buscaYborra(tema,ruta);//busqueda del titulo
}
/* Parametros de entrada: tema,ruta. Para indicarle nombre del libro y
 * su situación.
 * Función: esta función busca el libro y si lo encuentra lo borra.
 * Parametro de salida: void.
 * */
void buscaYborra(char tema[20],char ruta[255])
{
	char tmp[255],libro[20],des[20],op;
	strcpy(tmp,ruta);
	strcpy(libro,tema);
	strcat(libro,".txt");
	strcpy(des,tema);
	strcat(des,".des");
	DIR *d;
	struct dirent *entrada;
	//printf("Entra en busqueda con parametro=%s y ruta=%s\n",tema,ruta);
	d=opendir(ruta);
	while((entrada=readdir(d)))
		{
			if((entrada->d_type)==DT_REG)
				{
					if((strcmp(entrada->d_name,libro))==0)
					{
						printf("¿Desea borrar %s ? S o N: ",substr(entrada->d_name,0,strlen(entrada->d_name)-4));
						scanf(" %c",&op);
						if(op=='S' || op=='s')
						{
							strcpy(tmp,ruta);
							strcat(tmp,libro);
							remove(tmp);
							printf("Libro.txt borrado %s\n",tmp);
							strcpy(tmp,ruta);
							strcat(tmp,des);
							remove(tmp);
							printf("Descriptor.des borrado %s\n",tmp);
							sleep(2);
						}
					}
				}
				else if((entrada->d_type)==DT_DIR)
				{
					if((strcmp(entrada->d_name,".")!=0)&&(strcmp(entrada->d_name,"..")!=0))
					{
						strcpy(tmp,ruta);
						strcat(tmp,entrada->d_name);
						strcat(tmp,"/");
						//printf("Entra en nuevo directorio: %s\n",tmp);
						buscaYborra(tema,tmp);
					}
					else
					{
						strcpy(tmp,ruta);
					}
				}
			}
	closedir(d);
}
/* Parametros de entrada: ruta. Para indicarle su situación.
 * Función: esta función nos crea un directorio.
 * Parametro de salida: void.
 * */
void crear (char ruta[256])
{
	if(mkdir(ruta,0700)!=0)
	 {
		printf("\nEl tema ya estaba creado\n");
		sleep(3);
	 }
	 else
	 {
		printf("\nEl tema se ha creado con exito\n");
		sleep(3);
	 }
}
/* Parametros de entrada: ruta. Para indicarle su situación.
 * Función: esta función nos borra un directorio.
 * Parametro de salida: void.
 * */
void borrar(char directorio[256])
{
	struct dirent *entrada;
	DIR *dir;
	char tmp[200];
	dir=opendir(directorio);
	if (dir==NULL)
	    printf("Error en directorio %s\n",directorio);
	else
	while((entrada=readdir(dir)))
	{
		if((entrada->d_type)!=DT_DIR)
		{
			strcpy(tmp,directorio);
			strcat(tmp,entrada->d_name);
			printf("Borrando fichero %s\n",tmp);
			remove(tmp);
		}
		else
		{
			if((strcmp(entrada->d_name,".")!=0)&&(strcmp(entrada->d_name,"..")!=0))
			{
				strcpy(tmp,directorio);
				
				strcat(tmp,entrada->d_name);
				strcat(tmp,"/");
				printf("Entrando en directorio %s\n",tmp);
				borrar(tmp);
				printf("Borrando directorio %s\n",tmp);
				rmdir(tmp);
			}
		}
	}
	closedir(dir);
	rmdir(directorio);
}
/* Parametros de entrada: usuario,contra. Para indicarle el usuario y 
 * contraseña.
 * Función: esta función nos elimina un usuario del registro del servidor.
 * Parametro de salida: void.
 * */
void borrarusu(char usuario[20],char contra[10])
{
	USUARIO usu;
	int d,d2;
	d=open("./Servidor/registrousu",O_RDONLY);
	d2=open("./Servidor/registrousu2",O_CREAT|O_RDWR|O_APPEND,0777);
	while((read(d,&usu,sizeof(USUARIO))!=0) && (strcmp(usu.nombre,usuario))!=0)
	{
		write(d2,&usu,sizeof(USUARIO));
	}
	close(d2);
	close(d);
	remove("./Servidor/registrousu");
	rename("./Servidor/registrousu2","./Servidor/registrousu");
}
/* Parametros de entrada: proveedor,contra,usuario,contrau. Para 
 * indicarle el proveedor y contraseña, usuario y contraseña.
 * Función: esta función nos elimina un proveedor del registro del servidor.
 * Parametro de salida: void.
 * */
void borraruprov(char proveedor[20],char contra[10],char usuario[20],char contrau[10])
{
	PROV prov;
	int d,d2;
	d=open("./Servidor/registroprov",O_RDONLY);
	d2=open("./Servidor/registroprov2",O_CREAT|O_RDWR|O_APPEND,0777);
	while((read(d,&prov,sizeof(PROV))!=0) && (strcmp(prov.proveedor,proveedor)) && (strcmp(prov.nombre,usuario))!=0)
	{
		write(d2,&prov,sizeof(PROV));
	}
	close(d2);
	close(d);
	remove("./Servidor/registroprov");
	rename("./Servidor/registroprov2","./Servidor/registroprov");
}
/* Parametros de entrada: proveedor,contra. Para indicarle el proveedor y 
 * contraseña.
 * Función: esta función nos elimina un proveedor del registro del servidor.
 * Parametro de salida: void.
 * */
void borrarprov(char proveedor[20],char contra[10])
{
	PROV prov;
	int d,d2;
	d=open("./Servidor/registroprov",O_RDONLY);
	d2=open("./Servidor/registroprov2",O_CREAT|O_RDWR|O_APPEND,0777);
	while((read(d,&prov,sizeof(PROV))!=0) && (strcmp(prov.proveedor,proveedor))!=0)
	{
		write(d2,&prov,sizeof(PROV));
	}
	close(d2);
	close(d);
	remove("./Servidor/registroprov");
	rename("./Servidor/registroprov2","./Servidor/registroprov");
}
/* Parametros de entrada: clibro-Estructura COMANDOLIBROPROV. 
 * Contiene la informacion del libro.
 * Función: esta función nos guarda el libro recibido del proveedor, 
 * en el servidor.
 * Parametro de salida: void.
 * */
void guardaLibro(COMANDOLIBROPROV clibro)
{
	DESC des;
	LIBRO libro;
	char ruta[255],ruta2[255],tema[255],subtema[255];
	int ft,fd,creado=1;
	strcpy(ruta,"./Servidor/Temas/");
	strcpy(subtema,clibro.tema);
	strcat(subtema,"/");
	while(creado!=0)
	{
		partir(subtema,tema,subtema);
		printf("valor de tema: %s\n",tema);	
		printf("valor de subtema: %s\n",subtema);
		strcat(ruta,tema);
		strcat(ruta,"/");
		mkdir(ruta,0766);
		strcpy(tema,"");
		if(strcmp(subtema,"\0")==0)
		{
			creado=0;
		}
	}
	
	strcpy(des.titulo,clibro.titulo);//copiamos datos a la estructura
	strcpy(des.autor,clibro.autor);
	strcpy(des.adq,clibro.adq);
	strcpy(des.prov,clibro.prov);
	strcpy(des.tema,clibro.tema);
	des.pag=clibro.pag;
	des.isbn=clibro.isbn;
	des.desc=clibro.desc;
	
	strcat(ruta,clibro.titulo);
	strcat(ruta,".des");
	
	printf("Antes de crear libro ruta= %s\n",ruta);

	fd=open(ruta,O_CREAT|O_RDWR,0777);//creamos descripcion
	write(fd,&des,sizeof(DESC));
	//printf("Descriptor de libro creado\n");
	close(fd);
	
	strcpy(libro.texto,clibro.texto);//copiamos datos a la estructura
	printf("El valor de clibro.tema %s\n",clibro.tema);
	strcpy(ruta2,"./Servidor/Temas/");
	strcat(ruta2,clibro.tema);
	strcat(ruta2,"/");
	strcat(ruta2,clibro.titulo);
	strcat(ruta2,".txt");
	printf("Antes de crear libro ruta= %s\n",ruta2);
	ft=open(ruta2,O_CREAT|O_RDWR,0777);//creamos libro
	write(ft,&libro,sizeof(LIBRO));
	//printf("Libro creado\n");
	close(ft);
}
/* Parametros de entrada: void
 * Función: Recibe los mensajes del clientem diferenciando entre: 
 * LISTA y DESCARGA, para enviar un mensaje con una lista de libros 
 * disponibles o los datos del libro para su descarga.
 * Parametro de salida: void
 * */
void funcionsocketLibrosU(void)
{
	COMANDOLISTALIBROUSU clista;
	COMANDOLIBROUSU clibro;
	struct sockaddr_in dir, dircli;
	int sd,ss;
	int lon,res;
	char ruta[255];
	strcpy(ruta,"./Servidor/Temas/");
	sd=socket(AF_INET,SOCK_STREAM,0);
	if(sd<0)
	{
		printf("Error en el socket\n");
		exit(1);
	}
	//inicializamos la estructura
	bzero(&dir,sizeof(dir));
	dir.sin_family=AF_INET;
	dir.sin_port=htons(1051);
	dir.sin_addr.s_addr=INADDR_ANY;
	
	if(bind(sd,(struct sockaddr *)&dir,sizeof(dir))<0)
	{
		printf("Error en el bin socketLibrosU\n");
		exit(1);
	}
	
	listen(sd,3);
	while(1)
	{
		lon=sizeof(dircli);
		ss=accept(sd,(struct sockaddr *)&dircli,(socklen_t *)&lon);
		if(ss==-1)
		{
			printf("Error en la conexion\n");
		}
		else
		{
			recv(ss,&clista,sizeof(COMANDOLISTALIBROUSU),MSG_WAITALL);
			//printf("Peticion recibida. comando=%s\n",clista.comando);
			if (strcmp(clista.comando,"LISTA")==0)
			{
				strcpy(clista.lista,"");
				res=crearListalibros(clista.lista,ruta);	
				if(res==0)
				{
					//printf("Respuesta de lista enviada\n");
					send(ss,&clista,sizeof(COMANDOLISTALIBROUSU),0);
				} 
				else
				{
					//printf("Respuesta de lista enviada con lista vacia\n");
					strcpy(clista.lista,"");
					send(ss,&clista,sizeof(COMANDOLISTALIBROUSU),0);
				}
				close(ss);
			}
			else if (strcmp(clista.comando,"DESCARGA")==0)
			{
				strcpy(clibro.titulo,clista.lista);
				clibro=buscaLibro(clibro.titulo,ruta);
				//printf("Enviado libro en formato COMANDOLIBROUSU %s\n",clibro.titulo);
				send(ss,&clibro,sizeof(COMANDOLIBROUSU),0);
				
			}
			else
			{
				//printf("Error: Comando desconocido.");
				strcpy(clista.lista,"");
				send(ss,&clista,sizeof(COMANDOLISTALIBROUSU),0);
			}
		}
	}
}
/* Parametros de entrada: original,parte1,parte2. Le pasamos la ruta original,
 * y dos variables mas donde vamos a guardar las particiones.
 * Función: Esta funcion va a coger el contenido de la variable tema, 
 * del libro, y va a dividir esta variable para poder crear directorio a directorio.
 * Parametro de salida: void
 * */
void partir(char *original, char *parte1, char *parte2)
{
  int i=0;
  int j=0;
  int noencontrado=0;
  
  while (original[i]!='\0')
  {
     if (noencontrado==0)
     {
        if (original[i]=='/')
        {
           noencontrado=1;
           parte1[i]='\0';
        }
        else
        {
           parte1[i]=original[i];
        }
     }
     else
     {
        parte2[j++]=original[i];
        //noencontrado=0;
     }
     i++;
  }
  parte2[j++]='\0';
}
/* Parametros de entrada: lista,ruta. Para devolver a la funcion que 
 * le llama su contenido e indicarle su localizacion.
 * Función: Esta función crea una lista de libros existentes en el
 * servidor, para enviarle un mensaje al cliente con el resultado.
 * Parametro de salida: 0 si hay algun resultado, 1 en caso contrario.
 * */
int crearListalibros (char lista[200],char ruta[255])
{
	int longitud,res;
	char tmp[255],*extension,*libro;
	DIR *d;
	struct dirent *entrada;
	//printf("\nEntra en %s\n",ruta);
	d=opendir(ruta);	
	
	while((entrada=readdir(d)))
		{
			if((entrada->d_type)==DT_REG)
				{
					strcpy(tmp,ruta);
					strcat(tmp,"/");
					strcat(tmp,entrada->d_name);
					//printf("No es directorio: %s\n",tmp);
					longitud=strlen(entrada->d_name);
					extension=substr(entrada->d_name,longitud-4,4);
					//printf("extension=%s\n",extension);
					if(strcmp(extension,".txt")==0)
					{
						libro=substr(entrada->d_name,0,longitud-4);
						strcat(lista,libro);
						strcat(lista,"/");
						//printf("lista actual = %s\n",lista);
					}
				}		
				else
				{
					if((strcmp(entrada->d_name,".")!=0)&&(strcmp(entrada->d_name,"..")!=0))
					{
						
						strcpy(tmp,ruta);
						strcat(tmp,entrada->d_name);
						strcat(tmp,"/");
						//printf("Entra en nuevo directorio: %s\n",tmp);
						res=crearListalibros(lista,tmp);
						if(res!=0) return 1;
					}
					else
					{
						strcpy(tmp,ruta);
					}
				}
		}
		return 0;
}
/* Parametros de entrada: ruta. Para indicar su situacion.
 * Función: Esta funcion nos hace un listado de los libros que hay en el servidor.
 * Parametro de salida: void
 * */
void listaservidor (char ruta[255])
{
	int longitud;
	char tmp[255],*extension,*libro;
	DIR *d;
	struct dirent *entrada;
	
	d=opendir(ruta);
	while((entrada=readdir(d)))
		{
			if((entrada->d_type)!=DT_DIR)
				{
					strcpy(tmp,ruta);
					strcat(tmp,entrada->d_name);
					//printf("No es directorio: %s\n",tmp);
					longitud=strlen(entrada->d_name);
					extension=substr(entrada->d_name,longitud-4,4);
					//printf("extension=%s\n",extension);
					if(strcmp(extension,".txt")==0)
					{
						libro=substr(entrada->d_name,0,longitud-4);
						printf("%s\n",libro);
					}
				}		
				else
				{
					if((strcmp(entrada->d_name,".")!=0)&&(strcmp(entrada->d_name,"..")!=0))
					{
						strcpy(tmp,ruta);
						strcat(tmp,entrada->d_name);
						strcat(tmp,"/");
						//printf("Entra en nuevo directorio: %s\n",tmp);
						listaservidor(tmp);
					}
				}
		}
}
/* Parametros de entrada: seleccion,ruta. Para indicarle el nombre del 
 * libro y su situacion.
 * Función: ESta función busca un libro en concreto en el servidor.
 * Parametro de salida: COMANDOLIBROUSU
 * */
COMANDOLIBROUSU buscaLibro(char seleccion[20],char ruta[255])
{
	char tmp[255],libro[20],des[20];
	int fl,fd;
	COMANDOLIBROUSU clibro;
	LIBRO lib;
	DESC desc;
	strcpy(tmp,ruta);
	strcpy(libro,seleccion);
	strcat(libro,".txt");
	strcpy(des,seleccion);
	strcat(des,".des");
	DIR *d;
	struct dirent *entrada;
	//printf("Entra en busqueda con parametro=%s y ruta=%s\n",tema,ruta);
	d=opendir(ruta);
	while((entrada=readdir(d)))
		{
			if((entrada->d_type)==DT_REG)
				{
					if((strcmp(entrada->d_name,libro))==0)
					{
						strcat(tmp,entrada->d_name);
						fl=open(tmp,O_RDONLY);//leemos libro
						read(fl,&lib,sizeof(LIBRO));
						strcpy(clibro.texto,lib.texto);
						close(fl);				
					}
					else if((strcmp(entrada->d_name,des))==0)
					{
						strcat(tmp,entrada->d_name);
						fd=open(tmp,O_RDONLY);//leemos libro
						read(fd,&desc,sizeof(DESC));
						close(fd);
												
						strcpy(clibro.titulo,desc.titulo);
						strcpy(clibro.autor,desc.autor);
						strcpy(clibro.adq,desc.adq);
						strcpy(clibro.prov,desc.prov);
						strcpy(clibro.tema,desc.tema);
						clibro.pag=desc.pag;
						clibro.isbn=desc.pag;
						clibro.desc=desc.desc;
					}
				}
				else if((entrada->d_type)==DT_DIR)
				{
					if((strcmp(entrada->d_name,".")!=0)&&(strcmp(entrada->d_name,"..")!=0))
					{
						strcpy(tmp,ruta);
						strcat(tmp,entrada->d_name);
						strcat(tmp,"/");
						clibro=buscaLibro(seleccion,tmp);
					}
				}
		}
		return clibro;
}
/* Parametros de entrada: cadena,comienzo,longitud. Para indicarle la 
 * cadena que queremos dividir,el comienzo para dividir y el donde queremos cortar.
 * Función: esta función nos divide una cadena, para separar los nombres 
 * de los archivos de sus extensiones.
 * Parametro de salida: nuevo
 * */
char* substr (char* cadena, int comienzo, int longitud)
{
	char* nuevo;
	if (longitud==0)
	{
		longitud=strlen(cadena)-comienzo-1;
	}
	
	nuevo= (char*)malloc(sizeof(char) * longitud);
	strncpy(nuevo,cadena+comienzo,longitud);
	return nuevo;
}
