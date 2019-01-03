#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <unistd.h>

#define BUFLEN 256
#define BUFFE 1000

void error(char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sesiune=0,numaitrimite=0,ultimacomandalistsold=0;
	double soldul;

	FILE *fp3;

	char *token;

	char *bufferaux=(char *)malloc(BUFFE*sizeof(char));

	int sockfd, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char result[BUFLEN];

	fd_set read_fds;	//multimea de citire folosita in select()
	fd_set tmp_fds;

	char buffer[BUFLEN];
	if (argc < 3) {
		fprintf(stderr,"Usage %s server_address server_port\n", argv[0]);
		exit(0);
	}  
    
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");
    
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[2]));
	inet_aton(argv[1], &serv_addr.sin_addr);
    
    
	if (connect(sockfd,(struct sockaddr*) &serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR connecting");     

	FD_SET(sockfd,&read_fds);
	FD_SET(0,&read_fds);
    
	while(1){

	tmp_fds=read_fds;
	if(select(sockfd+1,&tmp_fds,NULL,NULL,NULL) == -1)
		error("ERROR in select");

	char str[BUFFE];
	memset(str,0,BUFFE);
	sprintf(str, "client-%d.log", getpid());
	
	if (FD_ISSET(0, &tmp_fds)) {

		//citesc de la tastatura
		memset(buffer, 0 , BUFLEN);
		fgets(buffer, BUFLEN-1, stdin);

		fp3=fopen(str,"a"); 
		fputs(buffer,fp3);
		fclose(fp3);

		memset(bufferaux,0,BUFLEN);

		for(int q=0;q<strlen(buffer)-1;q++)
		{
			bufferaux[q]=buffer[q];
		}

		if(sesiune!=0)
		{
	if(strcmp(bufferaux,"quit")==0)
	{
		n = send(sockfd,buffer,strlen(buffer), 0);
		if (n < 0) 
			error("ERROR writing to socket");
		
		close(sockfd);
                return 0;
	}

	if(strcmp(bufferaux,"logout")==0||strcmp(bufferaux,"listsold")==0)
			{
				if(strcmp(bufferaux,"listsold")==0)
				{
					ultimacomandalistsold=1;
				}	
				
				n = send(sockfd,buffer,strlen(buffer), 0);
				if (n < 0) 
					error("ERROR writing to socket");
			}
			else
			{
				printf("-2 : Sesiune deja deschisa\n");

				fp3=fopen(str,"a"); 
				fputs("-2 : Sesiune deja deschisa\n",fp3);
				fclose(fp3);
			}
		}


		if(sesiune==0)
		{
	if(strcmp(bufferaux,"quit")==0)
	{
		n = send(sockfd,buffer,strlen(buffer), 0);
		if (n < 0) 
			error("ERROR writing to socket");
		
		close(sockfd);
                return 0;
	}

	if(strcmp(bufferaux,"logout")==0||strcmp(bufferaux,"listsold")==0)
			{
				printf("-1 : Clientul nu este autentificat\n");
				
				fp3=fopen(str,"a"); 
		fputs("-1 : Clientul nu este autentificat\n",fp3);
				fclose(fp3);	
			}
			else
			{
				n = send(sockfd,buffer,strlen(buffer), 0);
				if (n < 0) 
					error("ERROR writing to socket");
			}
		}

	} else {
	n=recv(sockfd,result,sizeof(result),0);
	if(n<0)
		error("ERROR writing to socket");
	else
	{
		if(strcmp(result,"Deconectare de la bancomat!\n")==0)
		{
			printf("IBANK> Clientul a fost deconectat\n");

			sesiune=0;

			fp3=fopen(str,"a"); 
			fputs("IBANK> Clientul a fost deconectat\n",fp3);
			fclose(fp3);

		}
		else if(strcmp(result,"Server_inchis")==0)
		{
			fp3=fopen(str,"a");
			fputs("Serverul se inchide!\n",fp3);
			fclose(fp3);

			printf("Serverul se inchide\n");

                        close(sockfd);
                        return 0;
		}
		else if(ultimacomandalistsold==1)
		{
			ultimacomandalistsold=0;
			
			printf("IBANK> %s\n",result);

			fp3=fopen(str,"a");
			fputs("IBANK> ",fp3); 
			fputs(result,fp3);
			fclose(fp3);	
		}
		else
		{
			printf("%s\n",result);

			fp3=fopen(str,"a"); 
			fputs(result,fp3);
			fclose(fp3);
		
			token=strtok(result," ");
			if(token!=NULL)
			{
				token=strtok(NULL," ");
				if(strcmp(token,"Welcome")==0)
				{
					sesiune++;
				}
			}
		}
		
	}
	}

	}

	return 0;
}

