#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>

#define BUFFER_SIZE 64
#define BUFFER_SIZE2 64

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
	}while(bytes && rread);

	return(transfered);
}

int infile_str_finder(const char* filepath,const char* str){
	int fd,rread,res,strsize = strlen(str);
	char *needle;

	//buffer init
	char *buffer = malloc(BUFFER_SIZE+1);
	dmerror_checker(&buffer,__LINE__);
	//

	fd = open(filepath,O_RDONLY,00600);//open file
	error_checker(fd,__LINE__-1);

	//Search for the str in the file
	do{
		rread = myread(fd,buffer,BUFFER_SIZE);
		buffer[rread] = '\0';
		needle = strstr(buffer,str);
		if(needle){
			free(buffer);
			res = close(fd);
			error_checker(res,__LINE__-1);
			return(1);
		}
		lseek(fd,-(strsize-1),SEEK_CUR);

	}while(rread>5);
	//

	free(buffer);
	res = close(fd);
	error_checker(res,__LINE__-1);
	return(0);
}

int args_counter(const char *args_filename){
	int fd,rread,res,args=0;
	char c;

	fd = open(args_filename,O_RDONLY,00400);
	error_checker(fd,__LINE__-1);

	rread = myread(fd,&c,1);
	if(rread){//checking if the first argument exists
		args++;
	}

	//every other argument is separated by at least 1 space (' ')
	//So read till you find a space
	while(rread){
		rread = myread(fd,&c,1);
		if(c == ' '){
			//then continue reading till you found an argument
			do{
				rread = myread(fd,&c,1);
				if(rread && c != ' ' && c != 10 && c !='\n'){
					args++;
					break;
				}
			}while(rread);
			//
		}

	}
	//

	res = close(fd);
	error_checker(res,__LINE__-1);
	args+=2; //we need one extra argument for the progname (argv[0]) and one for NULL
	return(args);
}

void argv_array_constructor(const char *args_filename,char *argv_array[],const int args ,const char *exec_name){
	char *buffer,c = 0;
	int res,fd,rread,i,wordlength,buffersize = BUFFER_SIZE2;

	//open args file
	fd = open(args_filename,O_RDONLY,00400);
	error_checker(fd,__LINE__-1);
	//

	argv_array[0] =strdup(exec_name);//copy first argument

	//Buffer init
	buffer = (char*) malloc(buffersize);
	dmerror_checker(&buffer,__LINE__-1);
	//

	for(i=1 ;i<args-1/*the last pos is for NULL*/ ;i++){

		for(wordlength=0,c=0 ;c !=' ' && c != '\n';wordlength++){
			
			//read the argument one char at the time
			rread = myread(fd,&buffer[wordlength],1);
			c =buffer[wordlength];
			if(!rread){
				break;
			}
			//

			//enlarging buffer if the word doesn't fit in
			if(wordlength+1==buffersize){
				buffer = realloc(buffer,buffersize+BUFFER_SIZE2);
				dmerror_checker(&buffer,__LINE__-1);
			}
			//
		}
		
		//if we have read a single space we haven't read an argument
		if(wordlength == 1 && c == ' ' ){
			i--; 
			continue;
		}
		//

		//Making it a string
		wordlength--;
		buffer[wordlength] = '\0'; //Not needed cause we use strndup (better be safe though)
		//

		argv_array[i] = strndup(buffer,wordlength); //argument added to array

		//Restoring buffer size
		buffersize = BUFFER_SIZE2;
		buffer = realloc(buffer,buffersize);
		dmerror_checker(&buffer,__LINE__-1);
		//
	}

	argv_array[i] = NULL;
	free(buffer);
	res = close(fd);
	error_checker(res,__LINE__-1);
	return;
}

void score_print(const int compilation,const int output ,const int signal){
	int total = compilation + output + signal ;

	printf("Compilation: %d\n",compilation);
	printf("Output: %d\n",output);
	printf("Memory access: %d\n",signal);
	if(total > 0){
		printf("Total: %d\n",total);
	}
	else{
		printf("Total: %d\n",0);

	}
	return;
}

int main(int argc ,char *argv[]){
	if(argc != 5){
		printf("Not enough arguements\n");
		return(2);
	}

	pid_t pid1,pid2;
	int wstatus1,wstatus2,res,fd,compile_score = 0,output_score = 0,signal_score = 0;

	//Creating the prog name
	char *progname = strdup(argv[1]);
	char *dotpos= strpbrk(progname,".");
	*dotpos = '\0';
	//

	//Creating the stderr file name
	char *file_stderr_name = (char*) malloc(strlen(progname) + 5);
	strcpy(file_stderr_name,progname);
	strcat(file_stderr_name,".err");  
	//

	//P1
	if(!(pid1 = fork())){

		// redirecting stderr to <programme>.err
		int errfd = open(file_stderr_name,O_RDWR| O_CREAT | O_TRUNC,00600);
		res = dup2(errfd,STDERR_FILENO);
		error_checker(res,__LINE__-1);
		//

		execl("/usr/bin/gcc","gcc","-Wall",argv[1],"-o",progname,NULL);
		perror("failed");
		return(0);
	}
	//
	
	waitpid(pid1,&wstatus1,0);
	if(infile_str_finder(file_stderr_name,"error:")){//error finder
		free(progname);
		free(file_stderr_name);
		score_print(-100,0,0);
		return(0);	
	}
	if(infile_str_finder(file_stderr_name,"warning:")){//warning finder
		compile_score = -10;
	}

	//Create pipe
	int mypipe[2];
	pipe(mypipe);
	//

	//P2
	if(!(pid1 = fork() )){

		//Setting up the enironment
		fd = open(argv[3],O_RDONLY,00400);
		error_checker(fd,__LINE__-1);
		res = dup2(fd,STDIN_FILENO);
		error_checker(res,__LINE__-1);
		res = dup2(mypipe[1],STDOUT_FILENO);
		error_checker(res,__LINE__-1);
		res = close(mypipe[1]);
		error_checker(res,__LINE__-1);
		res = close(mypipe[0]);
		error_checker(res,__LINE__-1);
		res = close(fd);
		error_checker(res,__LINE__-1);
		//

		//creating the executable's name
		char prog_exec[strlen(progname)+3];
		prog_exec[0] = '.';
		prog_exec[1] = '/';
		strcpy(&(prog_exec[2]),progname);
		//

		//argv array maker
		int args = args_counter(argv[2]);
		char *argv_array[args];
		argv_array_constructor(argv[2],argv_array,args,prog_exec);
		//
		
		execv(prog_exec,argv_array);
		perror("execv error");
		return(0);
		///P2
	}
	//

	//P3
	if(!(pid2 = fork() )){
			
		//Setting up the environment
		res = dup2(mypipe[0],STDIN_FILENO);
		error_checker(res,__LINE__-1);
		res = close(mypipe[1]);
		error_checker(res,__LINE__-1);
		res = close(mypipe[0]);
		error_checker(res,__LINE__-1);
		//


		execl("./p4diff","./p4diff",argv[4],NULL);
		perror("P3 execl");
		return(0);
	}
	//

	//close pipe in parent
	res = close(mypipe[0]);
	error_checker(res,__LINE__-1);
	res = close(mypipe[1]);
	error_checker(res,__LINE__-1);
	//
	
	//wait P2 and then P3
	res = waitpid(pid1,&wstatus1,0);
	error_checker(res,__LINE__-1);
	res = waitpid(pid2,&wstatus2,0);
	error_checker(res,__LINE__-1);
	//

	//P2 signal handling
	if(WIFSIGNALED(wstatus1) && (WTERMSIG(wstatus1) == SIGSEGV || WTERMSIG(wstatus1) == SIGABRT || WTERMSIG(wstatus1) == SIGBUS)){
		signal_score = -15;
	}
	//

	//P3 return harvesting
	if(WIFEXITED(wstatus2)){
		output_score = WEXITSTATUS(wstatus2);
	}
	//

	score_print(compile_score,output_score,signal_score);
	free(file_stderr_name);
	free(progname);
	return(0);
}
