#include <bsp.h>

#define CONFIGURE_MAXIMUM_POSIX_THREADS 20
#define CONFIGURE_MAXIMUM_POSIX_MUTEXES 20
#define CONFIGURE_MAXIMUM_POSIX_TIMERS 20
#define CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS 40
#define CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES 40

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 20
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_EXECUTIVE_RAM_SIZE	(1024*1024)
#define CONFIGURE_MAXIMUM_SEMAPHORES	20
#define CONFIGURE_MAXIMUM_TASKS		20

#define CONFIGURE_MICROSECONDS_PER_TICK	10000

#define CONFIGURE_INIT_TASK_STACK_SIZE	(64*1024)
#define CONFIGURE_INIT_TASK_PRIORITY	100
#define CONFIGURE_INIT_TASK_INITIAL_MODES (RTEMS_PREEMPT | \
                                           RTEMS_NO_TIMESLICE | \
                                           RTEMS_NO_ASR | \
                                           RTEMS_INTERRUPT_LEVEL(0))

#define RTEMS_PCI_CONFIG_LIB
#define CONFIGURE_PCI_LIB PCI_LIB_AUTO

#define CONFIGURE_INIT
rtems_task Init (rtems_task_argument argument);

/* Configure RTEMS */
#include <rtems/confdefs.h>

#include <drvmgr/drvmgr.h>

/* Configure Driver manager */
#if defined(RTEMS_DRVMGR_STARTUP) && defined(LEON3) /* if --drvmgr was given to configure */
 /* Add Timer and UART Driver for this example */
 #ifdef CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
  #define CONFIGURE_DRIVER_AMBAPP_GAISLER_GPTIMER
 #endif
 #ifdef CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
  #define CONFIGURE_DRIVER_AMBAPP_GAISLER_APBUART
 #endif
#endif
#define CONFIGURE_DRIVER_AMBAPP_GAISLER_GRETH
#define CONFIGURE_DRIVER_AMBAPP_GAISLER_PCIF    /* PCI is for RASTA-IO/TMTC/.. GRETH */
#define CONFIGURE_DRIVER_AMBAPP_GAISLER_GRPCI   /* PCI is for RASTA-IO/TMTC/.. GRETH */
#define CONFIGURE_DRIVER_AMBAPP_GAISLER_GRPCI2  /* PCI is for RASTA-IO/TMTC/.. GRETH */
#define CONFIGURE_DRIVER_PCI_GR_RASTA_IO        /* GR-RASTA-IO has a GRETH network MAC */
#define CONFIGURE_DRIVER_PCI_GR_RASTA_ADCDAC    /* GR-RASTA-ADCDAC has a GRETH network MAC */
#define CONFIGURE_DRIVER_PCI_GR_LEON4_N2X       /* GR-CPCI-LEON4-N2X has two GRETH network MACs */




  #if 0
   #include <drvmgr/ambapp_bus.h>
   /* APBUART0 */
   struct drvmgr_key grlib_drv_res_apbuart0[] =
   {
   	{"mode", KEY_TYPE_INT, {(unsigned int)1}},
   	{"syscon", KEY_TYPE_INT, {(unsigned int)0}},
   	KEY_EMPTY
   };
   /* APBUART1 */
   struct drvmgr_key grlib_drv_res_apbuart1[] =
   {
  	{"mode", KEY_TYPE_INT, {(unsigned int)1}},
  	{"syscon", KEY_TYPE_INT, {(unsigned int)1}},
  	KEY_EMPTY
   };
   /* LEON3 System with driver configuration for 2 APBUARTs, the
    * the rest of the AMBA device drivers use their defaults.
    */
   struct drvmgr_bus_res grlib_drv_resources =
   {
   	.next = NULL,
   	.resource = {
   	{DRIVER_AMBAPP_GAISLER_APBUART_ID, 0, &grlib_drv_res_apbuart0[0]},
   	{DRIVER_AMBAPP_GAISLER_APBUART_ID, 1, &grlib_drv_res_apbuart1[0]},
   	RES_EMPTY
   	}
   };

   /* Override defualt debug UART assignment.
    * 0 = Default APBUART. APBUART[0], but on MP system CPU0=APBUART0,
    *     CPU1=APBUART1...
    * 1 = APBUART[0]
    * 2 = APBUART[1]
    * 3 = APBUART[2]
    * ...
    */
   int debug_uart_index = 2; /* second UART -- APBUART[1] */
  #endif

#ifdef LEON2
  /* PCI support for AT697 */
  #define CONFIGURE_DRIVER_LEON2_AT697PCI
  /* AMBA PnP Support for GRLIB-LEON2 */
  #define CONFIGURE_DRIVER_LEON2_AMBAPP
#endif

#include <drvmgr/drvmgr_confdefs.h>

/* Application */
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/error.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <stdlib.h>

#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>		/* struct timeval */
#include <pthread.h>
/* This is used in config.c to set up networking */
#define ENABLE_NETWORK
#define ENABLE_NETWORK_SMC_LEON2
/* Define this is a SMC91C111 CHIP is available on the I/O bus */
/*#define ENABLE_NETWORK_SMC_LEON3*/

/* Include driver configurations and system initialization */
#include "config.c"

/*************************************************************************************/
//#define ATRIAL_BUFF 1
//#define VENTRI_BUFF 2
#define TIMESIZE 20
#define SIZE_RECORD 1024
#define NoPace 0
#define AtrialPace 1
#define VentriPace 2

pthread_mutex_t lock_start = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv_start = PTHREAD_COND_INITIALIZER;

pthread_mutex_t lock_VAI = PTHREAD_MUTEX_INITIALIZER ;
pthread_cond_t cv_VAI = PTHREAD_COND_INITIALIZER;

pthread_mutex_t lock_AVI = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv_AVI = PTHREAD_COND_INITIALIZER;

pthread_mutex_t lock_AVI_trig = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv_AVI_trig = PTHREAD_COND_INITIALIZER;

pthread_mutex_t lock_VAI_trig = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv_VAI_trig = PTHREAD_COND_INITIALIZER;

pthread_mutex_t lock_VRP = PTHREAD_MUTEX_INITIALIZER ;
pthread_cond_t cv_VRP = PTHREAD_COND_INITIALIZER;

pthread_mutex_t lock_PVARP = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv_PVARP = PTHREAD_COND_INITIALIZER;

pthread_mutex_t lock_time_VAI = PTHREAD_MUTEX_INITIALIZER ;
pthread_cond_t cv_time_VAI = PTHREAD_COND_INITIALIZER;

pthread_mutex_t lock_time_AVI = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv_time_AVI = PTHREAD_COND_INITIALIZER;

pthread_mutex_t lock_G_VAI = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv_G_VAI = PTHREAD_COND_INITIALIZER;

pthread_mutex_t lock_G_AVI = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv_G_AVI = PTHREAD_COND_INITIALIZER;

pthread_mutex_t lock_G_heart = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv_G_heart = PTHREAD_COND_INITIALIZER;

pthread_mutex_t lock_trans=PTHREAD_MUTEX_INITIALIZER;


pthread_t t_ATRIAL,t_VENTRI,t_PVARP,t_VRP,t_AVI,t_VAI,t_config,t_atrial_sensor,t_ventri_sensor, t_uart;

int doc_init=0;
int VAI_init, AVI_init;
/*global variables */
//int AVI_ms=150.0/0.63, 	VAI_ms=349, 	VRP_ms=620.0/0.63,	PVARP_ms=350.0/0.63;
//int AVI_s=0,   		VAI_s=1,	VRP_s=0,		PVARP_s=0;
int AVI_ms=150.0, 	VAI_ms=850, 	VRP_ms=620.0,	PVARP_ms=350.0;
int AVI_s=0,   		VAI_s=0,	VRP_s=0,		PVARP_s=0;
int doc_check_interval=400;//ms
int AVItime_recv, VAItime_recv;
int flag_AS=0,flag_VS=0;
int count=0;
int Is_Going_To_Ventri, Is_Going_To_Atrial;

int flag_exit=0;
int flag_VRP=0, flag_PVARP=0;
int Buff_Atrial=0, Buff_Ventri=0;
int flag_state=0,flag_start=0;
int token=0;
int request=0;
/*timer*/
//long timetime[TIMESIZE];
int time_counter=0;
int time_rate1, time_rate2, heart_rate;
long time_after_boot[TIMESIZE];

int G_VAI,G_AVI,G_heart;

int client_sock;

int ATRIAL_BUFF=1,VENTRI_BUFF=2;


char VAI_trans[SIZE_RECORD][4];
char AVI_trans[SIZE_RECORD][3];
long global_counter=0;
/*functions*/

int check_buffer(int arg)
{
	if(arg==ATRIAL_BUFF)
	  return Buff_Atrial;
	if(arg==VENTRI_BUFF)
	  return Buff_Ventri;
	return 0;
}


/************************************************************************************/






/*
 * RTEMS Startup TaskC:\opt\rtems-4.10-mingw\src\samples
 */
static void
ttcpTask ();
void* th ( void* arg ) ;
void* POSIX_Init ( void *argument );
/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/
pthread_t id1 ;
pthread_t t_ATRIAL,t_VENTRI,t_PVARP,t_VRP,t_AVI,t_VAI,t_config,t_atrial_sensor,t_ventri_sensor, t_uart;



static void
rtems_ttcp_exit (int code)
{
	rtems_task_wake_after( RTEMS_MILLISECONDS_TO_TICKS(1000) );
	rtems_bsdnet_show_mbuf_stats ();
	rtems_bsdnet_show_if_stats ();
	rtems_bsdnet_show_ip_stats ();
	rtems_bsdnet_show_tcp_stats ();
	exit (code);
}

/*
 * Task to run UNIX ttcp command
 */
static void
ttcpTask ()
{
	int socket_desc  , c , read_size;
    struct sockaddr_in server , client;
    char client_message[2000];

    printf("Starting server\n");
    printk("Server Started\n\n");
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printk("Could not create socket");
    }
    printk("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        printk("bind failed. Error");
        return 1;
    }
    printk("bind done");

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    printk("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //accept connection from an incoming client
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0)
    {
        printk("accept failed");
        return 1;
    }
    printk("Connection accepted");

    //Receive a message from client
    while((read_size = read(client_sock , client_message , sizeof(client_message))) > 0 )
    {
        //Send the message back to client
        write(client_sock , client_message , strlen(client_message));
        memset( &client_message, 0, sizeof(client_message));
    }

    if(read_size == 0)
    {
        printk("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        printk("recv failed");
    }
	rtems_ttcp_exit (0);
}

/*****************************************************************************************/
void* th1 ( void* arg ) {

	struct timespec outtime;
//	printf( "Thread %d i s running \n" , pthread_self ( ) ) ;
	char str9[3]="9",str0[3]="0",str8[3]="8",str1[3]="1",str2[3]="2",str3[3]="3",msg_back[4];
	int socket_desc , client_sock , c , read_size;
	struct sockaddr_in server , client;
	char client_message[2000],data[10]={'\0'};
	long i;
	//printf("Starting server\n");
//	printk("Server Started\n\n");
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printk("Could not create socket");
	}
	    printk("Socket created");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 8888 );

	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		//print the error message
		printk("bind failed. Error");
		return 1;
	}
	printk("bind done");

	//Listen
	listen(socket_desc , 1);

	//Accept and incoming connection
//	printk("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	while(1)
	{
		printk("Waiting for incoming connections...");
	    //accept connection from an incoming client
	    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	    if (client_sock < 0)
	    {
	        printk("accept failed");
	        return 1;
	    }
	    printk("Connection accepted\n");
		while(1)
		{
			if(token==0)
			{
				int VAItemp=1200;
				puts("token ==0 here , now reading data!");

				read_size = read(client_sock , client_message , 1);
	//			for(i=0;i<3;i++)
	//				data[i]=client_message[i];
				if(strncmp(client_message,str9,1)==0) 		//request all data
				{
					//printf("yes, it's 9\n");

					if(flag_exit==1)
					{
						//printf("Please begin the pace maker!\n");
						continue;
					}

					request=1;
					token=1;
					printf("receie request\n");
					pthread_mutex_lock(&lock_G_VAI);
					pthread_cond_wait(&cv_G_VAI,&lock_G_VAI);
					pthread_mutex_unlock(&lock_G_VAI);
					sprintf(msg_back, "%d", G_VAI);
					write(client_sock,msg_back,4);

					pthread_mutex_lock(&lock_G_AVI);
					pthread_cond_wait(&cv_G_AVI,&lock_G_AVI);
					pthread_mutex_unlock(&lock_G_AVI);
					sprintf(msg_back, "%d", G_AVI);
					write(client_sock,msg_back,3);

					pthread_mutex_lock(&lock_G_heart);
					pthread_cond_wait(&cv_G_heart,&lock_G_heart);
					pthread_mutex_unlock(&lock_G_heart);
					sprintf(msg_back, "%d", G_heart);
					write(client_sock,msg_back,3);

				/*  sprintf(msg_back, "%d", VAItemp);
					printf("msg_back = ");
					puts(msg_back);
					write(client_sock,msg_back,4);*/
				}
				else if (strncmp(client_message,str3,1)==0)			//request record
				{
					printf("request record\n");
					pthread_mutex_lock(&lock_trans);
					strcpy(VAI_trans[global_counter+1],"8888");
					strcpy(AVI_trans[global_counter+1],"888");
					for(i=0;i<SIZE_RECORD;i++)
					{
						write(client_sock,VAI_trans[i],4);
					}
					for(i=0;i<SIZE_RECORD;i++)
					{
						write(client_sock,AVI_trans[i],3);
					}
					pthread_mutex_unlock(&lock_trans);
				}
				else if(strncmp(client_message,str0,1)==0)			//stop
				{
					printf("receie stop\n");
					doc_init=1;
					token=0;
				}
				else if(strncmp(client_message,str1,1)==0)		//begin
				{
					printf("receive begin\n");
					flag_start=1;

				}
				else if(strncmp(client_message,str8,1)==0)			//set AVI VAI
				{
					char data_VAI[4];
					printf("receive set\n");
					int VAI_Local,AVI_Local;
					read_size = read(client_sock , data_VAI , 4);
					sscanf(data_VAI,"%d",&VAI_Local);
					printf("VAI_Local = %d \n",VAI_Local);
					if(VAI_Local > 9000)
						VAI_Local-=9000;

					VAI_s=VAI_Local/1000;
					VAI_ms=VAI_Local%1000;

					read_size = read(client_sock , client_message , 3);
					int i;
					for(i=0;i<3;i++)
						data[i]=client_message[i];
					sscanf(data,"%d",&AVI_Local);

					if(AVI_Local > 900)
						AVI_Local -= 900;
					printf("AVI_Local = %d \n",AVI_Local);
					AVI_s=AVI_Local/1000;
					AVI_ms=AVI_Local%1000;
					if(flag_exit!=1)
						doc_init=1;
					token=0;
				}
				else if(strncmp(client_message,str2,1)==0)			//disconnect
				{
					printf("reveive disconn\n");
					break;
				}
				else
				{
					printf("It is not '9', receive data= %s \n",client_message);
				}
//				printf("msg = \n");
//				puts(data);
			}
			outtime.tv_sec =  0;
			outtime.tv_nsec = 10*1000000;
			nanosleep(&outtime,NULL);

		}
	}
}
/**************************************************************************************/
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
	}
	count++;
	printf("t_config exit");
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
			//printf("AVI trigger signal sent, last one!!\n");
			pthread_mutex_unlock(&lock_AVI_trig);
			Is_Going_To_Atrial=1;
			Is_Going_To_Ventri=1;
			break;
		}
//		timetime[time_counter]=clock();			//time_counter==1
//		VAItime=(timetime[time_counter]-timetime[time_counter-1])/1000;
		time_after_boot[time_counter]=rtems_clock_get_ticks_since_boot();
		long VAItime2=(time_after_boot[time_counter]-time_after_boot[time_counter-1])*10;
		if(flag_AS==0)
			printf("atrial pace generated!, VAI time interval = %d ms \n",VAItime2);
		else
			flag_AS=0;
		 if (request==1 && token==1)
		 {
			G_VAI=VAItime2;
			time_rate1=VAItime2;
			puts("token ==1 here");
		    if(G_VAI<1000)
		    {
		    	G_VAI+=9000;
		    }
			pthread_mutex_lock(&lock_G_VAI);
			pthread_cond_broadcast(&cv_G_VAI);
			pthread_mutex_unlock(&lock_G_VAI);
		    token=2;
		  }

		 if(VAItime2<1000)
		{
			 VAItime2+=9000;
		}

		 pthread_mutex_lock(&lock_trans);
		 sprintf(VAI_trans[global_counter], "%d", VAItime2);
		 pthread_mutex_unlock(&lock_trans);
		 printf("VAI_trans = %s ", VAI_trans[global_counter]);
//		 global_counter++;
		 if(global_counter>=SIZE_RECORD) global_counter=0;


		time_counter++;					//time_counter==2

		/*send message to trig AVI timer and VRP filter*/
		pthread_mutex_lock(&lock_AVI_trig);
		pthread_cond_broadcast(&cv_AVI_trig);
		//printf("AVI trig signal sent!!\n");
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
//		timetime[time_counter]=clock();			//time_counter==4
		time_after_boot[time_counter]=rtems_clock_get_ticks_since_boot();  //time_counter==4
//	new	long AVI_delay=(time_after_boot[time_counter]-time_after_boot[time_counter-1])*10;
//		printf("ventricular pace generated! Interval between AVI signal and VP = %d ms\n", AVI_delay);
		if(flag_VS==0)
			printf("ventricular pace generated!\n");
		else
			flag_VS=0;
		time_counter=0;					//time_counter==0
		/*send trigger AVI timer To AVI & PVARP */
		pthread_mutex_lock(&lock_VAI_trig);
		pthread_cond_broadcast(&cv_VAI_trig);
		//printf("VAI trig signal sent!!\n");
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
	pthread_mutex_unlock(&lock_start);
	printf("inc_PVARP!!\n");
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
		//printf("PVARP filter timer START! \n");
		flag_PVARP=1;
		nanosleep(&LocalTime,NULL);
		flag_PVARP=0;
		//printf("PVARP Filter timer runs out!!\n");
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
		//printf("VRP filter timer START! \n");
		flag_VRP=1;
		nanosleep(&LocalTime,NULL);
		flag_VRP=0;
		//printf("VRP Filter timer runs out!!\n");
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
	long AVItime;
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
//			printf("wait for the PVARP \n");
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
			//printf("AVI timer trigger next to end!\n");
			pthread_mutex_unlock(&lock_AVI);
			Is_Going_To_Atrial=1;
			Is_Going_To_Ventri=1;
			break;
		}

		/*AVI timer begin*/
		//timetime[time_counter]=clock();
		time_after_boot[time_counter]=rtems_clock_get_ticks_since_boot();  //time_counter==2
//	new	long AVI_delay=(time_after_boot[time_counter]-time_after_boot[time_counter-1])*10;
		//printf("AVI timer begin! before AVI begin delay = %d ms\n",AVI_delay);
		time_counter++;				//time_counter==3

		gettimeofday(&now, NULL);
		if((now.tv_usec* 1000+AVI_ms*1000000)/1000000000==1) outtime.tv_sec = now.tv_sec+AVI_s+1 ;
		else outtime.tv_sec = now.tv_sec+AVI_s;
		outtime.tv_nsec =(now.tv_usec* 1000+AVI_ms*1000000)%1000000000;
		pthread_mutex_lock(&lock_time_AVI);
		pthread_cond_timedwait(&cv_time_AVI, &lock_time_AVI, &outtime);//150ms
		pthread_mutex_unlock(&lock_time_AVI);

		pthread_mutex_lock(&lock_AVI);
		pthread_cond_broadcast(&cv_AVI);
//		timetime[time_counter]=clock();
		time_after_boot[time_counter]=rtems_clock_get_ticks_since_boot(); //time_counter==3
		AVItime=(time_after_boot[time_counter]-time_after_boot[time_counter-1])*10;
		//printf("AVI signal sent!! AVI time interval= %d ms \n",AVItime );
		pthread_mutex_unlock(&lock_AVI);
		if (request==1 && token==2)
		{
			G_AVI=AVItime;
			time_rate2=AVItime;
			if(G_AVI<100)
				G_AVI+=900;
			puts("token = 2 here !");
			pthread_mutex_lock(&lock_G_AVI);
			pthread_cond_broadcast(&cv_G_AVI);
			pthread_mutex_unlock(&lock_G_AVI);
//		    send AVI time ;
//		    char msg_back[4];
//		    sprintf(msg_back, "%d", (int)AVItime);
//			write(client_sock,msg_back,3);
		    token=3;
		}
		 if(AVItime<100)
		{
			 AVItime+=900;
		}

		 pthread_mutex_lock(&lock_trans);
		 sprintf(AVI_trans[global_counter], "%d", AVItime);
		 pthread_mutex_unlock(&lock_trans);

		 printf("AVI_trans = %s \n",AVI_trans[global_counter]);
		 global_counter++;
		 if(global_counter>=SIZE_RECORD) global_counter=0;

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
//		count++;
		//printf("count = %d \n",count);


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

		/* send message to Atrial using pthread_cond_timedwait() */
		time_after_boot[time_counter]=rtems_clock_get_ticks_since_boot();//time_counter==0
		time_counter++;				//time_counter==1


//		  time_rate2=time_rate1;
//		  time_rate1=rtems_clock_get_ticks_since_boot();
//		  printf("rate1 = %d ,rate2 = %d",time_rate1,time_rate2);
//		  heart_rate=1.0/(((float)time_rate1-(float)time_rate2)/100.0) * 60.0;
//		  time_rate2=rtems_clock_get_ticks_since_boot();

		heart_rate=60.0/(((float)time_rate1+(float)time_rate2)/1000.0);
		  if(heart_rate<100)
		  {
		    heart_rate=heart_rate * 10;
		  }
		  //printf("heart rate = %d ",heart_rate);
		if(request==1 && token==3)
		{
		  puts("token = 3 here");
		  //here send haeat_rate ===>>>> matlab
		  G_heart=heart_rate;
		  pthread_mutex_lock(&lock_G_heart);
		  pthread_cond_broadcast(&cv_G_heart);
		  pthread_mutex_unlock(&lock_G_heart);
		  token=0;
		}

		//printf("VAI timer begin!\n");
		gettimeofday(&now, NULL);
		if((now.tv_usec* 1000+VAI_ms*1000000)/1000000000) outtime.tv_sec = now.tv_sec+VAI_s+1 ;
		else outtime.tv_sec = now.tv_sec+VAI_s;
		outtime.tv_nsec =(now.tv_usec* 1000+VAI_ms*1000000)%1000000000;
		pthread_mutex_lock(&lock_time_VAI);
		pthread_cond_timedwait(&cv_time_VAI, &lock_time_VAI, &outtime);		//850ms
		pthread_mutex_unlock(&lock_time_VAI);
//ABOVE NEED IMPROVE: SENSED ATRIAL should have a flag, marking the sensed data
		pthread_mutex_lock(&lock_VAI);
		pthread_cond_broadcast(&cv_VAI);
		//printf("VAI signal sent!!\n");
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
				flag_AS=1;
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
				flag_VS=1;
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





/**************************************************************************************/

void* POSIX_Init ( void *argument ) {
//  pthread_t id1 ;
	long i;
	for(i=0;i<SIZE_RECORD;i++)
	{
		strcpy(VAI_trans[i], "9999");
		strcpy(AVI_trans[i], "999");
	}



	if (pthread_create (&id1 ,NULL, th1 ,NULL) != 0 )
		printf ( " pthread_create1 " ) ;

	while(1)
	{
		VAI_init=1;
		AVI_init=1;
		Is_Going_To_Atrial=0;
		Is_Going_To_Ventri=0;
		flag_start=0;
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
			 fprintf(stderr, "Error joining t_ATRIAL\n");
			 return 2;
		}
		if(pthread_join(t_VENTRI, NULL)){
			 fprintf(stderr, "Error joining t_VENTRI\n");
			 return 2;
		}
		if(pthread_join(t_PVARP, NULL)){
			 fprintf(stderr, "Error joining t_PVARP\n");
			 return 2;
		}
		if(pthread_join(t_VRP, NULL)){
			 fprintf(stderr, "Error joining t_VRP\n");
			 return 2;
		}
//		if(pthread_join(t_AVI, NULL)){
//			 fprintf(stderr, "Error joining t_AVI\n");
//			 return 2;
//		}
		if(pthread_join(t_VAI, NULL)){
			 fprintf(stderr, "Error joining t_VAI\n");
			 return 2;
		}
		if(pthread_join(t_config, NULL)){
			 fprintf(stderr, "Error joining t_config\n");
			 return 2;
		}
		if(pthread_join(t_atrial_sensor, NULL)){
			 fprintf(stderr, "Error joining t_atrial_sensor\n");
			 return 2;
		}
		if(pthread_join(t_ventri_sensor, NULL)){
			 fprintf(stderr, "Error joining t_ventri_sensor\n");
			 return 2;
		}
		sleep(2);

		doc_init=0;

		count=0;
		while(flag_start==0)
			sleep(1);
		flag_exit=0;
		printf("continue!\n");
	}




	if (pthread_join ( id1 ,NULL) != 0 )
		printf ( " pthread_join 1 " ) ;

	printf ( " End of the a p p l i c a t i o n \n " ) ;

	exit ( 0 ) ;
}


rtems_task
Init (rtems_task_argument ignored)
{
	/* Initialize Driver manager and Networking, in config.c */
	system_init();

	/* Run ttcp
	ttcpTask ();*/
	POSIX_Init(NULL);

	exit (0);
}
