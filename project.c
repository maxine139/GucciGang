#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <math.h>

typedef struct
{
    long rank;
    int start_text;
    int end_text;
} Thread_Args;

typedef struct  Search_t {
    char word[256];
    int len;
    int hits;
} Search;

void readInputDatafile();                                       /* Read from input file */
void processCommandLine(int argc, char* argv[]);                /* Read CMD line        */
void *threadWork(void* args);                                   /* Thread function      */
int findWord(Search search, int start_index, int end_index);    /* Find single word     */
void getTextIndexes(Thread_Args arg);               /* Updates threads' start/end positions */
int checkIfDeliminater(char c);                 /* Checks if character is deliminater   */

int thread_count = 0;
double searches_per_thread = 0;
int chars_per_thread = 0;

char* text;
int num_searches;
int num_chars;
char* input_file_name;
Search* searches;

const int MAX_THREADS = 1024;
const int MAX_TEXT_FILE = 10000;

const int DELIMINATER_NUM = 6;
const char DELIMINATERS[DELIMINATER_NUM] = {
  ' ', '.', ',', '!', '?', '\n'
};

int main(int argc, char* argv[])
{
  processCommandLine(argc, argv);
  readInputDatafile();
  
  // define global vars
  searches_per_thread = num_searches / thread_count;
  chars_per_thread = num_chars / thread_count;

  pthread_t* thread_handles;
  Thread_Args* thread_arguments;

  thread_handles = (pthread_t*)malloc(thread_count*sizeof(pthread_t));
  thread_arguments = (Thread_Args*)malloc(thread_count*sizeof(Thread_Args));

  for (int i = 0; i < thread_count; i++)
  {
    thread_arguments[i].rank = i;
    getTextIndexes(thread_arguments[i]);
    
    pthread_create(&thread_handles[i],
        NULL, threadWork, (void*) &(thread_arguments[i]));
  }
  
  printf("Main thread: All threads have been created.\n");

  for (int i = 0; i < thread_count; i++)
  {
    pthread_join(thread_handles[i], NULL);
  }

  printf("Main thread: All threads have completed.\n");

  return 0;
}

int checkIfDeliminater(char c) {
  for (int i = 0; i < DELIMINATER_NUM; i ++) {
    if (DELIMINATERS[i] == c) {
      return 1;
    }
  }
  
  return 0;
}

void getTextIndexes(Thread_Args args) {
  static int last_index = 0;
  
  int rank = args.rank;

  if (thread_count < num_searches) {
    // dont splt text
    args.start_text = 0;
    args.end_text = num_chars;
  }
  
  args.start_text = last_index;
  args.end_text = chars_per_thread + last_index;
  
  // search for next space or new line
  while(checkIfDeliminater(text[args.end_text]) == 0) {
    args.end_text ++;
  }
  
  last_index = args.end_text;
}

/* read the input data file */
void readInputDatafile()
{
    // open file
    FILE* fp = fopen(input_file_name, "r");
    if (fp == NULL)
    {
      printf("ERROR: could not read from file - ");
      return;
    }
    text = (char*) malloc(sizeof(char)*MAX_TEXT_FILE);
 
    fscanf(fp, "%d\n", &num_searches);
    searches = malloc(sizeof(Search) * num_searches);
  
    for (int i = 0; i < num_searches; i ++) {
        fscanf(fp, "%s\n", searches[i].word);
        searches[i].len = strlen(searches[i].word);
        searches[i].hits = 0;
    }
  
    //fills array
    fread(text, sizeof(char), MAX_TEXT_FILE, fp);
    fclose(fp);
  
    num_chars = strlen(text);
}

int findWord(Search search, int start_index, int end_index)
{
    int text_index;
    char* word = search.word;
    int word_len = search.len;
    int hits = 0;

    for (text_index = start_index; text_index < end_index; text_index ++) {
        for (int c = 0; c < word_len; c ++) {
            if (word[c] != text[text_index + c]) {
                // character invalid
                break;
            }

            // check if last character passed
            if (c == word_len - 1) {
                // hit!
                hits++;
            }
        }
    }

    return hits;
}

/* print command line usage message and abort program. */
void usage(char* prog_name) {
    fprintf(stderr, "usage: %s <fn>\n", prog_name);
    fprintf(stderr, "   <fn> is name of the file containing the data to be processed\n");
    exit(0);
}

/* interpret command lines and store in shared variables */
void processCommandLine(int argc, char* argv[]) {
    if (argc != 3)
        usage(argv[0]);
    
    input_file_name = argv[1];
    thread_count = strtol(argv[2], NULL, 10);
    if (thread_count <= 0 || thread_count > MAX_THREADS)
        usage(argv[0]);
}

void *threadWork(void* argstruct)
{
    Thread_Args* args = (Thread_Args*)argstruct;
  
    int my_rank = args->rank;
    int start_word = my_rank * searches_per_thread;
    int start_text = args->start_text;
    int end_text = args->end_text;
    int local_searches_per = (int)ceil(searches_per_thread);
    int* local_hits = malloc(sizeof(int) * local_searches_per);
    Search* search;
  
    // TEST
    printf("%d: %d - %d\n", my_rank, start_text, end_text);
  
    for (int i = 0; i < local_searches_per; i ++) {
        search = &(searches[i + start_word]);

        local_hits[i] = findWord(*search, 0, num_chars);
    }
  
    //TEST
    for (int i = 0; i < local_searches_per; i ++) {
      printf("%d: %s - %d\n", my_rank, searches[start_word + i].word, local_hits[i]);
    }
}