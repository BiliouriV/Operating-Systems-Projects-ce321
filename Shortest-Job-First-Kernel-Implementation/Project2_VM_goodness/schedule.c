/* schedule.c
 * This file contains the primary logic for the 
 * scheduler.
 */
#include "schedule.h"
#include "macros.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "list.h"
#include "privatestructs.h"

#define NEWTASKSLICE (NS_TO_JIFFIES(100000000))
#define a 0.5

/* Local Globals
 * rq - This is a pointer to the runqueue that the scheduler uses.
 * current - A pointer to the current running task.
 */
struct runqueue *rq;
struct task_struct *current;
unsigned long long start_time;
unsigned long long finish_time;
unsigned long num_task;

/* External Globals
 * jiffies - A discrete unit of time used for scheduling.
 *			 There are HZ jiffies in a second, (HZ is 
 *			 declared in macros.h), and is usually
 *			 1 or 10 milliseconds.
 */
extern long long jiffies;
extern struct task_struct *idle;

/*-----------------Initilization/Shutdown Code-------------------*/
/* This code is not used by the scheduler, but by the virtual machine
 * to setup and destroy the scheduler cleanly.
 */
 
 /* initscheduler
  * Sets up and allocates memory for the scheduler, as well
  * as sets initial values. This function should also
  * set the initial effective priority for the "seed" task 
  * and enqueu it in the scheduler.
  * INPUT:
  * newrq - A pointer to an allocated rq to assign to your
  *			local rq.
  * seedTask - A pointer to a task to seed the scheduler and start
  * the simulation.
  */
void initschedule(struct runqueue *newrq, struct task_struct *seedTask)
{
	
	seedTask->next = seedTask->prev = seedTask;
	newrq->head = seedTask;
	newrq->nr_running++;
	start_time = 0;
	finish_time = 0;
	num_task = 1;
	
}

/* killschedule
 * This function should free any memory that 
 * was allocated when setting up the runqueu.
 * It SHOULD NOT free the runqueue itself.
 */
void killschedule()
{
	
	free(rq);
	return;
}


void print_rq () {
	struct task_struct *curr;
	
	printf("Rq: \n");
	curr = rq->head;
	if (curr)
		printf("%p", curr);
	while(curr->next != rq->head) {
		curr = curr->next;	
	};
}

/*-------------Scheduler Code Goes Below------------*/
/* This is the beginning of the actual scheduling logic */

/* schedule
 * Gets the next task in the queue
 */
void schedule()
{
	static struct task_struct *curr;
	unsigned long long work_duration_time;
	long long min_goodness = 0;
	
	

	work_duration_time = 0;
	current->need_reschedule = 0; /* Always make sure to reset that, in case *
								   * we entered the scheduler because current*
								   * had requested so by setting this flag   */
	
	if (rq->nr_running == 1) {
		context_switch(rq->head);
		curr = rq->head->next;
	}
	else {
		finish_time = sched_clock();	
		curr = rq->head->next;
		min_goodness = min_goodness_calc(rq->head->next);

	
		while (curr->goodness != min_goodness){
			curr = curr->next;
		}
		
		if ((curr->thread_info->id != current->thread_info->id) && (num_task == rq->nr_running)){
			work_duration_time = finish_time - start_time;
			current->latest_burst = work_duration_time;
			current->expected_burst = ((work_duration_time+(a*current->expected_burst))/(1+a));
			current->last_run_time = finish_time;
			curr->goodness = goodness_calc(curr);
			

			context_switch(curr);
			start_time = sched_clock();
				
		}
		else if (num_task == (rq->nr_running + 1)){
			context_switch(curr);
			start_time = sched_clock();
		}
	}
	num_task = rq->nr_running;
}

/* sched_fork
 * Sets up schedule info for a newly forked task
 */
void sched_fork(struct task_struct *p)
{
	p->time_slice = 100;

	p->latest_burst = 0;
	p->expected_burst = 0;
	p->last_run_time = sched_clock();
	p->goodness = 0;


}

/* scheduler_tick
 * Updates information and priority
 * for the task that is currently running.
 */
void scheduler_tick(struct task_struct *p)
{
	schedule();
}

/* wake_up_new_task
 * Prepares information for a task
 * that is waking up for the first time
 * (being created).
 */
void wake_up_new_task(struct task_struct *p)
{	
	p->next = rq->head->next;
	p->prev = rq->head;
	p->next->prev = p;
	p->prev->next = p;
	
	rq->nr_running++;
}

/* activate_task
 * Activates a task that is being woken-up
 * from sleeping.
 */
void activate_task(struct task_struct *p)
{
	p->next = rq->head->next;
	p->prev = rq->head;
	p->next->prev = p;
	p->prev->next = p;
	
	rq->nr_running++;
}

/* deactivate_task
 * Removes a running task from the scheduler to
 * put it to sleep.
 */
void deactivate_task(struct task_struct *p)
{

	unsigned long long work_duration_time = 0;

	finish_time = sched_clock();
	work_duration_time = finish_time - start_time;
	current->latest_burst = work_duration_time;
	current->expected_burst = ((work_duration_time+(a*current->expected_burst))/(1+a));
	current->last_run_time = finish_time;

	p->prev->next = p->next;
	p->next->prev = p->prev;
	p->next = p->prev = NULL; /* Make sure to set them to NULL *
							   * next is checked in cpu.c      */

	rq->nr_running--;
}

//Here we calculate the minimum goodness time which we have calculated(goodness) it 
//later in the code
long long min_goodness_calc(struct task_struct *p){

	long long min_goodness = sched_clock();

	
	while(p != rq->head){

		if(p->goodness < min_goodness){
			min_goodness = p->goodness;
				
		}
		p = p->next;
	}

	return(min_goodness);

}

long long min(struct task_struct *p){

	long long min_exp_burst;

	min_exp_burst = sched_clock();
	
	while(p != rq->head){
		
		if(p->expected_burst < min_exp_burst){
			min_exp_burst = p->expected_burst;
				
		}
		

		p = p->next;

	}
	min_exp_burst = min_exp_burst + 1;

	return(min_exp_burst);

}
/* A function in which we calculate the maximum time that a process
is waiting at the rq
*/
long long max_wait_calc(struct task_struct *p) {

	long long wait_in_rq = 0;
	long long max_wait = 0;

	
	while(p != rq->head){
		
		wait_in_rq = start_time - p->last_run_time;
		if((wait_in_rq > max_wait) && (wait_in_rq > 0)){
			max_wait = wait_in_rq;

		}

		p = p->next;

	}
	max_wait = max_wait + 1;

	return(max_wait);
}

// In this function we're calculating the requested value
//which we will use so as to implement the sjf algorithm
long long goodness_calc(struct task_struct *p){
	
	long long max_wait;
	long long min_exp_burst;
	long long wait_in_rq;
	long long goodness;

	min_exp_burst = min(p);
	max_wait = max_wait_calc(p);
	wait_in_rq = start_time - p->last_run_time;
	if (wait_in_rq < 0) {
		wait_in_rq = 0;
	}
	
	goodness = ((1 + p->expected_burst)/min_exp_burst)*(max_wait/(1 + wait_in_rq));
	
	
	return(goodness);

}






