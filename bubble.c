#include <stdio.h>
#include<omp.h>
#include<stdbool.h>
#include <x86intrin.h>
#include <sched.h>
#include <stdlib.h>

#define NBEXPERIMENTS    7
static long long unsigned int experiments [NBEXPERIMENTS] ;

/* 
  bubble sort -- sequential, parallel -- 
*/

static   unsigned int N ;

typedef  int  *array_int ;

/* the X array will be sorted  */

static   array_int X  ;

long long unsigned int average (long long unsigned int *exps)
{
  unsigned int i ;
  long long unsigned int s = 0 ;

  for (i = 2; i < (NBEXPERIMENTS-2); i++)
    {
      s = s + exps [i] ;
    }

  return s / (NBEXPERIMENTS-2) ;
}

void init_array (array_int T)
{
  register int i ;

  for (i = 0 ; i < N ; i++)
    {
      T [i] = N - i ;
    }
}

void print_array (array_int T)
{
  register int i ;

  for (i = 0 ; i < N ; i++)
    {
      printf ("%d ", T[i]) ;
    }
  printf ("\n") ;
}

/*
  test if T is properly sorted
 */
int is_sorted (array_int T)
{
  register int i ;
  
  for (i = 1 ; i < N ; i++)
    {
      if (T[i-1] > T [i])
	return 0 ;
    }
  return 1 ;
}

void sequential_bubble_sort (int *T, const int size)
{
    /* TODO: sequential implementation of bubble sort */ 
    register int i;
    int temp;
    int flag1 = 1;
    int flag2 = 0;
    while (flag1){
        flag2 =  0;
        for(i =0; i < N-1; i++){
            if(T[i] > T[i+1]){
                temp = T[i];
                T[i] = T[i+1];
                T[i+1] = temp;
                flag2 = 1;
            }
            
        }
       //print_array(T);
        //printf("flag2 = %d\n", is_sorted(T));
        if (flag2 == 0)
            flag1 = 0;
    }
    return ;
}

void parallel_bubble_sort (int *T, const int size)
{
    /* TODO: parallel implementation of bubble sort */
    register unsigned int i ;
	register unsigned int j ;
	register unsigned int k ;
	register int temp;
	register int temp2;
	register unsigned int flag2 = 1;
	register unsigned int flag3 = 1;
	register unsigned int th = omp_get_max_threads ();
    //print_array(T);
    while(flag3 == 1  || flag2 ==1){
    	flag3 = 0;
		flag2 = 0;
		#pragma omp parallel for schedule(guided) private(i,j,temp) reduction (||:flag3)
		for(j = 0; j < N; j += N/th){
			//printf("Thread %d has completed iteration %d on CPU %d \n", omp_get_thread_num( ), j, sched_getcpu());
		    //printf ("flag1 = %d \n", flag1);
	    	for(i =j; i <j+ N/th; i++){
	            if(T[i] > T[i+1]){
	                temp = T[i];
	                T[i] = T[i+1];
	                T[i+1] = temp;
	                flag3 =  1;
	        	}
	    	}
	    	//print_array(T);
	    }
	   
    	for(k = 0; k < N ; k += N/th){
    		if(T[k + N/th -1] > T[k + N/th]){
				temp2 = T[k + N/th - 1];
				T[k + N/th -1] = T[k + N/th];
				T[k + N/th] = temp2;	
				flag2 =  1;	
			}
		}		
		//print_array(T);
		
	}
	

  return ;

}


int main (int argc, char **argv)
{
  unsigned long long int start, end, residu ;
  unsigned long long int av1,av2 ;
  unsigned int exp ;

  /* the program takes one parameter N which is the size of the array to
     be sorted. The array will have size 2^N */
  if (argc != 2)
    {
      fprintf (stderr, "bubble N \n") ;
      exit (-1) ;
    }

  N = 1 << (atoi(argv[1])) ;
  X = (int *) malloc (N * sizeof(int)) ;

  printf("--> Sorting an array of size %u\n",N);
  
  start = _rdtsc () ;
  end   = _rdtsc () ;
  residu = end - start ;
  

  for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
      init_array (X) ;
      
      start = _rdtsc () ;

         sequential_bubble_sort (X, N) ;
     
      end = _rdtsc () ;
      experiments [exp] = end - start ;

      /* verifying that X is properly sorted */
      if (! is_sorted (X))
	{
        
	  fprintf(stderr, "SERIAL ERROR: the array is not properly sorted\n") ;
	  exit (-1) ;
	}
	        //fprintf(stderr, "SERIAL sorted \n");
    }

  av1 = average (experiments) ;  

  printf ("\n bubble serial %Ld cycles\n", av1-residu) ;

  
  for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
      init_array (X) ;
      start = _rdtsc () ;

          parallel_bubble_sort (X, N) ;
     
      end = _rdtsc () ;
      experiments [exp] = end - start ;

      /* verifying that X is properly sorted */
      if (! is_sorted (X))
	{
            fprintf(stderr, "PARALLEL ERROR: the array is not properly sorted\n") ;
            //exit (-1) ;
	}
    else{
        
        //printf("Parallel sorted array \n");
        //print_array(X);
        }
    }
   
  av2 = average (experiments) ;  
  printf ("\n bubble parallel %Ld cycles\n", av2-residu) ;
  printf("Ratio = serial/parallel = %f\n", (double)av1/av2);

  
  //print_array (X) ;

}
