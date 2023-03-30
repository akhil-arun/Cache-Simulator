#include <iostream>
#include <cmath>
#include <string>
#include <bitset>
#include <iomanip>
#include <algorithm>

using namespace std;


 int convertToDecimal(string str, int len, int end){
 	int number = 0;
 	int exp = len-1;
 	for(int i = ((len + end) - 1); i >= end; i--){
    	  if(str[i] == '1')
    		number += pow(2,exp);
    	exp--;

 	}
 	
 	return number;
 }

 void slideSetLeft(int ** cache, int setNumber, int colSize, int wordsPerBlock, int tag){
 	for(int i = wordsPerBlock; i < colSize; i+= wordsPerBlock){
 		int movedTag = cache[setNumber][i];
 		for(int j = 0; j < wordsPerBlock; j++){
 			cache[setNumber][i - wordsPerBlock + j] = movedTag;
 		}
 	}
 	for(int j = 0; j < wordsPerBlock; j++){
 		cache[setNumber][colSize - wordsPerBlock + j] = tag;
 	}
 }

 void addToCache(int **cache, int setNumber, int colSize, int wordsPerBlock, int tag){
 	for(int i = 0; i < colSize; i+=wordsPerBlock){
 		if(cache[setNumber][i] < 0){
 			for(int j = 0; j<wordsPerBlock; j++){
 				cache[setNumber][i + j] = tag;
 			}
 			return;
 		}
 	}
 	slideSetLeft(cache, setNumber, colSize, wordsPerBlock, tag);
 }

 bool cacheHit(int ** cache, int colSize, int wordsPerBlock, int tag, int setNumber){
 	for(int i = 0; i < colSize; i+= wordsPerBlock){
 		if(cache[setNumber][i] == tag)
 			return true;
 	}
 	return false;
 }

 void printCache( int ** cache, int numSets, int colSize, int wordsPerBlock, int blockOff){
 	cout << "Cache:";
 	for(int i = 0; i < numSets; i++){
 		cout <<"[";
 		for(int j = blockOff; j <colSize; j+=wordsPerBlock){
 			if(cache[i][j] > -1){
 				if(j >= wordsPerBlock)
 					cout << ", "<< cache[i][j];
 				else
 					cout << cache[i][j];
 			}
 		}
 	   cout << "]";
 	}
 	cout << endl;
 }

 void updateSet(int ** cache, int setNumber, int start, int end, int wordsPerBlock, int tag){
 	for(int i = start + wordsPerBlock ; i <= end; i +=wordsPerBlock ){
 		int movedTag = cache[setNumber][i];
 		for(int j = 0; j < wordsPerBlock; j++)
 			cache[setNumber][i - wordsPerBlock + j ] = movedTag;
 	}
 	for(int i = 0; i < wordsPerBlock; i++){
 		cache[setNumber][end + i] = tag;
 	 }
 }

 void updateLRU(int ** cache, int setNumber, int colSize, int wordsPerBlock, int tag){
 	int nonNeg = 0;
 	int hitIndex = 0;
 	for (int i = 0; i < colSize; i += wordsPerBlock){
 		if(cache[setNumber][i] > 0)
 			nonNeg = i;
 		if (cache[setNumber][i] == tag)
 			hitIndex = i;
 		
 	}
 
 	updateSet(cache, setNumber, hitIndex, nonNeg, wordsPerBlock, tag);
 	
 	
 }

 // change to int
 int runCache(int* adr, int ** cache, int adrSize, int numSets, int colSize, int wordsPerBlock, int setBitLength, int blockOffLength){
 	int hits = 0;
   string ifHit;
 	for(int i = 0; i < adrSize; i++){
 		string memBits = bitset<12>(adr[i]).to_string();
 		reverse(memBits.begin(), memBits.end());
 		int setNumber = convertToDecimal(memBits,setBitLength, blockOffLength + 2);
 		int tagLen = 12 - setBitLength - blockOffLength - 2;
 		int tagStart = setBitLength + blockOffLength + 2;
 		int tag = convertToDecimal(memBits, tagLen, tagStart);
 		if(cacheHit(cache, colSize, wordsPerBlock, tag, setNumber)){
 			hits++;
 			updateLRU(cache, setNumber, colSize, wordsPerBlock, tag);
 			ifHit = "true";
 		}
 		else{
 			addToCache(cache, setNumber, colSize, wordsPerBlock, tag);
 			ifHit = "false";
 		}
 		for(int j = 0; j < wordsPerBlock; j++){
 			cout << "Address:" << setw(8) << left << hex << adr[i];
 			cout << "BlockOfs:" << setw(9) << left << dec << j;
 			cout << "Set:" << setw(4) << left << setNumber;
 			cout << "Tag:" << setw(4) << left << tag;
 			cout << "Hit:" << setw(12) << left << ifHit;
 			printCache(cache, numSets, colSize, wordsPerBlock, j);
 		}
 		
 	}
 	return hits;
 }

 void evaluateCache (string problem, int * adr, int size,  int wordsPerBlock, int nWay){
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
	cout << problem << endl;
	int numBlocks = 16 / wordsPerBlock;
	int numSets = numBlocks / nWay;

	int setBitLength = (int) (ceil(log2(numSets)));
	int blockOffLength = (int) (ceil(log2(wordsPerBlock)));

	int ** cache = new int * [numSets];
	int colSize = nWay * wordsPerBlock;
	
	for(int i = 0; i < numSets; i++)
		cache[i] = new int[colSize];

	for(int i = 0; i < numSets; i++){
		for(int j = 0; j < colSize; j++){
			cache[i][j] = -1;
		}
	}
	int hits, misses;
	double missRate, hitRate;
	hits = runCache(adr, cache, size, numSets, colSize, wordsPerBlock, setBitLength, blockOffLength);
   misses = size - hits;
   hitRate = ((double) hits/size) * 100;
   missRate = ((double) misses/size) * 100;

   cout << "Pass 1 hits:" << hits << " = " << round(hitRate) << "%" << endl;
   cout << "Pass 1 misses:" << misses << " = " << round(missRate) << "%" << endl;
   cout << endl;
	

	hits = runCache(adr, cache, size, numSets, colSize, wordsPerBlock, setBitLength, blockOffLength);
	misses = size - hits;
	hitRate = ((double) hits/size) * 100;
   missRate = ((double) misses/size) * 100;
	cout << "Pass 2 hits:" << hits << " = " << round(hitRate) << "%" <<endl;
   cout << "Pass 2 misses:" << misses << " = " << round(missRate) << "%" << endl;
   cout << endl;

	for(int i = 0; i < numSets; i++)
		delete [] cache[i];

	delete [] cache;

}

void problemNine(){
	int adr[]  = { 0x40, 0x44, 0x48, 0x4C, 0x70, 0x74, 0x78, 0x7C, 0x80, 0x84, 0x88, 
		     0x8C, 0x90 , 0x94 , 0x98, 0x9C, 0x0, 0x4, 0x8, 0xC ,0x10, 0x14, 0x18, 0x1C, 0x20};
     evaluateCache("Problem 8.9a", adr, 25, 1, 1);
     evaluateCache("Problem 8.9b", adr, 25, 1, 16);
     evaluateCache("Problem 8.9c", adr, 25, 1, 2);
     evaluateCache("Problem 8.9d", adr, 25, 2, 1);
}

void problemTen(){
	int adr[] = { 0x74, 0xA0, 0x78, 0x38C, 0xAC, 0x84, 0x88, 0x8C, 0x7C, 0x34, 0x38, 0x13C, 0x388, 0x18C};
   evaluateCache("Problem 8.10a", adr, 14, 1, 1);
   evaluateCache("Problem 8.10b", adr, 14, 2, 8);
   evaluateCache("Problem 8.10c", adr, 14, 2, 2);
   evaluateCache("Problem 8.10d", adr, 14, 4, 1);
}


int main (void){
	problemNine();
	problemTen();

}