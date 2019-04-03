#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FILE_BUFFER_SIZE 512
#define INIT_BUFF_SIZE 50
#define INC_BUFF_SIZE 10

	//prototypes
	void process_stream(int fd, int flag, char *str, int wid, char *fpname);
	char *fgetline(int fd);
	int fgetchar(int fd);
	

	int main(int argc, char *argv[])
	{
		
		char *file_pathname;
		int fd;
		
		int opt, flag, numfiles;
		char *str = "\t";
		int width = 6;
		flag = 0;
		numfiles = 0;
		int exitSuccess = 0;

		while ((opt = getopt(argc, argv, "bs:w:")) != -1) {
			switch (opt) {
	       		case 'b':
	       			flag = 0;
	       			opt = getopt(argc, argv, "ant");
	       			switch (opt) {
	       				case 'a':
	       					//printf("Option -ba\n");
	       					flag = 1;
	       					break;
	       				case 'n':
	       					//printf("Option -bn\n");
	       					flag = 2;
	       					break;
	       				case 't':
	       					//printf("Option -bt\n");
	       					flag = 0;
	       					break;
	       			}
		  			break;
	       		case 's':
	       			str = optarg;
	      			//printf("this is the string: %s\n", str);
		   			break;
		   		case 'w':
		   			//printf("optarg: %s", optarg);
		   			width = atoi(optarg);  //width of number column
		   			//printf("width: %d", width);
		   			break;
	       		default:  //'?'
      				fprintf(stderr,"Usage: %s [-bSTYLE] [-sSTRING] [FILE...]\n",argv[0]);
      				fprintf(stderr,"(STYLE must be one of: a, n, t)\n");
     				exit(EXIT_FAILURE);
	       	}
	   	}
	   	
	   	//printf("argc=%d optind=%d\n", argc, optind);
	   	numfiles = argc - optind;
	   	//printf("There are %d file arguments.\n", numfiles);
	   	//determines whether use standard input of if file arguments were given
	   	if(numfiles == 0){
	   		process_stream(0, flag, str, width, file_pathname);
	   	} else {
	   		while(numfiles > 0){
	   			errno = 0;
	   			file_pathname = argv[optind++];
	   			//opening file for reading:
				if((fd = open(file_pathname, O_RDONLY)) <= 0){
					fprintf(stderr, "Error opening file %s: %s\n", file_pathname, strerror(errno));
					exitSuccess = 1;
				} else {
	   				process_stream(fd, flag, str, width, file_pathname);
	   				close(fd);
	   			}
	   			numfiles--;
	   		}
	   	}
	
	   	/* Other code omitted */
	   if(exitSuccess > 0){
	   		return EXIT_FAILURE;
	   }
	   
	   return EXIT_SUCCESS;
	}
	
	//prints lines with proper formatting
	void process_stream(int fd, int flag, char *str, int wid, char *fpname){
		char *line;
		static int count = 1;
		int blank_indent_width = 6 + strlen(str);
		while((line = fgetline(fd)) != NULL){
			if(flag == 0){
				if(strcmp(line,"\n") == 0){
        			printf("%*s\n",blank_indent_width,"");
        			count--;
        		} else {
        			printf("%*d%s%s",wid,count,str,line);
        		}
			} else if(flag == 1) {
				printf("%*d%s%s",wid,count,str,line);
			} else if(flag == 2) {
				printf("%*s%s",blank_indent_width," ",line);
			} 
			count++;
			free(line);
		}
		
		if (errno > 0) {
    		fprintf(stderr,"Error reading from file %s: %s\n",fpname,strerror(errno));
    		exit(EXIT_FAILURE); 
    	}
		
	}
	
	//returns the line of a file
	char *fgetline(int fd){
		char *line_buff = malloc(INIT_BUFF_SIZE);
		int buffsize = INIT_BUFF_SIZE;
		int next, pos = 0;
		while((next = fgetchar(fd)) != '\n' && next != EOF){
			if(pos >= buffsize - 2){ //must fix so works as buff size increased as well as for \n and \0
				buffsize = buffsize + INC_BUFF_SIZE;
				line_buff = realloc(line_buff, buffsize); //must fix so can determine new size required
			}
			line_buff[pos++] = next;
		}
		if(next == EOF && (pos == 0 || errno)){
			free(line_buff);
			return NULL;
		}
		line_buff[pos++] = '\n';
  		line_buff[pos] = '\0';
  		return line_buff;
	}
	
	//returns the next character in the buffer, or if empty fills the buffer
	int fgetchar(int fd){
	
		static char buff[FILE_BUFFER_SIZE];
		static int pos = 0;
		static int nread = 0;
		
		//Determine if need to fill buffer
		if(pos == nread || pos == 0){//checks if buffer is currently empty or entirely used
			if((nread = read(fd, buff, FILE_BUFFER_SIZE)) <= 0){
				pos = 0;//pos is set back to 0 so the next file starts at beginning of buffer
				return EOF;
			}
			pos = 0;
		}
		
		//Return next char in buffer
		return buff[pos++];
	}
	
	
	
	
	


