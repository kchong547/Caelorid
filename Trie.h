#ifndef TRIE_INCLUDED
#define TRIE_INCLUDED

#include <string>
#include <vector>
#include <list>

template<typename ValueType>
class Trie
{
public:
    //DO NOT ADD ANY PUBLIC MEMBER FUNCTIONS!!!!
    Trie();
    ~Trie();
    void reset();
    void insert(const std::string& key, const ValueType& value);
    std::vector<ValueType> find(const std::string& key, bool exactMatchOnly) const;
    
    // C++11 syntax for preventing copying and assignment
    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;
    
    void printTrie()
    {
        printing(root, "");
    }

private:
    //YOU MAY ADD AS MANY PUBLIC MEMBER FUNCTIONS AS YOU WANT
    struct Node
    {
        //list of the different values
        //starts off empty
        std::list<ValueType> values;
        
        //character label for the pointer
        char label;
        
        //list of different pointers to children
        std::list<Node*> children;
    };
    void freeTree(Node *curr);
    void addContinue(const std::string& key, Node*& curr);
    void findMatch(Node *curr, std::string keySub, bool mismatch, std::vector<ValueType>& matches, bool& exactMatchOnly) const;
    Node* root;
    
    //REMOVE LATER
    void printing(Node* p, std::string tabs)
    {
        if (p == nullptr) {
            std::cout << std::endl;
            return;
        }
        std::cout << tabs << p->label<< ": ";
        //                for (auto  = 0; i < p->valuesList.size(); i++)
        for(auto it = p->values.begin(); it != p->values.end(); it++)
            std::cout << (*it) << " ";
        std::cout << std::endl;
        
        for(auto j = p->children.begin(); j != p->children.end(); j++)
            printing(*j, tabs + "  ");
    }
};

template<typename ValueType>
inline
Trie<ValueType>::Trie()
{
    //create a root trie node with no children or value
    //set root pointer to said node
    root = new Node;
    root->label = '\0';
}

template<typename ValueType>
inline
Trie<ValueType>::~Trie()
{
    //free all memory associated with a trie
    freeTree(root);
}

template<typename ValueType>
inline
void Trie<ValueType>::reset()
{
    //free all memory associated with a trie
    //allocate a new empty trie with single root node
    //MUST RUN IN O(N) TIME!
    freeTree(root);
    
    root = new Node;
}

template<typename ValueType>
inline
void Trie<ValueType>::insert(const std::string& key, const ValueType& value)
{
    if(key == "")
    {
        return;
    }
    
    Node* curr = this->root;
    
    //go through the list of children, checking their markers
    for(int i = 0; i < key.size(); i++)
    {
        bool contains = false;
        std::string keySub = key.substr(i);
        //if there are no children nodes
        if(curr->children.empty())
        {
             //then you must create new nodes for all following characters in the string
            addContinue(keySub, curr);
            break;
        }

        typename std::list<Node*> :: iterator it;
        it = curr->children.begin();
        while(it != curr->children.end())
        {
            //if child with matching marker,
            if((*it)->label == key[i])
            {
                //then move onto the next child
                contains = true;
                curr = (*it);
                break;
            }
            it++;
        }
        
        //if no match then create a new node with the character as the marker
        //must create all nodes following!
        //break out of the loop!
        if(!contains)
        {
            addContinue(keySub, curr);
            break;
        }
    }
    //when reaching the end of the string, add the value to the current node with the last character marker
    curr->values.push_back(value);
}

template<typename ValueType>
std::vector<ValueType> Trie<ValueType>::find(const std::string& key, bool exactMatchOnly) const
{
    std::vector<ValueType> matchValues;
    //using recursion
        //check if any of the children of the node have values that match up with the next character in the string
            //if so, move curr to that child node and call recursion again
            //if not, check mismatch
                //if no prev mismatch
                    //set mismatch to true and continue
                //if prev mismatch
                    //break out of recursion
    if(!(root->children.empty()))
    {
        //must check to make sure that the first value matches
        //otherwise dont bother with the recursion
        typename std::list<Node*> :: iterator it;
        it = root->children.begin();
        while(it != root->children.end())
        {
            //if child with matching marker with the first in Key
            if((*it)->label == key[0])
            {
                //if this is the only character in the key then push back the values in the node to matchValues
                if(key.size() == 1)
                {
                    typename std::list<ValueType> :: iterator it2;
                    it2 = (*it)->values.begin();
                    while( it2 != (*it)->values.end())
                    {
                        matchValues.push_back((*it2));
                        it2++;
                    }
                }
                
                findMatch(*it, key.substr(1), false, matchValues, exactMatchOnly);
                //we can break because it is guaranteed that there wont be a duplicate child node in the root with the same marker
                break;
            }
            else
            {
                it++;
            }
        }
    }
    return matchValues;
}

template<typename ValueType>
void Trie<ValueType>::findMatch(Node *curr, std::string keySub, bool mismatch, std::vector<ValueType>& matches, bool& exactMatchOnly) const
{
    std::string tempKeySub = "";
    //base cases
    //if the substring is empty then theres nothing to compare
    if(keySub == "")
    {
        return;
    }
    
    if(curr->children.empty())
    {
        //if children empty and you havent checked everything in the string, then the string isnt in there!
        //break recursion!!
        return;
    }
    
    if(keySub.size() == 1)
    {
        //do the check
        //if pass
        typename std::list<Node*> :: iterator it;
        it = curr->children.begin();
        
        while(it != curr->children.end())
        {
            //if child with matching marker with the first and only character in KeySub
            if((*it)->label == keySub[0])
            {
                //then add values of the child node that matches the final character to the vector!
                typename std::list<ValueType> :: iterator it2;
                it2 = (*it)->values.begin();
                while( it2 != (*it)->values.end())
                {
                    matches.push_back((*it2));
                    it2++;
                }
            }
            //current children's marker doesnt match!!!
            //creation of a SNiP!
            else
            {
                //if there wasnt a previous mismatch and the search isnt looking for exact matches
                if(!mismatch && !exactMatchOnly)
                {
                    //then add values of the child node that matches the final character ot the vector!
                    typename std::list<ValueType> :: iterator it2;
                    it2 = (*it)->values.begin();
                    while( it2 != (*it)->values.end())
                    {
                        matches.push_back((*it2));
                        it2++;
                    }
                }
                //if there was a previous mismatch then just break out of the recursion its not a SNiP
                //or if we're only looking or exact matches
            }
            it++;
        }
        //finish the recursion. the deed is done
        return;
    }
    //time to iterate through all the children!!!!!woooooot
    typename std::list<Node*> :: iterator it;
    it = curr->children.begin();
    while(it != curr->children.end())
    {
        //if child with matching marker with the first character in KeySub
        if((*it)->label == keySub[0])
        {
            //then move onto the next child
            tempKeySub = keySub.substr(1);
            findMatch((*it), tempKeySub, mismatch, matches, exactMatchOnly);
        }
        //current children's marker doesnt match!!!
        //creation of a SNiP!
        else
        {
            //if there wasnt a previous mismatch
            if(!mismatch && !exactMatchOnly)
            {
                tempKeySub = keySub.substr(1);
                findMatch((*it), tempKeySub, true, matches, exactMatchOnly);
            }
            //if there was a previous mismatch then just break out of the recursion its not a SNiP
        }
        it++;
    }
}

//non-member function
template<typename ValueType>
void Trie<ValueType>::freeTree(Node *curr)
{
    if(curr->children.empty())
    {
        //theres no children to delete
        delete curr;
        return;
    }
    
    typename std::list<Node*>:: iterator it;
    it = curr->children.begin();
    while(it != curr->children.end())
    {
        freeTree(*it);
        it = curr->children.erase(it);
    }
    
    delete curr;
}

//input the substring with the remaining characters into the function that must be added
//node curr is the current node that has no children or does not have the next character
template<typename ValueType>
void Trie<ValueType>::addContinue(const std::string& keySub, Node*& curr)
{
    for(int i = 0; i != keySub.size(); i++)
    {
        Node* temp = new Node;
        (*temp).label = keySub[i];
        curr->children.push_back(temp);
        curr = temp;
    }
}
#endif // TRIE_INCLUDED


