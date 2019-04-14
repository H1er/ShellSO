/**
UNIX Shell Project

Sistemas Operativos
Grados I. Informatica, Computadores & Software
Dept. Arquitectura de Computadores - UMA

Some code adapted from "Fundamentos de Sistemas Operativos", Silberschatz et al.

To compile and run the program:
   $ gcc Shell_project.c job_control.c -o Shell
   $ ./Shell
	(then type ^D to exit program)

**/

#include "job_control.h"   // remember to compile with module job_control.c
#include <string.h>

#define MAX_LINE 256 /* 256 chars per line, per command, should be enough. */

// -----------------------------------------------------------------------
//                            MAIN
// -----------------------------------------------------------------------

int main(void)
{
	ignore_terminal_signals();
	char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
	int background;             /* equals 1 if a command is followed by '&' */
	char *args[MAX_LINE/2];     /* command line (of 256) has max of 128 arguments */
	// probably useful variables:
	int pid_fork, pid_wait; /* pid for created and waited process */
	int status;             /* status returned by wait */
	enum status status_res; /* status processed by analyze_status() */
	int info;
	int ret;				/* info processed by analyze_status() */

	while (1)   /* Program terminates normally inside get_command() after ^D is typed*/
	{
		printf("COMMAND?->");
		continue;
		fflush(stdout);
		get_command(inputBuffer, MAX_LINE, args, &background);  /* get next command */

		if(args[0]==NULL) continue;   // if empty command

		else if (!strcmp(args[0], "cd")) 	//implementacion propia del cd
		{
			if (args[1]!=NULL)
			{
				int n = chdir(args[1]);

				if (n==-1)
				{
					printf("error: %s, no existe el directorio\n", args[1]);
				}
			}
			else
			{
				chdir(getenv("HOME"));
			}
		}
		else
		{

			pid_fork = fork();


			if (pid_fork ==0)	//hijo
			{
				new_process_group(getpid());//se le asigna un id de grupo unico
				if (background ==0)
				{
					set_terminal(getpid());
				}
				//printf("\n\n%d\n\n",getpgid(pid_fork)); //se muestra el pgid
				restore_terminal_signals();
				execvp(args[0], args);
				perror("Error, command not found");
				exit(-1);
			}
			else	//padre
			{
				if (!background)	//si no es bg espera al hijo y muestra su informacion
				{
				    pid_wait = waitpid(pid_fork, &status, WUNTRACED);
					set_terminal(getpid());

					status_res = analyze_status(status, &info);
					printf("Foreground pid: %d, command %s, %s, info: %i\n\n", pid_fork, args[0], status_strings[status_res], info);
				}
				else
				{
					printf("Background job running... pid: %d, command: %s\n\n",pid_fork, args[0]);
					continue;
				}
			}
		}
		/* the steps are:
			 (1) fork a child process using fork()
			 (2) the child process will invoke execvp()
			 (3) if background == 0, the parent will wait, otherwise continue
			 (4) Shell shows a status message for processed command
			 (5) loop returns to get_commnad() function
		*/

	} // end while
}
