#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <bitset>
#include <string.h>
using namespace std;



struct CacheLine
{
    bool valid;
    string tag;
};

bool validate(string binaryAddress)
{
    int size = binaryAddress.size();
    if (size > 24)
        return false;
    else 
        return true;
}

void simulateCache(vector<int> &accessSequence, int cacheSize, int cacheLineSize)
{
    int addressBits = 24;
    int cacheLines = cacheSize / cacheLineSize;
    vector<CacheLine> cache;
    cache.resize(cacheLines);

    int hits = 0;
    int misses = 0; 

    for (int address : accessSequence)
    {
            bitset<sizeof(int)*4> binaryAddress(address);
            string bitsetStr = binaryAddress.to_string();

            int cacheLines = cacheSize / cacheLineSize;
            int indexSize = log2(cacheLines);
            int dispSize = log2(cacheLineSize);
            int tagSize = addressBits - (dispSize + indexSize); //tag size is 24 - tag offset 

            bitsetStr = bitsetStr.substr(bitsetStr.find('1'));
            string tag = bitsetStr.substr(0, tagSize);

            bool val_result = validate(bitsetStr);

            bool hit = false;

        if (val_result)
        {
            // Check for cache hit
            for (int i = 0; i < cacheLines; i++)
            {
                if (cache[i].valid && cache[i].tag == tag)
                {
                    hits++;
                    hit = true;
                    break;
                }
            }

            // If it's a miss, update the cache
            if (!hit)
            {
                misses++;
                int lineIndex = address % cacheLines;
                cache[lineIndex].valid = true;
                cache[lineIndex].tag = tag;
            }

            // Output cache state and performance metrics after each access
            cout << "Addess: " << address
                << " Hits: " << hits
                << " Misses: " << misses
                << " Cache State: [";

            for (int i = 0; i < cacheLines; i++)
            {
                cout << " [" << (cache[i].valid ? "Valid" : "Invalid") << ","
                    << "Tag:" << cache[i].tag
                    << "]";
            }
            cout << " ]" <<endl;
        }
        else 
        {
            cout<<"Error: memory values out of range.";
        }
    }

    cout << "Final Summary:" << endl;
    cout << "Total Number of Accesses: " << accessSequence.size() << endl;
    cout << "Hit Ratio: " << static_cast<double>(hits) / accessSequence.size() << endl;
    cout << "Miss Ratio: " << static_cast<double>(misses) / accessSequence.size() << endl;
    // add average memory access time 

}


int main()
{
    int totalCacheSize, cacheLineSize, accessCycles;
    
    cout << "Enter total cache size (in bytes): ";
    cin >> totalCacheSize; //S = CxL

    cout << "Enter cache line size (in bytes): ";
    cin >> cacheLineSize; //L

    cout << "Enter number of cycles needed to access the cache: ";
    cin >> accessCycles;

    string inst_accessSeqFile = "instruction_accessSequence.txt";
    ifstream accessSeqFileStream(inst_accessSeqFile); 
    vector<int> instruction_accessSequence;
    string inst_addressStr;
    while (getline(accessSeqFileStream, inst_addressStr, ','))
    {
        instruction_accessSequence.push_back(stoi(inst_addressStr)); //Convert the string to an integer and add to the access sequence
    }
    accessSeqFileStream.close();

    string data_accessSeqFile = "data_accessSequence.txt";
    ifstream data_accessSeqFileStream(data_accessSeqFile); 
    vector<int> data_accessSequence;
    string data_addressStr;
    while (getline(data_accessSeqFileStream, data_addressStr, ','))
    {
        data_accessSequence.push_back(stoi(data_addressStr)); //Convert the string to an integer and add to the access sequence
    }
    data_accessSeqFileStream.close();

    cout<<"Instruction Access Sequence Cache Simulation: "<<endl;
    simulateCache(instruction_accessSequence, totalCacheSize, cacheLineSize);
    cout<<"Data Access Sequence Cache Simulation: "<<endl;
    simulateCache(data_accessSequence, totalCacheSize, cacheLineSize);

    return 0;
} 
