#include <cstdio>
#include <cstdint>
#include <cstdlib>


#include "scheduler.h"

#define NBSINKS 5
#define NBSAMPLES 192

float input_buffer[NBSAMPLES]={0};
float output_buffer[NBSAMPLES*NBSINKS];


int main(void)
{
  
  int error;
  uint32_t nbSched = 0;
  int32_t overhead = 0;


  printf("Start\n\r");

  
  nbSched=scheduler(&error,input_buffer,output_buffer);
  
  printf("Number of schedule iterations = %d\n\r",nbSched);
  printf("Error code = %d\n\r",error);

  int diff=0;
  for(int i=0;i<NBSINKS;i++)
  {
    for(int k=0;k<NBSAMPLES;k++)
    {
        if (input_buffer[k]!=output_buffer[i*NBSAMPLES+k])
        {
            diff=1;
        }
    }
  }
  if (diff==1)
  {
    printf("Error : some output different from input");
  }
  else
  {
    printf("OK: All outputs are equal to the input");
  }


  exit(0);

}
