#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <bitset>
#include <string.h>
#include <sstream>
#include <string>
#include <map>

using namespace std;
map<int,int>memory;
vector<int> sequence;

//for bonus feature later 
bool writePolicy; 
bool writeAllocate; 
vector<int> writingIndexStorage;
vector<int> writingData;  

struct CacheLine
{
    bool valid;
    string tag;
    int index; 
    vector<int> data; 

    CacheLine(int i, int d, string t, bool v) 
    {
    index = i;
    data = vector<int>(d, 0);
    tag = t;
    valid = v;
    } 
};

struct Cache
{ 
    int accessCycles;
    int cacheSize;//total bytes in cache user
    int lines;//cache_size/line_size->block calculated
    int lineSize;
    int offset;
    int indexBits;
    int tagBits;
    int accesses;
    int hits;
    int misses;

    vector<CacheLine> cacheEntries; 

   Cache(int size, int lineS, int c) : cacheSize(size), lineSize(lineS), accessCycles(c) // User inputs, will be sent to myCache
   {
        lines = cacheSize / lineSize;
        for (int i = 0; i < lines; i++) 
        {
            CacheLine entry(i, lineSize, "", false);
            cacheEntries.push_back(entry);
        }

        offset = int(log2(lineSize));
        indexBits = int(log2(lines));
        tagBits = 24 - (indexBits + offset);
        accesses = 0;
        hits = 0;
        misses = 0;
    }

    void accessSequence(vector<int> &sequence); 
};

bool validate(string binaryAddress)
{
    int size = binaryAddress.size();
    if (size > 24)
        return false;
    else 
        return true;
}

string decimal_binary(int n)
{
    string binary = "";

    if (n < 0 || n >= (1 << 24)) {
        return "Error: Number out of 24-bit range";
    }
    for (int i = 23; i >= 0; i--) {
        binary += ((n >> i) & 1) ? "1" : "0";
    }

    return binary;
}

void Cache::accessSequence(vector<int> &sequence)
{
    for (auto memAddress : sequence)
    {
        string binAddress = decimal_binary(memAddress);
        string x = binAddress.substr(0, tagBits);
        x = binAddress.substr(tagBits, indexBits);
        x = binAddress.substr(tagBits + indexBits, offset);

        string tag = binAddress.substr(0, tagBits);
        int index = stoi(binAddress.substr(tagBits, indexBits), 0, 2);
        int disp = stoi(binAddress.substr(tagBits + indexBits, offset), 0, 2);
        bool hit = false;

        accesses++;

        if (cacheEntries[index].tag == tag && cacheEntries[index].valid)
        {
            hit = true;
            hits++;
        }
        else
        {
            misses++;
            cacheEntries[index].tag = tag;
            cacheEntries[index].valid = true;
        }

        cout << "Access: " << dec << accesses << endl;
        cout << "Getting address: 0x" << hex << memAddress << endl;

        if (hit)
        {
            cout << "HIT" << endl;
        }
        else
        {
            cout << "MISS" << endl;
        }

        for (auto entry : cacheEntries)
        {
            cout << "Cache Entry " << dec << entry.index + 1 << ": Validity: " << entry.valid << ", Tag bits- " << entry.tag << endl;
        }

        cout << "Total Number of Accesses: " << accesses << endl;
        double hitRatio = (double)hits / accesses;
        cout << "Hit Ratio: " << hitRatio << endl;
        double missRatio = (double)misses / accesses;
        cout << "Miss Ratio: " << missRatio << endl;
        double AMAT = ((double)accessCycles + (missRatio * 120));
        cout << "AMAT: " << AMAT << " cycles" << endl
             << endl;
    }
}


// void simulateCache(vector<int> &accessSequence, int cacheSize, int cacheLineSize, int cycles)
// {
//     int addressBits = 24;
//     int cacheLines = cacheSize / cacheLineSize;
//     vector<CacheLine> cache;
    
//     cache.resize(cacheLines);

//     int hits = 0;
//     int misses = 0; 
//     int accesses = 0; 

//     for (int address : accessSequence)
//     {
//             bitset<sizeof(int)*4> binaryAddress(address);
//             string bitsetStr = binaryAddress.to_string();

//             int cacheLines = cacheSize / cacheLineSize;
//             int indexSize = int(log2(cacheLines));
//             int dispSize = int(log2(cacheLineSize));
//             int tagSize = addressBits - (dispSize + indexSize); //tag size is 24 - tag offset 

//             bitsetStr = bitsetStr.substr(bitsetStr.find('1'));
//             string tag = bitsetStr.substr(0, tagSize);

//             bool val_result = validate(bitsetStr);

//             bool hit = false;

//             accesses ++; 
//         if (val_result)
//         {
//             // Check for cache hit
//             for (int i = 0; i < cacheLines; i++)
//             {
//                 if (cache[i].valid && cache[i].tag == tag)
//                 {
//                     hits++;
//                     hit = true;
//                     break;
//                 }
//             }

//             // If it's a miss, update the cache
//             if (!hit)
//             {
//                 misses++;
//                 int lineIndex = address % cacheLines;
//                 cache[lineIndex].valid = true;
//                 cache[lineIndex].tag = tag;
//             }

//             // Output cache state and performance metrics after each access
//             cout << "Address: " << address
//                 << " Hits: " << hits
//                 << " Misses: " << misses
//                 << " Cache State: [";
//                 double miss_ratio= (double) misses / accesses;
//                 double hit_ratio= (double) hits / accesses;
//                  double AMAT= ((double)cycles + (miss_ratio * 120));
//                  cout << "Average Memory Access Time (AMAT): " << AMAT << " cycles" << endl<<endl;

//             for (int i = 0; i < cacheLines; i++)
//             {
//                 cout << " [" << (cache[i].valid ? "Valid" : "Invalid") << ","
//                     << "Tag:" << cache[i].tag
//                     << "]";
//             }
//             cout << " ]" <<endl;
//         }
//         else 
//         {
//             cout<<"Error: memory values out of range.";
//         }
//     }

//     cout << "Final Summary:" << endl; //this has to be for each access, cannot be summarized. ~nour
//     cout << "Total Number of Accesses: " << accessSequence.size() << endl;
//     cout << "Hit Ratio: " << static_cast<double>(hits) / accessSequence.size() << endl;
//     cout << "Miss Ratio: " << static_cast<double>(misses) / accessSequence.size() << endl;
//     // add average memory access time 

// }

bool power2(int number) 
{
    return (number > 0 && (log2(number) == round(log2(number))) );
}

int main()
{
    int totalCacheSize, cacheLineSize, accessCycles;

do
{
    cout << "Enter total cache size (in bytes): ";
    cin >> totalCacheSize; //S = CxL
    if (!power2(totalCacheSize))
    {
        cout << "Total cache size must be a power of 2. Please try again." << endl;
    }
} while (!power2(totalCacheSize));

do
{
    cout << "Enter cache line size (in bytes): ";
    cin >> cacheLineSize; //L
    if (!power2(cacheLineSize))
    {
        cout << "Cache line size must be a power of 2. Please try again." << endl;
    }
} while (!power2(cacheLineSize));

cout << "Enter number of cycles needed to access the cache: ";
cin >> accessCycles;

    // string inst_accessSeqFile = "instruction_accessSequence.txt";
    // ifstream accessSeqFileStream(inst_accessSeqFile); 
    // vector<int> instruction_accessSequence;
    // string inst_addressStr;
    // while (getline(accessSeqFileStream, inst_addressStr, ','))
    // {
    //     instruction_accessSequence.push_back(stoi(inst_addressStr)); //Convert the string to an integer and add to the access sequence
    // }
    // accessSeqFileStream.close();

    string data_accessSeqFile = "access_sequence.txt";
    ifstream data_accessSeqFileStream(data_accessSeqFile); 
    vector<int> data_accessSequence;
    string data_addressStr;
    while (getline(data_accessSeqFileStream, data_addressStr, ','))
    {
        data_accessSequence.push_back(stoi(data_addressStr)); //Convert the string to an integer and add to the access sequence
        cout << "File opened successfully " << endl; 
    }
    data_accessSeqFileStream.close();

 
    Cache myCache(totalCacheSize, cacheLineSize, accessCycles);
    myCache.accessSequence(data_accessSequence);


    cout << "Choose writing policy: " << endl;
    cout << "For write through press 1, for write back press 0: ";
    cin >> writePolicy;
    cout << "Choose writing miss policy: " << endl;
    cout << "For write allocate choose 1, for no write allocation choose 0: ";
    cin >> writeAllocate; 
    // cout<<"Instruction Access Sequence Cache Simulation: "<<endl;
    // simulateCache(instruction_accessSequence, totalCacheSize, cacheLineSize, accessCycles);

    // cout<<"Data Access Sequence Cache Simulation: "<<endl;
    // simulateCache(data_accessSequence, totalCacheSize, cacheLineSize, accessCycles);

    return 0;
} 