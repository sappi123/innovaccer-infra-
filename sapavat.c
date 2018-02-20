/* Created by Sapavat Venkatesh (sapavat.venkatesh@iitg.ernet.in)
   
   Precautions while Executing this code :
	   1) Make sure the folders Documents and Desktop are present in ~
	   2) sudo privileges might be required in some cases. 
   Functionality :
	   1)This code will classify and move files from ~/Desktop to ~/Documents.
	   	 Classification will be done on basis of extension.
	     Extra Features :
	      	a) Post cleanup file stucture optimisation of ~/Desktop.
	      	b) In case of file name involving multiple .'s , the suffix of the last dot will be considered as the extension
	   2)The top 10 largest file names along with their size (in MB) in /home/* shall be displayed.
	     The underlying algorithm to achieve this runs in linear time and constant space(ignoring the space used by recursive calls) 	 	
*/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// source and destination
char src[] = "test";
char dest[] = "dest"; 

typedef struct file_data
{
	char* name;
	long  size;
}fd;

//gobal array which holds file data of largest 10 files , count determines the index at which an entry is expected
fd largest_files[10];
int count = 0;

// a new file will get into the largest_file array only if it's size is greater than the minimum sized file in the array
// This algorithm takes linear time and constant space
void insert_top_ten(fd f)
{
	if(count <= 9)
	{
		largest_files[count++]=f;
		return;
	}
	int j=0;
	for(int i=1;i<=9;i++)
		j = (largest_files[j].size < largest_files[i].size) ? j : i;
	if(f.size > largest_files[j].size)
		largest_files[j]=f;
	return;
}

// checks emptiness of a directory
int isDirectoryEmpty(char *dirname) {
  int n = 0;
  struct dirent *d;
  DIR *dir = opendir(dirname);
  if (dir == NULL) //Not a directory or doesn't exist
    return 1;
  while ((d = readdir(dir)) != NULL) {
    if(++n > 2)
      break;
  }
  closedir(dir);
  if (n <= 2) //Directory Empty
    return 1;
  else
    return 0;
}

//recursively visits all files in the directory and classifies them
static void clean_up(const char* dir_name)
{
	DIR* d;
	d = opendir(dir_name);
	while(1)
	{
		struct dirent* entry;
		const char* d_name;
		entry = readdir(d);
		if(entry==NULL)
			break;
		d_name = entry->d_name;
		int path_length;
		char path[PATH_MAX];
		path_length = snprintf(path,PATH_MAX,"%s/%s",dir_name,d_name);
		//printf("%s\n",path);
		if( (entry->d_type & DT_DIR) && strcmp(d_name,"..")!=0 && strcmp(d_name,".")!=0 )
		{
			clean_up(path);
			// if the directory is empty after the clean-up , remove it.
			if(isDirectoryEmpty(path))
			{
				rmdir(path);
			}
		}
		if(!(entry->d_type & DT_DIR))
		{
			//dot variable is meant to store the index of rightmost occuring dot in the file name
			int dot=-1,i=0;
			while(d_name[i]!='\0')
			{
				if(d_name[i]=='.')
					dot = i;
				i++;
			}
			// if dot exists and is not at first place(since hidden files name starts with a '.''), let the classification begin
			if(dot!=-1 && dot!=0)
			{
				char temp[i-dot];
				for(int j=dot+1;j<=i;j++)
					temp[j-dot-1]=d_name[j];
				//temp stores the extension name
				char new_dir[PATH_MAX];
				int new_dir_len;
				new_dir_len = snprintf(new_dir,PATH_MAX,"%s/%s",dest,temp);
				struct stat st = {0};
				// if the folder corresponding to the extension doesn't exist , create one.
				if(stat(new_dir, &st) == -1) 
				{
	   				 mkdir(new_dir, 0700);
				}
				char new_loc[PATH_MAX];
				new_dir_len = snprintf(new_loc,PATH_MAX,"%s/%s/%s",dest,temp,d_name);
				// move the folder from it's current position to it's extension folder in the destination
				rename(path,new_loc);
			}
		}
	}
	closedir(d);
	return;
}

//recursively visits all files and determines the top 10 largest of them.
static void load_top_ten(const char* dir_name)
{
	static int flag = 0;
	DIR* d;
	d = opendir(dir_name);
	if(d==NULL)
	{
		//printf("%s not found\n",dir_name);
		return;
	}
	if(flag==0)
	{
		d = opendir(".");
		flag=1;
	}
	while(1)
	{
		struct dirent* entry;
		const char* d_name;
		entry = readdir(d);
		if(entry==NULL)
			break;
		d_name = entry->d_name;
		int path_length;
		char path[PATH_MAX];
		path_length = snprintf(path,PATH_MAX,"%s/%s",dir_name,d_name);
		//printf("%s\n",path);
		if( (entry->d_type & DT_DIR) && strcmp(d_name,"..")!=0 && strcmp(d_name,".")!=0)
		{
			load_top_ten(path);
			
		}
		if(!(entry->d_type & DT_DIR))
		{
			//printf("%s\n",path);			
			struct stat dt = {0};
			stat(path,&dt);
			fd f;
			f.name = (char*)malloc(PATH_MAX*sizeof(char));
			strcpy(f.name,path);
			f.size = dt.st_size;
			// insert the file details into the algorithm which computes the top 10
			insert_top_ten(f);
			
		}
	}
	closedir(d);
	return;
}

void display_top_ten()
{
	// The global array will have the top 10 sized files , but not in a sorted manner .
	// Sorting the 10 values into descending order 
	for(int i=0;i<=count-2;i++)
	{
		for(int j=0;j<=count-2;j++)
		{
			if(largest_files[j].size<largest_files[j+1].size)
			{
				fd temp = largest_files[j];
				largest_files[j] = largest_files[j+1];
				largest_files[j+1] = temp;
			}
		}
	}
	printf("The top 10 largest files in /home/* are \n");
	printf("size(in MB)  Relative Filepath(wrt /home)\n");
	if(count==0)
	{
		printf("No files found\n");
		return;
	}
	// output displayed (The size is right formatted to 15 units of space)
	for(int i=0;i<count;i++)
	{
		printf("%15lf %s\n",(double)largest_files[i].size/(double)1000000,largest_files[i].name);
	}
	return;
}

int main(void)
{
 	//changing directory to ~ where Desktop and Documents will be available
 	chdir("~");
 	struct stat st = {0};
 	//In case destination directory doesn't exist , create one.
 	if(stat(dest,&st)==-1)
 	{
 		printf("destination directory doesn't exist\n");
 		printf("Creating destination directory (~/%s) ....\n",dest);
 		mkdir(dest,0700);
 		printf("~/%s created\n",dest);
 	}
 	printf("Cleaning up the contents of ~/%s directory.....\n",src);
 	clean_up(src);
 	printf("The contents of ~/%s will be found in ~/%s within their corresponding extension folders\n",src,dest);
 	printf("Empty directories in ~/%s have been deleted as well\n",src);
 	printf("---------------------------------------------------------------------------------------------------\n");
 	chdir("/home/");
 	load_top_ten(".");   
    display_top_ten();
    return 0;

}	

