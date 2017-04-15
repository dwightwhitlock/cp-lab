#include <cstdlib>
#include <string>
#include <iostream>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <stdlib.h>

using std::cout;
using std::cin;
using std::endl;
using std::string;


bool rFlag;
char * source;
char * target;

int copy(char * source, char * target);
int recursiveCopy(char * source, char * target);

int main(int argc, char * argv[]){

  rFlag = false;
  string usageErr = "USAGE ERROR:./cp [-r] source-file target-file";

  cout.setf(std::ios_base::unitbuf); //unbuffer cout
  if(argc < 3)
    {
      cout << usageErr << endl;
      exit(EXIT_FAILURE);
    }

  if(strcmp(argv[1],"-r") == 0)
    {
      rFlag = true;
    }
  
  if(rFlag) // If recursive mode enabled
    {
      if(argc != 4)
	{
	  cout << usageErr << endl;
	  exit(EXIT_FAILURE);
	}
      source = argv[2];
      target = argv[3];
      
    }
  else //If normal mode
    {
      if(argc != 3)
	{
	  cout << usageErr << endl;
	  exit(EXIT_FAILURE);
	}
      source = argv[1];
      target = argv[2];
    }
  copy(source, target);

  return EXIT_SUCCESS;

}//main


/**
 * Uses low level system calls open, read, and write to copy a source
 * file to a target file. Target can be either a file or directory.
 *
 **/
int copy(char * source, char * target){
  
  char * cwd = get_current_dir_name();

  struct stat sourceBuf;
  stat(source, &sourceBuf);
 
  struct stat targetBuf;
  stat(target, &targetBuf);
  
  int fdSource = open(source, O_RDONLY);  
  int fdTarget = 0;
  
  /*Check errors on source */
  if(fdSource == -1)
	{
	  cout << "Could not open " << source << endl
	       << "error = "<<  strerror(errno) << endl;
	  exit(EXIT_FAILURE);
	}

  if(strcmp(source,target) == 0)
	{
	  cout << "cp: '" << source << "' and '" << target
	       << "' are the same file" << endl;
	  exit(EXIT_FAILURE);
	}


  const int BUFF_SIZE = 1024; //size of data buffer
  char buffer[BUFF_SIZE]; //data buffer
  int n = 0; //number of bytes read
  
  /*Check if target is a directory by looking for '/' at end of string */
  
  if(target[strlen(target)-1] == '/' || S_ISDIR(targetBuf.st_mode))
    {
      if(S_ISDIR(targetBuf.st_mode))
	{
	  /*Change into target directory*/
	  chdir(target);
	  
	  int fdTarget = open(source,O_RDWR|O_CREAT,sourceBuf.st_mode);//preserve mode
	  //write the contents of source to 
	  while((n = read(fdSource, buffer, BUFF_SIZE)) > 0)
	    {
	      if(write(fdTarget, buffer, n) == -1) perror("write");
	    } //while
	  exit(EXIT_SUCCESS);
 
	}
      else
	{
	  cout << "cp: cannot create regular file '" << target
	       << "': is a directory" << endl;
	  exit(EXIT_FAILURE);
	}
    }//target dir
  else //target not dir (regular file)
    {
       int fdTarget = open(target,O_RDWR|O_CREAT,sourceBuf.st_mode);//preserve mode
      while((n = read(fdSource, buffer, BUFF_SIZE)) > 0)
	{
	  if(write(fdTarget, buffer, n) == -1) perror("write");
	} //while
      
    }
      close(fdSource);
      close(fdTarget);
     
     chdir(cwd);
     
  return 1;

}//copy


/**
 * Called when rFlag is true. Recursively copies a file tree (source must be directory)
 * to the target directory. 
 **/
int recursiveCopy(char * source, char * target){


}
