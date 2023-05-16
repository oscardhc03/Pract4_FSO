#include <scheduler.h>

extern THANDLER threads[MAXTHREAD];
extern int currthread;
extern int blockevent;
extern int unblockevent;

#define MAXPRIORITY 10

int numProcs=0;

QUEUE ready[MAXPRIORITY]; //una cola para cada prioridad
QUEUE waitinginevent[MAXTHREAD];

void scheduler(int arguments)
{
	int old,next;
	int changethread=0;
	int waitingthread=0;
	
	int localPriority=0;
	
	int event=arguments & 0xFF00;
	int callingthread=arguments & 0xFF;
	
	if(event==TIMER){
		if(numProcs!=0 && localPriority+1<MAXPRIORITY){
			localPriority++;
		}
		
		threads[callingthread].status=BLOCKED;
		_enqueue(&ready[localPriority],callingthread);
		changethread=1;
	}
	
	if(event==NEWTHREAD)
	{
		
		// Un nuevo hilo va a la cola de listos
		threads[callingthread].status=READY;
		_enqueue(&ready[0],callingthread); //siempre se empieza con prioridad 0
		numProcs++;
	}
	
	if(event==BLOCKTHREAD)
	{

		threads[callingthread].status=BLOCKED;
		_enqueue(&waitinginevent[blockevent],callingthread);

		changethread=1;
	}

	if(event==ENDTHREAD)
	{
		threads[callingthread].status=END;
		changethread=1;
		numProcs--;
	}
	
	if(event==UNBLOCKTHREAD)
	{
			threads[callingthread].status=READY;
			_enqueue(&ready[localPriority],callingthread);
	}

	
	if(changethread)
	{
		old=currthread;
		
		//se busca un proceso con la prioridad mas baja posible
		int i;
		for(i=0;i<MAXPRIORITY;i++){
			if(!_emptyq(&ready[i]))
			   break;
		}
			   
		localPriority=i;
		
		next=_dequeue(&ready[localPriority]);
		
		threads[next].status=RUNNING;
		_swapthreads(old,next);
	}

}
