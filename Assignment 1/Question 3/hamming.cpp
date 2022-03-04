/*

Hamming Code Decoding

Author: Akshat Joshi
Roll: EE19B136

*/
#include<iostream>
#include<vector>
#include<cstdlib>

using namespace std;

// Contains error position and decoded characters of the block
struct blockInfo
{
     int errorPos;
    vector<char> chars;

};

// Check if number is a power of 2
bool isPowerOfTwo (int x)
{
   
    return x && (!(x&(x-1)));
}

// Converts hex in string form to a 64 bit number
unsigned long int stringToHex(string s)
{
    char* p;
    return strtoul(s.c_str(),&p,16);
}

// Splits the given hex into blocks of 40 bits each.
void getblocks(string& hexcode,vector<unsigned long int>& blocks)
{
    for(int i=0;i<hexcode.size();i+=10)
    {
        blocks.push_back(stringToHex(hexcode.substr(i,10)));
    }

}

// Performs error correction
blockInfo processBlock(unsigned long int block)
{
  
    int errorPos;
    bool occured = false;
    
    // errorPos is the xor of all positions where bit is set.
    
    for(int i=39;i>=0;i--)
    {
        if(((block>>i)&1) == 1)
        {
            if(!occured)
            {
                errorPos = 39-i; // Here "position" is relative to the MSB of the block. Hence 39-i is used.
                occured = true;
                continue;
            }
            
            errorPos = errorPos^(39-i);
            
        }
    }
    
    unsigned long int corrected = block; // stores the corrected block
    
    bool isError = false; // flag to check if error has been detected
    
    if(errorPos != 0) // Error detected
    {
        isError = true;
        corrected = corrected^(((unsigned long int)1)<<(39-errorPos));
    }
    else if((block>>39)&1 == 1) // Extended hamming bit flipped
    {
        isError = true;
        corrected = corrected^(((unsigned long int)1)<<(39));
    }
    
    blockInfo b;
    if(isError)
        b.errorPos = errorPos;
    else
        b.errorPos = -1;
    vector<char> chars(4,0);
    
    
    int k = 0;
    
    for(int i=38;i>=1;--i)
    {
        // If the position is a power of 2, it is a parity check bit. So ignore it
        if(!isPowerOfTwo(39-i)) 
        {
            
            char a = ((char)((corrected>>i)&1));
           
            chars[k/8]=(chars[k/8]<<1) + a;
         
            k+=1;
        }

    
    }
    
    b.chars = chars;

    return b;


}


int main()
{
    string hexcode;
    cin >> hexcode;

    vector<unsigned long int> blocks;
    getblocks(hexcode,blocks);

    string s;
    vector<unsigned int> errorpos;

    for(int i=0;i<blocks.size();i++)
    {
        blockInfo b = processBlock(blocks[i]);
        for(int i=0;i<b.chars.size();i++)
        {
           
            s+=b.chars[i];
        }
        if(b.errorPos != -1) // if errorPos is -1, it means no error is detected.
            errorpos.push_back(b.errorPos);
        
    }

    cout << "Text: " << s << endl;
    cout << "code word: " << hexcode << endl;
    cout << "Bit flip idx:";
    for(int i=0;i<errorpos.size();i++)
    {
        cout << errorpos[i] << " ";
    }
    if(errorpos.size() == 0)
    {
        cout << "Not flipped";
    }
    cout << endl;
    cout << "Num Blocks: " << blocks.size() << endl;


    return 0;
}