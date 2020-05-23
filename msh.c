//A mini command shell
//Created by Tia Benson

//Alternative Compilation: gcc -Wall msh.c -o msh -std=gnu99

#define _GNU_SOURCE
#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"  //whitespace will be delimiters
#define MAX_COMMAND_SIZE 1024
#define MAX_NUM_ARGUMENTS 12    // 10 maximum arguments including the command

int main()
{
    char* cmd_str = (char*) malloc(MAX_COMMAND_SIZE);
    char* history[100];
    int pids[100] = {0};
    
    int i;
    
    //Create history array to keep track of last 100 commands
    for(i = 0; i < 100; i++)
    {
        history[i] = (char*) malloc(1024);
        memset(history[i], 0, 1024);
    }
    
    while(1)
    {
        printf ("msh> ");
        
        while(!fgets(cmd_str, MAX_COMMAND_SIZE, stdin)); //read input from the terminal
        
        //Copy command line string into history array,
        //and keep replacing history & pid array with latest 15 commands
        
        static int count = 0; //count for history and pid array
        static int count2 = 0;
        
        //Reset history array if maximum has been reached
        if(count == 100)
        {
            count = 0;
        }
        
        strncpy(history[count], cmd_str, strlen(cmd_str));
        count++;
        
        //Account for and ignore the new line character before parsing the command string
        if(strcmp(cmd_str, "\n") == 0)
        {
            continue;
        }
        
        //Parse input
        char* token[MAX_NUM_ARGUMENTS];
        
        int token_count = 0; //keeps track of number of arguments user has typed
       
        char* arg_ptr; //points to token
        
        char* working_str  = strdup(cmd_str);
        
        // move the working_str pointer to
        // keep track of its original value
        char *working_root = working_str;
        int ret;
       
        // Tokenize the input strings with whitespace used as the delimiter
        while(((arg_ptr = strsep(&working_str, WHITESPACE)) != NULL) &&
               (token_count < MAX_NUM_ARGUMENTS))
        {
            token[token_count] = strndup(arg_ptr, MAX_COMMAND_SIZE);
            
            //If there are places where nothig is typed, put NULL values
            if(strlen(token[token_count]) == 0 )
            {
                token[token_count] = NULL;
            }
            
            token_count++;
        }
        
        //Check if user types special commands (history, exit, cd, etc.)
        if((strcmp(token[0], "exit") == 0) || (strcmp(token[0], "quit") == 0))
        {
            exit(0);
        }
        
        else if(strcmp(token[0], "history") == 0)
        {
            for(i = 0; i < 100; i++)
            {
                printf("%s", history[i]);
            }
            
            continue;
        }
        
        else if(strcmp(token[0], "showpids") == 0)
        {
            for(i = 0; i < 100; i++)
            {
                if(pids[i] != 0)
                {
                    printf("%d: %d\n", i, pids[i]);
                }
            }
            
            continue;
        }
    
        else if(strcmp(token[0], "cd") == 0)
        {
            ret = chdir(token[1]);
        }
        
        else //If not a special command, then execute like a shell command
        {
            pid_t pid = fork();
      
            if(count2 == 100)
            {
                count2 = 0;
            }
        
            if(pid == 0) //if in the child process
            {
                ret = execvp(token[0], token);
            
                if(strcmp(token[0], "cd") != 0)
                {
                    if(ret == -1) //If exec reaches this point, then the command wasn't found
                    {
                        printf("%s: Command not found.\n", token[0]);
                    }
                }
            
                fflush(NULL);
                return 0;
            }

            else
            {
                int status;
            
                waitpid(pid, &status, 0);
                pids[count2] = pid;
                count2++;
            }
        }
        
        free(working_root);
    }
    
    return 0;
}



