/* Project6: Virtual Memory Management
   0416047 Andy Wang */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

int main()
{
	/* VARIABLE DECLARATION */
	// file probe
	FILE *BACKING_STORE;
	FILE *ADDRESS;
	FILE *RESULT1;
	FILE *RESULT2;
	
	/*
	   pageContent is used for handling the content 
	   transmission between BACKING_STORE.bin and physicalMemory
	*/
	char pageContent[256] = {0};
	char physicalMemory[256][256] = {0};
	
	/*
	   For each entry m in pageTable, pageTable[m][0] is for 
	   invalid-valid and pageTable[m][1] is for frame number.
	*/
	int pageTable[256][2] = {0};

	/*
	   For each entry m in TLB, TLB[m][0] is for page number and
	   and TLB[m][1] is for frame number.
	*/
	int TLB[16][2] = {0};

	vector<int> freeFrame;
	queue<int> TLBReplacement;

	char getLine[10] = {0};
	unsigned accessAddress = 0;
	unsigned pageNumber = 0;
	unsigned frameNumber = 0;
	unsigned offset = 0;

	bool TLBHit = false;

	// event counter
	int accessTime = 0;
	int pageFaultTime = 0;
	int TLBHitTime = 0;

	/* OPEN SOME REQUIRED FILES */
	BACKING_STORE = fopen("BACKING_STORE.bin", "rb");
	ADDRESS = fopen("addresses.txt", "rb");
	RESULT1 = fopen("0416047_address.txt", "w");
	RESULT2 = fopen("0416047_value.txt", "w");

	if (BACKING_STORE == NULL)printf("Openning BACKING_STORE.bin failed!\n");
	if (ADDRESS == NULL)printf("Openning address.txt failed!\n");
	if (RESULT1 == NULL)printf("Creating 0416047_address.txt failed!\n");
	if (RESULT2 == NULL)printf("Creating 0416047_value.txt failed!\n");

	/* START THE PROCEDURE */
	// initialize the free frame pool
	for(int i=0; i<16; i++)TLBReplacement.push(i);
	for(int i=255; i>=0; i--)freeFrame.push_back(i);

	while( fgets(getLine, 10, ADDRESS) != NULL )
	{
		accessTime++;

		// read a line from address.txt and turn it into 'pageNumber' and 'offset'
		for(int i=0; i<10; i++)
		{
			if(getLine[i] == 13)
			{
				getLine[i]=0;
				getLine[i+1]=0;
			}
		}
		accessAddress = atoi(getLine);
		pageNumber = accessAddress>>8;
		offset = accessAddress & 0xFF;

		// first, find pageNumber in TLB
		for(int i=0; i<16; i++)
		{
			// TLB hit
			if( TLB[i][0] ==  pageNumber )
			{
				TLBHitTime++;
				TLBHit = true;

				frameNumber = TLB[i][1];
				fprintf(RESULT1, "%d\n", frameNumber*256+offset);
				fprintf(RESULT2, "%d\n", physicalMemory[frameNumber][offset]);
			}

			// TLB miss
			else { /*empty method*/ }
		}

		// if we get a TLB miss, next, we have to find frame number in page table
		if( TLBHit == false )
		{
			// when frame number is available in page table
			if( pageTable[pageNumber][0] == 1 )
			{
				frameNumber = pageTable[pageNumber][1];
				fprintf(RESULT1, "%d\n", frameNumber*256+offset);
				fprintf(RESULT2, "%d\n", physicalMemory[frameNumber][offset]);

				// TLB replacement with FIFO method
				int updatedTLBEntry = TLBReplacement.front();
				TLB[updatedTLBEntry][0] = pageNumber;
				TLB[updatedTLBEntry][1] = frameNumber;
				TLBReplacement.pop();
				TLBReplacement.push(updatedTLBEntry);
			}

			// when frame number is NOT available which means page fault occurred
			else
			{
				pageFaultTime++;

				// move the file probe to the position '256*pageNumber'
				fseek(BACKING_STORE, 256*pageNumber, SEEK_SET);
				// read 256 bytes starting from 'BACKING_STORE'
				fread(pageContent, 1, 256, BACKING_STORE);

				// find a free frame and update the new info. into this 'page fault' entry
				pageTable[pageNumber][0] = 1;
				pageTable[pageNumber][1] = freeFrame.back();
				frameNumber = freeFrame.back();
				freeFrame.pop_back();

				// put the pageContent into this free frame
				for(int i=0; i<256; i++)
					physicalMemory[frameNumber][i] = pageContent[i];

				fprintf(RESULT1, "%d\n", frameNumber*256+offset);
				fprintf(RESULT2, "%d\n", physicalMemory[frameNumber][offset]);

				// TLB replacement with FIFO method
				int updatedTLBEntry = TLBReplacement.front();
				TLB[updatedTLBEntry][0] = pageNumber;
				TLB[updatedTLBEntry][1] = frameNumber;
				TLBReplacement.pop();
				TLBReplacement.push(updatedTLBEntry);
			}
		}


		TLBHit = false;
		accessAddress = 0;
		pageNumber = 0;
		frameNumber = 0;
		offset = 0;
		memset(pageContent, 0, sizeof(pageContent));
		memset(getLine, 0, sizeof(getLine));
	}

	/* PRINT OUT THE RESULT */
	printf("Page fault time: %d\n", pageFaultTime);
	printf("Page fault ratio: %.2f%%\n", (float) (pageFaultTime*100) / accessTime);
	printf("TLB hit time: %d\n", TLBHitTime); 
	printf("TLB hit ratio: %.2f%%\n", (float) (TLBHitTime*100) / accessTime);

	fclose(BACKING_STORE);
	fclose(ADDRESS);
	fclose(RESULT1);
	fclose(RESULT2);
	return 0;
}
