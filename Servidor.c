#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <ctype.h>
#include <pthread.h>

int contador; 

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


int main(int argc, char *argv[])
{
	contador= 0;
	int sock_conn, sock_listen, ret;
	struct sockaddr_in serv_adr;
	char peticion[512];
	char respuesta[512];
	// INICIALITZACIONS
	// Obrim el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creant socket");
	// Fem el bind al port
	
	
	memset(&serv_adr, 0, sizeof(serv_adr));// inicialitza a zero serv_addr
	serv_adr.sin_family = AF_INET;
	
	// asocia el socket a cualquiera de las IP de la m?quina. 
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	// establecemos el puerto de escucha
	serv_adr.sin_port = htons(9300);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error al bind");
	
	if (listen(sock_listen, 3) < 0)
		printf("Error en el Listen");
	
	int i;
	
	// bucle infinito
	for (;;){
		printf ("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("He recibido conexion\n");
		//sock_conn es el socket que usaremos para este cliente
		
		//Bucle atencion
		int terminar =0;
		while (terminar ==0)
		{
			// Ahora recibimos la petici?n
			ret=read(sock_conn,peticion, sizeof(peticion));
			printf ("Recibido\n");
			
			// Tenemos que a￱dirle la marca de fin de string 
			// para que no escriba lo que hay despues en el buffer
			peticion[ret]='\0';
			
			
			
			
			char *p = strtok( peticion, "/");
			printf ("Peticion: %s\n",peticion);
			
			int codigo =  atoi (p);
			// Ya tenemos el c?digo de petici?n
			char nombre[20];
			
			if ((codigo !=0) && (codigo!=6))
			{
				p = strtok( NULL, "/");
				
				strcpy (nombre, p);
				// Ya tenemos el nombre
				printf ("Codigo: %d, Nombre: %s\n", codigo, nombre);
			}
			
			if (codigo == 0)
			{
				terminar=1;
			
			}
			
			else if (codigo ==1) //piden la longitd del nombre
			{
				sprintf (respuesta,"%d",strlen (nombre));
			}
			
			else if (codigo ==2)
			{
				// quieren saber si el nombre es bonito
				if((nombre[0]=='M') || (nombre[0]=='S'))
				{
					strcpy (respuesta,"SI");
				}
				else
				{
					strcpy (respuesta,"NO");
				}
			}
			else  if (codigo ==3)//quiere saber si es alto
			{
				p = strtok( NULL, "/");
				float altura =  atof (p);
				if (altura > 1.70)
					sprintf (respuesta, "%s: eres alto",nombre);
				else
					sprintf (respuesta, "%s: eresbajo",nombre);
			}
			
			else  if (codigo ==4)//Quiere saber s￯﾿ﾭ es palindromo
			{		
				
				char s[1000];  
				int i,n,c=0;
				
				n=strlen(s);
				
				for(i=0;i<n/2;i++)  
				{
					if(s[i]==s[n-i-1])
						c++;
					
				}
				if(c==i)
					
				{
					strcpy (respuesta,"SI, es pal￯﾿ﾭndromo");
				}
				else
				   strcpy (respuesta,"No, no es pal￯﾿ﾭndromo");
				
				
			}				
			
			else  if (codigo ==5)// Devolver un nombre en may￯﾿ﾺsculas 
			{		
				
				for (int in = 0; nombre[in] != '\0'; in++)
				{
					nombre[in] = toupper(nombre[in]);
				}
				sprintf (respuesta,"%s",nombre);
				
			}				
			
			else if (codigo==6)
			{
				sprintf (respuesta,"%d",contador);
			}
			if (codigo !=0)
			{
				
				printf ("Respuesta: %s\n", respuesta);
				// Enviamos respuesta
				write (sock_conn,respuesta, strlen(respuesta));
			}
			if ((codigo==1)|| (codigo==2)||(codigo==3)||(codigo==4)||(codigo==5))
			{
				pthread_mutex_lock(&mutex); //No interrumpir ahora
				contador =contador +1;
				pthread_mutex_unlock(&mutex); //Ahora si se puede
			}
		}
		// Se acabo el servicio para este cliente
		close(sock_conn); 
	}
}
