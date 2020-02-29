/*
NAME: Di Wu,Jingnong Qu
EMAIL: xiaowu200031@gmail.com,andrewqu2000@g.ucla.edu
ID: 205117980,805126509
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "ext2_fs.h"

#define BLOCK 1024

void error(int ret)
{
  if(ret < 0)
    {
      fprintf(stderr, "%s", strerror(errno));
      exit(1);
    }
}

int main(int argc, char** argv)
{ 
  if(argc != 2)
    {
      fprintf(stderr, "Wrong number of command line arguments.\n");
      fprintf(stderr, "Usage: ./lab3a fs_image_name\n");
      exit(1);
    }

  char* fs_name = argv[1];
  int fs = open(fs_name, O_RDONLY);
  if(fs == -1)
    {
      fprintf(stderr, "Error when opening file system image.\n");
      exit(1);
    }

  int ret = 0;
  
  //////////////////////////////
  // Super block summary
  //////////////////////////////
  struct ext2_super_block sb;
  ret = pread(fs, &sb, BLOCK, BLOCK);
  error(ret);
  printf("SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n", sb.s_blocks_count, sb.s_inodes_count, 1024<<sb.s_log_block_size, sb.s_inode_size, sb.s_blocks_per_group, sb.s_inodes_per_group, sb.s_first_ino);
  
  return 0;
}
