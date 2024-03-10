#include<mqueue.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#define MAX_MSG_SIZE 1024

void send_message(mqd_t mqd,const char* msg){
	if(mq_send(mqd,msg,strlen(msg)+1,0)==-1){
		printf("mq_send failed\n");
	}
}

void receive_message(mqd_t mqd){
	char buffer[MAX_MSG_SIZE];
	int bytes_read;
	unsigned int prio;
	
	bytes_read = mq_receive(mqd,buffer,MAX_MSG_SIZE,&prio);
	if(bytes_read==-1){
		printf("mq_receive faild\n");
		return;
	}
	printf("buffer:%s\n",buffer);
}

int main(int argc,char *argv[]){
	int ret,flags;
	mqd_t mqd;
	flags = O_RDWR|O_CREAT;
	struct mq_attr attr;
	while((ret = getopt(argc,argv,"e"))!=-1){
		
		printf("getopt in\n");
		switch(ret){
			case 'e':
				printf("e have catched\n");
				flags |= O_EXCL;
				break;
		}
	}

	if(optind!=argc-1){
		printf("usage:mqcreate [-e] <name>\n");
		return -1;
	}
	
	attr.mq_flags = 0;//block
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = MAX_MSG_SIZE;
	attr.mq_curmsgs = 0;
	
	mqd = mq_open(argv[optind],flags,S_IRUSR|S_IWUSR,&attr);

	if(mqd==-1){
		printf("mq_open faild\n");
		return -1;
	}
	
	send_message(mqd,"hello");

	receive_message(mqd);		
		
	mq_close(mqd);

	return 0;
}

