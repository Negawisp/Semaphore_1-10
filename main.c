#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>

#define CHILDREN_AMMOUNT 10

#define CHILD	child_pid == 0

#define FORK()				\
	child_pid = fork ();		\
	assert (child_pid >= 0);


#define A( value_to_increase )				\
	sembuffer.sem_num = 0;				\
	sembuffer.sem_flg = 0;				\
	sembuffer.sem_op  = value_to_increase;		\
	assert (semop (semid, &sembuffer, 1) >= 0);

#define D( value_to_wait )				\
	sembuffer.sem_num = 0;				\
	sembuffer.sem_flg = 0;				\
	sembuffer.sem_op  = -value_to_wait;		\
	assert (semop (semid, &sembuffer, 1) >= 0);

#define Z()						\
	sembuffer.sem_num = 0;				\
	sembuffer.sem_flg = 0;				\
	sembuffer.sem_op  = 0;				\
	assert (semop (semid, &sembuffer, 1) >= 0);


int get_sem_id (char* filename, int length)
{
	assert (filename);

	key_t 	key = ftok (filename, 0);			assert (key >= 0);
	int 	id  = semget (key, length, 0666|IPC_CREAT);	assert (id  >= 0);

	return id;
}

int create_pipe (int* pipe_in, int* pipe_out)
{
	assert (pipe_in && pipe_out);
	
	int fd [2] = {};
	assert (pipe (fd) == 0);
	
	*pipe_out = fd[0];
	*pipe_in  = fd[1];

	return 0;
}



int main (int agrc, char* argv [], char* envp [])
{	
	int		semid      = get_sem_id ("dragon_scroll.txt", 1);
	struct sembuf 	sembuffer  = {};

	int pipe_in  = 0;
	int pipe_out = 0;
	create_pipe (&pipe_in, &pipe_out);

	int child_retvalue = 0;

	pid_t	child_pid = -1;


	int i = 0;
	for (i = 0; i < CHILDREN_AMMOUNT; i++)
	{
		FORK ();
		if (CHILD) break;
	}

	if (CHILD)
	{
		D (2*i + 1);
		assert (write (pipe_in, &i, sizeof(int)) == sizeof(int));
		D (1);
		return 0;
	}
	
	for (i = 0; i < CHILDREN_AMMOUNT; i++)
	{
		A (2*i + 2);
		assert (read (pipe_out, &child_retvalue, sizeof(int)) == sizeof(int));
		printf ("%d %d\n", i, child_retvalue);

	}
	return 0;
}
