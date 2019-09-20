#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
#include <istream>
using namespace std;

class GenomeImpl
{
public:
    //DO NOT ADD ANY ADDITIONAL PUBLIC MEMBER FUNCTIONS
    GenomeImpl(const string& nm, const string& sequence);
    static bool load(istream& genomeSource, vector<Genome>& genomes);
    int length() const;
    string name() const;
    bool extract(int position, int length, string& fragment) const;
    
private:
    //YOU MAY ADD AS MANY PRIVATE VARIABLES/FUNCTIONS AS YOU LIKE
    string m_name;
    string m_sequence;
};

GenomeImpl::GenomeImpl(const string& nm, const string& sequence)
{
    m_name = nm;
    m_sequence = sequence;
}

bool GenomeImpl::load(istream& genomeSource, vector<Genome>& genomes)
{
    bool greaterBefore = false;
    string genomeName = "";
    string genomeSequence = "";

    // Example 3 - read and process each character of a file until end
    char c;
    // get returns infile; the while tests its success/failure state
    while (genomeSource.get(c))
    {
        //... process c
        //if the line starts with a >, that means that everything following it and until you hit an newline will be the genome name
        if((c == '>' || greaterBefore))
        {
            if(greaterBefore)
            {
                genomeName += c;
            }
            //extract characters until you hit a newline
            while (genomeSource.get(c) && c != '\n')
            {
                //concantenate the character into the genomeName until you hit a newline
                genomeName += c;
            }
            //move on to the next character following the last character extracted from c (which should be a newline character and you dont want to append that to anywhere
            greaterBefore = false;
            continue;
        }
        //if the first character is an alphabetical character, then it means that the string will be a genomeSequence
        else if(isalpha(c) && c != '\n')
        {
            //add the current
            genomeSequence += c;
            //extract characters until you reach the next >, which indicates a new genome completely
            while(c != '>' && genomeSource.get(c))
            {
                //when extracting, ignore any newline characters and do not append them to genomesequence
                //if any character other than G,A,T,C, or N then return false. improper format
                //remember to change any lowercase character to upper case
                //change to uppercase

                c = toupper(c);
                //ignore newlines
                if(c != '\n' && c != '>')
                {
                   if(c == 'A' || c == 'G' || c == 'C' || c == 'T' || c == 'N')
                   {
                       //concantenate the character into the genomeName
                       genomeSequence += c;
                   }
                   else
                   {
                       //this means that there was s character other than A,G,C,T, or N in the genome sequence which makes it invalid
//                       cerr << c << endl;
                       return false;
                   }
                }
            }
            //this ensures that the code wont recognize the next substring as a sequence when there was a greater than sign before it
            if(c == '>')
            {
                greaterBefore = true;
            }
            //after successfully extracting a sequence, then create a new genome object
            //if genomeName == ""; then return false
            if(genomeName.size() == 0 || genomeSequence.size() == 0)
            {
                return false;
            }
            else
            {
                //if both genomeName and genomeSequence arent empty, then create new genome object and push back
                //reset genomeName and genomeSequence
                Genome temp = Genome(genomeName, genomeSequence);
                genomes.push_back(temp);
                genomeName = "";
                genomeSequence = "";
            }
        }
        //if the newline starts with anything other than an alphabetical character, then bad formating
        //return false
        else
        {
            return false;
        }
    }
    
    if(genomes.size() == 0)
    {
        return false;
    }
    return true;  // This compiles, but may not be correct
}

int GenomeImpl::length() const
{
    return m_sequence.size();
}

string GenomeImpl::name() const
{
    return m_name;
}

bool GenomeImpl::extract(int position, int length, string& fragment) const
{
    if(position + length - 1 > this->length() - 1)
    {
        //the only thing that would cause this to fail is if the end position of the to be substring goes beyond the last position in the sequence of the genome
        //fragment remains unchanged if the extraction is unsuccessful
        return false;
    }
    else
    {
        //if the substring has valid indexes within the sequence (extraction didnt fail)
        //change fragment
        //return true
        fragment = this->m_sequence.substr(position, length);
        return true;
    }
}

//******************** Genome functions ************************************

// These functions simply delegate to GenomeImpl's functions.
// You probably don't want to change any of this code.

Genome::Genome(const string& nm, const string& sequence)
{
    m_impl = new GenomeImpl(nm, sequence);
}

Genome::~Genome()
{
    delete m_impl;
}

Genome::Genome(const Genome& other)
{
    m_impl = new GenomeImpl(*other.m_impl);
}

Genome& Genome::operator=(const Genome& rhs)
{
    GenomeImpl* newImpl = new GenomeImpl(*rhs.m_impl);
    delete m_impl;
    m_impl = newImpl;
    return *this;
}

bool Genome::load(istream& genomeSource, vector<Genome>& genomes)
{
    return GenomeImpl::load(genomeSource, genomes);
}

int Genome::length() const
{
    return m_impl->length();
}

string Genome::name() const
{
    return m_impl->name();
}

bool Genome::extract(int position, int length, string& fragment) const
{
    return m_impl->extract(position, length, fragment);
}


