#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <utility>
#include "Trie.h"
#include <unordered_map>
using namespace std;

class GenomeMatcherImpl
{
public:
    //DO NOT ADD ANY PUBLIC MEMBER FUNCTIONS TO YOUR CODE!!!
    GenomeMatcherImpl(int minSearchLength);
    void addGenome(const Genome& genome);
    int minimumSearchLength() const;
    bool findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const;
    bool findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const;
    
    //addition of destructor for dynamically allocated genome library
    ~GenomeMatcherImpl();
private:
    //YOU MAY ADD AS MANY PRIVATE ITEMS AS NEEDED
    int m_minSearchLength;
    //genome library for storing genomes
    //first element is the genome name
    //second element is the genome object itself
    unordered_map<string, const Genome*> genomeLibrary;
    //first int is for Genome number, second int is for position number
    Trie<pair<string, int>> triePair;
};

//custom comparison operator
//may need to reverse so its in descending order...
bool genomeCompare(const GenomeMatch &a, const GenomeMatch &b)
{
    if(a.percentMatch > b.percentMatch)
    {
        return true;
    }
    //tie breaker
    if(a.percentMatch == b.percentMatch && a.genomeName < b.genomeName)
    {
        return true;
    }
    return false;
}

GenomeMatcherImpl::~GenomeMatcherImpl()
{
    unordered_map <string,const Genome*>::iterator iter;
    iter = genomeLibrary.begin();
    while(iter != genomeLibrary.end())
    {
        //deleting the dynamically allocated genome
        delete iter->second;
        iter++;
    }
    //deleting everything in the genomeLibrary
    genomeLibrary.erase(genomeLibrary.begin(), genomeLibrary.end());
}

GenomeMatcherImpl::GenomeMatcherImpl(int minSearchLength)
{
    m_minSearchLength = minSearchLength;
}

void GenomeMatcherImpl::addGenome(const Genome& genome)
{
    //add the genome to the genomeLibrary
    genomeLibrary[genome.name()] = new Genome(genome);
    
    string fragment;
    int position = 0;
    //add every substring of length minSearchLength of the genome's DNA sequence into the trie library
    //continue adding until extract returns false
    while(genome.extract(position, m_minSearchLength, fragment))
    {
//        cerr << fragment << endl;
        pair <string, int> genomePair (genome.name() , position);
        triePair.insert(fragment, genomePair);
        position += 1;
    }
}

int GenomeMatcherImpl::minimumSearchLength() const
{
    //return the minimum search length passed into the constructor
    return m_minSearchLength;
}

bool GenomeMatcherImpl::findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const
{
    string genomeRest;
    bool mismatch = false;
    bool add = true;
    int length = 0;
    
    vector<pair<string, int>> DNAmatches;
    //if the fragment is smaller than the minimum length or the minimiumlength is less than the minimum search length
    //return false
    if(fragment.size() < minimumLength ||  minimumLength < m_minSearchLength)
    {
        return false;
    }

    //need to extract the fragment that is the same size as m_minSearchLength to find the first few that fit into the trie
    string fragmentSmall = fragment.substr(0, m_minSearchLength);
    //returns the vectors with the matches
    DNAmatches = triePair.find(fragmentSmall, exactMatchOnly);
//    cerr << DNAmatches.size() << endl;
    
    //now manually check the rest of the characters in fragment
    if(fragment.size() > m_minSearchLength)
    {
        for(int i = 0; i < DNAmatches.size(); i++)
        {
            mismatch = false;
            add = true;
            length = 0;
            //extract the remaining potential matches
            int position = DNAmatches[i].second;
            string name = DNAmatches[i].first;
            
            Genome temp = *(genomeLibrary.at(name));
            
            //extracts sequence of genomes that we need to compare from the sequence of length fragment
            //does this to consider the SNiPs
            temp.extract(position, fragment.size(), genomeRest);
            //extract the rest of the genomes that we need to compare in the fragment
//            cerr << genomeRest << endl;
            //compare the entire genomeSequence that we want to compare with the entire fragment
            for (int i = 0 ; i < fragment.size() ; i++)
            {
                //if match then keep iterating
                if(fragment[i] == genomeRest[i])
                {
                    length++;
                    continue;
                    
                }
                else if(fragment[i] != genomeRest[i] && exactMatchOnly)
                {
                    
//                    add = false;
                    break;
                }
                else if(fragment[i] != genomeRest[i] && !exactMatchOnly)
                {
                    //if snip
                    //no need to check if we're only looking for exact matches
                    if(!mismatch)
                    {
                        mismatch = true;
                        length++;
                        continue;
                    }
                    //if more than two errors
                    else
                    {
                        if(length >= minimumLength)
                        {
                            break;
                        }
                        add = false;
                        break;
                    }
                }
            }
            //add the match into matches
            if(add)
            {
                if(length >= minimumLength)
                {
                    bool alreadyThere = false;
                    //must check if there already is a match for a certain genome
                    //if so, compare the lengths of the matches
                    //only change if length of new is greater
                    for(int i = 0; i < matches.size(); i++)
                    {
                        if(matches[i].genomeName == name)
                        {
                            if(length > matches[i].length)
                            {
                                matches[i].genomeName = name;
                                matches[i].length = length;
                                matches[i].position = position;
                            }
                            alreadyThere = true;
                        }
                    }
                    //if the genome doesnt have a current sequence added
                    if(!alreadyThere)
                    {
                        DNAMatch temp;
                        temp.length = length;
                        temp.position = position;
                        temp.genomeName = name;
                        matches.push_back(temp);
                    }
                }
            }
        }
    }
    //fragment.size ==  m_minSearchLength && minimumLength
    else
    {
        for(int i = 0; i < DNAmatches.size(); i++)
        {
            mismatch = false;
            //extract the remaining potential matches
            int position = DNAmatches[i].second;
            string name = DNAmatches[i].first;
            
            bool alreadyThere = false;
            //must check if there already is a match for a certain genome
            //if so, compare the lengths of the matches
            //only change if length of new is greater
            for(int i = 0; i < matches.size(); i++)
            {
                if(matches[i].genomeName == name)
                {
                    if(length > matches[i].length)
                    {
                        matches[i].genomeName = name;
                        matches[i].length = m_minSearchLength;
                        matches[i].position = position;
                    }
                    alreadyThere = true;
                }
            }
            //if the genome doesnt have a current sequence added
            if(!alreadyThere)
            {
                DNAMatch temp;
                temp.length = m_minSearchLength;
                temp.position = position;
                temp.genomeName = name;
                matches.push_back(temp);
            }
        }
    }
    
    //if there was no match within the fragment and the sequence, return false
    if(matches.empty())
    {
        return false;
    }
    return true;
}

bool GenomeMatcherImpl::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
    //string is the genome name
    //int is the number of sequence matches
    unordered_map<string, int> matchNum;
    if(fragmentMatchLength < m_minSearchLength)
    {
        return false;
    }
    
    //number of sequences
    int S = query.length()/fragmentMatchLength;
    
    for(int i = 0; i< S; i++)
    {
        //this needs to reset per sequence
        vector<DNAMatch> matchQuery;
        string fragFind = "";
        int index = i * fragmentMatchLength;
        query.extract(index, fragmentMatchLength, fragFind);
        
        //stores all of the genome information for those that have the sequence
        if(this->findGenomesWithThisDNA(fragFind, fragmentMatchLength, exactMatchOnly, matchQuery))
        {
            for(int i = 0; i < matchQuery.size(); i++)
            {
                matchNum[matchQuery[i].genomeName]++;
            }
        }
    }
    
    unordered_map<string,int>::const_iterator iter = matchNum.begin();
    while(iter != matchNum.end())
    {
        //divide the number of sequences matched by the total number of sequences
        double p = (iter->second / static_cast<double>(S)) * 100;
        
        //if p > matchPercentThreshold
        //then the genome is a matching genome
        if(p >= matchPercentThreshold)
        {
            GenomeMatch temp = GenomeMatch();
            temp.genomeName = iter->first;
            temp.percentMatch = p;
            results.push_back(temp);
        }
        iter++;
    }
    
    //if no genome passed the matchPercentThreshold
    if(results.empty())
    {
        return false;
    }
    
    //sort the dang vector oof there goes my big O
    //sort using mergeSort probably to minimize big O
    std::sort(results.begin(), results.end(), genomeCompare);
    
    //at least one genome passed the matchPercentThreshold
    return true;
}

//******************** GenomeMatcher functions ********************************

// These functions simply delegate to GenomeMatcherImpl's functions.
// You probably don't want to change any of this code.

GenomeMatcher::GenomeMatcher(int minSearchLength)
{
    m_impl = new GenomeMatcherImpl(minSearchLength);
}

GenomeMatcher::~GenomeMatcher()
{
    delete m_impl;
}

void GenomeMatcher::addGenome(const Genome& genome)
{
    m_impl->addGenome(genome);
}

int GenomeMatcher::minimumSearchLength() const
{
    return m_impl->minimumSearchLength();
}

bool GenomeMatcher::findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const
{
    return m_impl->findGenomesWithThisDNA(fragment, minimumLength, exactMatchOnly, matches);
}

bool GenomeMatcher::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
    return m_impl->findRelatedGenomes(query, fragmentMatchLength, exactMatchOnly, matchPercentThreshold, results);
}
