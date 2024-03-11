#include<mqueue.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>

#define MAX_MSG_SIZE 1024
#define NOTIFY_SIG SIGUSR1
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

void handle_signal(int sig){
	printf("receive sig%d,new message available\n",sig);
}

int main(int argc,char *argv[]){
	int ret,flags,signo;
	mqd_t mqd;
	flags = O_RDWR|O_CREAT;
	struct mq_attr attr;
	struct sigevent sev;
	sigset_t newmask;
	int bytes;
	char buffer[MAX_MSG_SIZE];
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
	if(signal(NOTIFY_SIG,handle_signal)==SIG_ERR){
		printf("signal failed\n");
	}

	sev.sigev_notify = SIGEV_SIGNAL;//notify用信号方式传递
	sev.sigev_signo = NOTIFY_SIG;
	
	sigemptyset(&newmask);
	sigaddset(&newmask,NOTIFY_SIG);
	sigprocmask(SIG_BLOCK,&newmask,NULL);
	if(mq_notify(mqd,&sev)==-1){
		printf("mq_notify failed\n");
	}

	send_message(mqd,"hello,first message");
		
	//对进程注册notify，当消息队列mqd有新消息进来，则发送信号，接收到信号后打印消息内容（需要重新notify）
	for(;;){
		sigwait(&newmask,&signo);
		if(signo==NOTIFY_SIG){		
			mq_notify(mqd,&sev);
			while(bytes = mq_receive(mqd,buffer,MAX_MSG_SIZE,NULL)>=0){
				printf("buf:%s\n",buffer);
			}
		}
	}		
		

	return 0;
}

