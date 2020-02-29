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
#include <time.h>
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
  __u32 i, j, k;

  // Super block summary
  struct ext2_super_block sb;
  ret = pread(fs, &sb, BLOCK, BLOCK);
  error(ret);
  __u32 b_size = 1024<<sb.s_log_block_size;
  printf("SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n", sb.s_blocks_count, sb.s_inodes_count, b_size, sb.s_inode_size, sb.s_blocks_per_group, sb.s_inodes_per_group, sb.s_first_ino);
  
  // Group summary
  struct ext2_group_desc group_desc;   // size is 32
  ret = pread(fs, &group_desc, 32, b_size*2);
  error(ret);
  printf("GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n", 0, sb.s_blocks_per_group, sb.s_inodes_per_group, group_desc.bg_free_blocks_count, group_desc.bg_free_inodes_count, group_desc.bg_block_bitmap, group_desc.bg_inode_bitmap, group_desc.bg_inode_table);

  
  // Free block entries
  char b_bitmap[sb.s_blocks_count/8 + 1];
  ret = pread(fs, &b_bitmap, sb.s_blocks_count/8 + 1, b_size*group_desc.bg_block_bitmap);
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
  ret = pread(fs, &i_bitmap, sb.s_inodes_count/8 + 1, b_size*group_desc.bg_inode_bitmap);
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
  ret = pread(fs, &inodes, sb.s_inodes_count*sizeof(struct ext2_inode), b_size*group_desc.bg_inode_table);
  error(ret);
  for(i=0; i<sb.s_inodes_count; i++)
    {
      if(inodes[i].i_mode != 0 && inodes[i].i_links_count != 0)
	{
	  __u16 type_code = inodes[i].i_mode & 0xF000;
	  __u16 mode_code = inodes[i].i_mode & 0x0FFF;
	  char file_type = '?';
	  switch(type_code)
	    {
	    case 0x8000:
	      file_type = 'f';
	      break;
	    case 0x4000:
	      file_type = 'd';
	      break;
	    case 0xA000:
	      file_type = 's';
	      break;
	    default: break;
	    }

	  char c_time_str[100];
	  char a_time_str[100];
	  char m_time_str[100];
	  
	  time_t c_time_raw = (time_t)inodes[i].i_ctime;   //creation or change ???
	  time_t m_time_raw = (time_t)inodes[i].i_mtime;
	  time_t a_time_raw = (time_t)inodes[i].i_atime;
	  
	  struct tm* tmp;
	  tmp = gmtime(&c_time_raw);  
	  strftime(c_time_str, sizeof(c_time_str), "%D %I:%M:%S", tmp);  
	  tmp = gmtime(&m_time_raw);  
	  strftime(m_time_str, sizeof(m_time_str), "%D %I:%M:%S", tmp);  
	  tmp = gmtime(&a_time_raw);  
	  strftime(a_time_str, sizeof(a_time_str), "%D %I:%M:%S", tmp);  
	  
	  printf("INODE,%d,%c,%o,%d,%d,%d,%s,%s,%s,%d,%d", i+1, file_type, mode_code, inodes[i].i_uid, inodes[i].i_gid, inodes[i].i_links_count, c_time_str, m_time_str, a_time_str, inodes[i].i_size, inodes[i].i_blocks);
	  
	  // the remaining (no more than 15) entries
	  if(file_type == 'f' || file_type == 'd')
	    {
	      for(j=0; j<EXT2_N_BLOCKS; j++)
		printf(",%d", inodes[i].i_block[j]);
	      printf("\n");
	    }
	  else if(file_type == 's')
	    {
	      if(inodes[i].i_size >= sizeof(inodes[i].i_block))
		{
		  for(j=0; j<EXT2_N_BLOCKS; j++)
		    printf(",%d", inodes[i].i_block[j]);
		}
	      printf("\n");
	    }
	  else
	    printf("\n");
	}
    }


  // Directory entries
  struct ext2_dir_entry dirent;
  for (i = 0; i < sb.s_inodes_count; i++) {
    // If the things is a directory
    if ((inodes[i].i_mode & 0xF000) == 0x4000) {
      for (j = 0; j < EXT2_NDIR_BLOCKS; j++) {
	if (inodes[i].i_block[j] != 0) {
	  k = 0;
	  while (k < b_size) {
	    ret = pread(fs, &dirent, sizeof(struct ext2_dir_entry), b_size * inodes[i].i_block[j] + k);
	    error(ret);
	    if (dirent.inode != 0) {
	      printf("DIRENT,%d,%d,%d,%d,%d,'%.*s'\n", i + 1, j * b_size + k, dirent.inode, dirent.rec_len, dirent.name_len, dirent.name_len, dirent.name);
	    }
	    k += dirent.rec_len;
	  }
	}
      }
    }
  }

  for (i = 0; i < sb.s_inodes_count; i++) {
    // If the things is a directory
    if ((inodes[i].i_mode & 0xF000) == 0x4000 || (inodes[i].i_mode & 0xF000) == 0x8000) {
      
  return 0;
}
