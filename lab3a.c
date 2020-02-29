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
  int i, j;

  // Super block summary
  struct ext2_super_block sb;
  ret = pread(fs, &sb, BLOCK, BLOCK);
  error(ret);
  printf("SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n", sb.s_blocks_count, sb.s_inodes_count, 1024<<sb.s_log_block_size, sb.s_inode_size, sb.s_blocks_per_group, sb.s_inodes_per_group, sb.s_first_ino);

  
  // Group summary
  struct ext2_group_desc group_desc;   // size is 32
  ret = pread(fs, &group_desc, 32, BLOCK*2);
  error(ret);
  printf("GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n", 0, sb.s_blocks_per_group, sb.s_inodes_per_group, group_desc.bg_free_blocks_count, group_desc.bg_free_inodes_count, group_desc.bg_block_bitmap, group_desc.bg_inode_bitmap, group_desc.bg_inode_table);

  
  // Free block entries
  char b_bitmap[sb.s_blocks_count/8 + 1];
  ret = pread(fs, &b_bitmap, sb.s_blocks_count/8 + 1, BLOCK*group_desc.bg_block_bitmap);
  error(ret);
  for(i=0; i<sb.s_blocks_count/8; i++)
    {
      for(j=0; j<8; j++)
	{
	  if(i*8+(7-j) >= sb.s_blocks_count)
	    break;
	  if(!(b_bitmap[i] & (1<<j)))
	    printf("BFREE,%d\n", i*8+j+1);
	}
    }
  

  // Free inode entries
  char i_bitmap[sb.s_inodes_count/8 + 1];
  ret = pread(fs, &i_bitmap, sb.s_inodes_count/8 + 1, BLOCK*group_desc.bg_inode_bitmap);
  error(ret);
  for(i=0; i<sb.s_inodes_count/8; i++)
    {
      for(j=0; j<8; j++)
	{
	  if(i*8+(7-j) >= sb.s_inodes_count)
	    break;
	  if(!(i_bitmap[i] & (1<<j)))
	    printf("IFREE,%d\n", i*8+j+1);
	}
    }


  // Inode summary
  struct ext2_inode inodes[sb.s_inodes_count];
  ret = pread(fs, &inodes, sb.s_inodes_count*sizeof(struct ext2_inode), BLOCK*group_desc.bg_inode_table);
  error(ret);
  printf("Inode 20:\nMode: %d\nSize: %d\nFlags: %o\n", inodes[19].i_mode, inodes[19].i_size, inodes[19].i_flags);



  
  
  return 0;
}
