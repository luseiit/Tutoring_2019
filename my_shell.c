/* my_shell.c Parkjw November 10th, 32151859@dankook.ac.kr*/

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>

#define debug
#define Max_buf 256

int tokenize(char *buf, char *delim, char *tokens[], int max_tokens){
	  #ifdef debug
	    printf("function parser var\n");
	        #endif
	        int token_count;
		      char *token =strtok(buf, delim);    
		            #ifdef debug
		              printf("function parser end while\n");
			              #endif
			              for(token_count = 0;token!=NULL && token_count<max_tokens; token_count++){
					                    tokens[token_count] = token;
							                    token = strtok(NULL, delim);
									                    }
				                tokens[token_count] = NULL;
						          return token_count;
}
bool background(char *line){
	  #ifdef debug
	    printf("function backgournd\n");
	        #endif
	        int size;
		      for(size = 0; size<strlen(line); size++){
			                if(line[size] == '&'){
						                line[size] = '\0';
								                    return true;
										                        }
					          }
		            return false;
}
int cmd_cd(int argc, char* argv[]){
	  #ifdef debug
	    printf("function cmd_cd\n");
	        #endif
	        if(argc == 1) chdir(getenv("HOME"));
		      else if(argc == 2){
			                if(chdir(argv[1])) printf("ERROR: No directory\n");
					          }
		            else printf("USAGE: cd [directory]\n");
			            return 0;
}
int cmd_help(int argc, char *argv[]){
	  #ifdef debug
	    printf("function help\n");
	        #endif
	        if(argc >1) printf("If you want to know about %s, send me Email~\n", argv[1]);
		      else printf("If you want to know something , send me Email~\n");
		            return 0;
}
int cmd_exit(){
	  #ifdef debug
	    printf("function exit\n");
	        #endif
	        printf("Bye~\n");
		      return -1;
}

int redirection(int stdout, int idx, char *argv[], bool back_check){
	  #ifdef debug
	    printf("function redirection\n");
	      #endif
	      int fd,i = idx;
	        pid_t child;
		  if(!argv[i+1]){
			      printf("ERROR : rediction failed(redirection command must be \"command > filename\")\n");
			          return -1;
				    }
		    #ifdef debug
		      printf("function redirection check %s\n", argv[i+1]);
		        #endif
		        if((fd=open(argv[i+1], O_RDWR|O_CREAT|O_TRUNC, 0644)) == -1){
				    printf("ERROR : rediction failed(Cannot open %s)\n", argv[i+1]);
				        return -1;
					  }
			  dup2(fd, STDOUT_FILENO);
			    close(fd);
			      argv[i] = NULL;
			        argv[i+1] = NULL;
				  for(; argv[i]!=NULL; i++){
					  	argv[i] = argv[i+2];
						  }  
				    if((child=fork())==0){
					        #ifdef debug
					          printf("function redirection duplicate\n");
						      #endif
						      
						      #ifdef debug
						        printf("function redirection exec %s on %s\n", argv[0], argv[1]);
							    #endif
							    execvp(argv[0], argv);
							        printf("ERROR : execvp failed\n");
								    exit(errno);
								      }
				      else if(child<0) {
					          printf("ERROR : fork failed\n");
						      exit(errno);
						        }
				        else{
						    if(back_check == false){
							          #ifdef debug
							            printf("background is not working\n");
								          #endif
								          wait(&child);
									      }
						      }
					  dup2(stdout, STDOUT_FILENO);
					    close(fd);
					      return 0;
}

int run(char *line){
	  #ifdef debug
	    printf("function run var\n");
	      #endif
	      int stdout = dup(1);
	        char *tokens[Max_buf];
		  char delim[] = " \n\t\r";
		    int token_count, redirect, i;
		      bool back_check = background(line);
		        pid_t child;
			  #ifdef debug
			  printf("function run parser\n");
			    #endif
			    token_count = tokenize(line, delim, tokens, sizeof(tokens)/ sizeof(char*));
			      #ifdef debug
			        printf("function run redirection\n");
				  #endif
				  for(i = 0; tokens[i] != NULL; i++){
					    if(!strcmp(tokens[i], ">")) {
						          if((redirect = redirection(stdout, i, tokens, back_check))<0) return -1;
							        else return 0;
								    }
					      }
				    if(strcmp(tokens[0], "cd") == 0) return cmd_cd(token_count, tokens);
				      if(strcmp(tokens[0], "exit") == 0) return cmd_exit();
				        if(strcmp(tokens[0], "help") == 0) return cmd_help(token_count, tokens);
					  if(token_count == 0) return -1;
					    #ifdef debug
					      printf("function run fork\n");
					        #endif
					        if((child=fork())==0){
							    execvp(tokens[0], tokens);
							        printf("ERROR : execvp failed\n");
								    exit(errno);
								      }
						  else if(child<0) {
							      printf("ERROR : fork failed\n");
							          exit(errno);
								    }
						    else{
							        if(back_check == false){
									      #ifdef debug
									        printf("background is not working\n");
										      #endif
										      wait(&child);
										          }
								  }
						      return 0;
}

int main(){
	  char line[Max_buf];
	    char *username = getlogin();
	      #ifdef debug
	        printf("Start my_shell\n");
		  #endif
		  while(1){
			      printf("[%s]$ ", username);
			          fgets(line, sizeof(line)-1, stdin);
				      #ifdef debug
				        printf("get command line\n");
					    #endif
					    if(run(line) < 0) break;
					      }
		    #ifdef debug
		      printf("End my_shell\n");
		        #endif
}

