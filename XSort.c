#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
// includes for C Streams
#include <stdio.h>
// include for c strings
#include <string.h>
// Record to save file
#include <stdlib.h>
#include <ctype.h>


struct _Record {
    		int FieldNumber;
    		char FieldName[90];
		};//Data Structure

#define REC_SIZE sizeof(struct _Field)
typedef struct _Record Record;

void readRecord(char *,char *,int);// function is read record line by line
void writeFile(char *,Record);// function is write record line by line
int sizeofFile(char *);// The number of record for one file
void split(Record *, char *,int);// Split Reading Data
int heapSort(int,int,char *);//heapsort
void heapFunction(Record *,int);// this for make heap Structure
void heapify(Record *,int);// heapify Function
void adjust(Record *,int);// necessary for heap sort
int mergeSort(int,int,int,int);// mergeSort function


int main(int argc, char **argv)
{
    	char *fileName;
	char *sortedfileName;
	int bufferNumber;
	int fieldNumber;
	int start=0;
	int end;

	int i;
	for(i=0;i<argc;i++)// This for find input file
	{
		if(i==1)// take unsorted file name 
		{
			fileName=argv[i];
		}
		else if(i==2)// take sorted file name 
		{
			sortedfileName=argv[i];
		}
		else if(strcmp(argv[i],"B=")==0)// take number of buffer 
		{
			bufferNumber= atoi(argv[i+1]);
		}
		else if(strcmp(argv[i],"P=")==0)// take page size
		{
			end= atoi(argv[i+1]);
		}
	}

        int phaseNumberOfFile=heapSort(end-start,start,fileName);// phase 0 heap sort (phaseNumberOfFile for keeping creating file)

     	int phase=0;
	printf("Phase 0: %d sorted segment created initially\n",phaseNumberOfFile);
	do
	{
		
		int unnecessaryFileNumber=phaseNumberOfFile;// necessary for delete last unnecessary file

   	    	phaseNumberOfFile = mergeSort(phase,phaseNumberOfFile,bufferNumber,end-start);//merge sort
		
		
		int k;
		for(k=0;k<unnecessaryFileNumber;k++)// for delete last unnecessary files
		{
			char phaseName[10];
			snprintf(phaseName,10,"%d",phase);
			char deleteFile[100]="Data";
			strcat(deleteFile, phaseName);
			char counterName[10];
			snprintf(counterName,10,"%d",k);
			strcat(deleteFile,counterName);
			strcat(deleteFile,".txt");
			remove(deleteFile);

		}
		
		phase++;
		printf("Phase %d: %d merged segments created \n",phase,phaseNumberOfFile);

	}while(phaseNumberOfFile>1);

	char phaseName[10];
	snprintf(phaseName,10,"%d",phase);
	char resultFile[100]="Data";
	strcat(resultFile, phaseName);
	strcat(resultFile, "0.txt");
	rename(resultFile,sortedfileName);// rename with user sorted file name
	
	printf("Sorted Data ====> %s \n",sortedfileName);

	return 0;
}


int mergeSort(int phase,int numberOfFile,int bufferNumber,int size)
{

 
    char fileName[40];
    char phaseName[10];
    char counterName[10];
    int fileNumber=0;
    int start=0;
    snprintf(phaseName,10,"%d",phase);
    int counter2=0;
    int last=numberOfFile/(bufferNumber-1);
    int counter=0;
    int t;
    
    for(t=0;t<numberOfFile;t=t+bufferNumber-1)
    {
	int tempBuffer=0;
        int pageCounter=0;// reading file
        int numberOfRecords=0;
        Record buffer [bufferNumber-1];// this for keep initally number of buffer
        int sizeArray [bufferNumber-1];// this for keep initally number of record in files
        char fileNameArray[bufferNumber-1][50];
        int i;
	
        for(i=t;i<t+bufferNumber-1;i++)
        {
	   if(counter<numberOfFile)
	   {
	   	 strcpy(fileName,"Data");// there are calculate reading file name
	   	 snprintf(phaseName,10,"%d",phase);//
          	 strcat(fileName,phaseName);//
	    	snprintf(counterName,10,"%d",counter);//
            	strcat(fileName,counterName);//
            	strcat(fileName,".txt");//
            	char * recordData= (char *)malloc(1000*size);// First Reading Data
            	sizeArray[i-t]=sizeofFile(fileName);
            	strcpy(fileNameArray[i-t],fileName);
            	numberOfRecords=numberOfRecords+sizeArray[i-t];
		tempBuffer++;
	   }	
           
            counter++;
        }

        int phase2=phase+1;// calculate writing file 
        char fileName2[40];//
        char phaseName2[10];//
        char counterName2[10];//
        strcpy(fileName2,"Data");//
	snprintf(phaseName2,10,"%d",phase2);//
        strcat(fileName2,phaseName2);//
	snprintf(counterName2,10,"%d",counter2);//
        strcat(fileName2,counterName2);//
        strcat(fileName2,".txt");//

        int index[bufferNumber-1];
        int startPoint[bufferNumber-1];//reading record index in file
        int k;
        for(k=0;k<tempBuffer;k++)
        {
            startPoint[k]=0;
        }
	for(k=0;k<tempBuffer;k++)// first number of buffers
        {
           
            	char * recordData= (char *)malloc(1024*sizeof(char));// First Reading Data
            	readRecord(recordData,fileNameArray[k],startPoint[k]);
            	split(buffer,recordData,k);
        }	
	int lasttemp=0;// control bigger smaller last writting record
        int p=0;// control number of records
        while(p<numberOfRecords)
        {
           int h;
           int tempNumber=999999999;// this keep smal number of buffers
           int tempIndex=0;// keep index how buffer
           for(h=0;h<bufferNumber-1;h++)
           {

               if(startPoint[h]<sizeArray[h] && tempNumber>buffer[h].FieldNumber )// compaire all of the buffer find small
               {

                   tempNumber=buffer[h].FieldNumber;
                   tempIndex=h;
               }

           }
            if(buffer[tempIndex].FieldNumber!=0 )// if buffer != null
            {
                
		  startPoint[tempIndex]=startPoint[tempIndex]+1;// for read next record
		  if(startPoint[tempIndex]*2<sizeArray[tempIndex])
		  {
			   
			writeFile(fileName2,buffer[tempIndex]);// write small data 
                    	char * recordData2= (char *)malloc(1024*sizeof(char));// read next data
            		readRecord(recordData2,fileNameArray[tempIndex],startPoint[tempIndex]*2);// * 2 for skip null line
            		split(buffer,recordData2,tempIndex);
                  }
		  else if(startPoint[tempIndex]*2-2==sizeArray[tempIndex])// write last elemnets in reading files
		  {
			writeFile(fileName2,buffer[tempIndex]);	
		  }
		  p++;// numeber of records		  
		
            }

        }
        counter2++;// next file for write
    }
    return counter2;
}


int heapSort(int size,int start,char * fileName)
{

    Record heap1 [size];// sorted datas  max page size
    Record heap2 [size];// unsorted datas
    Record temp[2];// temp for compaire
    int heap1counter=0;// This for keep index heap1
    int heap2counter=0;// This for keep index heap2
    int control=0;
    int counterPhase0=0;
    
    int endofFile = sizeofFile(fileName);

     int last=0;
    while(start<endofFile)
    {
	char  filePhase0[20] = "Data0";
	char phaseCounterName[10];
	snprintf(phaseCounterName,10,"%d",counterPhase0);
	strcat(filePhase0,phaseCounterName);
	strcat(filePhase0,".txt");
        int i;
        for(i=heap1counter;i<size;i++)
        {
            char * recordData= (char *)malloc(1024*sizeof(char));//  Reading Data size of page
            readRecord(recordData,fileName,start);
            split(heap1,recordData,i);
            start++;
        }

	heapFunction(heap1,size);// sort heap1
	writeFile(filePhase0,heap1[0]);// write smallest element in file

	control++;	
	last=heap1[0].FieldNumber;// keep writing data for compaire
	for(i=1;i<size;i++)
	{
			if(start<endofFile)
			{
				char * recordData= (char *)malloc(1024*sizeof(char));// Reading Data
            			readRecord(recordData,fileName,start);
            			split(temp,recordData,0);
            			start++;
			}
			
		
		if(temp[0].FieldNumber<heap1[i].FieldNumber && temp[0].FieldNumber >last && start<endofFile)// heap1 number bigger reading number
		{
			writeFile(filePhase0,temp[0]);
			last=temp[0].FieldNumber;
			i--;
			control++;
		}
		else // heap1 smaller than reading data heap2 = temp
		{
			heap2[heap2counter]=temp[0];
			heap2counter++;
			writeFile(filePhase0,heap1[i]);
			last=heap1[i].FieldNumber;
			control++;
		}

	}	
	heap1counter=0;
	for(i=0;i<heap2counter;i++)// transfer heap2 to heap1
	{
		heap1[i]=heap2[i];
		heap1counter++;
	}
	heap2counter=0;
	counterPhase0++;
	//printf("NextPage %d \n",counterPhase0);
    }   
	char  filePhase0[20] = "Data0";
	char phaseCounterName[10];
	snprintf(phaseCounterName,10,"%d",counterPhase0);
	strcat(filePhase0,phaseCounterName);
	strcat(filePhase0,".txt");

	int i;
	Record big;
	big.FieldNumber=99999999;
	for(i=heap1counter;i<size;i++)
	{
		heap1[i]=big;
	}

	  heapFunction(heap1,size);// last heap1 sort 
	int tempControl=0;
	for(i=0;i<heap1counter;i++)
	{
	    if(tempControl!=heap1[i].FieldNumber)
	    {		
		writeFile(filePhase0,heap1[i]);
		tempControl=heap1[i].FieldNumber;
		control++;

            }	
	}
		
     counterPhase0++; // last number of file
    return counterPhase0;
}

void heapFunction(Record * heap,int size)
{
            heapify(heap,size);
            Record temp;
            int i;
            for(i=size-1;i>0;i--)
            {
                temp = heap[0];
                heap[0] = heap[i];
                heap[i] = temp;
                adjust(heap,i);
            }
}

void heapify(Record *a,int size)
{
    Record item;
    int k,i,j;
    for(k=0;k<size;k++)
    {
        item = a[k];
        i = k;
        j = (i-1)/2;

        while((i>0)&&(item.FieldNumber>a[j].FieldNumber))
        {
            a[i] = a[j];
            i = j;
            j = (i-1)/2;
        }
        a[i] = item;
    }
}


void adjust(Record *a,int size)
{
    int i,j;
    Record item;

    j = 0;
    item = a[j];
    i = 2*j+1;

    while(i<=size-1)
    {
    if(i+1 <= size-1)
    if(a[i].FieldNumber <a[i+1].FieldNumber)
        i++;
        if(item.FieldNumber<a[i].FieldNumber)
        {
            a[j] = a[i];
            j = i;
            i = 2*j+1;
        }
        else
        break;
    }
    a[j] = item;
}

void writeFile(char *filename,Record line)
{
      FILE *fp;
      /* open the file */
      fp = fopen(filename, "a");
      if (fp == NULL)
      {
         printf("I couldn't open results.dat for appending.\n");
         exit(0);
      }
      /* write to the file */
      fprintf(fp,"%d, %s\n",line.FieldNumber,line.FieldName);
      /* close the file */
      fclose(fp);

}

// for this function split reading Data
void split(Record *page, char *recordData,int index)
{

     char *token;
      token = strtok(recordData,",");
   
     page[index].FieldNumber=atoi(token);

      token = strtok(NULL, ",");
 
      strcpy(page[index].FieldName,token);
    

}

void readRecord(char *recordData,char* file_name ,int start)
{
            FILE *ptr_file;
    		char buf[1000];

    		ptr_file =fopen(file_name,"r");
    		if (!ptr_file)
        		return exit(0);

            int counter=0;
    		while (fgets(buf,1000, ptr_file)!=NULL && start >= counter && !feof(ptr_file))
            {
                if(start==counter)
                {
                   strcat(recordData,buf);
                }
        		counter++;
            }
            fclose(ptr_file);

}


int sizeofFile(char *filename)
{
    FILE *fp = fopen(filename, "r");
    		char buf[1000];
    int counter=0;
  	while (fgets(buf,1000, fp)!=NULL)
    {

        		counter++;
    }
    fclose(fp);
	return counter;
}





