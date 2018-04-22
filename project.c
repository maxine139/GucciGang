#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

typedef struct
{
    long rank;
    long num_characters;
} THREAD_ARG;


void readInputDatafile(char* filename);
void processCommandLine(int argc, char* argv[]);
void extractCommandLineArgs(int argc, char* argv[]);
void *threadWork(void* args);  /* Thread function */

int num_chars = 10000;
char* text;
char* search1;
char* search2;
char* input_file_name;

const int MAX_THREADS = 1024;

int main(int argc, char* argv[])
{
    int i_text = 0;		// index in main text array
    int i_search1 = 0;	// index in search string 1
    int i_search2 = 0;	// index in search string 2
    int times_s1 = 0;	// how many times string 1 is found
    int times_s2 = 0;	// how many times string 2 is found
    int length1;		// length of string 1
    int length2;		// length of string 2
    
    processCommandLine(argc, argv);
    readInputDatafile(input_file_name);
    
    length1 = strlen(search1);
    length2 = strlen(search2);
    
    /* ALL OF THIS IS PTHREAD STUFF. GOTTA MAKE SERIAL SOLUTION WORK FIRST
     pthread_t* thread_handles;
     THREAD_ARG* thread_arguments;
     
     thread_handles =
     (pthread_t*)malloc(thread_count*sizeof(pthread_t));
     thread_arguments =
     (THREAD_ARG*)malloc(thread_count*sizeof(THREAD_ARG));
     
     for (t_rank = 0; t_rank < thread_count; t_rank++)
     {
     thread_arguments[t_rank].rank = t_rank;
     thread_arguments[t_rank].num_vectors = vectors_per_thread;
     pthread_create(&thread_handles[t_rank],
     NULL,
     threadWork,
     (void*) &(thread_arguments[t_rank])
     );
     }
     printf("Main thread: All threads have been created.\n");
     
     for (t_rank = 0; t_rank < thread_count; t_rank++)
     pthread_join(thread_handles[t_rank], NULL);
     
     printf("Main thread: All threads have completed.\n");
     */
    
    // this stuff is copied and pasted. need to edit this to be paralellized
    while(i_text < num_chars)
    {
        //SEARCH FOR STRING 1:
        if((toupper(text[i_text]) == toupper(search1[i_search1])) && text[i_text-1] == ' ')
        {
            int counter = 0;
            // if first character of search string matches keep on searching
            while(toupper(text[i_text]) == toupper(search1[i_search1])  && text[i_text] !='\0')
            {
                i_text++;
                i_search1++;
                counter++;
            }
            
            // if we sequence of characters matching with the length of searched string
            if(i_search1 == length1 && (text[i_text] == ' ' || text[i_text] == '\0' || text[i_text] == '.' || text[i_text] == ',' || text[i_text] == '!' || text[i_text] == '?'))
            {
                // BINGO!! we find our search string.
                // Test print:
                printf("Search1: ");
                for(int i=i_text-length1; i <= i_text+1; i++) {
                    printf("%c", text[i]);
                }
                printf("\n");
                times_s1++;
            }
            i_text -= counter;
        }
        
        //SEARCH FOR STRING 2:
        if((toupper(text[i_text]) == toupper(search2[i_search2])) && text[i_text-1] == ' ')
        {     // if first character of search string matches
            // keep on searching
            while(toupper(text[i_text]) == toupper(search2[i_search2])  && text[i_text] !='\0')
            {
                i_text++;
                i_search2++;
            }
            
            // if we sequence of characters matching with the length of searched string
            if(i_search2 == length2 && (text[i_text] == ' ' || text[i_text] == '\0' || text[i_text] == '.' || text[i_text] == ',' || text[i_text] == '!' || text[i_text] == '?'))
            {
                // BINGO!! we find our search string.
                // Test print:
                printf("Search2: ");
                for(int i=i_text-length2; i <= i_text+1; i++) {
                    printf("%c", text[i]);
                }
                printf("\n");
                times_s2++;
            }
        }
        else
        {            // if first character of search string DOES NOT match
            while(text[i_text] != ' ')
            {        // Skip to next word
                i_text++;
                if(text[i_text] == '\0')
                    break;
            }
        }
        i_text++;
        i_search1 = 0;  // reset the counter to start from first character of the search string.
        i_search2 = 0;
    }
    
    if(times_s1 > 0)
    {
        printf("'%s' appears %d time(s)\n", search1, times_s1);
    }
    else
    {
        printf("'%s' does not appear in the file.\n", search1);
    }
    
    if(times_s2 > 0)
    {
        printf("'%s' appears %d time(s)\n", search2, times_s2);
    }
    else
    {
        printf("'%s' does not appear in the file.\n", search2);
    }
    
    return 0;
}

/* read the input data file */
void readInputDatafile(char* filename)
{
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) return;
    text = (char*) malloc (sizeof(char)*10000);
    //fills array
    fread(text, sizeof(char), num_chars, fp);
    fclose(fp);
}

/* print command line usage message and abort program. */
void usage(char* prog_name) {
    fprintf(stderr, "usage: %s <fn>\n", prog_name);
    fprintf(stderr, "   <fn> is name of the file containing the data to be processed\n");
    exit(0);
}

/* interpret command lines and store in shared variables */
void processCommandLine(int argc, char* argv[]) {
    if (argc != 4)
        usage(argv[0]);
    
    input_file_name = argv[1];
    //thread_count = strtol(argv[2], NULL, 10);
    search1 = argv[2];
    search2 = argv[3];
    //if (thread_count <= 0 || thread_count > MAX_THREADS)
    //  usage(argv[0]);
}

/*
 void *threadWork(void* argstruct)
 {
 long my_rank = ((THREAD_ARG*)argstruct)->rank;
 
 
 }*/
