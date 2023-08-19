#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>

#define BUFFER_SIZE 64

// dynamic memory error checker
void dmerror_checker(char **ptr,const int LINE){
	if(*ptr == NULL){
		fprintf(stderr,"Error at line %d\n",LINE);
		free(*ptr);
		*ptr = NULL;
		exit(-1);
	}
}

void error_checker(const int RETURN_VALUE ,const int LINE){
	if(RETURN_VALUE == -1){
		perror("Error");
		fprintf(stderr,"at line %d\n",LINE);
		exit(EXIT_FAILURE);
	}
	return;
}

//myread will read exactly bytes  number of bytes except if it finds EOF or gets cancelled (e.i. by a signal)
//Buffer should always be large enough
int myread(const int fd ,char *buffer ,int bytes){
	int rread =0,transfered=0;

	do{
		rread = read(fd,&buffer[transfered],bytes);
		error_checker(rread,__LINE__-1);
		bytes-=rread;
		transfered+=rread;
	}while(bytes && rread && rread != -1);

	return(transfered);
}


int main(int argc ,char *argv[]){
	int fd ,pipe_read ,output_read,pipe_total = 0 ,output_total = 0 ,diff_bytes = 0 ,i,minimum,max,common_bytes;
	char *pipe_buffer, *output_buffer;

	//open output file    
	fd = open(argv[1],O_RDONLY,00400);
	error_checker(fd,__LINE__-1);
	//
	
	//Buffers init
	pipe_buffer = malloc(BUFFER_SIZE);
	dmerror_checker(&pipe_buffer,__LINE__);
	output_buffer = malloc(BUFFER_SIZE);
	dmerror_checker(&output_buffer,__LINE__);
	//

	do{
		//read the two blocks of bytes
		pipe_read = myread(STDIN_FILENO,pipe_buffer,BUFFER_SIZE);
		output_read = myread(fd,output_buffer,BUFFER_SIZE);
		//

		//In case of EOF or some problem (i.e. signal) make sure only the common bytes are been compared
		if(pipe_read > output_read){
			minimum = output_read;
			diff_bytes+= pipe_read - output_read;
		}
		else{
			minimum = pipe_read;
			diff_bytes+= output_read - pipe_read;
		}
		//
				
		//bytes comparison
		for(i=0 ;i<minimum ;i++){
				if(pipe_buffer[i] != output_buffer[i]){
				diff_bytes++;
			}
		}
		//

		//calc the total size of each output
		output_total+= output_read;
		pipe_total+= pipe_read;
		//

	}while(pipe_read || output_read);
	
	//close and free
	close(fd);
	error_checker(fd,__LINE__-1);
	free(pipe_buffer);
	free(output_buffer);
	//

	//calculating the return value elements
	if(output_total > pipe_total){
		max = output_total;
	}
	else{
		max = pipe_total;
	}
	common_bytes = max - diff_bytes;
	//

	if(output_total == 0 && pipe_total == 0 ){
		return(100);
	}
	return((common_bytes*100/max));
}
