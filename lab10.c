/* Author(s): Warren Wnuck.
 *
 * This is lab9.c the csc60mshell
 * This program serves as a skeleton for doing labs 9, 10, 11.
 * Student is required to use this program to build a mini shell
 * using the specification as documented in direction.
 * Date: Spring 2019
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

/* Define Section */
#define MAXLINE 80
#define MAXARGS 20
#define MAX_PATH_LENGTH 50
#define TRUE 1

/* function prototypes */
int parseline(char *cmdline, char **argv);

//The two function prototypes below will be needed in lab10. 
//Leave them here to be used later.
/* void process_input(int argc, char **argv); */
/* void handle_redir(int argc, char *argv[]); */

/* ----------------------------------------------------------------- */
/*                  The main program starts here                     */
/* ----------------------------------------------------------------- */
int main(void)
{
    char path[MAX_PATH_LENGTH];
    char cmdline[MAXLINE];
    char *argv[MAXARGS];
    int argc;
    int status;
    pid_t pid;
    int i;

    /* Loop forever to wait and process commands */
    while (TRUE) {
	/* Print your shell name: csc60msh (m for mini shell) */
	printf("csc60msh > ");

	/* Read the command line */
	fgets(cmdline, MAXLINE, stdin);

	/* Call parseline to build argc/argv */
        argc = parseline(cmdline, argv);

	/* If user hits enter key without a command, continue to loop */
	/* again at the beginning */
	/*  Hint: if argc is zero, no command declared */
	/*  Hint: look up for the keyword "continue" in C */
	/* Handle build-in command: exit, pwd, or cd  */
	/* Put the rest of your code here */
	for (i = 0; i < argc; i++){
		printf("Argc %i = %s\n",i,argv[i]);
	}

	if (argc == 0){
	   continue;
	}
	else if(strcmp(argv[0], "exit") == 0){
		return EXIT_SUCCESS;
	}
	else if(strcmp(argv[0], "pwd") == 0){
		(getcwd(path, MAX_PATH_LENGTH)) ? printf("%s\n", path) : printf("Cannot Print The Current Working Directory.\n");
	}
	else if(strcmp(argv[0], "cd") == 0){
		if (argc == 1){
			chdir(getenv("HOME"));
			continue;
		}
		else{
			if (chdir(argv[1]) == -1){
				int dirError = chdir(argv[1]);
				(dirError < 0) ? perror("ERROR") : chdir(argv[1]);
				continue;
			}
		}
	}
	//else{
		//fprintf(stderr, "Error: Unknown Command.\n");
	//}
   


 
	
	/* Else, fork off a process */
        else {
	    pid = fork();
          switch(pid)
    	    {
	    	case -1:
		    perror("Shell Program fork error");
	            exit(EXIT_FAILURE);
         	case 0:
		    /* I am child process. I will execute the command, */
		    /* and call: execvp */
		    process_input(argc, argv);
		    break;
	   	default:
		    /* I am parent process */
		    if (wait(&status) == -1)
		    	perror("Parent Process error");
		    else
		   	printf("Child returned status: %d\n",status);
		    break;
	    } 	/* end of the switch */
	}	/* end of the if-else-if */

    }		/* end of the while */
} 		/* end of main */

/* ----------------------------------------------------------------- */
/*                  parseline                                        */
/* ----------------------------------------------------------------- */
/* parse input line into argc/argv format */

int parseline(char *cmdline, char **argv)
{
    int argc = 0;
    char *separator = " \n\t"; /* Includes space, Enter, Tab */
 
    /* strtok searches for the characters listed in separator */
    argv[argc] = strtok(cmdline, separator);

    while ((argv[argc] != NULL) && (argc+1 < MAXARGS)) 
    	argv[++argc] = strtok((char *) 0, separator);
     		
    return argc;  //This becomes "argc" back in main.
}
/* ----------------------------------------------------------------- */
/*                  process_input                                    */
/* ----------------------------------------------------------------- */
void process_input(int argc, char **argv) {                       

    /* Step 1: Call handle_redir to deal with operators:            */
    /* < , or  >, or both                                           */
    handle_redir(argc,argv);
    /* Step 2: perform system call execvp to execute command        */
    /* Hint: Please be sure to review execvp.c sample program       */
    int ret = execvp(argv[0], argv);
    if (ret == -1) {                                        
        fprintf(stderr, "Error on the exec call\n");              
        _exit(EXIT_FAILURE);                                      
     }                                                            
 
 }
/* ----------------------------------------------------------------- */
void handle_redir(int argc, char *argv[])
/* ----------------------------------------------------------------- */
{
    int i,fd;
    int in_redir = 0;
    int out_redir = 0;
 
    

    for (i = 0; i < argc; i++){
        if ( strcmp(argv[i],">") == 0){
	    if (out_redir != 0){
		fprintf(stderr,"Cannot output to more than one file.\n");
		_exit(EXIT_FAILURE);
	    }
	
            else if (i == 0){
	        fprintf(stderr,"No command entered.\n");
		_exit(EXIT_FAILURE);
            }
	    out_redir = i;
	}
	else if(strcmp(argv[i],"<") == 0){
	    if (in_redir != 0){
		fprintf(stderr,"Cannot input from more than one file.\n");
		_exit(EXIT_FAILURE);
	    }
	    else if(i == 0){
		fprintf(stderr,"No command entered.\n");
		_exit(EXIT_FAILURE);
	    }
	    in_redir = i;
	}
    }
    if (out_redir != 0){
	if(argv[out_redir+1] == NULL){
	    fprintf(stderr,"There is no file.\n");
	    _exit(EXIT_FAILURE);
	}
	fd = open(argv[out_redir+1], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (fd == 2){
	    perror("File failed to open.");
	    _exit(EXIT_FAILURE);
	}
	dup2(fd,1);
	if (close(fd) == -1){
	    perror("Close file error.");
	}
	argv[out_redir] = NULL;
    }
    if (in_redir != 0){
	if(argv[in_redir+1] == NULL){
	    perror("There is no file.");
	    _exit(EXIT_FAILURE);
	}
	fd = open(argv[in_redir+1], O_RDONLY);
	if(fd == 2){
	    perror("File failed to open");
	    _exit(EXIT_FAILURE);
	}
	dup2(fd,0);
	if (close(fd) == -1){
	    perror("Close file error.");
	}
	argv[in_redir] = NULL;
    }
}
/* ----------------------------------------------------------------- */
