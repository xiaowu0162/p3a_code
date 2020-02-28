/*
NAME: Di Wu,Jingnong Qu
EMAIL: xiaowu200031@gmail.com,andrewqu2000@g.ucla.edu
ID: 205117980,805126509
*/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{ 
  if(argc != 2)
    {
      fprintf(stderr, "Wrong number of command line arguments.\n");
      fprintf(stderr, "Usage: ./lab3a fs_image_name\n");
      exit(1);
    }

  char* fs_name = argv[1];
  
  return 0;
}
