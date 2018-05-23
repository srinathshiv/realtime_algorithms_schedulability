#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>       /* ceil */

struct job{
	float wcet;
	float deadline;
	float period;
};
typedef struct job job;

struct jobset{
	job* job_array;
	int num_jobs;
	float util;
	float util_precise;
	float util_deadline;
	int* rms_arr;
	int* dms_arr;
};
typedef struct jobset jobset;

void bubblesort(int arr[],int size)
{
int tmp_ptr;
for(int i=0; i< size; i++){
	for(int j=0; j<size; j++){
		if(arr[i] > arr[j]){
			tmp_ptr = arr[j];
			arr[j] = arr[i];
			arr[i] = tmp_ptr;
		}	
	}
}
}

void bubblesort_float(float arr[], int size)
{

for(int i=0;i<size;i++){
	for(int j=0;j<size;j++){
		if(arr[i] > arr[j]){
			float tmp = arr[j];
			arr[j] = arr[i];
			arr[i] = tmp;
		}
	}
}
}

void utilization(jobset *tasksets)
{ 
	float util = 0.0;

	for(int j=0;j< tasksets->num_jobs ;j++)
		util += tasksets -> job_array[j].wcet / tasksets -> job_array[j].period;
	tasksets->util=util;
}

int precise_test(jobset* tasksets,char op) 
{
	for(int i=0;i<tasksets->num_jobs;i++){
		float util = 0.0;	
		for(int j=0;j<tasksets->num_jobs;j++){
			if(op=='r'&& i != j )
				util += tasksets->job_array[j].wcet/tasksets->job_array[j].period;
			else if(op=='d' && i!=j)
				util += tasksets->job_array[j].wcet/tasksets->job_array[j].deadline;
		}
		int tmp_pr = tasksets->num_jobs - 1;
		float bound_precise = tmp_pr * ( (pow(2, 1./tmp_pr) ) -1);
		if(util<=bound_precise){
			float Ri= 0.0;
			for(int j=0;j< tasksets->num_jobs ;j++) Ri  += tasksets -> job_array[j].wcet;
			float Ti = (op=='r')?tasksets->job_array[i].period:tasksets->job_array[i].deadline;
			float Ci = tasksets->job_array[i].wcet;
			while(1){
				float dummy = Ri;
				Ri = 0;
				for(int j=0;j<tasksets->num_jobs;j++){
					if(i==j) Ri += Ci;
					else{
						float ti = (op=='r')?tasksets->job_array[i].period:tasksets->job_array[i].deadline;
                                                float ci = tasksets->job_array[i].wcet;	
						Ri += ceilf(dummy/ti)*ci;
					} 
				}
				if(Ri>Ti) return 0;
				if(Ri==dummy && Ri<=Ti) return 1;	
			}
		}	
	}
	return 0;	
}

//removes duplicate elements from the array and retuns the final size of new array
int removeDuplicates(int arr[],int size)
{
for(int i=0; i<size; i++){
    for(int j=0; j<size; j++){
	if( i!=j && arr[i]==arr[j]){
		for(int k=i; k< size; k++)
			arr[k]=arr[k+1];

		size=size-1;
	}	
    }
}
return size;
}


int edf_schedulability(jobset* tasksets)
{
	int l0=0,l_prev=0,l_current=0;
	int temp;
	int j;
	int count[tasksets->num_jobs];
	int count_sum=0;
	int incr=1;

	for(int z=0; z< tasksets->num_jobs; z++)
		count[z]=0;

	for(int i=0; i<tasksets->num_jobs; i++)
		l0 += tasksets->job_array[i].wcet;

	temp=l_prev=l0;

	while(1)
	{
		if(l_current==temp)
			break;

		else
		{   
			l_current=0;

			for(int i=0; i<tasksets->num_jobs; i++)
				l_current += ( ceilf(l_prev/ tasksets->job_array[i].period) ) * tasksets->job_array[i].wcet;

			temp   = l_prev;
			l_prev = l_current;
		}
	}

	int i=0;
	//determine array size
	while( i< tasksets->num_jobs )
	{
		if( (tasksets->job_array[i].deadline + (incr * tasksets->job_array[i].period)) <= l_current){	
			count[i]++;
			incr++;
		}

		else{
			i++;
			incr=1;
		}
	}

	for(int i=0; i< tasksets->num_jobs;i++)
		count_sum += count[i];

	int test[tasksets->num_jobs+count_sum];
	int h[tasksets->num_jobs+count_sum];
	int util[tasksets->num_jobs+count_sum];
	float deadlines[tasksets->num_jobs];
	for(int i=0;i < tasksets->num_jobs;i++)
		deadlines[i] = tasksets->job_array[i].deadline;

	bubblesort_float(deadlines,tasksets->num_jobs);

	//find test,h,util
	for(i=0,j=0; i< tasksets->num_jobs; )
	{
		if(i==0){
			for(j=0 ;j< tasksets->num_jobs; j++)
				test[j] = tasksets->job_array[j].deadline + (incr * tasksets->job_array[j].period);
		}

		if( (tasksets->job_array[i].deadline + (incr * tasksets->job_array[i].period)) <= l_current){
			test[j] = tasksets->job_array[i].deadline + (incr * tasksets->job_array[i].period);
			j++;
		}

		else{
			i++;
			incr=1;
		}
	}


	bubblesort(test,count_sum);

	int newSize = removeDuplicates(test, ( sizeof(test)/sizeof(int) ));

	j=0;
	for(i=0;i<tasksets->num_jobs+newSize;i++)
	{	
		while(tasksets->job_array[j].deadline <=test[i] && j!=tasksets->num_jobs)
			h[i]+=tasksets->job_array[j++].wcet;
		if(j==tasksets->num_jobs){
			j = 0;
			for(int k=0;k<tasksets->num_jobs;k++)
				deadlines[k] += tasksets->job_array[k].period;
		}		
	}
	for(int w;w<tasksets->num_jobs+newSize;w++){
		util[w] = h[w]/test[w];
		if(util[w]>1) return 0;
	}
	return 1;	
}

int edf(jobset *tasksets)
{
	if(tasksets -> util <= 1) 
		return 1;
	else 
		return edf_schedulability(tasksets);
}

float minimum(float a, float b)
{
if(a<b)
    return a;
else
    return b;
}

struct perden
{
    int id;
    float data;
};
typedef struct perden perden;

void bubblesort_struct(perden *perd,int size)
{

perden tmp;

for (int i=0; i<size; i++){
    for( int j=0; j<size; j++){
        if( perd[i].data > perd[j].data){
	    tmp     = perd[j];
	    perd[j] = perd[i];
	    perd[i] = tmp;
	}
    }
}

}

float calcBound(int n)
{
float bound =n*( (pow(2, 1./n) ) -1); 
return bound; 
}

int eff_util_rms(jobset *tasksets)
{ 
int count=0;

    for(int i=0; i<tasksets->num_jobs; i++){
	float util1=0.0;
	count=0;
        for(int j=0; j<tasksets->num_jobs; j++){
	    if( i!=j && tasksets->job_array[j].period < tasksets->job_array[i].deadline && tasksets->job_array[j].period < tasksets->job_array[i].period){
	        util1 += (tasksets->job_array[j].wcet/ tasksets->job_array[j].period) ;
		count++;
	     }
	}	
        util1 += (tasksets->job_array[i].wcet / tasksets->job_array[i].period) ;

	if(util1 > calcBound(count+1))	
	    return 0;
    }	
return 1;
}

int eff_util_dms(jobset *tasksets)
{
int count=0;

    for(int i=0; i<tasksets->num_jobs; i++){
	float util1=0.0;
	count=0;
        for(int j=0; j<tasksets->num_jobs; j++){
	    if( i!=j && tasksets->job_array[j].period < tasksets->job_array[i].deadline && tasksets->job_array[j].deadline < tasksets->job_array[i].deadline){
	        util1 += (tasksets->job_array[j].wcet/ tasksets->job_array[j].period) ;
		count++;
	     }
	}	
        util1 += (tasksets->job_array[i].wcet / tasksets->job_array[i].period) ;

	if(util1 > calcBound(count+1))	
	    return 0;
    }	
return 1;

}

int rms(jobset *tasksets)
{
float bound;
bound = tasksets->num_jobs * ( ( pow(2, 1./tasksets->num_jobs) ) -1 ) ;

if( tasksets->util <= bound)
{
return 1;
}

else
{

    int count=0;
    int flag_util=0;

    perden period_ptr[tasksets->num_jobs];
    perden density_ptr[tasksets->num_jobs];

    //period order
    for(int i=0; i<tasksets->num_jobs;i++)
    {
        period_ptr[i].id    = i;
	period_ptr[i].data  = tasksets->job_array[i].period;

	density_ptr[i].id    = i;
        density_ptr[i].data = tasksets->job_array[i].wcet / minimum( tasksets->job_array[i].period, tasksets->job_array[i].deadline) ;
    }

    bubblesort_struct(period_ptr, tasksets->num_jobs);
    bubblesort_struct(density_ptr, tasksets->num_jobs);   

    for(int i=0; i<tasksets->num_jobs; i++){
        if( period_ptr[i].id == density_ptr[i].id){
        count++;
            
	    if(count == tasksets->num_jobs){
	    flag_util=0;
	    break;
	    }
	}
        flag_util=1;
    }

    if(flag_util==1){
	if( eff_util_rms(tasksets)==1) 
    	    return 1;
	else 
	    return precise_test( tasksets,'r');		
    }
    else	
	return 1;

} 

}

int dms(jobset *tasksets)
{

float bound;
bound = tasksets->num_jobs * ( ( pow(2, 1./tasksets->num_jobs) ) -1 ) ;

if( tasksets->util <= bound)
{
return 1;
} 

else
{
 int count=0;
    int flag_util=0;

    perden deadline_ptr[tasksets->num_jobs];
    perden density_ptr[tasksets->num_jobs];

    //period order
    for(int i=0; i<tasksets->num_jobs;i++)
    {
        deadline_ptr[i].id    = i;
	deadline_ptr[i].data  = tasksets->job_array[i].deadline;

	density_ptr[i].id    = i;
        density_ptr[i].data = tasksets->job_array[i].wcet / minimum( tasksets->job_array[i].period, tasksets->job_array[i].deadline) ;
    }

    bubblesort_struct(deadline_ptr, tasksets->num_jobs);
    bubblesort_struct(density_ptr, tasksets->num_jobs);   

    for(int i=0; i<tasksets->num_jobs; i++){
        if( deadline_ptr[i].id == density_ptr[i].id){
        count++;
            
	    if(count == tasksets->num_jobs){
	    flag_util=0;
	    break;
	    }
	}
        flag_util=1;
    }

    if(flag_util==1){
	if( eff_util_dms(tasksets)==1) 
    	    return 1;
	else 
	    return precise_test( tasksets,'d');		
    }
    else	
	return 1;
}

}

int main(int argc, char* argv[]){
	FILE *pFile = fopen(argv[1],"rb");
	if (pFile==NULL) perror ("Error opening file");
	char *line = NULL;
	size_t len = 0;
	getline(&line, &len, pFile);
	int num_tasksets;
	sscanf(line,"%d",&num_tasksets);
	jobset tasksets[num_tasksets];

	for(int i=0;i< num_tasksets;i++){
		int num_tasks;
		getline(&line, &len, pFile);
		sscanf(line,"%d",&num_tasks);
 		tasksets[i].num_jobs = num_tasks;
		tasksets[i].job_array = (job*) malloc(num_tasks*sizeof(job));
		for(int j=0;j<num_tasks;j++){
			getline(&line, &len, pFile);
			sscanf(line,"%f %f %f",&tasksets[i].job_array[j].wcet,&tasksets[i].job_array[j].deadline,&tasksets[i].job_array[j].period);
		}	
	}

	for(int i=0;i<num_tasksets;i++){
		int ret=0;
		utilization(&tasksets[i]);
		ret= edf(&tasksets[i]);
		(ret==1)?(printf("EarliestDeadline is feasible\n")):(printf("EarliestDeadline is NOT feasible\n")) ;
		
		ret= rms(&tasksets[i]);
		(ret==1)?(printf("RateMonotonic is feasible\n")):(printf("RateMonotonic is NOT feasible\n")) ;

		ret= dms(&tasksets[i]);
		(ret==1)?(printf("DeadlineMono is feasible\n")):(printf("DeadlineMono is NOT feasible\n")) ;

		printf("----------\n");
		}

return 0;
}

