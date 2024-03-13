#include<semaphore.h>
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>

#define NUM_THREADS 3

sem_t semaphore;

int g_sum = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;//mutex static init

pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;//cond static init
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond3 = PTHREAD_COND_INITIALIZER;

int ready1=0,ready2=0,ready3=0;

void *func_1(void *thread_id){
	long tid = (long)thread_id;
	while(1){
		sleep(1);
		pthread_mutex_lock(&mutex);	
		ready1 = 1;
		printf("ready1 ok\n");
		pthread_cond_signal(&cond1);
	
		if(!ready3){
			printf("pthread%d waitting\n",tid);
			pthread_cond_wait(&cond3,&mutex);
			printf("pthread%d can go\n",tid);
		}

		printf("func_1:g_sum=%d\n",g_sum);
		ready3 = 0;
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(NULL);
}
void *func_2(void *thread_id){
	long tid = (long)thread_id;
	while(1){
		sleep(2);
		pthread_mutex_lock(&mutex);	
		ready2 = 1;
		printf("ready2 ok\n");
		pthread_cond_signal(&cond2);
		
		if(!ready3){
			printf("pthread%d waitting\n",tid);
			pthread_cond_wait(&cond3,&mutex);
			printf("pthread%d can go\n",tid);
		}
		
		printf("func_2:g_sum=%d\n",g_sum);
		ready3 = 0;	
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(NULL);
}
void *func_master(void *thread_id){
	int i;
	long tid = (long)thread_id;
	while(1){
		pthread_mutex_lock(&mutex);
		while((!ready1)||(!ready2)){
			printf("pthread%d wait\n",tid);
			pthread_cond_wait(&cond1,&mutex);
			pthread_cond_wait(&cond2,&mutex);
			printf("pthread%d can go\n",tid);
		}
		g_sum++;
		printf("func_master:g_sum++\n");
		ready3 = 1;
		pthread_cond_broadcast(&cond3);
		
		ready1 =ready2 =0;

		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(NULL);
}
int main(){
	pthread_t threads[NUM_THREADS];	
	int ret,i;
	
	ret = pthread_create(&threads[0],NULL,func_1,(void *)1);
	if(ret){
		printf("thread_create failed\n");
	}
	
	ret = pthread_create(&threads[1],NULL,func_2,(void *)2);
	if(ret){
		printf("thread_create failed\n");
	}

	ret = pthread_create(&threads[2],NULL,func_master,(void *)3);
	if(ret){
		printf("thread_create failed\n");
	}
	for(i=0;i<NUM_THREADS;i++){
		pthread_join(threads[i],NULL);
	}
	

	return 0;
}
