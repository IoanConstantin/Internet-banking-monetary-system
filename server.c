#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_CLIENTS	5
#define BUFLEN 256
#define BUFFE 1000

void error(char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	char *tokeniz,*tokcurent;

	int flagnumarcard=0,flagpin=0,numaitrimite=0;

	int numarcardcurent=-1;

	int contor=0;

	int cardblocat[1000];

	int kcurent,kcurentfinal;

	int pingres[1000];

	int sockfd, newsockfd, portno, clilen;
	char buffer[BUFLEN];
	struct sockaddr_in serv_addr, cli_addr;
	int n, i, j;

	FILE *fp1=fopen(argv[2],"r");
	
	int *numar=(int *)malloc(BUFFE*sizeof(int));
	
	char *nume=(char *)malloc(BUFFE*sizeof(char));
	char *prenume=(char *)malloc(BUFFE*sizeof(char));
	int *numarcard=(int *)malloc(BUFFE*sizeof(int));
	int *pin=(int *)malloc(BUFFE*sizeof(int));
	char *parola=(char *)malloc(BUFFE*sizeof(char));
	double *sold=(double *)malloc(BUFFE*sizeof(double));

	char *bufferaux=(char *)malloc(BUFFE*sizeof(char));

	char **numevector=(char **)malloc(BUFFE*sizeof(char*));
        for(int i=0;i<BUFFE;i++)
       		numevector[i]=(char *)calloc(BUFFE,sizeof(char));

	char **prenumevector=(char **)malloc(BUFFE*sizeof(char*));
	for(int i=0;i<BUFFE;i++)
       		prenumevector[i]=(char *)calloc(BUFFE,sizeof(char));

	int **numarcardvector=(int **)malloc(BUFFE*sizeof(int*));
	for(int i=0;i<BUFFE;i++)
       		numarcardvector[i]=(int *)calloc(BUFFE,sizeof(int));

	int **pinvector=(int **)malloc(BUFFE*sizeof(int*));
	for(int i=0;i<BUFFE;i++)
       		pinvector[i]=(int *)calloc(BUFFE,sizeof(int));

	char **parolavector=(char **)malloc(BUFFE*sizeof(char*));
	for(int i=0;i<BUFFE;i++)
       		parolavector[i]=(char *)calloc(BUFFE,sizeof(char));

	double **soldvector=(double **)malloc(BUFFE*sizeof(double*));	
	for(int i=0;i<BUFFE;i++)
       		soldvector[i]=(double *)calloc(BUFFE,sizeof(double));

	int **sesiunedeschisa=(int **)malloc(BUFFE*sizeof(int*));
	for(int i=0;i<BUFFE;i++)
       		sesiunedeschisa[i]=(int *)calloc(BUFFE,sizeof(int));

	int **proceseclient=(int **)malloc(BUFFE*sizeof(int*));
	for(int i=0;i<BUFFE;i++)
       		proceseclient[i]=(int *)calloc(BUFFE,sizeof(int));

	fscanf(fp1,"%d\n",numar);

	for(int w=1;w<=*numar;w++)
	{
		*sesiunedeschisa[w]=0;
	}

	for(int i=1;i<=*numar;i++)
	{
fscanf(fp1,"%s %s %d %d %s %lf\n",numevector[i],prenumevector[i],numarcard,pin,parolavector[i],sold);

		*numarcardvector[i]=*numarcard;
		*pinvector[i]=*pin;
		*soldvector[i]=*sold;
	}

	fclose(fp1);

	fd_set read_fds;//multimea de citire folosita in select()
	fd_set tmp_fds;	//multime folosita temporar 
	int fdmax;	//valoare maxima file descriptor din multimea read_fds

	if (argc < 2) {
		fprintf(stderr,"Usage : %s port\n", argv[0]);
		exit(1);
	}

	//golim multimea de descriptori de citire (read_fds) si multimea tmp_fds 
	FD_ZERO(&read_fds);
	FD_ZERO(&tmp_fds);
     
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");
     
	portno = atoi(argv[1]);

	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;	// foloseste adresa IP a masinii
	serv_addr.sin_port = htons(portno);
     
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) < 0) 
		error("-10 : ERROR on binding");
     
	listen(sockfd, MAX_CLIENTS);

	//adaugam noul file descriptor (socketul pe care se asculta conexiuni) in multimea read_fds
	FD_SET(sockfd, &read_fds);
	FD_SET(STDIN_FILENO, &read_fds);
	fdmax = sockfd;

	// main loop
	while (1) {
		tmp_fds = read_fds; 
		if (select(fdmax + 1, &tmp_fds, NULL, NULL, NULL) == -1) 
			error("-10 : ERROR in select");
	
		for(i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &tmp_fds)) {
			
				if (i == sockfd) {

					// a venit ceva pe socketul inactiv(cel cu listen) = o noua conexiune
					// actiunea serverului: accept()
					clilen = sizeof(cli_addr);
					if ((newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen)) == -1) {
						error("-10 : ERROR in accept");
					} 
					else {
						//adaug noul socket intors de accept() la multimea descriptorilor de citire
						FD_SET(newsockfd, &read_fds);
						if (newsockfd > fdmax) { 
							fdmax = newsockfd;
						}
					}
					
				*proceseclient[contor]=newsockfd;
				contor++;
				}
				else if(i==STDIN_FILENO)
				{
					memset(buffer, 0 , BUFLEN);
					fgets(buffer, BUFLEN-1, stdin);
					
														
					if(strcmp(buffer,"quit\n")==0)
					{
											
						for(int z=0;z<contor;z++)
						{
	send(*proceseclient[z],"Server_inchis",sizeof("Server_inchis"),0);
						}

						for(int r=1;r<=*numar;r++)
						{
							*sesiunedeschisa[r]=0;
						}	
					
						sleep(1);
						close(sockfd);
						return 0;
					}
					else
					{
						printf("-10 : Nu este quit");
					}	
				}				
				else {
					// am primit date pe unul din socketii cu care vorbesc cu clientii
					//actiunea serverului: recv()
					memset(buffer, 0, BUFLEN);
					if ((n = recv(i, buffer, sizeof(buffer), 0)) <= 0) {
						if (n == 0) {
							//conexiunea s-a inchis
							printf("Conexiunea cu clientul %d s-a inchis\n", i);
						} else {
							error("ERROR in recv");
						}
						close(i); 
						FD_CLR(i, &read_fds); // scoatem din multimea de citire socketul pe care 
					} 
					
					else { //recv intoarce >0
						
	
	memset(bufferaux,0,BUFLEN);

	for(int q=0;q<strlen(buffer);q++)
	{
		bufferaux[q]=buffer[q];
	}

	tokeniz=strtok(buffer," ");

	if(tokeniz!=NULL)
	{
		if(strcmp(tokeniz,"login")==0)
		{
			tokeniz=strtok(NULL," ");

			for(int k=1;k<=*numar;k++)
			{
				if(*numarcardvector[k]==atoi(tokeniz))
				{
					flagnumarcard=1;
					kcurent=k;
				}
			}

			if(atoi(tokeniz)!=numarcardcurent)
			{
				pingres[i]=0;
			}

			numarcardcurent=atoi(tokeniz);

			if(flagnumarcard==1)
			{
				flagnumarcard=0;

				tokeniz=strtok(NULL," ");

				if(*pinvector[kcurent]==atoi(tokeniz))
				{
					flagpin=1;
				}

				if(flagpin==1)
				{	
					if(*sesiunedeschisa[kcurent]==0)
					{
				if(pingres[i]>2||cardblocat[kcurent]==1)
				{
						sprintf(buffer,"IBANK> -5 : Card blocat\n");
						cardblocat[kcurent]=1;
				}
				else
				{
					*sesiunedeschisa[kcurent]=1;	
					sprintf(buffer,"IBANK> Welcome %s %s\n",numevector[kcurent],prenumevector[kcurent]);
				}
					}
					else
					{
						sprintf(buffer,"IBANK> -2 : Sesiune deja deschisa\n");
					}
					
					flagpin=0;
				}
				else
				{
			if(*sesiunedeschisa[kcurent]==0)
			{ 
					sprintf(buffer,"IBANK> -3 : Pin gresit\n");
			}
			else
			{
					sprintf(buffer,"IBANK> -2 : Sesiune deja deschisa\n");
			}		
					if(pingres[i]<2&&cardblocat[kcurent]==0)
						pingres[i]++;
					else
					{	
						pingres[i]++;
				
						sprintf(buffer,"IBANK> -5 : Card blocat\n");
						cardblocat[kcurent]=1;
					}

					kcurentfinal=kcurent;
				}
			}
			else sprintf(buffer,"IBANK> -4 : Numar card inexistent\n");
		}

	if(strcmp(bufferaux,"logout\n")==0||strcmp(bufferaux,"quit\n")==0)
		{
			*sesiunedeschisa[kcurent]=0;
			pingres[i]=0;
			sprintf(buffer,"Deconectare de la bancomat!\n");
		}

		if(strcmp(bufferaux,"listsold\n")==0)
		{
			sprintf(buffer,"%.2lf\n",*soldvector[kcurent]);
		}

	}
	
	n=send(i,buffer,sizeof(buffer),0);	
	
	if(n<0)
		error("ERROR writing to socket");
					}
				} 
			}
		}
	}

	close(sockfd);
   
	return 0; 
}

