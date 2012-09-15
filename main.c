//KSHITIJ PRASAD
//Assuming that in !histn will not choose for user defined commands
#include<stdio.h>
#include<error.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<string.h>
#include<signal.h>
char hist[10000][100],namec[10000][100],nameh[10000][100],home[100],prehome[100],preprehome[100],killname[1000][100];
int pidc[10000],pidh[10000],killid[1000],killstatus[1000]={0};
int gi=0,gj=0,gk=0,ghist=0,gkill=0;
//int pipel=0;
int fd;
void display(){
	char z[1000];
	gethostname(z,100);
	printf("<%s@%s:",getenv("USER"),z);
}
void background(char x1[100]){
	int z=fork();
	if(z==0){
		system(x1);
	}
}
int checkpipe(char *a[],int len){
	int i=0,j=0;
	for(i=0;i<len;i++)
		if(a[i][0]=='|')
			return 1;
	return 0;
}
int runpipe(char *a[],int fd,int flag){
	int ret=0,pid=fork();
	if(pid==0){
		if(flag==0){
			dup2(fd,1);
		}
		else if(flag==1){
			dup2(fd,0);
		}
		ret=execvp(a[0],a);
		if(ret==-1){
			perror(NULL);
		}
		exit(0);
	}
	wait(NULL);
	return 1;
}
int pipehandle(char *command[],int length){
	char *b[100];
	int i=0,count=0,first_time=1,pos=1,fd1,fd2;
	fd1=creat("/tmp/buffer1.txt",00700);
	fd2=creat("/tmp/buffer2.txt",00700);
	for(i=0;i<=length;i++){
		if(command[i]!=NULL && command[i][0]=='>'){
			int out=open(command[i+1],O_WRONLY | O_TRUNC | O_CREAT,S_IRUSR | S_IWGRP | S_IWUSR);
			if(pos==1){
					b[count]="/tmp/buffer1.txt";
					b[count+1]=NULL;
			}
			else{
					b[count]="/tmp/buffer2.txt";
					b[count+1]=NULL;
			}
			count=0;
			((pos==1)?(pos=2):(pos=1));
			count=0;
			runpipe(b,out,0);

		}
		else if(command[i]==NULL || command[i][0]=='|'){
			if(first_time){
				b[count]=NULL;
				count=0;
				first_time=0;
				runpipe(b,fd1,0);
			}
			else{
				if(pos==1){
					b[count]="/tmp/buffer1.txt";
					b[count+1]=NULL;
					if(command[i]!=NULL){
						fd2=creat("/tmp/buffer2.txt",00700);
						runpipe(b,fd2,0);
					}
					else{
						runpipe(b,1,0);
					}
				}
				else{
					b[count]="/tmp/buffer2.txt";
					b[count+1]=NULL;
					if(command[i]!=NULL){
						fd1=creat("/tmp/buffer1.txt",00700);
						runpipe(b,fd1,0);
					}
					else
						runpipe(b,1,0);
				}
				count=0;
				((pos==1)?(pos=2):(pos=1));
			}

		}
		else if(command[i][0]=='<'){
			if(first_time){
				b[count]=command[i+1];
				b[count+1]=NULL;
				count=0;
				first_time=0;
				i=i+2;
				runpipe(b,fd1,0);
			}
			count=0;
		}
		else{
			b[count]=command[i];
			count++;

		}
	}
}
void displayx(char prehome[100],char home[100]){
	display();
	int i;
	if(strstr(prehome,home)==NULL){
		printf("%s> ",prehome);
	}
	else{
		printf("~");
		for(i=strlen(home);i<strlen(prehome);i++){
			printf("%c",prehome[i]);
		}
		printf("> ");
	}
}
void history(char val[100],int flag){
	int i=0,k=0;
	if(flag==0){
		for(i=0;i<ghist;i++){
			printf("%d. %s\n",i+1,hist[i]);
		}
		display();
	}
	else if(flag==1){
		k=1;
		for(i=ghist-val[4]+'0';i<ghist;i++,k++){
			printf("%d. %s\n",k,hist[i]);
		}
	}
	else if(flag==2){
		char delim[]=" \t";
		char *x2,*tok[100];
		int i=0;
		x2=strtok(hist[val[5]-'0'-1],delim);
		while(x2!=NULL){
			tok[i]=x2;
			x2=strtok(NULL,delim);
			i++;
		}
		tok[i]=NULL;
		int pid=fork();
		if(pid==0){
			if(execvp(tok[0],tok)==-1){
				perror(NULL);
			}
			exit(0);
		}
		wait(NULL);
	}
	else{
		strcpy(hist[ghist],val);
		ghist++;
	}
}
void signalhandler(int signum){
	int status,j;
	int pid=wait(&status,0);
	if(pid!=-1){
		if(WIFEXITED(status)){
			killid[gkill]=pid;
			killstatus[gkill]=1;
			for(j=0;j<gj;j++){
				if(pidh[j]==pid){
					strcpy(killname[gkill],nameh[j]);
				}
			}
			gkill++;
	//		printf("Exited Normally\n");
		}
		else{
			killid[gkill]=pid;
			killstatus[gkill]=-1;
			for(j=0;j<gj;j++){
				if(pidh[j]==pid){
					strcpy(killname[gkill],nameh[j]);
				}
			}
			gkill++;
//			printf("Exited Abnormally\n");
		}
		int i=0;
		for(i=0;i<gj;i++){
			if(pid==pidc[i]){
				pidc[i]=0;
			}
		}
	}
}
void redirection(char *tok[100],int ile,int igt)
{
	int in=0,out=0;
	if(ile>0 && igt==0){
		int fle=fork();
		if(fle==0){
			in=open(tok[ile+1],O_RDONLY);
			dup2(in,0);
			close(in);
			tok[ile]=NULL;
			if(execvp(tok[0],tok)==-1){
				perror(NULL);
			}
			exit(0);
		}
	}
	else if(igt>0 && ile==0){
		int fgt=fork();
		if(fgt==0){
			out=open(tok[igt+1],O_WRONLY | O_TRUNC | O_CREAT,S_IRUSR | S_IWGRP | S_IWUSR);
			dup2(out,1);
			close(out);
			tok[igt]=NULL;
			if(execvp(tok[0],tok)==-1){
				perror(NULL);
			}
			exit(0);
		}
	}
	else if(ile>0 && igt>0){
		int flg=fork();
		if(flg==0){
			in=open(tok[ile+1],O_RDONLY);
			out=open(tok[igt+1],O_WRONLY | O_TRUNC | O_CREAT,S_IRUSR | S_IWGRP | S_IWUSR);
			dup2(in,0);
			dup2(out,1);
			close(in);
			close(out);
			tok[ile]=NULL;
			if(execvp(tok[0],tok)==-1){
				perror(NULL);
			}
			exit(0);
		}
	}
	wait(NULL);
	displayx(prehome,home);
}
void signalINThandler(int sig){
}
int main(){
	int per;
	signal(SIGCHLD,signalhandler);
	signal(SIGINT,signalINThandler);
	char x1[1000],bin;
	char delim[]=" \t";
	char *x2;
	getcwd(home,100);
	strcpy(prehome,home);
	strcpy(preprehome,prehome);
	display();
	printf("~> ");
	x1[0]='\0';
	while(1){
		scanf("%[^\n]",x1);
		scanf("%c",&bin);
		if(x1[0]=='\0'){
			int killcount=0;
			for(killcount=0;killcount<gkill;killcount++){
			if(killstatus[killcount]==1){
				printf("process name:%s pid:%d exited normally\n",killname[killcount],killid[killcount]);
			}
			else if(killstatus[killcount]==-1){
				printf("process name:%s pid:%d exited abnormally\n",killname[killcount],killid[killcount]);
			}
			}
			gkill=0;
			displayx(prehome,home);
	/*		if(strstr(prehome,home)!=NULL){
				int j2=0;
				printf("~");
				for(j2=strlen(home);j2<strlen(prehome);j2++){
					printf("%c",prehome[j2]);
				}
				printf("> ");
			}
			else{
				printf("%s> ",prehome);
			}
*/
			continue;
		}
		int i=0;
		char *tok[100],x3[100];
		strcpy(x3,x1);
		x2=strtok(x1,delim);
		while(x2!=NULL){
			tok[i]=x2;
			x2=strtok(NULL,delim);
			i++;
		}
		tok[i]=NULL;
		char copy[100];
		if(!strcmp(tok[0],"cd")){
			if(i==1 || !strcmp(tok[1],"~")){
				per=chdir(home);
				display();
				printf("~> ");
			}
			else if(!strcmp(tok[1],"-")){
				per=chdir(preprehome);
				display();
				if(strstr(home,preprehome)==NULL){
					printf("%s> ",prehome);
				}
				else{
					int j=0,k=0;
					copy[0]='\0';
					for(j=strlen(preprehome);j<strlen(home);j++,k++){
						copy[k]=prehome[j];
					}
					copy[k]='\0';
					printf("~%s> ",copy);
				}
			}
			else if(tok[1][0]=='~'){
				char current1[100];
				strcpy(current1,home);
				int j1=0;
				int j2=strlen(current1);
				for(j1=1;j1<=strlen(tok[1]);j1++){
					current1[j2+j1-1]=tok[1][j1];
				}
				current1[j2+j1-1]='\0';
				int status3=chdir(current1);
				if(status3==-1){
					perror(NULL);
				}
				getcwd(current1,100);
				display();
				if(strstr(current1,home)==NULL){
					printf("%s> ",current1);
				}
				else{
					int j=0,k=0;
					copy[0]='\0';
					for(j=strlen(home);j<strlen(current1);j++,k++){
						copy[k]=current1[j];
					}
					copy[k]='\0';
					printf("~%s> ",copy);
				}
			}
			else if(tok[1][0]=='/'){
				int status1=chdir(tok[1]);
				if(status1==-1){
					perror(NULL);
				}
				getcwd(tok[1],100);  
				displayx(tok[1],home);
			}
			else{
				char current[100];
				getcwd(current,100);
				int j=0;
				strcat(current,"/");
				strcat(current,tok[1]);
				int status2=chdir(current);
				if(status2==-1){
					perror(NULL);
				}
				getcwd(current,100);
				if(strstr(current,home)==NULL){
					display();
					printf("%s> ",current);
				}
				else{
					int j=0,k=0;
					char copy[100];
					copy[0]='\0';
					for(j=strlen(home);j<strlen(current);j++,k++){
						copy[k]=current[j];
					}
					copy[k]='\0';
					display();
					printf("~%s> ",copy);
				}
			}
		}
		else if(!strcmp(tok[0],"pid") && tok[1]==NULL){
			printf("pid:./a.out process id: %d\n",getpid());
			display(prehome,home);
		}
		else if(!strcmp(tok[0],"pid") && !strcmp(tok[1],"current")){
			int valc=0;
			for(valc=0;valc<gj;valc++){
				if(pidc[valc]!=0){
					printf("command name:%s process id: %d\n",namec[valc],pidc[valc]);
				}
			}
			displayx(prehome,home);
		}
		else if(!strcmp(tok[0],"pid") && !strcmp(tok[1],"all")){
			int valh=0;
			for(valh=0;valh<gj;valh++){
				printf("command name: %s process id: %d\n",nameh[valh],pidh[valh]);
			}
			displayx(prehome,home);
		}
		else if(!strcmp(tok[0],"hist")){
			history(tok[0],0);
			displayx(prehome,home);
		}
		else if(strstr(tok[0],"hist") && strlen(tok[0])==5){
			history(tok[0],1);
			displayx(prehome,home);
		}
		else if(strstr(tok[0],"!hist") && strlen(tok[0])==6){
			history(tok[0],2);
			displayx(prehome,home);
		}
		else if(!strcmp(tok[0],"quit")){
			exit(1);
		}
		else if(checkpipe(tok,i)){
			pipehandle(tok,i);
			displayx(prehome,home);
		}
		else if(strstr(x3,">") || strstr(x3,"<") || strstr(x3,"|")){
			int ile=0,igt=0,countre=0,in,out,pipe[100]={0},countp=0;
			for(countre=0;countre<i;countre++){
				if(!strcmp(tok[countre],"<")){
					ile=countre;
				}
				else if(!strcmp(tok[countre],">")){
					igt=countre;
				}
				else if(!strcmp(tok[countre],"|")){
					pipe[countp]=countre;
					countp++;
				}	
			}
			redirection(tok,ile,igt);
		}
		else{
			int val=fork();
			if(tok[i-1][0]=='&'){
				tok[i-1]=NULL;
				if(val==0){
					if(execvp(tok[0],tok)==-1){
						perror(NULL);
					}
					exit(0);
				}
				else if(val>0){
					strcpy(nameh[gj],tok[0]);
					strcpy(namec[gj],tok[0]);
					pidh[gj]=val;
					pidc[gj]=val;
					gj++;
				}
			}
			else if(tok[0]!=NULL){
				if(val==0){
					if(execvp(tok[0],tok)==-1){
						perror(NULL);
					}
					exit(0);
				}
				else if(val>0){
					wait(NULL);
				}
			}
			display();
			if(!strcmp(prehome,home)){
				int j2=0;
				printf("~");
				for(j2=strlen(home);j2<strlen(prehome);j2++){
					printf("%c",prehome[j2]);
				}
				printf("> ");
			}
			else{
				printf("%s> ",prehome);
			}
		}
		strcpy(preprehome,prehome);
		getcwd(prehome,100); 
		x1[0]='\0';
		history(x3,9);
		if(per==-1){
			printf(NULL);
		}
	}
	return 0;
	}
