#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

#define MAX_LINE 80
#define BUFFER_SIZE 80

char prompt[5]="sh->";
int prolen=4;
char history[10][BUFFER_SIZE];
int count=0;

void printHistory(){
	int i;
	int j=0;
	int hcount=count;
	printf("\nCommand History:\n");
	if(hcount==0){
		write(STDIN_FILENO, prompt, prolen);
	}else{
		for (i=0; i<10; ++i){
	 		printf("%d.  ", hcount);
			while (history[i][j] != '\n' && history[i][j] != '\0'){
				printf("%c", history[i][j]);
				++j;
			}
			printf("\n");
			j=0;
			--hcount;
			if (hcount==0) break;
		}
		write(STDIN_FILENO, prompt, prolen);
	}
} 

void sighandler(int signum){
	printHistory();
}

void partion(char inputBuffer[], char *args[], int *background, int *commandLength){
	int start=-1, ct=0;
	int length=strlen(inputBuffer);
	if (length==0) exit(0);
	if (length<0){
		perror("error reading the command\n");
		exit(-1);
	}
 	for (int i=0; i<length; i++){
		switch (inputBuffer[i]){
			case ' ':
			case '\t' :               
				if(start != -1){
					args[ct] = &inputBuffer[start];
					ct++;
					++*commandLength;
				}
				inputBuffer[i] = '\0';
				start = -1;
				break;
     
			case '\n':          
				if(start != -1){
					args[ct] = &inputBuffer[start];
					ct++;
					++*commandLength;
				}
         			inputBuffer[i] = '\0';
         			args[ct] = NULL;
				break;
 
  			default :          
            			if(inputBuffer[i] == '&'){
    					*background  = 1;
                			inputBuffer[i] = '\0';
   				}else if(start == -1){
   					start = i;
   				}
		}
	}
	args[ct] = NULL;
}

void setup(char inputBuffer[], char *args[], int *background, int *commandLength){
	int length=read(STDIN_FILENO, inputBuffer, MAX_LINE);
	if (length==0) exit(0);
	if (length<0){
		perror("error reading the command\n");
		exit(-1);
	}
	for (int i=9; i>0; --i) strcpy(history[i], history[i-1]);
	strcpy(history[0], inputBuffer);
	++count;
	partion(inputBuffer, args, background, commandLength);
}

int main(void)
{
	char inputBuffer[MAX_LINE]; 
	int background;
	char *args[MAX_LINE/2+1];
	pid_t pid;
	int i;
	int commandLength;

	signal(SIGINT,sighandler);

	while (1){            
		background=0;
		commandLength=0;
		write(STDIN_FILENO, prompt, prolen);
		setup(inputBuffer, args, &background, &commandLength);
		if(commandLength>=1 && strcmp(args[0], "cd")==0){
			if(commandLength==1){
				chdir(getenv("HOME"));
			}else{
				if(strcmp(args[1], "~")==0){
					chdir(getenv("HOME"));
				}else{
					chdir(args[1]);
				}
			}
		}else{
			pid=fork();
			if(pid < 0){
				printf("Fork failed.\n");
				exit(1);
			}
			else if(pid == 0){
				if(commandLength>=1 && strcmp(args[0],"r")==0){
					if(commandLength==1){				
						char input[MAX_LINE];
						char *command[MAX_LINE/2+1];
						int a, b;
						strcpy(input, history[1]);
						partion(input, command, &a, &b);
						if(execvp(command[0], command)==-1){
							printf("Error executing command\n");
							exit(0);
						}
					}
					else if(commandLength==2){
						char input[MAX_LINE];
						char *command[MAX_LINE/2+1];
						int a,b;
						int i,hcount=count;
						for(i=1; i<=10; ++i){
							--hcount;
							if(hcount==0) break;
							strcpy(input, history[i]);
							partion(input, command, &a, &b);
							if(args[1][0]==command[0][0]){
								if(execvp(command[0], command)==-1){
									printf("Error executing command\n");
									exit(0);
								}
								break;
							}else{
								continue;
							}
						}
						if(i==11 || i==count){
							printf("History command not found\n");
							exit(0);
						}
					}else{
						printf("Command format error\n");
						printf("Please input r or r x\n");
						exit(0);
					}
				}else if(execvp(args[0], args)==-1){
					printf("Error executing command\n");
					exit(0);
				}
			}
			else{
				if(background == 0){
					//wait(NULL);
					waitpid(pid, NULL,0);
					//printf("%d\n",status);
				}
			}
		}
		
	}
}
