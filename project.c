#include <stdio.h>
#include <string.h>
#include <pthread.h>

typedef struct
{
    long rank;
    long num_characters;
} THREAD_ARG;


float* readInputDatafile(char* filename, long* num_chars);
void processCommandLine(int argc, char* argv[]);
void *threadWork(void* args);  /* Thread function */



int main() {
   char s1[] = "Beauty is in the eye of the beholder";
   char s2[] = "the";

   int n = 0;
   int m = 0;
   int times = 0;
   int len = strlen(s2);      // contains the length of search string

   pthread_t* thread_handles;
   THREAD_ARG* thread_arguments;

   /* allocate thread handles */
    thread_handles =
    (pthread_t*)malloc(thread_count*sizeof(pthread_t));
    /* allocate thread argument structures */
    thread_arguments =
    (THREAD_ARG*)malloc(thread_count*sizeof(THREAD_ARG));

   /* create the threads, give each a unique rank, a random number
     *   and the number of darts they need to throw. */
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
    
    /* wait for all threads to finish */
    for (t_rank = 0; t_rank < thread_count; t_rank++)
        pthread_join(thread_handles[t_rank], NULL);
    
    printf("Main thread: All threads have completed.\n");

   

   return 0;
}

//CHANGE THIS FUNCTION
/* read the input data file */
float* readInputDatafile(char* filename, long* num_chars)
{
   long i = 0, j = 0, n = 0;
   float* input_vectors;

   FILE* fp = fopen(filename, "r");
   if (fp == NULL) return NULL;
   fscanf(fp, "%f, %f, %f\n", &(angles[0]), &(angles[1]), &(angles[2]));
   fscanf(fp, "%ld\n", &n);
   *num_vects = n;
   input_vectors = (float*)malloc(3 * n * sizeof(float));
   for (i = 0; i<n; i++)
   {
      fscanf(fp, "%f, %f, %f\n", &(input_vectors[j]), &(input_vectors[j + 1]), &(input_vectors[j + 2]));
      j += 3;
   }
   fclose(fp);
   return input_vectors;
}

/* interpret command lines and store in shared variables */
void processCommandLine(int argc, char* argv[]) {
   if (argc != 3) usage(argv[0]);
   input_file_name = argv[1];
    thread_count = strtol(argv[2], NULL, 10);
    if (thread_count <= 0 || thread_count > MAX_THREADS) usage(argv[0]);
}

void *threadWork(void* argstruct)
{
    /* unpack the thread arguments */
    long my_rank = ((THREAD_ARG*)argstruct)->rank;
    
    // this stuff is copied and pasted. need to edit this to be paralellized
    while(s1[n] != '\0') {

      if(s1[n] == s2[m]) {     // if first character of search string matches

         // keep on searching

         while(s1[n] == s2[m]  && s1[n] !='\0') {
            n++;
            m++;
         }

         // if we sequence of characters matching with the length of searched string
         if(m == len && (s1[n] == ' ' || s1[n] == '\0')) {

            // BINGO!! we find our search string.
            times++;
         }
      }else {            // if first character of search string DOES NOT match
         while(s1[n] != ' ') {        // Skip to next word
            n++;
            if(s1[n] == '\0')
            break;
         }
      }
      
      n++;
      m=0;  // reset the counter to start from first character of the search string.
   }

   if(times > 0) {
      printf("'%s' appears %d time(s)\n", s2, times);
   }else {
      printf("'%s' does not appear in the sentence.\n", s2);
   }
}
