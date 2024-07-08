#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>


int batchMode = 0, True = 0;

int breakCommand(char *str){
	char *tmp;
	char *subcmds[1000];
	char buffer[1000];
	strcpy(buffer, str);
	tmp = strtok(buffer," \n\t");
	int num_subcmds = 0;
	int out, flag1=0;
	char *subnew[1000];
        
	while(tmp){
          subcmds[num_subcmds] = tmp;
          num_subcmds++;
          tmp = strtok(NULL," \n\t");
	}
	
	int j, loc=0;

	for (j = 0; j < num_subcmds; j++){
		
	}

	subcmds[j] = NULL;

	if(num_subcmds > 1){
		for(j = 0; j < num_subcmds; j++){
			if(strcmp(subcmds[j], ">") == 0){
				loc = j;
				flag1 = 1;
				break;
			}
		}

		if(flag1 == 1){
			for (j = 0; j < loc; j++){
				subnew[j] = subcmds[j];
			}
	    }
		subnew[loc] = NULL;
	}
       
	int i, savedJ, flag2 = 0;

	if(flag1!=1){
		for (j = 0; j < num_subcmds; j++){
			i = strlen(subcmds[j]) - 1;
			if(subcmds[j][i] == '>'){
				subcmds[j][i] = '\0';
				flag2 = 1;
				savedJ = j;
			}
		}

		if(flag2 == 1){
			for (j = 0; j <= savedJ; j++){
				subnew[j]=subcmds[j];
			}
			subnew[savedJ+1]=NULL;
		}
       
		if(flag1 == 1){
			close(STDOUT_FILENO);
			/*out = open(subcmds[loc+1], O_RDWR | O_CREAT , 0666 ); 
			if(out < 0){
				char error_message[30] = "An error has occurred\n";
				write(STDERR_FILENO, error_message, strlen(error_message));
				exit(0);
			}
			dup2(out, STDOUT_FILENO);
			if(execvp(subnew[0], subnew) < 0){
				char error_message[30] = "An error has occurred\n";
				write(STDERR_FILENO, error_message, strlen(error_message));
				exit(101);
			}
			close(out);
			return 101;
		}
		
		else if(flag2==1){
			close(STDOUT_FILENO);
			out = open(subcmds[savedJ+1], O_RDWR | O_CREAT , 0666 ); 
			
			if(out < 0){
				char error_message[30] = "An error has occurred\n";
				write(STDERR_FILENO, error_message, strlen(error_message));
				exit(0);
			}
			dup2(out, STDOUT_FILENO);
			
			if(execvp(subnew[0], subnew) < 0){
				char error_message[30] = "An error has occurred\n";
				write(STDERR_FILENO, error_message, strlen(error_message));
				exit(101);
			}
			close(out);
			return 101;*/
			
		}
		
		else if(strcmp(subcmds[0], "cd") == 0){
			int res;
			if(subcmds[1]!=NULL){
				res = chdir(subcmds[1]);
			}
			else{
				res = chdir(getenv("HOME"));
			}

			if(res == -1){
				char error_message[30] = "An error has occurred\n";
				write(STDERR_FILENO, error_message, strlen(error_message));
				exit(101);
			}
		}
		
		else if(strcmp(subcmds[0], "exit") == 0){
			exit(0);
		}
		//change this into a path built in command
		else if(strcmp(subcmds[0], "pwd") == 0){
			if(subcmds[1]!=NULL){
				char error_message[30] = "An error has occurred\n";
				write(STDERR_FILENO, error_message, strlen(error_message));
				exit(0);
			}
			
			else if (execvp(subcmds[0], subcmds) < 0){
				char error_message[30] = "An error has occurred\n";
				write(STDERR_FILENO, error_message, strlen(error_message));
				exit(101); 
			}
		}
		
		else if (execvp(subcmds[0], subcmds) < 0){
			char error_message[30] = "An error has occurred\n";
			write(STDERR_FILENO, error_message, strlen(error_message));
			return 1;
		}
		return 1;
	}
}

int breakString(char *str){
	char *tmp;
	char *subcmds[1000];
	char buffer[1000];
	strcpy(buffer, str);
	tmp = strtok(buffer,";");
	int num_subcmds = 0;
        
	while (tmp){
		subcmds[num_subcmds] = tmp;
		num_subcmds++;
		tmp = strtok(NULL,";");
	} 
	   
	int j, status;
           
	for (j = 0; j < num_subcmds; j++){
		int ret;
		
		if((subcmds[j][0]=='c' && subcmds[j][1]=='d') == 1){
			breakCommand(subcmds[j]);
		}
		
		else if(strcmp(subcmds[j],"exit") == 0){
			breakCommand(subcmds[j]);
		}
		
		else{
			if( (ret=fork()) > 0 ){
				//   parent(ret);
				//wait(&status);
				waitpid(ret,&status,WUNTRACED);
				int x = WEXITSTATUS(status);
				if(x==101)
					return 101;
				
			}
			
			else if ( ret == 0 ){
				//child();
				if(breakCommand(subcmds[j])==1){
					exit(1);
					return 101; 
				}
				
				else return 0;
				break;
	       }
		   
			else{
				char error_message[30] = "An error has occurred\n";
				write(STDERR_FILENO, error_message, strlen(error_message));
				exit(101);
			}
		}
	}
}	

int showPrompt(void){
	char *cmds[100];
	char buffer[100];
	char *tmp;
	int num_cmds, i, flag, rc=0;
	while(!rc){
		num_cmds = 0;
		printf("\n./witsshell> ");
		fgets(buffer, 512, stdin);
		for(i=0; buffer[i]!='\0'; i++) {
			if(buffer[i]=='&')
				flag++;
		}
		int j = i-2;
		int endingWithPlus = 0;
		for(; j >= 0; j--){
			if(buffer[j] == ' ')
				continue;
			else if(buffer[j] == '&'){
				endingWithPlus = 1;
				break;
			}
			else
				break;
		}
		
        tmp = strtok(buffer,"&");
		
        while(tmp){
			cmds[num_cmds] = tmp;
			num_cmds++;
			tmp = strtok(NULL,"&");
        }
		
		int numCmndsToFork = num_cmds;	
		
		if(flag == 0){
			if((rc = breakString(buffer)) == 101){
				break; 
				exit(1);
			}
		}
		else{	      
			if(endingWithPlus == 0){
				numCmndsToFork = num_cmds - 1;
				if((rc = breakString(cmds[num_cmds-1])) == 101){
					break; exit(1);
				}
			}
			int i;
			for (i = 0; i < numCmndsToFork; i++){         
				int ret;
				if( (ret=fork()) > 0 ){
					//parent(ret);
				}
				else if( ret == 0 ){
					//child();
					//printf("Running %s in background.\n------------------------------\n", cmds[i] );
					if(breakString(cmds[i]) == 101){
						exit(0); 
					}
					break;
				}
				else{
					char error_message[30] = "An error has occurred\n";
					write(STDERR_FILENO, error_message, strlen(error_message));
				}
			}
	    }
	}
	return 0;
}
/*
int main(int argc, char *argv[]){
	
	char *cmds[1000];
	char buffer[1000]="test";
	char * tmp;
	int num_cmds, i, flag, rc=0;
	
	char *fileToRead = "/no/such/file";

	if(argc>2){
		char error_message[30] = "An error has occurred\n";
		write(STDERR_FILENO, error_message, strlen(error_message));
		exit(1);
	}
	
	else if(argc==2){
		batchMode = 1;	        
		fileToRead = argv[1];
	}

	FILE *new;
	FILE *fp;

	if(batchMode==1){
		fp = fopen(fileToRead,"r");
		if (fp==NULL){
			char error_message[30] = "An error has occurred\n";
			write(STDERR_FILENO, error_message, strlen(error_message));
			exit(1);
	    }
		new=fp;
	}
	
	else {new=stdin;}
	
	while (!feof(new)){
		num_cmds = 0;
		if(batchMode==0) write(STDOUT_FILENO, "witsshell> ", strlen("witsshell> "));
		if(batchMode==1)
			fgets(buffer, 1000, fp);
		
		else
			fgets(buffer, 1000, stdin);
		
		int j;

		strtok(buffer, "\n\r");

		if(batchMode == 1 && strcmp(buffer,"xyz")!=0){
			
			write(STDOUT_FILENO, buffer, strlen(buffer));
			write(STDOUT_FILENO, "\n", strlen("\n"));
			
			if(strcmp(buffer,"&")==0){
				exit(0);
			}
	    }

		if(strcmp(buffer,"xyz")==0) exit(0);

		for(i=0; buffer[i]!='\0'; i++) {
			if(buffer[i]=='&')
				flag++;
		}
	  
		if(strlen(buffer)==0){
			char error_message[30] = "An error has occurred\n";
			write(STDERR_FILENO, error_message, strlen(error_message));
		}

		j = i-2;
		int endingWithPlus = 0;
		for(; j >= 0; j--){
			if(buffer[j] == ' ')
				continue;
			else if(buffer[j] == '&'){
				endingWithPlus = 1;
				break;
			}
			else
				break;
		}

		tmp = strtok(buffer,"&");
           
		while (tmp) {
              cmds[num_cmds] = tmp;
              num_cmds++;
              tmp = strtok(NULL,"&");
		} 

		int numCmndsToFork = num_cmds;
	  
		if(flag==0){
			if((rc=breakString(buffer))==101){
				break; exit(0);
			}
		}
		
		else{
	      
			if(endingWithPlus == 0){
				numCmndsToFork = num_cmds - 1;
				if((rc=breakString(cmds[num_cmds-1]))==101){
					break; 
					exit(0);
				}
			}

			int i, status;
			for (i = numCmndsToFork-1; i >= 0; i--){
            
				int ret;
				if( (ret=fork()) > 0 ){
					//   parent();
					while (1){
						int status;
						pid_t done = waitpid(ret,&status,WUNTRACED);
						if (done == -1) {
							if (errno == ECHILD) break; // no more child processes
						}
						
						else {
							int x = WEXITSTATUS(status);

							if (!WIFEXITED(x) || WEXITSTATUS(x) != 101){
								exit(0);
							}
						}
					}
				}
				
				else if ( ret == 0 ){
					//    child();
					//printf("Running %s in background.\n------------------------------\n", cmds[i] );
					if(breakString(cmds[i])==101){
						exit(0); 
					}
				}
				
				else{
					char error_message[30] = "An error has occurred\n";
					write(STDERR_FILENO, error_message, strlen(error_message));
					exit(0);
				}
			}
	    }
		strcpy(buffer,"xyz");
	}
	return 0;
}

*/

char *paths;
struct command {
    char *command;
    char **arguments;
    char *standard_out;
};
int is_empty(const char *str) {
    while (*str != 0) {
        if (isspace(*str)) {
            str++;
        } else {
            return 0;
        }
    }
    return 1;
}
char *construct_executable(char *program, char **args) {
    if (strcmp("exit", program) == 0) {
        if (args[1] != 0) {
            return NULL;
        } else {
            return program;
        }
    }
    if (strcmp("cd", program) == 0) {
        if (args[1] == 0 || args[2] != 0) {
            return NULL;
        } else {
            return program;
        }
    }
    if (strcmp("path", program) == 0) {
        return program;
    }
    if (paths == NULL || strcmp(paths, "") == 0) {
        if (access(program, X_OK) == 0) {
            return program;
        } else {
            return NULL;
        }
    }
    char *to_separate = strdup(paths);
    char *to_free = to_separate;
    char *path;
    while ((path = strsep(&to_separate, ":")) != NULL) {
        char *executable = malloc(strlen(path) + strlen(program) + 1 + 1);
        executable = strcpy(executable, path);
        executable = strcat(executable, "/");
        executable = strcat(executable, program);
        if (access(executable, X_OK) == 0) {
            free(to_free);
            return executable;
        }
        free(executable);
    }
    free(to_free);
    return NULL;
}
int construct_command(char *line, struct command *command) {
    char **args = NULL;
    int size = 1;
    char *before_redirect = strsep(&line, ">");
    char *after_redirect = strsep(&line, ">");
    char *after_after_redirect = strsep(&line, ">");

    if (after_after_redirect != NULL) {
        return -1;
    }
    for (char *argument; (argument = strsep(&before_redirect, " ")) != NULL;) {
        if (strcmp("", argument) == 0) {
            continue;
        }
        size++;
        args = realloc(args, size * sizeof(char *));
        args[size - 2] = argument;
    }
    if (args == NULL) {
        return -1;
    }
    args[size - 1] = 0;

    char *redirect_file = NULL;
    if (after_redirect != NULL) {
        char *argument;
        while ((argument = strsep(&after_redirect, " ")) != NULL) {
            if (strcmp("", argument) == 0) {
                continue;
            }
            if (redirect_file != NULL) {
                return -1;
            }
            redirect_file = argument;
        }
        if (redirect_file == NULL) {
            return -1;
        }
    }
    char *program = construct_executable(args[0], args);
    if (program == NULL) {
        return -1;
    }
    command->command = program;
    command->arguments = args;
    command->standard_out = redirect_file;
    return 0;
}
struct command **construct_commands(char *line, int *size) {
    int i = 0;
    struct command **commands = NULL;
    for (char *command; (command = strsep(&line, "&")) != NULL;) {
        if (is_empty(command) == 1) {
            continue;
        }
        i++;
        commands = realloc(commands, i * sizeof(struct command));

        struct command *c = malloc(sizeof(struct command));
        if (construct_command(command, c) == -1) {
            free(commands);
            return NULL;
        }
        commands[i - 1] = c;
    }
    *size = i;
    return commands;
}
void print_prompt(FILE *stream) {
    if (stream == stdin) {
        printf("%s", "witsshell> ");
    }
}

void print_error() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

char *read_line(FILE *stream) {
    char *line = NULL;
    size_t linecap = 0;
    if (getline(&line, &linecap, stream) == -1) {
        return NULL;
    }
    line[strcspn(line, "\n")] = 0;
    return line;
}

pid_t execute_command(struct command *command) {
    if (strcmp("exit", command->command) == 0) {
        exit(0);
    }
    if (strcmp("cd", command->command) == 0) {
        return chdir(command->arguments[1]);
    }
    if (strcmp("path", command->command) == 0) {
        free(paths);
        paths = NULL;
        char *argument;
        int i = 1;
        while ((argument = command->arguments[i]) != 0) {
            int length = 0;
            if (paths != NULL) {
                length = strlen(paths) + 1;
            }
            paths = realloc(paths, length + strlen(argument) + 1);
            if (length != 0) {
                paths = strcat(paths, ":");
                paths = strcat(paths, argument);
            } else {
                paths = strcpy(paths, argument);
            }
            i++;
        }
        return 0;
    }
    pid_t pid = fork();
    if (pid == 0) {
        if (command->standard_out != NULL) {
            fclose(stdout);
            fopen(command->standard_out, "w");
        }
        execv(command->command, command->arguments);
        perror("execv failed");
        exit(1);
    } else {
        return pid;
    }
}

int execute_commands(struct command **commands, int size) {
    int pids_size = 0;
    pid_t *pids = NULL;
    for (int i = 0; i < size; i++) {
        struct command *command = commands[i];
        pid_t pid = execute_command(command);
        if (pid == -1) {
            return -1;
        } else if (pid != 0) {
            pids = realloc(pids, (pids_size + 1) * sizeof(pid_t));
            pids[pids_size] = pid;
            pids_size++;
        }
    }
    for (int j = 0; j < pids_size; j++) {
        waitpid(pids[j], NULL, 0);
    }
    return 0;
}

int main(int argc, char *argv[]) {
	while(True){
		char *cmds[1000];
		char buffer[1000]="test";
		char * tmp;
		int num_cmds, i, flag, rc=0;
	
		char *fileToRead = "/no/such/file";

	if(argc>2){
		char error_message[30] = "An error has occurred\n";
		write(STDERR_FILENO, error_message, strlen(error_message));
		exit(1);
	}
	
	else if(argc==2){
		batchMode = 1;	        
		fileToRead = argv[1];
	}

	FILE *new;
	FILE *fp;

	if(batchMode==1){
		fp = fopen(fileToRead,"r");
		if (fp==NULL){
			char error_message[30] = "An error has occurred\n";
			write(STDERR_FILENO, error_message, strlen(error_message));
			exit(1);
	    }
		new=fp;
	}
	
	else {new=stdin;}
	
	while (!feof(new)){
		num_cmds = 0;
		if(batchMode==0) write(STDOUT_FILENO, "witsshell> ", strlen("witsshell> "));
		if(batchMode==1)
			fgets(buffer, 1000, fp);
		
		else
			fgets(buffer, 1000, stdin);
		
		int j;

		strtok(buffer, "\n\r");

		if(batchMode == 1 && strcmp(buffer,"xyz")!=0){
			
			write(STDOUT_FILENO, buffer, strlen(buffer));
			write(STDOUT_FILENO, "\n", strlen("\n"));
			
			if(strcmp(buffer,"&")==0){
				exit(0);
			}
	    }

		if(strcmp(buffer,"xyz")==0) exit(0);

		for(i=0; buffer[i]!='\0'; i++) {
			if(buffer[i]=='&')
				flag++;
		}
	  
		if(strlen(buffer)==0){
			char error_message[30] = "An error has occurred\n";
			write(STDERR_FILENO, error_message, strlen(error_message));
		}

		j = i-2;
		int endingWithPlus = 0;
		for(; j >= 0; j--){
			if(buffer[j] == ' ')
				continue;
			else if(buffer[j] == '&'){
				endingWithPlus = 1;
				break;
			}
			else
				break;
		}

		tmp = strtok(buffer,"&");
           
		while (tmp) {
              cmds[num_cmds] = tmp;
              num_cmds++;
              tmp = strtok(NULL,"&");
		} 

		int numCmndsToFork = num_cmds;
	  
		if(flag==0){
			if((rc=breakString(buffer))==101){
				break; exit(0);
			}
		}
		
		else{
	      
			if(endingWithPlus == 0){
				numCmndsToFork = num_cmds - 1;
				if((rc=breakString(cmds[num_cmds-1]))==101){
					break; 
					exit(0);
				}
			}

			int i, status;
			for (i = numCmndsToFork-1; i >= 0; i--){
            
				int ret;
				if( (ret=fork()) > 0 ){
					//   parent();
					while (1){
						int status;
						pid_t done = waitpid(ret,&status,WUNTRACED);
						if (done == -1) {
							if (errno == ECHILD) break; // no more child processes
						}
						
						else {
							int x = WEXITSTATUS(status);

							if (!WIFEXITED(x) || WEXITSTATUS(x) != 101){
								exit(0);
							}
						}
					}
				}
				
				else if ( ret == 0 ){
					//    child();
					//printf("Running %s in background.\n------------------------------\n", cmds[i] );
					if(breakString(cmds[i])==101){
						exit(0); 
					}
				}
				
				else{
					char error_message[30] = "An error has occurred\n";
					write(STDERR_FILENO, error_message, strlen(error_message));
					exit(0);
				}
			}
	    }
		strcpy(buffer,"xyz");
	}
	return 0;
	}
    if (argc > 2) {
        print_error();
        exit(1);
    }
    FILE *stream = stdin;
    if (argc == 2) {
        stream = fopen(argv[1], "r");
        if (stream == NULL) {
            print_error();
            //fclose(stream);
            exit(1);
        }
    }
    paths = strdup("/bin");
    while (1) {
        print_prompt(stream);
        char *line = read_line(stream);
        if (line == NULL) {
            if (stream == stdin) {
                print_error();
                continue;
            } else {
                exit(0);
            }
        }
        int size = -1;
        struct command **commands = construct_commands(line, &size);
        if (size == 0) { 
            free(line);
            free(commands);
            continue; 
        }
        if (commands == NULL || execute_commands(commands, size) != 0) {
            print_error();
        }
        free(line);
        free(commands);
    }
		/*if(batchMode == 1){
		for (j = 0; j < num_subcmds; j++){
			printf("Command %d: \"%s\"\n",j,subcmds[j]);
			//write(STDOUT_FILENO, subcmds[j], strlen(subcmds[j]));
			//if(j!=num_subcmds-1) write(STDOUT_FILENO, " ", strlen(" "));
		}
		//write(STDOUT_FILENO, "\n", strlen("\n"));

	}*/

       /*if(strlen(buffer)>513){
			char error_message[30] = "An error has occurred\n";
			write(STDERR_FILENO, error_message, strlen(error_message));
			subcmds[0]=NULL;
		}*/
		
		/*if(strcmp(str, subcmds[j]) == 0 && strcmp(subcmds[j], "ls") != 0 && strcmp(subcmds[j], "pwd") != 0)
		{
		if(breakCommand(subcmds[j])==1);
			return (101);
		}

		else */
		
		
    /*if(strcmp(subcmds[0], "exit") == 0)
	 {
	   //printf("exiting1...");
	   return 101;
	   }*/
}
