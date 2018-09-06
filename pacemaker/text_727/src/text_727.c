/*
 ============================================================================
 Name        : pacemaker.c
 Author      : Wang, Han
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <newlib.h>
#include <fsu_pthread.h>


#define ATRIAL_BUFF 1
#define VENTRI_BUFF 2
#define TIMESIZE 20

#define NoPace 0
#define AtrialPace 1
#define VentriPace 2


pthread_mutex_t lock_start = PTHREAD_MUTEX_INITIALIZER(lock_start) ;
pthread_cond_t cv_start = PTHREAD_COND_INITIALIZER(cv_start);

pthread_mutex_t lock_VAI = PTHREAD_MUTEX_INITIALIZER(lock_VAI) ;
pthread_cond_t cv_VAI = PTHREAD_COND_INITIALIZER(cv_VAI);

pthread_mutex_t lock_AVI = PTHREAD_MUTEX_INITIALIZER(lock_AVI) ;
pthread_cond_t cv_AVI = PTHREAD_COND_INITIALIZER(cv_AVI);

pthread_mutex_t lock_AVI_trig = PTHREAD_MUTEX_INITIALIZER(lock_AVI_trig) ;
pthread_cond_t cv_AVI_trig = PTHREAD_COND_INITIALIZER(cv_AVI_trig);

pthread_mutex_t lock_VAI_trig = PTHREAD_MUTEX_INITIALIZER(lock_VAI_trig) ;
pthread_cond_t cv_VAI_trig = PTHREAD_COND_INITIALIZER(cv_VAI_trig);

pthread_mutex_t lock_VRP = PTHREAD_MUTEX_INITIALIZER(lock_VRP) ;
pthread_cond_t cv_VRP = PTHREAD_COND_INITIALIZER(cv_VRP);

pthread_mutex_t lock_PVARP = PTHREAD_MUTEX_INITIALIZER(lock_PVARP) ;
pthread_cond_t cv_PVARP = PTHREAD_COND_INITIALIZER(cv_PVARP);

pthread_mutex_t lock_time_VAI = PTHREAD_MUTEX_INITIALIZER(lock_time_VAI) ;
pthread_cond_t cv_time_VAI = PTHREAD_COND_INITIALIZER(cv_time_VAI);

pthread_mutex_t lock_time_AVI = PTHREAD_MUTEX_INITIALIZER(lock_time_AVI) ;
pthread_cond_t cv_time_AVI = PTHREAD_COND_INITIALIZER(cv_time_AVI);

pthread_t t_ATRIAL,t_VENTRI,t_PVARP,t_VRP,t_AVI,t_VAI,t_config,t_atrial_sensor,t_ventri_sensor, t_uart;

int doc_init=0;
int VAI_init, AVI_init;
/*global variables */
int AVI_ms=150.0/0.63, 	VAI_ms=349, 	VRP_ms=620.0/0.63,	PVARP_ms=350.0/0.63;
int AVI_s=0,   		VAI_s=1,	VRP_s=0,		PVARP_s=0;
int doc_check_interval=400;
int AVItime_recv, VAItime_recv;

int count=0;
int Is_Going_To_Ventri, Is_Going_To_Atrial;

int flag_exit=0;
int flag_Sensed_Atrial=0, flag_Sensed_Ventri=0;
int flag_VRP=0, flag_PVARP=0;
int Buff_Atrial=0, Buff_Ventri=0;
int flag_state=0;
int token=1;

/*timer*/
int time[TIMESIZE];
int time_counter=0;
int time_rate1, time_rate2, heart_rate;

/*functions*/

int check_buffer(int arg)
{
	if(arg==ATRIAL_BUFF)
	  return Buff_Atrial;
	if(arg==VENTRI_BUFF)
	  return Buff_Ventri;
	return 0;
}


/* Threads */
void *conn_thread(void *x_void_ptr) {
  struct timespec outtime;

  /*
    here to config
   * */
  while(1) {
      //Label:
      //Wait for the connection   accept();

      //connection established

     /* while(1)
      {
	if (token==0)
	{
	  wait for the request
	  get request
	  switch (data)
	  {
	    case 9:		//request
	      request=1;
	      token=1;
	      break;
	    case 0:		//stop
	      request=0;
	      token=0;
	      break;
	    case 8:		//config
	      get data recv()
	      VAItime_recv = rece(4);
	      if ()
	      {

	      }
	      AVItime_recv = rece(3);
	      VAI_s=(int)VAItime_recv/1000;
	      VAI_ms=(int)VAItime_recv%1000;
	      AVI_ms=(int)AVItime_recv;
	      doc_init=1;
	      break;
	    case 2:		//disconnect
	      goto Label;
	    default:
	      break;
	  }
	}

	outtime.tv_sec =  0;
	outtime.tv_nsec = 10*1000000;
	nanosleep(&outtime,NULL);
      }

*/


    outtime.tv_sec =  0;
    outtime.tv_nsec = 100*1000000;
    nanosleep(&outtime,NULL);
  }
  return 0;
}



void *inc_config(void *x_void_ptr)
{
	struct timespec outtime;
	struct timeval now;

	pthread_mutex_lock(&lock_start);
	pthread_cond_wait(&cv_start, &lock_start);
	printf("inc_config!!\n");
	pthread_mutex_unlock(&lock_start);
	while(1)
	{
		outtime.tv_sec =  0;
		outtime.tv_nsec = doc_check_interval*1000000;				//ms
		nanosleep(&outtime,NULL);

		if(doc_init==1)
		{
			flag_exit=1;
			break;
		}
//		doc_init--;
	}
	count++;
	printf("t_config exit, count= %d \n",count);
	return 0;
}


void *inc_ATRIAL(void *x_void_ptr)
{
  int VAItime;
	struct timespec LocalTime_AVI;
	pthread_mutex_lock(&lock_start);
	printf("inc_ATRIAL!!\n");
	/*TODO: remove this
	sp_printf("Hello Uart\n");
	mp_printf("Hello Uart\n");*/

	pthread_cond_wait(&cv_start, &lock_start);

	pthread_mutex_unlock(&lock_start);
	while(1)
	{
		/*wait for VAI signal */
		pthread_mutex_lock(&lock_VAI);
		pthread_cond_wait(&cv_VAI, &lock_VAI);
		pthread_mutex_unlock(&lock_VAI);
		Is_Going_To_Atrial=1;						//Sensor disable!
		if(flag_exit)
		{
			pthread_mutex_lock(&lock_AVI_trig);
			pthread_cond_broadcast(&cv_AVI_trig);
			printf("AVI trigger signal sent, last one!!\n");
			pthread_mutex_unlock(&lock_AVI_trig);
			Is_Going_To_Atrial=1;
			Is_Going_To_Ventri=1;
			break;
		}
		time[time_counter]=clock();			//time_counter==1
		VAItime=(time[time_counter]-time[time_counter-1])/1000;
		printf("atrial pace generated!, VAI time interval = %d ms \n",VAItime);
		/*
		 if (request==1 && token==1)
		 {
		    if(VAItime<1000)
		    {
		      VAItime+=9000;
		    }
		    send VAItime here
		    token=2;
		  }
		 */

		time_counter++;					//time_counter==2

		/*send message to trig AVI timer and VRP filter*/
		pthread_mutex_lock(&lock_AVI_trig);
		pthread_cond_broadcast(&cv_AVI_trig);
		printf("AVI trig signal sent!!\n");
		pthread_mutex_unlock(&lock_AVI_trig);
	}
	printf("t_ATRIAL exit!\n");
	return 0;
}

void *inc_VENTRI(void *x_void_ptr)
{
	struct timespec LocalTime;
	pthread_mutex_lock(&lock_start);
	printf("inc_VENTRI!!\n");
	pthread_cond_wait(&cv_start, &lock_start);

	pthread_mutex_unlock(&lock_start);
	while(1)
	{
		/*wait for AVI signal*/
		pthread_mutex_lock(&lock_AVI);
		pthread_cond_wait(&cv_AVI, &lock_AVI);
		pthread_mutex_unlock(&lock_AVI);
		Is_Going_To_Ventri=1;
		if(flag_exit)
		{
			pthread_mutex_lock(&lock_VAI_trig);
			pthread_cond_broadcast(&cv_VAI_trig);
			printf("Ventric trigger VIA to end\n");
			pthread_mutex_unlock(&lock_VAI_trig);
			Is_Going_To_Atrial=1;
			Is_Going_To_Ventri=1;
			break;
		}
		time[time_counter]=clock();			//time_counter==4
		printf("ventricular pace generated! Interval between AVI signal and VP = %d ms\n", (time[time_counter]-time[time_counter-1])/1000);
		time_counter=0;					//time_counter==0
		/*send trigger AVI timer To AVI & PVARP */
		pthread_mutex_lock(&lock_VAI_trig);
		pthread_cond_broadcast(&cv_VAI_trig);
		printf("VAI trig signal sent!!\n");
		pthread_mutex_unlock(&lock_VAI_trig);
	}
	printf("t_VENTRI exit!\n");
	return 0;
}

void *inc_PVARP(void *x_void_ptr)
{
	struct timespec LocalTime;
	pthread_mutex_lock(&lock_start);
	pthread_cond_wait(&cv_start, &lock_start);
	printf("inc_PVARP!!\n");
	pthread_mutex_unlock(&lock_start);
	while(1)
	{
		/* Wait for the VAI trigger*/
		pthread_mutex_lock(&lock_VAI_trig);
		pthread_cond_wait(&cv_VAI_trig, &lock_VAI_trig);
		pthread_mutex_unlock(&lock_VAI_trig);
		if(flag_exit)
		{
			pthread_mutex_lock(&lock_PVARP);
			pthread_cond_broadcast(&cv_PVARP);
			pthread_mutex_unlock(&lock_PVARP);
			Is_Going_To_Atrial=1;
			Is_Going_To_Ventri=1;
			break;
		}

		/* Start PVARP filter*/
		LocalTime.tv_sec =  PVARP_s;
		LocalTime.tv_nsec = PVARP_ms*1000000;				//ms
		printf("PVARP filter timer START! \n");
		flag_PVARP=1;
		nanosleep(&LocalTime,NULL);
		flag_PVARP=0;
		printf("PVARP Filter timer runs out!!\n");
		pthread_mutex_lock(&lock_PVARP);
		pthread_cond_broadcast(&cv_PVARP);
		pthread_mutex_unlock(&lock_PVARP);
		if(flag_exit)
		{
			Is_Going_To_Atrial=1;
			Is_Going_To_Ventri=1;
			break;
		}
	}
   	printf("t_PVARP exit\n");
	return 0;
}

void *inc_VRP(void *x_void_ptr)
{
	struct timespec LocalTime;
	pthread_mutex_lock(&lock_start);
	pthread_cond_wait(&cv_start, &lock_start);
	printf("inc_VRP!!\n");
	pthread_mutex_unlock(&lock_start);
	while(1)
	{
		/*wait for the AVI trigger*/
		pthread_mutex_lock(&lock_AVI_trig);
		pthread_cond_wait(&cv_AVI_trig, &lock_AVI_trig);
		pthread_mutex_unlock(&lock_AVI_trig);
		if(flag_exit)
		{
			pthread_mutex_lock(&lock_VRP);
			pthread_cond_broadcast(&cv_VRP);
			pthread_mutex_unlock(&lock_VRP);
			Is_Going_To_Atrial=1;
			Is_Going_To_Ventri=1;
			break;
		}

		/*start VRP filter*/
		LocalTime.tv_sec =  VRP_s;
		LocalTime.tv_nsec = VRP_ms*1000000;				//ms
		printf("VRP filter timer START! \n");
		flag_VRP=1;
		nanosleep(&LocalTime,NULL);
		flag_VRP=0;
		printf("VRP Filter timer runs out!!\n");
		pthread_mutex_lock(&lock_VRP);
		pthread_cond_broadcast(&cv_VRP);
		pthread_mutex_unlock(&lock_VRP);
		if(flag_exit)
		{
			Is_Going_To_Atrial=1;
			Is_Going_To_Ventri=1;
			break;
		}
	}
   	printf("t_VRP exit\n");
	return 0;
}

void *inc_AVI(void *x_void_ptr)
{
  int AVItime;
	struct timespec LocalTime_AVI;
	struct timespec outtime;
	struct timeval now;
	pthread_mutex_lock(&lock_start);
	pthread_cond_wait(&cv_start, &lock_start);
	printf("inc_AVI!!\n");
	pthread_mutex_unlock(&lock_start);
	while(1)
	{
		/* Wait for PVARP filter */
		if(AVI_init==1) AVI_init=0;
		else if(flag_exit)
		{
			pthread_mutex_lock(&lock_AVI);
			pthread_cond_broadcast(&cv_AVI);
			printf("AVI timer trigger next to end!\n");
			pthread_mutex_unlock(&lock_AVI);
			Is_Going_To_Atrial=1;
			Is_Going_To_Ventri=1;
			break;
		}
		else
		{
			pthread_mutex_lock(&lock_PVARP);
			pthread_cond_wait(&cv_PVARP, &lock_PVARP);
			pthread_mutex_unlock(&lock_PVARP);
		}
		/*wait for the AVI trigger*/
		pthread_mutex_lock(&lock_AVI_trig);
		pthread_cond_wait(&cv_AVI_trig, &lock_AVI_trig);
		pthread_mutex_unlock(&lock_AVI_trig);
		Is_Going_To_Atrial=0;					//Reset flag, Enable Sensor
		if(flag_exit)
		{
			pthread_mutex_lock(&lock_AVI);
			pthread_cond_broadcast(&cv_AVI);
			printf("AVI timer trigger next to end!\n");
			pthread_mutex_unlock(&lock_AVI);
			Is_Going_To_Atrial=1;
			Is_Going_To_Ventri=1;
			break;
		}

		/*AVI timer begin*/
		time[time_counter]=clock();		//time_counter==2
		printf("AVI timer begin! before AVI begin delay = %d ms\n",(time[time_counter]-time[time_counter-1])/1000 );
		time_counter++;				//time_counter==3

		gettimeofday(&now, NULL);
		if((now.tv_usec* 1000+AVI_ms*1000000)/1000000000) outtime.tv_sec = now.tv_sec+AVI_s+1 ;
		else outtime.tv_sec = now.tv_sec+AVI_s;
		outtime.tv_nsec =(now.tv_usec* 1000+AVI_ms*1000000)%1000000000;
		pthread_mutex_lock(&lock_time_AVI);
		pthread_cond_timedwait(&cv_time_AVI, &lock_time_AVI, &outtime);//150ms
		pthread_mutex_unlock(&lock_time_AVI);

		pthread_mutex_lock(&lock_AVI);
		pthread_cond_broadcast(&cv_AVI);
		time[time_counter]=clock();			//time_counter==3
		AVItime=(time[time_counter]-time[time_counter-1])/1000;
		printf("AVI signal sent!! AVI time interval= %d ms \n",AVItime );
		pthread_mutex_unlock(&lock_AVI);
		/*if (request_AVI && token==2)
		  {

		    send AVI time ;
		    token=3;
		  }

		 */
		time_counter++;					//time_counter==4
	}
	printf("t_AVI exit\n");
	return 0;
}


/*Begin here*/
void *inc_VAI(void *x_void_ptr)
{
	int tmp;
	struct timespec LocalTime_VAI;
	struct timespec outtime;
	struct timeval now;
	pthread_mutex_lock(&lock_start);
	pthread_cond_wait(&cv_start, &lock_start);
	printf("inc_VAI!!\n");
	pthread_mutex_unlock(&lock_start);

	while(1)
	{
		count++;
		printf("count = %d \n",count);


		/* wait for the VRP filter */
		if(VAI_init==1) VAI_init=0;
		else if(flag_exit)
		{
			pthread_mutex_lock(&lock_VAI);
			pthread_cond_broadcast(&cv_VAI);
			pthread_mutex_unlock(&lock_VAI);
			printf("VAI last send /n");
			Is_Going_To_Atrial=1;
			Is_Going_To_Ventri=1;
			break;
		}
		else
		{
			pthread_mutex_lock(&lock_VRP);
			pthread_cond_wait(&cv_VRP, &lock_VRP);
			pthread_mutex_unlock(&lock_VRP);
		}
//		if(flag_exit)
//		{
//			pthread_mutex_lock(&lock_VAI);
//			pthread_cond_broadcast(&cv_VAI);
//			pthread_mutex_unlock(&lock_VAI);
//			printf("VAI last send \n");
//			Is_Going_To_Atrial=1;
//			Is_Going_To_Ventri=1;
//			break;
//		}

		/* send message to Atrial using pthread_cond_timedwait() */
		time[time_counter]=clock();		//time_counter==0
		time_counter++;				//time_counter==1
		/*
		if(request && token==3)
		{
		  time_rate2=time_rate1;
		  time_rate1=clock();
		  heart_rate=1/(((float)time_rate2-(float)time_rate1)/1000000) * 60;
		  if(heart_rate<100)
		  {
		    heart_rate=heart_rate * 10;
		  }

		  //here send haeat_rate ===>>>> matlab
		   token=0;
		}
		 */
		printf("VAI timer begin!\n");
		gettimeofday(&now, NULL);
		if((now.tv_usec* 1000+VAI_ms*1000000)/1000000000) outtime.tv_sec = now.tv_sec+VAI_s+1 ;
		else outtime.tv_sec = now.tv_sec+VAI_s;
		outtime.tv_nsec =(now.tv_usec* 1000+VAI_ms*1000000)%1000000000;
		pthread_mutex_lock(&lock_time_VAI);
		pthread_cond_timedwait(&cv_time_VAI, &lock_time_VAI, &outtime);		//850ms
		pthread_mutex_unlock(&lock_time_VAI);
//ABOVE NEED IMPROVE: SENSED ATRIAL should have a flag, marking the sense data
		pthread_mutex_lock(&lock_VAI);
		pthread_cond_broadcast(&cv_VAI);
		printf("VAI signal sent!!\n");
		pthread_mutex_unlock(&lock_VAI);

		/*wait for VAI trig*/
		pthread_mutex_lock(&lock_VAI_trig);
		pthread_cond_wait(&cv_VAI_trig, &lock_VAI_trig);
		pthread_mutex_unlock(&lock_VAI_trig);
		Is_Going_To_Ventri=0;				//Reset sensor,Enable sensor!
	}
	printf("t_VAI exit\n");
	return 0;
}

/* Atrial sensed  no filter condsidered*/
void *inc_Sensed_Atrial(void *x_void_ptr)
{
	struct timespec LocalTime;
	pthread_mutex_lock(&lock_start);
	pthread_cond_wait(&cv_start, &lock_start);
	printf("inc_Sensed_ATRIAL!!\n");
	pthread_mutex_unlock(&lock_start);
	while(1)
	{
		/*wait for VAI trig*/
		pthread_mutex_lock(&lock_VAI_trig);
		pthread_cond_wait(&cv_VAI_trig, &lock_VAI_trig);
		pthread_mutex_unlock(&lock_VAI_trig);
		while(1)
		{
			LocalTime.tv_sec  = 0;
			LocalTime.tv_nsec = 5*1000000;				//5ms
			nanosleep(&LocalTime,NULL);
			if(check_buffer(ATRIAL_BUFF))
			{
				if(flag_VRP)
				    continue;			//should reset buffer??
				pthread_cond_broadcast(&cv_time_VAI);
				pthread_mutex_unlock(&lock_time_VAI);
				break;
			}
			if(Is_Going_To_Atrial)
			{
				Is_Going_To_Atrial=0;
				break;
			}
		}
		if(flag_exit)
		{
			Is_Going_To_Atrial=1;
			Is_Going_To_Ventri=1;
			break;
		}
	}
	printf("inc_Sensed_Atrial exit\n");
	return 0;
}

void *inc_Sensed_Ventri(void *x_void_ptr)
{
	struct timespec LocalTime;
	pthread_mutex_lock(&lock_start);
	pthread_cond_wait(&cv_start, &lock_start);
	printf("inc_Sensed_Ventri!!\n");
	pthread_mutex_unlock(&lock_start);
	while(1)
	{
	    /*wait for AVI trig*/
	    pthread_mutex_lock(&lock_AVI_trig);
	    pthread_cond_wait(&cv_VAI_trig, &lock_AVI_trig);
	    pthread_mutex_unlock(&lock_AVI_trig);
	    while(1)
	    {
		LocalTime.tv_sec  = 0;
		LocalTime.tv_nsec = 5*1000000;				//5ms
		nanosleep(&LocalTime,NULL);
		if(check_buffer(VENTRI_BUFF))
		{
		    if(flag_PVARP)
		      continue;
		    pthread_cond_broadcast(&cv_time_AVI);
		    pthread_mutex_unlock(&lock_time_AVI);
		    break;
		}
		if(Is_Going_To_Ventri)
		{
		    Is_Going_To_Ventri=0;
		    break;
		}
	    }
	    if(flag_exit)
	    {
		Is_Going_To_Atrial=1;
		Is_Going_To_Ventri=1;
		break;
	    }
	}
	printf("inc_Sensed_Ventri exit\n");
	return 0;
}

int main(void) {
	puts("!!!Hello World!!!");
	pthread_init();
	/*sp_printf("Hello Uart\n");
	mp_printf("Hello Uart\n");*/
	/*Creat New threads!!*/
	while(1)
	{
		VAI_init=1;
		AVI_init=1;
		Is_Going_To_Atrial=0;
		Is_Going_To_Ventri=0;
		if(pthread_create(&t_config, NULL, inc_config, NULL)){
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
		if(pthread_create(&t_ATRIAL, NULL, inc_ATRIAL, NULL)){
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
		if(pthread_create(&t_VENTRI, NULL, inc_VENTRI, NULL)){
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
		if(pthread_create(&t_PVARP, NULL, inc_PVARP, NULL)){
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
		if(pthread_create(&t_VRP, NULL, inc_VRP, NULL)){
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
		if(pthread_create(&t_AVI, NULL, inc_AVI, NULL)){
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
		if(pthread_create(&t_VAI, NULL, inc_VAI, NULL)){
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
		if(pthread_create(&t_atrial_sensor, NULL, inc_Sensed_Atrial, NULL)){
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
		if(pthread_create(&t_ventri_sensor, NULL, inc_Sensed_Ventri, NULL)){
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
		if(pthread_create(&t_uart, NULL, conn_thread, NULL)){
		    fprintf(stderr, "Error creating thread\n");
		    return 1;
		}
		sleep(1);
		/*broadcost to start all thread*/
		pthread_mutex_lock(&lock_start);
		pthread_cond_broadcast(&cv_start);
		pthread_mutex_unlock(&lock_start);

		/*Wait pthread finish*/

		if(pthread_join(t_ATRIAL, NULL)){
			 fprintf(stderr, "Error joining thread\n");
			 return 2;
		}
		if(pthread_join(t_VENTRI, NULL)){
			 fprintf(stderr, "Error joining thread\n");
			 return 2;
		}
	   	if(pthread_join(t_PVARP, NULL)){
			 fprintf(stderr, "Error joining thread\n");
			 return 2;
		}
	   	if(pthread_join(t_VRP, NULL)){
			 fprintf(stderr, "Error joining thread\n");
			 return 2;
		}
	   	if(pthread_join(t_AVI, NULL)){
			 fprintf(stderr, "Error joining thread\n");
			 return 2;
		}
		if(pthread_join(t_VAI, NULL)){
			 fprintf(stderr, "Error joining thread\n");
			 return 2;
		}
		if(pthread_join(t_config, NULL)){
			 fprintf(stderr, "Error joining thread\n");
			 return 2;
		}
		if(pthread_join(t_atrial_sensor, NULL)){
			 fprintf(stderr, "Error joining thread\n");
			 return 2;
		}
		if(pthread_join(t_ventri_sensor, NULL)){
			 fprintf(stderr, "Error joining thread\n");
			 return 2;
		}
		sleep(1);
		printf("continue!\n");
		doc_init=0;
		flag_exit=0;
		count=0;
	}
	return 0;
}
