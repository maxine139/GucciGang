#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

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

void readInputDatafile();                                       /* Read from input file                 */
void processCommandLine(int argc, char* argv[]);                /* Read CMD line                        */
void *threadWork(void* args);                                   /* Thread function                      */
int findWord(Search search, int start_index, int end_index);    /* Find single word                     */
void getTextIndexes(int rank, int* start, int* end);            /* Updates threads' start/end positions */
int checkIfDeliminater(char c);                 /* Checks if char is deliminater      */
double getProcessTime();                    /* Gets process time          */

int num_threads = 0;
double searches_per_thread = 0;
int chars_per_thread = 0;
pthread_mutex_t* mutex;

char* text;                           /* Stored sjearch text in mem       */
int num_searches;                       /* Number of words to search        */
int num_chars;                          /* Number of chars in search text     */
char* input_file_name;                      /* File location              */
Search* searches;                       /* Stores all search struct         */
int largest_word_len = 0;                   /* Used to calc overlapping text regions    */

const int MAX_THREADS = 1024;
const int MAX_TEXT_FILE = 10000;

const int DELIMINATER_NUM = 6;
const char DELIMINATERS[6] = {
  ' ', '.', ',', '!', '?', '\n'
};

int main(int argc, char* argv[])
{
  double time_1 = getProcessTime();
  // get input from command line and read the input data file
  processCommandLine(argc, argv);
  readInputDatafile();
  double time_2 = getProcessTime();
  
  // define global vars
  searches_per_thread = ((double)num_searches) / ((double)num_threads);
  chars_per_thread = (int) ceil((double)num_chars * (searches_per_thread));
  
  pthread_t* thread_handles;
  Thread_Args* thread_arguments;
  
  // initialize the mutex locks (one lock for each string to search for)
  for(int i = 0; i < num_searches; i++)
  {
      pthread_mutex_init(&(mutex[i]), NULL);
  }

  thread_handles = (pthread_t*)malloc(num_threads*sizeof(pthread_t));
  thread_arguments = (Thread_Args*)malloc(num_threads*sizeof(Thread_Args));

  for (int i = 0; i < num_threads; i++)
  {
    thread_arguments[i].rank = i;
    
    // get text indexes
    /* TODO...broken
    getTextIndexes(
        thread_arguments[i].rank,
        &(thread_arguments[i].start_text),
        &(thread_arguments[i].end_text));
    */
    
    pthread_create(&thread_handles[i],
      NULL, threadWork, (void*) &(thread_arguments[i]));
  }
  
  for (int i = 0; i < num_threads; i++)
  {
    pthread_join(thread_handles[i], NULL);
  }
  double time_3 = getProcessTime();
  
  // print result
  for (int i = 0; i < num_searches; i ++) {
    printf("%s\tfound %d times\n", searches[i].word, searches[i].hits);
  }
  
  
  
  for (int i = 0; i < num_searches; i++)
    pthread_mutex_destroy(&(mutex[i]));
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
    mutex = malloc(sizeof(pthread_mutex_t) * num_searches);
  
    for (int i = 0; i < num_searches; i ++) {
        fscanf(fp, "%s\n", searches[i].word);
        searches[i].len = strlen(searches[i].word);
        searches[i].hits = 0;
      
        if (searches[i].len > largest_word_len) {
          largest_word_len = searches[i].len;
        }
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
        if (text_index >= num_chars) {
          // text index passed
          break;
        }
      
        for (int c = 0; c < word_len; c ++) {
            if (word[c] != text[text_index + c]) {
                // character invalid
                break;
            }

            // check if last character passed
            if (c == word_len - 1) {
                // hit! boom dead.
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
    fprintf(stderr, "   <tc> is the number of threads to use\n");
    exit(0);
}

/* interpret command lines and store in shared variables */
void processCommandLine(int argc, char* argv[]) {
    if (argc != 3)
        usage(argv[0]);
    
    input_file_name = argv[1];
    num_threads = strtol(argv[2], NULL, 10);
    if (num_threads <= 0 || num_threads > MAX_THREADS)
      usage(argv[0]);
}

double getProcessTime()
{
    clock_t t;
    t = clock();
    return ((double)t)/CLOCKS_PER_SEC;
}

void getTextIndexes(int rank, int* start, int* end) {
  static int last_index = 0;
  *start = last_index;
  *end = *start + chars_per_thread;

  while(*end < num_chars && checkIfDeliminater(text[*end]) == 0) {
    *end ++;
  }

  last_index = *end;

  if(last_index >= num_chars - 1) {
    last_index = 0;
  }

  //*start = rank % (int)((double)num_threads / (double) num_searches) * chars_per_thread;
  //*end = start_text + chars_per_thread;

}

void *threadWork(void* argstruct)
{
  Thread_Args* args = (Thread_Args*)argstruct;
  
    int my_rank = args->rank;
    int start_word = my_rank * searches_per_thread;
    int start_text = 0;
  int end_text = num_chars;
    int local_searches_per = (int)ceil(searches_per_thread);
    int* local_hits = malloc(sizeof(int) * local_searches_per);
    Search* search;

    // if extra threads, split text
    if (num_threads > num_searches) {
        start_text = my_rank % (int)((double)num_threads / (double) num_searches) * chars_per_thread;
        end_text = start_text + chars_per_thread + largest_word_len;
    }
  
    for (int i = 0; i < local_searches_per; i ++) {
        search = &(searches[i + start_word]);

        // TEST
        printf("Thread %d:\tsearching %s...\t%d - %d\n", my_rank, search->word, start_text, end_text);

        local_hits[i] = findWord(*search, start_text, end_text);
      
        /* LOCK   */
        pthread_mutex_lock(&(mutex[i+start_word]));
        search->hits += local_hits[i];
        pthread_mutex_unlock(&(mutex[i+start_word]));
        /* UNLOCK */
    }
}