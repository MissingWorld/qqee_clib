#ifndef MM_THREADPOOL_H
#define MM_THREADPOOL_H
#ifdef _cplusplus
extern "C"{
#endif

//链表实现模型，最小化内存占用 & 无需预估任务条数上限
#include "mm_array.v2.h"

#include <stdint.h>
#ifdef _WIN32
	#include <windows.h>
	#include <process.h>
#else
	//gcc -lpthread
	#include <pthread.h>
	#include <unistd.h>
#endif

#ifdef _WIN32
	//毫秒级sleep
	#define SLEEP_MS__(x) Sleep(x)    //Sleep(x) _sleep(x)
	//原子操作宏
	#define LOCK_INC__(x) InterlockedIncrement(&(x))    //返回变量执行增减操作之后的值  //32位
	#define LOCK_DEC__(x) InterlockedDecrement(&(x))    //返回变量执行增减操作之后的值  //32位
	//原子实现自旋锁
	#define LOCK__(x) while(InterlockedExchange(&(x),1)){}
	#define UNLOCK__(x) InterlockedExchange(&(x),0);
#else
	//毫秒级sleep
	#define SLEEP_MS__(x) usleep(x*1000)
	//原子操作宏
	#define LOCK_INC__(x) __sync_add_and_fetch(&(x),1)
	#define LOCK_DEC__(x) __sync_sub_and_fetch(&(x),1)
	//原子实现自旋锁
	#define LOCK__(x) while(__sync_lock_test_and_set(&(x),1)){}
	#define UNLOCK__(x) __sync_lock_release(&(x));
#endif

//任务结构
typedef struct tag_threadpool_job{
	struct mm_array_head list;
	void(*job_function)(uint64_t var);//任务函数指针，参数为8字节
	uint64_t var;//参数
}mm_threadpool_job;

typedef struct tag_threadpool{
	volatile uint8_t thread_flag;//线程池控制标记，0=运行，1=停止线程，2=暂停
	volatile uint8_t thread_now;//当前线程数 0~255
	//volatile uint16_t jobs_now;//当前任务数 0~65535
	volatile uint32_t jobs_now;//当前任务数 0~42亿
	volatile uint32_t jobs_running;//执行中的任务数，任务函数正在被执行
	//uint16_t jobs_max;//最大任务数 0~65535
	//uint32_t jobs_max;//最大任务数 0~42亿 1MB内存可存放 65536 条
	uint32_t jobs_max;//最大任务数，0=无限制
	struct mm_array_head*jobs_list_P;//任务列表链表头
	//MM_LOCK threads_lock;
	int threads_lock;//锁
}mm_threadpool;


//线程函数，抢占式获取任务，从头部弹出
int threadpool_master(mm_threadpool*threadpool){
	mm_threadpool_job*job_tmp;//取出的（任务指针+参数）结构
	while(1){
		if(threadpool->thread_flag==1){//停止线程
			return(--(threadpool->thread_now));//线程池已被手动关闭的信号
		}else if(threadpool->thread_flag==2){//暂停
			SLEEP_MS__(500);//500ms
			continue;
		}
		//队列空时等待
		LOCK__(threadpool->threads_lock);
		if(0==(threadpool->jobs_now)){
			UNLOCK__(threadpool->threads_lock);//释放锁
			SLEEP_MS__(100);//100ms
			continue;
		}
		//取出链表成员
		//LOCK__(threadpool->threads_lock);
		job_tmp=(mm_threadpool_job*)(threadpool->jobs_list_P->next);
		MM_ARRAY_DEL(threadpool->jobs_list_P->next);
		--(threadpool->jobs_now);//当前任务数
		UNLOCK__(threadpool->threads_lock);
		//执行任务
		LOCK_INC__(threadpool->jobs_running);
		(job_tmp->job_function)(job_tmp->var);
		LOCK_DEC__(threadpool->jobs_running);
		//释放游离成员
		free(job_tmp);
		job_tmp=0;
	}
}

//初始化线程池（线程数，最大任务队列长度），失败返回0，成功返回构造体指针
//线程数最好<255，任务队列最大长度=0时将无限制队列长度
mm_threadpool*mm_threadpool_init(int thread_num_min, uint32_t job_num_max){
	if(thread_num_min<1)return 0;
	//准备链表头
	struct mm_array_head*head=malloc(sizeof(struct mm_array_head));
	MM_ARRAY_INIT(head);
	//准备线程池结构
	mm_threadpool*threadpool=(mm_threadpool*)malloc(sizeof(mm_threadpool));
	if(0==threadpool){
		free(head);
		return 0;
	}
	threadpool->jobs_max=job_num_max;
	threadpool->thread_now=0;//当前线程数 0~255
	threadpool->thread_flag=0;//线程池控制标记，0=运行，1=停止线程，2=暂停
	threadpool->jobs_now=0;//当前任务数 0~42亿
	threadpool->jobs_running=0;//执行中的任务数，任务函数正在被执行
	threadpool->threads_lock=0;//锁
	//threadpool->jobs_max=job_num_max;//最大任务数 0~42亿
	threadpool->jobs_list_P=(struct mm_array_head*)head;//任务列表数组头指针
	//准备线程
	int n,i=0,err_times=0;
	for(;i<thread_num_min;){
#ifdef _WIN32
		n=_beginthreadex(0,0,(void*)&threadpool_master,(void*)threadpool,0,0);
		if(n>0){
			CloseHandle((HANDLE)n);
#else
		pthread_t ptid;
		n=pthread_create(&ptid,0,(void*)&threadpool_master,(void*)threadpool);
		if(0==n){
			//pthread_join(ptid,0);
#endif
			++i;
			++(threadpool->thread_now);
		}else{
			if(err_times++>10){//连续10次创建线程失败则放弃
				free(head);
				return 0;
			}
		}
	}
	return threadpool;
}

//添加任务，失败返回负数，成功返回当前待分配的任务数(>0)
int mm_threadpool_add_job(mm_threadpool*threadpool,void(*job_function)(uint64_t var),uint64_t var){
	if(threadpool->jobs_max>0 && (threadpool->jobs_now)>=(threadpool->jobs_max))return -1;//队列总数溢出
	mm_threadpool_job*tmp=(mm_threadpool_job*)malloc(sizeof(mm_threadpool_job));
	if(tmp==0)return -2;
	tmp->var=var;
	tmp->job_function=job_function;
	LOCK__(threadpool->threads_lock);
	MM_ARRAY_PUSH(&(tmp->list),threadpool->jobs_list_P);
	UNLOCK__(threadpool->threads_lock);
	LOCK_INC__(threadpool->jobs_now);//++(threadpool->jobs_now);//单线程添加任务时无需原子
	return threadpool->jobs_now;
}

//关闭线程池，不会立即停止全部线程，队列将被丢弃
//执行本函数后，需要手动判断当前线程数 threadpool->thread_now 是否为0，且需要手动释放 free(threadpool->jobs_list_P) 和 free(threadpool->threads_lock)
//不建议关闭线程池操作
void mm_threadpool_close(mm_threadpool*threadpool){
	threadpool->thread_flag=1;
}

//暂停线程池线程操作
void mm_threadpool_stop(mm_threadpool*threadpool){
	threadpool->thread_flag=2;
}
//恢复线程池线程操作
void mm_threadpool_resum(mm_threadpool*threadpool){
	threadpool->thread_flag=0;
}

//获取当前任务队列数
uint32_t mm_threadpool_get_jobs_n(mm_threadpool*threadpool){
	return threadpool->jobs_now;
}

//获取当前任务队列数
uint32_t mm_threadpool_get_running_n(mm_threadpool*threadpool){
	return threadpool->jobs_running;
}

#ifdef _cplusplus
}
#endif
#endif