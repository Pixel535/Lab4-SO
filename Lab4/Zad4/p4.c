#include<stdio.h>
#include<semaphore.h>
#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <stdlib.h>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <dirent.h> 
#include<stdlib.h>
#include<sys/wait.h>
#include <pthread.h>
#include <sys/mman.h>

sem_t *producent;
sem_t *konsument1;
sem_t *konsument2;
sem_t *konsument3;


void obsluga_sygnalu(int signal)
{
	printf("\n");
	sem_close(producent);
	sem_close(konsument1);
	sem_close(konsument2);
	sem_close(konsument3);
	sem_unlink("prod");
	sem_unlink("kons1");
	sem_unlink("kons2");
	sem_unlink("kons3");
	exit(0);
}

int main()
{
	char data[50];
	int i = 0;
	char* shmem = mmap(NULL,sizeof(data),PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	producent = sem_open("prod", O_CREAT, 0666, 1);
	konsument1 = sem_open("kons1", O_CREAT, 0666, 0);
	konsument2 = sem_open("kons2", O_CREAT, 0666, 0);
	konsument3 = sem_open("kons3", O_CREAT, 0666, 0);
	switch(fork())
	{
		case 0:
		while(1)
		{
			sem_wait(konsument1);
			printf("Konsument #1 [id: %d] - odczytal: %s\n", getpid(), shmem);
			
			sem_post(producent);
		}
		exit(0);

		case -1:
		printf("ERROR");
		exit (1);
			
		default:
		switch(fork())
		{
			case 0:
			while(1)
			{
				sem_wait(konsument2);
				printf("Konsument #2 [id: %d] - odczytal: %s\n", getpid(), shmem);
				
				sem_post(producent);
			}
			exit(0);
				
			case -1:
			printf("ERROR");
			exit (1);
			
			default:
			switch(fork())
			{
				case 0:
				while(1)
				{
					sem_wait(konsument3);
					printf("Konsument #3 [id: %d] - odczytal: %s\n", getpid(), shmem);
					
					sem_post(producent);
				}
				exit(0);
		
				case -1:
				printf("ERROR");
				exit (1);
			
				default:
				signal(SIGTSTP,obsluga_sygnalu);
				while(1)
				{
					sem_wait(producent);
					printf("Podaj tekst: ");
					scanf("%[^\n]%*c",data);
					memcpy(shmem, data, sizeof(data));
					if(i % 7 < 4)
					{
						sem_post(konsument1);
					}
					else if(i %7 < 6)
					{
						sem_post(konsument2);
					}
					else if(i %7 < 7)
					{
						sem_post(konsument3);
					}
					i++;
				}
				exit(0);
			}
		}
			
	}
				
	return 0;
}
