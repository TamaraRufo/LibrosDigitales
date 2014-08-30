#include "milibreria.h"
void menu (char servidor[20]);
void acceder(char servidor[20]);
void submenu(char servidor[20],char usuario[20],char contra[10]);
void disponibles(char servidor[20],char usuario[20]);
void copiar(char ruta[256],char tema[20]);
void leerlibro(char usuario[20]);
void eliminarlibro(char usuario[20]);
void alta(char servidor[20]);
void eliminar(char servidor[20]);
void borrar (char ruta[256]);
int comandoservidor(char servidor[20],char comando[20],char usuario[20],char contra[10]);
int comandoservidorDescargar(char servidor[20],char comando[20],char libro[50],char usuario[20]);
int comandoservidorLista(char servidor[20],char comando[20],char lista[500]);
int comprobardir(char usuario[20]);
int listar (char ruta[256]);
void partir(char *original, char *parte1, char *parte2);
void guardaLibro(COMANDOLIBROUSU clibro,char usuario[20]);

/* Parametros de entrada: argc,argv para darle la ip del servidor
 * Función: principal
 * Parametro de salida: 0
 * */
int main(int argc, char *argv[])
{
	if(argc<2)printf("Error en parametros\n");
	else
	{
		mkdir("./Cliente",0700);
		menu(argv[1]);
	}
	return 0;	
}
/* Parametros de entrada: servidor, para pasarle la ip
 * Función: esta función nos muestra el menu principal
 * Parametro de salida: void
 * */
void menu (char servidor[20])
{
	char op;
	do
		{
			system("clear");
			printf("CLIENTES\n");
			printf("=========\n");
			printf("A. Acceder\n");
			printf("B. Dar de alta\n");
			printf("C. Eliminar usuario\n");
			printf("S. Cerrar\n");
			printf("\n");
			printf("Introduce una opcion: ");
			scanf(" %c",&op);
			switch(op)
			{
				case 'A':
				case 'a':
						 system("clear");
						 acceder(servidor);						 
						 break;
				case 'B':
				case 'b':
						 system("clear");
						 alta(servidor);			 
						 break;
				case 'C':
				case 'c':
						 system("clear");
						 eliminar(servidor);
						 break;
				case 'S':
				case 's':
						 system("clear");
						 printf("Hasta la proxima...");
						 sleep(5);
						 break;
				default:
						 printf("La opcion escogida no es correcta...\n");
			}
		}
	while(op!='S' || op!='s');
}
/* Parametros de entrada: servidor, para pasarle la ip
 * Función: esta función accede al servidor con el nombre y contraseña de un cliente
 * Parametro de salida: void
 * */
void acceder(char servidor[20])
{
	char usuario[20],contra[20],comando[20];
	int res;
	printf("ACCEDER\n");
	printf("=======\n");
	printf("\n");
	printf("Introduce el usuario: ");
	scanf("%s",usuario);
	printf("\nIntroduce la contraseña: ");
	scanf("%s",contra);
	strcpy(comando,"LOGIN");
	res=comandoservidor(servidor,comando,usuario,contra);
	if(res==0)
	{
		printf("\n");
		printf("Bienvenido al sistema usuario %s\n",usuario);
		sleep(2);
		submenu(servidor,usuario,contra);
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
 * parametro de salida: 0 si se ha ejecutado correctamente, 1 en caso contrario.
 * */
int comandoservidor(char servidor[20],char comando[20],char usuario[20],char contra[10])
{
	COMANDOUSU cusu;
	RESPUESTA v;
	struct hostent *h;
	struct sockaddr_in d;
	int s;
	
	strcpy(cusu.comando,comando);
	strcpy(cusu.nombre,usuario);
	strcpy(cusu.contrau,contra);
	
	h=gethostbyname(servidor);
	if(h)
	{
		bzero(&d,sizeof(d));
		d.sin_family=AF_INET;
		d.sin_port=htons(1050);
		bcopy(h->h_addr,&d.sin_addr,4);
		
		s=socket(AF_INET,SOCK_STREAM,0);
		if(s<0)
		{
			printf("Error en el socket\n");
			return 1;
		}
		if(connect(s,(struct sockaddr *)&d,sizeof(d)))
		{
			printf("Error al conectar\n");return 1;
		}
		else
		{
			send(s,&cusu,sizeof(COMANDOUSU),0); //enviamos mensaje
			recv(s,&v,sizeof(RESPUESTA),MSG_WAITALL);//recibimos respuesta
			close(s);
			if(v.valor==0)return 0;
			else return 1;
		}
		
	}
	else 
	{
		//printf("Error al solucionar localhost\n");
		return 1;		
	}
}
/* Parametros de entrada: servidor,comando,libro,usuario. 
 * Para pasarle la ip del servidor, el comando a ejecutar, el libro y 
 * el usuario.
 * Función: esta función recibe por socket un mensaje al servidor con los datos.
 * parametro de salida: 0 si se ha ejecutado correctamente, 1 en caso contrario. 
 * */
int comandoservidorDescargar(char servidor[20],char comando[20],char libro[50],char usuario[20])
{
	COMANDOLIBROUSU clibro;
	COMANDOLISTALIBROUSU clista;
	struct hostent *h;
	struct sockaddr_in d;
	int s;
	
	strcpy(clista.comando,comando);
	strcpy(clista.lista,libro);
	
	h=gethostbyname(servidor);
	if(h)
	{
		bzero(&d,sizeof(d));
		d.sin_family=AF_INET;
		d.sin_port=htons(1051);
		bcopy(h->h_addr,&d.sin_addr,4);
		
		s=socket(AF_INET,SOCK_STREAM,0);
		if(s<0)
		{
			printf("Error en el socket\n");
			return 1;
		}
		if(connect(s,(struct sockaddr *)&d,sizeof(d)))
		{
			printf("Error al conectar\n");return 1;
		}
		else
		{
			send(s,&clista,sizeof(COMANDOLISTALIBROUSU),0); //enviamos mensaje
			recv(s,&clibro,sizeof(COMANDOLIBROUSU),MSG_WAITALL);//recibimos respuesta
			guardaLibro(clibro,usuario);
			close(s);
			return 0;
		}
		
	}
	else return 1;
}
/* Parametros de entrada: servidor,comando,lista. 
 * Para pasarle la ip del servidor, el comando a ejecutar, y la lista.
 * Función: esta función recibe por socket un mensaje al servidor con 
 * los datos de los libros que hay disponibles en el servidor.
 * parametro de salida: 0 si hay resultado, 1 en caso contrario. 
 * */
int comandoservidorLista(char servidor[20],char comando[20],char lista[255])
{
	COMANDOLISTALIBROUSU clista;
	struct hostent *h;
	struct sockaddr_in d;
	int s;
	
	strcpy(clista.comando,comando);
	
	h=gethostbyname(servidor);
	if(h)
	{
		bzero(&d,sizeof(d));
		d.sin_family=AF_INET;
		d.sin_port=htons(1051);
		bcopy(h->h_addr,&d.sin_addr,4);
		
		s=socket(AF_INET,SOCK_STREAM,0);
		if(s<0)
		{
			printf("Error en el socket\n");
			return 1;
		}
		if(connect(s,(struct sockaddr *)&d,sizeof(d)))
		{
			printf("Error al conectar\n");return 1;
		}
		else
		{
			//printf("Peticion de lista enviada\n");
			send(s,&clista,sizeof(COMANDOLISTALIBROUSU),0); //enviamos mensaje
			recv(s,&clista,sizeof(COMANDOLISTALIBROUSU),MSG_WAITALL);//recibimos respuesta
			//printf("Respuesta de lista recibida\n");
			close(s);
			strcpy(lista,clista.lista);
			//printf("lista=%s\n",lista);
			if(strcmp(lista,"")!=0)return 0;
			else return 1;
		}
		
	}
	else return 1;
}
/* Parametros de entrada: servidor,usuario,contra. 
 * Para pasarle la ip del servidor, el usuario, y la contraseña.
 * Función: esta función nos mostrará un submenu cuando el usuario 
 * se haya logeado.
 * parametro de salida: void. 
 * */
void submenu(char servidor[20],char usuario[20],char contra[10])
{
	char op;
	do
		{
			system("clear");
			printf("USUARIO\n");
			printf("=======\n");
			printf("A. Libros disponibles en el servidor\n");
			printf("B. Leer uno de nuestros libros\n");
			printf("C. Borrar libro\n");
			printf("S. Salir\n");
			printf("\n");
			printf("Introduce una opcion: ");
			scanf(" %c",&op);
			switch(op)
			{
				case 'A':
				case 'a':
							system("clear");
							disponibles(servidor,usuario);
							break;
				case 'B':
				case 'b':
							system("clear");
							leerlibro(usuario);
							break;
				case 'C':
				case 'c':
							system("clear");
							eliminarlibro(usuario);
							break;
				default:
							printf("Opcion incorrecta...\n");
							
			}
		}
	while(op!='S' || op!='s');
}
/* Parametros de entrada: servidor,usuario. 
 * Para pasarle la ip del servidor, el usuario.
 * Función: esta función envia al servidor una petición del listado de 
 * libros disponibles y los muestra.
 * parametro de salida: void. 
 * */
void disponibles(char servidor[20],char usuario[20])
{
	char lista[500],comando[20],libro1[50],restolista[500],seleccion[20];
	int res,finaldelista=0,indice=1;
	
	strcpy(comando,"LISTA");
	res=comandoservidorLista(servidor,comando,lista);
	if (res==0)
	{
		strcpy(restolista,lista);
		system("clear");
		printf("LISTADO DE LIBROS EN EL SERVIDOR\n");
		printf("================================\n");
		while(finaldelista==0) 
		{
			partir(restolista,libro1,restolista);	
			printf("%d. %s\n",indice,libro1);
			if(strcmp(restolista,"\0")==0)
			{
				finaldelista=1;
			}
		}
		printf("\n");
		printf("Introduce el nombre del libro que quieres descargar: ");
		scanf(" %s",seleccion);
		
		//llamar al servidor y pedir ese libro
		strcpy(comando,"DESCARGA");
		res=comandoservidorDescargar(servidor,comando,seleccion,usuario);
		if(res==0)
		{
			printf("Libro recibido.\n");
		}
		else
		{
			printf("Error al recibir el libro solicitado.\n");
		}
	}
	else 
	{
		printf("Hubo un error al recuperar la lista de libros del servidor.\n");
		sleep(2);
	}
	printf("Los libros existentes son:\n");
}
/* Parametros de entrada: usuario. Para indicarle el directorio del 
 * usuario que queremos comprobar.
 * Función: esta función nos comprueba si tenemos menos de 5 libros.
 * parametro de salida: 0 si el resultado es positivo, 1 en caso contrario. 
 * */
int comprobardir(char usuario[20])
{
	struct dirent *entrada;
	DIR *dir;
	int cont=0;
	char ruta[256];
	
	strcpy(ruta,"./Cliente/");
	strcat(ruta,usuario);
	strcat(ruta,"/");
	dir=opendir(ruta);
	while((entrada=readdir(dir)))
	{
		if(entrada->d_type==DT_REG)
		{
			cont++;
		}
	}
	closedir(dir);
	if(cont==0 || cont<10) return 0;
	else return 1;
}
/* Parametros de entrada: usuario. Para indicarle el directorio del 
 * usuario que queremos utilizar.
 * Función: esta función muestra los libros que tenemos descargados y 
 * nos da la opcion de elegir uno para leerlo.
 * parametro de salida: void. 
 * */
void leerlibro(char usuario[20])
{
	DESC des;
	LIBRO libro;
	char ruta[256],titulo[20],nombre[20],aux[256];
	int l,fl,fd;
	strcpy(ruta,"./Cliente/");
	strcat(ruta,usuario);
	strcat(ruta,"/");
	printf("Los libros existentes son:\n");
	l=listar(ruta);
	if(l==1)printf("No hay ningun libro por el momento\n");
	else
	{
		printf("Introduce el nombre del libro que desea leer: ");
		scanf(" %s",titulo);
		strcpy(nombre,titulo);
		strcat(nombre,".des");
		strcpy(aux,ruta);
		strcat(ruta,nombre);
		fd=open(ruta,O_RDONLY);
		read(fd,&des,sizeof(DESC));
		printf("titulo: %s\n",des.titulo);
		printf("autor: %s\n",des.autor);
		printf("fecha adquisicion: %s\n",des.adq);
		printf("proveedor: %s\n",des.prov);
		printf("paginas: %d\n",des.pag);
		printf("ISBN: %d\n",des.isbn);
		close(fd);
		strcpy(ruta,aux);
		strcat(ruta,".txt");
		fl=open(ruta,O_RDONLY);
		read(fl,&libro,sizeof(LIBRO));
		printf("Texto: %s\n",libro.texto);
		close(fl);		
	}
}
/* Parametros de entrada: usuario. Para indicarle el directorio del 
 * usuario que queremos utilizar.
 * Función: esta función nos elimina un libro de los que tenemos en local.
 * parametro de salida: void. 
 * */
void eliminarlibro(char usuario[20])
{
    int l;
    char ruta[256],tema[20];
    strcpy(ruta,"./Cliente/");
    strcat(ruta,usuario);
    strcat(ruta,"/");
    printf("Tus libros son: \n");
    l=listar(ruta);
    if(l==1)printf("No hay ningun libro por el momento\n");
    else
    {
        printf("Introduce el nombre del libro que desea borrar: ");
        scanf(" %s",tema);
        strcpy(ruta,"./Cliente/");
        strcat(ruta,usuario);
        strcat(ruta,"/");
        strcat(ruta,tema);
        strcat(ruta,".txt");
        remove(ruta);
        strcpy(ruta,"./Cliente/");
        strcat(ruta,usuario);
        strcat(ruta,"/");
        strcat(ruta,tema);
        strcat(ruta,".des");
        remove(ruta);
    }
}
/* Parametros de entrada: directorio. Para indicarle el directorio a 
 * eliminar.
 * Función: esta función nos elimina un directorio recursivamente.
 * parametro de salida: void. 
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
/* Parametros de entrada: ruta. Para indicarle el directorio del 
 * usuario que queremos utilizar.
 * Función: esta función nos muestra un listado de los libros descargados..
 * parametro de salida: 0 en caso de obtener resultado, 1 en caso contrario. 
 * */
int listar (char ruta[256])
{
	int cont=1;
	DIR *d;
	struct dirent *entrada;
	d=opendir(ruta);
	while((entrada=readdir(d))!=NULL)
		{
			if((entrada->d_type)==DT_REG)
				{
						printf("%d. %s\n",cont,entrada->d_name);
						cont++;
				}
		}
	if(cont==1)return 1;
	else return 0;
}
/* Parametros de entrada: servidor. Para indicarle la ip del servidor.
 * Función: esta función nos da de alta un usuario en el sistema.
 * parametro de salida: void. 
 * */
void alta(char servidor[20])
{
	char ruta[255],usuario[20],contra[10],comando[20];
	int res;
	printf("DAR DE ALTA\n");
	printf("===========\n");
	printf("\n");
	printf("Introduce el usuario: ");
	scanf(" %s",usuario);
	printf("\nIntroduce la contraseña: ");
	scanf(" %s",contra);
	strcpy(comando,"ALTA");
	res=comandoservidor(servidor,comando,usuario,contra);
	if(res==0)
	{
		strcpy(ruta,"./Cliente/");
		strcat(ruta,usuario);
		strcat(ruta,"/");
		res=mkdir(ruta,0766);
		printf("¡¡Felicidades!!\n");
		printf("El usuario se ha creado con exito\n");
		sleep(2);
	}
	else 
	{
		printf("El usuario no ha podido ser creado.\n");
		sleep(2);
	}
}
/* Parametros de entrada: servidor. Para indicarle la ip del servidor.
 * Función: esta función nos da de baja un usuario en el sistema.
 * parametro de salida: void. 
 * */
void eliminar(char servidor[20])
{
	char usuario[20],contra[10],comando[20],ruta[256];
	int res;
	printf("BORRAR USUARIO\n");
	printf("==============\n");
	printf("\n");
	printf("Introduce el usuario: ");
	scanf("%s",usuario);
	printf("\nIntroduce la contraseña: ");
	scanf("%s",contra);
	strcpy(comando,"BAJA");
	res=comandoservidor(servidor,comando,usuario,contra);
	if(res==0)
	{
		strcpy(ruta,"./Cliente/");
		strcat(ruta,usuario);
		borrar(ruta);
		printf("El usuario ha sido dado de baja correctamente.");
		sleep(2);
	}
	else 
	{
		printf("El usuario no ha podido ser eliminado.\n");
		sleep(2);
	}
}
/* Parametros de entrada: original,parte1,parte2. Le pasamos la ruta original,
 * y dos variables mas donde vamos a guardar las particiones.
 * Función: Esta funcion va a coger el contenido de la variable tema, 
 * del libro, y va a dividir esta variable para poder crear directorio a directorio.
 * Parametro de salida: void
 * */
void partir(char *original, char * parte1, char * parte2)
{
  int i = 0;
  int j = 0;
  int noencontrado = 0;

  while (original[i] != '\0')
  {
     if (noencontrado == 0)
     {
        if (original[i] == '/')
        {
           noencontrado = 1;
           parte1[i] = '\0';
        }
        else
        {
           parte1[i] = original[i];
        }
     }
     else
     {
        parte2[j++] = original[i];
     }
     i++;
  }
  parte2[j++]='\0';
}
/* Parametros de entrada: clibro-Estructura COMANDOLIBROPROV,usuario. 
 * Contiene la informacion del libro, y el nombre del usuario.
 * Función: esta función nos guarda el libro recibido del servidor.
 * Parametro de salida: void.
 * */
void guardaLibro(COMANDOLIBROUSU clibro,char usuario[20])//guardar libro en cliente
{
	DESC des;
	LIBRO libro;
	char ruta[255];
	int ft,fd,res;
	strcpy(ruta,"./Cliente/");
	strcat(ruta,usuario);
	strcat(ruta,"/");
	res=comprobardir(usuario);
	if(res==1)
	{
		printf("Lo sentimos debe eliminar algun libro.\n");
	}
	else
	{
		
		//printf("ruta=%s\n",ruta);
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
		//printf("ruta=%s\n",ruta);
		fd=open(ruta,O_CREAT|O_RDWR|O_APPEND,0766);//creamos libro
		write(fd,&des,sizeof(DESC));
		//printf("Descriptor de libro creado\n");
		close(fd);
		
		strcpy(libro.texto,clibro.texto);//copiamos datos a la estructura
		
		strcpy(ruta,"./Cliente/");
		strcat(ruta,usuario);
		strcat(ruta,"/");
		strcat(ruta,clibro.titulo);
		strcat(ruta,".txt");
		printf("Antes de crear libro ruta=%s\n",ruta);
		ft=open(ruta,O_CREAT|O_RDWR|O_APPEND,0766);//creamos descripcion
		write(ft,&libro,sizeof(LIBRO));
		printf("Libro creado\n");
		close(ft);
	}
}
