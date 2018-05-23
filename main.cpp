
#include <iostream>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>       /* ceil */

#define NUMTS 5000 //Number of tasksets

struct job{
	float wcet;
	float deadline;
	float period;
	float util;
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

struct perden
{
    int id;
    float data;
};
typedef struct perden perden;

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

//removes duplicate elements from the array and returns the final size of new array
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

float deadline_10[10];
float deadline_25[25];

int edf_schedulability(jobset* tasksets)
{
	int l0=0,l_prev=0,l_current=0;
	int temp;
	int i,j;
	int count[tasksets->num_jobs];
	 int count_sum=0;
	int incr=1;

	for(int z=0; z< tasksets->num_jobs; z++)
		count[z]=0;


	for(i=0; i<tasksets->num_jobs; i++)
		l0 += tasksets->job_array[i].wcet;

	temp=l_prev=l0;

	while(1)
	{
		if(l_current==temp)
			break;

		else
		{   
			l_current=0;

			for(i=0; i<tasksets->num_jobs; i++)
				l_current += ( ceilf(l_prev/ tasksets->job_array[i].period) ) * tasksets->job_array[i].wcet;

			temp   = l_prev;
			l_prev = l_current;
		}
	}

	i=0;
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

	for(i=0; i< tasksets->num_jobs;i++)
		count_sum += count[i];

	int test[tasksets->num_jobs+count_sum];
	int h[tasksets->num_jobs+count_sum];
	float util[tasksets->num_jobs+count_sum];


	for(i=0;i<tasksets->num_jobs;i++)
	    (tasksets->num_jobs==10)? deadline_10[i] = tasksets->job_array[i].deadline: deadline_25[i] = tasksets->job_array[i].deadline;

	if(tasksets->num_jobs==10)
	{
	float tmp=0.0;
		for(int aa=0; aa<10; aa++){
			for(int bb=0; bb<10; bb++){
				if(deadline_10[aa] > deadline_10[bb]){
				tmp = deadline_10[bb];
				deadline_10[bb] = deadline_10[aa];
				deadline_10[aa] = tmp;
				}
			}
		}
	} 
	else
	{
	float tmp=0.0;
		for(int aa=0; aa<25; aa++){
			for(int bb=0; bb<25; bb++){
				if(deadline_25[aa] > deadline_25[bb]){
				tmp = deadline_25[bb];
				deadline_25[bb] = deadline_25[aa];
				deadline_25[aa] = tmp;
				}
			}
		}

	}

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
			incr++;
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
		while(tasksets->job_array[j].deadline <=test[i] && j!=tasksets->num_jobs && j<tasksets->num_jobs)
			h[i]+=tasksets->job_array[j++].wcet;
		if(j==tasksets->num_jobs){
			j = 0;
			for(int k=0;k<tasksets->num_jobs;k++)
			{
				if(tasksets->num_jobs==10) deadline_10[k] += tasksets->job_array[k].period;
				if(tasksets->num_jobs==25) deadline_25[k] += tasksets->job_array[k].period;
			}
		}		
	}
	for(int w;w<tasksets->num_jobs+newSize;w++){
		util[w] = (float)h[w] / test[w];
		if(util[w]>=1) return 0;
	}

	return 1;	
}

int edf(jobset* tasksets)
{
	int ret= edf_schedulability(tasksets);
	return ret;
}

float minimum(float a, float b)
{
if(a<b)
    return a;
else
    return b;
}


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

utilization(tasksets);
if( tasksets->util <= bound)
{
return 1;
}

else{
    int i;
    int count=0;
    int flag_util=0;

    perden period_ptr[tasksets->num_jobs];
    perden density_ptr[tasksets->num_jobs];

    //period order
    for(i=0; i<tasksets->num_jobs;i++)
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
utilization(tasksets);

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

void UUniFast(jobset* taskset, float U, int n){

//	srand(time(NULL));	
	float sumU = U;
	float nextSumU=0.0;
	for(int i=1;i<n;i++){
		nextSumU = sumU*pow( ((float)rand()/(float)RAND_MAX ),1/(n-i));
		taskset->job_array[i-1].util = sumU - nextSumU; 
		sumU = nextSumU;
	}
	taskset->job_array[n-1].util = sumU;
}

int main(int argc, char* argv[]){
	float U = atof(argv[1]);
	jobset P1[NUMTS],P2[NUMTS],P3[NUMTS],P4[NUMTS];
	int edf_12=0,edf_34=0,rms_12=0,rms_34=0,dms_12=0,dms_34=0;
	float result_12=0,result_34=0;

	for(int i=0;i<NUMTS;i++){
		P1[i].job_array = (job*) malloc(sizeof(job)*10);
		P2[i].job_array = (job*) malloc(sizeof(job)*25);
		P3[i].job_array = (job*) malloc(sizeof(job)*10);
           	P4[i].job_array = (job*) malloc(sizeof(job)*25);
	}

	for(int i=0;i<NUMTS;i++){	
		std::random_device generator;
                std::uniform_real_distribution<float> distribution1(1000,10000);
                std::uniform_real_distribution<float> distribution2(10000,100000);
                std::uniform_real_distribution<float> distribution3(100000,1000000);
                std::uniform_real_distribution<float> distribution4(1000000,10000000);
		for(int j=0;j<10;j++){
			if(j>=0&&j<=2){
				P1[i].job_array[j].period = distribution1(generator);
				P3[i].job_array[j].period = distribution1(generator);
				
			}
			else if(j>=3&&j<=5){
				P1[i].job_array[j].period = distribution2(generator);
                                P3[i].job_array[j].period = distribution2(generator);
			}
			else if(j>=6&&j<=8){
				P1[i].job_array[j].period = distribution3(generator);
                                P3[i].job_array[j].period = distribution3(generator);
			}
			else{
				P1[i].job_array[j].period = distribution4(generator);
				P3[i].job_array[j].period = distribution4(generator);
			}
			
		}
		for(int j=0;j<25;j++){
			if(j>=0&&j<=7){
				P1[i].job_array[j].period = distribution1(generator);
				P3[i].job_array[j].period = distribution1(generator);
				
			}
			else if(j>=8&&j<=15){
				P2[i].job_array[j].period = distribution2(generator);
                                P4[i].job_array[j].period = distribution2(generator);
			}
			else if(j>=16&&j<=23){
				P2[i].job_array[j].period = distribution3(generator);
                                P4[i].job_array[j].period = distribution3(generator);
			}
			else{
				P2[i].job_array[j].period = distribution4(generator);
				P4[i].job_array[j].period = distribution4(generator);
			}
		}							
	}
	for(int i=0;i<NUMTS;i++){

		UUniFast(&P1[i],U,10);
		P1[i].num_jobs = 10;
		UUniFast(&P2[i],U,25);
		P2[i].num_jobs = 25;
		UUniFast(&P3[i],U,10);
		P3[i].num_jobs = 10;
		UUniFast(&P4[i],U,25);
		P4[i].num_jobs = 25;
	
	}
	for(int i=0;i<NUMTS;i++){

		for(int j=0;j<10;j++){
			P1[i].job_array[j].wcet=P1[i].job_array[j].util*P1[i].job_array[j].period;
			P3[i].job_array[j].wcet=P1[i].job_array[j].util*P3[i].job_array[j].period;
			float Ci = P1[i].job_array[j].wcet, period = P1[i].job_array[j].period;
                        std::random_device generator;
                        std::uniform_real_distribution<float> distribution1(Ci,period);
			P1[i].job_array[j].deadline = distribution1(generator);
			Ci = P3[i].job_array[j].wcet, period = P3[i].job_array[j].period;
			std::uniform_real_distribution<float> distribution2(Ci, ( (period-Ci)/(2*period) ) );
			P3[i].job_array[j].deadline = distribution1(generator);
		}
		for(int j=0;j<25;j++){
                	P2[i].job_array[j].wcet=P1[i].job_array[j].util*P2[i].job_array[j].period;
                        P4[i].job_array[j].wcet=P1[i].job_array[j].util*P4[i].job_array[j].period;
			float Ci = P2[i].job_array[j].wcet, period = P2[i].job_array[j].period;
                        std::random_device generator;
                        std::uniform_real_distribution<float> distribution1(Ci,period);
                        P2[i].job_array[j].deadline = distribution1(generator);
                        Ci = P4[i].job_array[j].wcet, period = P4[i].job_array[j].period;
                        std::uniform_real_distribution<float> distribution2(Ci,( (period-Ci)/(2*period) ) );
                        P4[i].job_array[j].deadline = distribution1(generator);
                }

		if(*argv[2]=='e'){
		if( edf(&P1[i])==0) edf_12++;
		if( edf(&P2[i])==0) edf_12++;
		if( edf(&P3[i])==0) edf_34++;
		if( edf(&P4[i])==0) edf_34++;
		}

		if(*argv[2]=='r'){
		if( rms(&P1[i])==0) rms_12++;
		if( rms(&P2[i])==0) rms_12++;
		if( rms(&P3[i])==0) rms_34++;
		if( rms(&P4[i])==0) rms_34++;
		}

		if(*argv[2]=='d'){
		if( dms(&P1[i])==0) dms_12++;
		if( dms(&P2[i])==0) dms_12++;
		if( dms(&P3[i])==0) dms_34++;
		if( dms(&P4[i])==0) dms_34++;
		}

		if(i== (NUMTS-1)){
			if(*argv[2]=='e'){
			result_12 = (float)edf_12/ NUMTS;
			result_34 = (float)edf_34/ NUMTS;
			}

			if(*argv[2]=='r'){
			result_12 = (float)rms_12/ NUMTS;
			result_34 = (float)rms_34/ NUMTS;
			}

			if(*argv[2]=='d'){
			result_12 = (float)dms_12/ NUMTS;
			result_34 = (float)dms_34/ NUMTS;
			}
		}
	}

printf("\nFailed task percentage:\nDistibution1:%f \tDistribution2:%f\n",result_12*100,result_34*100);
return 0;
}

