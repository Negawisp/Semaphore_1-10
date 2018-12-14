#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <wait.h>
#include <assert.h>


#define SEMOP( number, value, flag )			\
{							\
	sembuffer.sem_num = number;			\
	sembuffer.sem_flg = flag;			\
	sembuffer.sem_op  = value;			\
	assert (semop (id, &sembuffer, 1) >= 0);	\
}



int main ()
{
	key_t		key = ftok   ("dragon_scroll.txt", 0);
	int		id  = semget (key, 1, 0666|IPC_CREAT);	assert (id >= 0);
	struct sembuf 	sembuffer = {};

	sembuffer.sem_num = 0;
	sembuffer.sem_flg = 0;

	int fd[2] = {};
	pipe (fd);
	int child_retvalue = 0;
	int wstatus;

	int i = 0;
	int ch_pID = 0;


	for (i = 1; i <= 10; i++)
	{
		ch_pID = fork();
		assert (ch_pID >= 0);
		
		if (ch_pID == 0) break;
	}

	if (ch_pID == 0)
	{
		SEMOP (0, -i, 0);
		assert (write (fd[1], &i, sizeof(int)) == sizeof(int));
		return 0;
	}

	for (i = 1; i <= 10; i++)
	{
		SEMOP (0, i, 0);
		assert (read (fd[0], &child_retvalue, sizeof(int)) == sizeof(int));
		wait(&wstatus);
		printf ("%d %d\n", i, child_retvalue);
	}

	
	return 0;
}
