//
//  main.cpp
//  Btree
//  Created by Zeinab Kazemi on 4/17/15.
//  Copyright (c) 2015 Zeinab Kazemi. All rights reserved.
//

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <map>
using namespace std;

//each node of the B+ tree
class Node
{
public:
    int numofkeys;
    vector <string> keys;
    bool leaf;
    map <string, string> data;
    vector <Node*> pointers;
    Node* next;
    Node* prev;
    bool marked;
    Node(int num)
    {
        //keys are num of pointers -1
        numofkeys = num;
        leaf = false;
        next = prev = NULL;
        marked=false;
    }
};
class Splitreturn
{
public:
    Node* first;
    Node* second;
    string midval;
};
//the data base itself
class BPlusTree
{
    
    int level;
    int numofkeys;
    Node* Treeroot;
    //Node* temproot;
    Node* listhead;
    int num_of_records;
public:
    BPlusTree(int num)
    {
        //setting default values
        //keys are num of pointers -1
        numofkeys = num-1;
        Treeroot = NULL;
        num_of_records =0;
        // tree with one node is assumed has a level of zero
        level = 0;
    }
    void insert_to_BTree(string );
    void fixpointers(Node*& , int& , Splitreturn* );
    Splitreturn* insert(string, Node*& );
    Splitreturn* split(Node* );
    Splitreturn* midnode_split(Node*);
    void traverse(Node*);
    void unmark(Node*);
    bool search(string , Node* , bool, string, bool);
    void mydelete(string , Node*);
    void update(string , string );
    void add_to_list( Node*);
    void snapshot(Node*);
    void list();
    Node* getroot()
    {
        return Treeroot;
    }
    const int getlevel()
    {
        return level;
    }
    const int getnumrecords()
    {
        return num_of_records;
    }
    Node* getheadlist()
    {
        // this function returns the head of the linked list
        return listhead;
    }
};

// this is called when the middle nodes of the tree need to be splited
Splitreturn* BPlusTree::midnode_split(Node* node)
{
    //increasing the level of tree by 1 in case of split node
    Splitreturn* res = new Splitreturn();
    vector<string> temp;
    
    Node* secondnode =  new Node(numofkeys);
    //<<node->keys.size()<<endl;
    for(int i =0; i < node->keys.size(); i++)
    {
        temp.push_back(node->keys[i]);
    }
    sort(temp.begin(), temp.end());
    
    string mid = temp[temp.size()/2];
    //erasing the previous keys from the node
    node->keys.erase(node->keys.begin(), node->keys.end());
    int i =0;
    vector <Node*> tempptr = node->pointers;
    node->pointers.erase(node->pointers.begin(), node->pointers.end());
    //dividing the node into two seperate nodes, changing the pointers and keys
    while(temp[i] < mid && i < numofkeys)
    {
        node->keys.push_back(temp[i]);
        node->pointers.push_back(tempptr[i]);
        i++;
    }
    node->pointers.push_back(tempptr[i]);
    i++;
    while( i < temp.size())
    {
        secondnode->keys.push_back(temp[i]);
        secondnode->pointers.push_back(tempptr[i]);
        i++;
    }
    secondnode->pointers.push_back(tempptr[i]);
    
    res->midval = mid;
    res->first = node;
    res->second = secondnode;
    return res;
}
// this function is called when the leaf node needs to be splited
Splitreturn* BPlusTree::split(Node* node)
{
    //increasing the level of tree by 1 in case of split node
    Splitreturn* res = new Splitreturn();
    vector<string> temp;
    
    Node* secondnode =  new Node(numofkeys);
   // cout<<node->keys.size()<<endl;
    for(int i =0; i < node->keys.size(); i++)
    {
        temp.push_back(node->keys[i]);
    }
    sort(temp.begin(), temp.end());
    
    string mid = temp[temp.size()/2];
    //erasing the previous keys from the node
    node->keys.erase(node->keys.begin(), node->keys.end());
    int i =0;
    while(temp[i] < mid && i < numofkeys)
    {
        node->keys.push_back(temp[i]);
        i++;
    }
    while( i < temp.size())
    {
        secondnode->keys.push_back(temp[i]);
        i++;
    }
    // add the new node to the linked list

    secondnode->next = node->next;
    if(node->next)
        node->next->prev = secondnode;
    secondnode->prev = node;
    node->next = secondnode;
    secondnode->leaf =true;
    
    if(!listhead)
        listhead=node;
    
    res->midval = mid;
    res->first = node;
    res->second = secondnode;
    
    return res;
}

// fixing the pointers after spliting the node
void BPlusTree::fixpointers(Node*& root, int& i, Splitreturn* retval)
{
    vector<Node*> copy = root->pointers;
    root->pointers.erase(root->pointers.begin(), root->pointers.end());
    //root->pointers[i] = retval->first;
    //root->pointers[i+1] = retval->second;
    int j;
    for(j = 0; j <= copy.size(); j++)
    {
        if( j == i)
        {
            root->pointers.push_back(retval->first);
        }
        else if(j == i+1)
        {
            root->pointers.push_back(retval->second);
        }
        else if(j < i)
        {
            root->pointers.push_back(copy[j]);
        }
        else
        {
            root->pointers.push_back(copy[j-1]);
        }
    }
}
void BPlusTree::insert_to_BTree(string item)
{
    Node* root = Treeroot;
   if(search(item, root, false, "", false))
    {
        cout<<"item "<<item <<" already exist"<<endl;
        return;
    }
    Splitreturn*retval =  insert( item, root);
   
    num_of_records++;
    if(!retval)
    {
        if(!Treeroot)
        {
            Treeroot = root;
            listhead = Treeroot;
        }
        return;
    }
    Node* newroot = new Node(numofkeys);
    newroot->keys.push_back(retval->midval);
    newroot->pointers.push_back(retval->first);
    newroot->pointers.push_back(retval->second);
    Treeroot = newroot;
    //level is increased when a new node is created at the top
    level++;
    
}

Splitreturn* BPlusTree::insert(string item, Node* & root)
{
    if(!root)
    {
        root = new Node(numofkeys);
        root->keys.push_back(item);
        root->leaf = true;
        return NULL;
    }
    // if there is a leaf we need to insert it here
    else if(root->leaf)
    {
        if(root->keys.size() < numofkeys)
        {
            root->keys.push_back(item);
            sort(root->keys.begin(), root->keys.end());
            return NULL;
        }
        else
        {
            // split the leaf and return the pointers and mid values to the parent
            root->keys.push_back(item);
            Splitreturn* retval = split(root);
            return retval;
        }
    }
    Splitreturn* retval = NULL;
    int i;
    for(i=0; i < root->keys.size() ; i++)
    {
        if(item > root->keys[i])
        {
            if(i == root->keys.size()-1)
            {
                retval = insert(item, root->pointers[i+1]);
                break;
            }
        }
        else
        {
            retval = insert(item, root->pointers[i]);
            break;
        }
    }
    if(!retval)
        return NULL;
    root->keys.push_back(retval->midval);
    sort(root->keys.begin(), root->keys.end());

    for(i=0; i< root->keys.size(); i++)
    {
        if(root->keys[i] == retval->midval)
        {
            break;
        }
    }
    //fix the pointers after inserting the new item to the current node from
    //subtree by shifting them
    if(i < root->pointers.size())
        fixpointers(root, i, retval);
    if(root->keys.size() <= numofkeys)
    {
        return NULL;
    }
    return midnode_split(root);
}

bool BPlusTree::search(string item, Node* root, bool UPDATE, string data, bool SEARCH)
{
    
    if(!root)
    {
        return false;
    }
    for(int i=0; i < root->keys.size() ; i++)
    {
        if(item == root->keys[i])
        {
            // it means we found the record and we want to update it
            if(UPDATE && root->leaf)
            {
                root->data[item]=data;
                return true;
            }
            else if(root->leaf)
            {
                if(SEARCH)
                {
                    if(root->data.find(item) == root->data.end())
                        cout<<"there is no confidential data for item "<<item<<endl;
                    else
                        cout<<"the confidential data for item "<<item<<" is "<<root->data[item]<<endl;
                }
                return true;
            }
            //it means that we need to go to the subtree that item may exist in
            return search(item, root->pointers[i+1], UPDATE, data,SEARCH);
        }
        else if(item > root->keys[i])
        {
            if(!root->leaf && (i == root->keys.size()-1))
            {
                return search(item, root->pointers[i+1], UPDATE, data,SEARCH);
            }
        }
        else
        {
            if(!root->leaf)
                return search(item, root->pointers[i], UPDATE, data,SEARCH);
        }
    }
    return false;
}
//using search first the item is found and then the data is going to be added to the record
void BPlusTree::update(string item, string data)
{
     Node* root = Treeroot;
    if(!(search(item, root, true, data, false)))
        cout<<"record "<<item<<" does not exist in data base"<<endl;
    else
        cout<<"record "<<item<<" is updated"<<endl;
    
    return;
    
}


void BPlusTree::mydelete(string item, Node* root)
{
    if(!root)
    {
        return;
    }
    for(int i=0; i < root->keys.size() ; i++)
    {
        if(item == root->keys[i])
        {
            // it means we found the record and we want to delete it
            if(root->leaf)
            {
                root->keys.erase(root->keys.begin()+i);
                num_of_records--;
                return;
            }
            else
                mydelete(item, root->pointers[i+1]);
        }
        else if(item > root->keys[i])
        {
            if(!root->leaf && (i == root->keys.size()-1))
            {
                mydelete(item, root->pointers[i+1]);
            }
        }
        else
        {
            if(!root->leaf)
                mydelete(item, root->pointers[i]);
        }
    }
}
//listing all the elements in the leaves
void BPlusTree::list()
{
    Node* head = this->getheadlist();
    if(num_of_records == 0)
    {
        cout<<"data base is empty"<<endl;
        return;
    }
    while(head)
    {
        for(int i =0; i< head->keys.size(); i++)
        {
            if(head->keys[i] != "")
                cout<<head->keys[i]<<"  ";
        }
        cout<<endl;
        head = head->next;
    }
}
void BPlusTree::snapshot(Node* root)
{
    Node* head = listhead;
    Node* temp = listhead;
    if(num_of_records == 0)
    {
        cout<<"data base is empty"<<endl;
        return;
    }
    cout<<"first and last key of all the nodes in the tree :"<<endl;
    
    Node* myroot = Treeroot;
    //in order to print first and last key of every node including the inner nodes
    traverse(myroot);
    //to unmark the visited nodes
    unmark(myroot);
    /*while(temp->keys.empty())
    {
        temp = temp->next;
    }
    cout<<"first key in the data base is "<<temp->keys[0]<<endl;
    while(temp->next)
    {
        temp = temp->next;
    }
    while(temp->keys.empty())
        temp = temp->prev;
    cout<<"last key in the tree is "<<temp->keys[temp->keys.size()-1]<<endl;*/
   // Node* previous = NULL;
    cout<<"all the keys in the leaves of the tree:"<<endl;
    while(head)
    {
        for(int i =0; i< head->keys.size(); i++)
        {
            if(head->keys[i] != "")
                cout<<head->keys[i]<<"  ";
        }
        cout<<endl;
      //  previous = head;
        head = head->next;
        
    }
    
    cout<<"level of the tree is "<<this->level<<endl;
    cout<<"number of records in the data base is "<<num_of_records<<endl;
    cout<<"number of data blocks in data base is "<<ceil((double)(num_of_records/4.0))<<endl;
    
}
void BPlusTree::unmark(Node* root)
{
    if(!root)
    {
        return;
    }
    
    for(int i =0; i< root->keys.size(); i++)
    {
        root->marked=false;
        if(!root->pointers.empty())
            traverse(root->pointers[i]);
    }
    if(!root->pointers.empty())
        traverse(root->pointers[root->pointers.size()-1]);
}
//traversing all the nodes including midnodes of the tree
void BPlusTree::traverse(Node* root)
{
    if(!root)
    {
        return;
    }

    for(int i =0; i< root->keys.size(); i++)
    {
        
        if(!root->marked)
        {
            root->marked =true;
            int j=0;
            while(root->keys[j]=="")
            j++;
            
            if(j < root->keys.size())
                cout<<root->keys[j]<<"  ";
        
            int k=(int )root->keys.size()-1;
            while(root->keys[k]=="")
                k--;
            if(k >=0)
                cout<<root->keys[k]<<endl;
        }
        //cout<<root->keys[i]<<endl;
        if(!root->pointers.empty())
          traverse(root->pointers[i]);
        
    }
    if(!root->pointers.empty())
        traverse(root->pointers[root->pointers.size()-1]);
}

void callfunctions(string query, string name, string item, BPlusTree& tree)
{
    if(query == "INSERT")
    {
        tree.insert_to_BTree(item);
    }
    else if(query == "SEARCH")
    {
        Node* root= tree.getroot();
        if(tree.search(item, root, false, "",true))
            cout<<"item "<<item<< " was found"<<endl;
        else
            cout<<"item "<<item<< " does not exists"<<endl;
    }
    else if(query == "DELETE")
    {
        Node* root= tree.getroot();
        if(tree.search(item, root, false, "",false))
        {
            tree.mydelete(item, root);
            cout<<"item "<< item << " was deleted from the data base"<<endl;
            return;
        }
        cout<<"item "<< item << " does not exist"<<endl;
    }
    else if(query == "SNAPSHOT")
    {
        Node* root= tree.getroot();
        tree.snapshot(root);
    }
    else if(query == "UPDATE")
    {
        Node* root= tree.getroot();
        if(tree.search(name, root, true, item, false))
            cout<<"item "<<name<< " updated"<<endl;
        else
            cout<<"item "<<name<< " does not exist"<<endl;
    }
    else if(query == "LIST")
    {
        tree.list();
    }
    else
    {
        cout<<"query is not found, valid queries are *insert, *search, *delete, *snapshot, *update, *list, and exit"
        <<endl;
    }
    
}
//parsing the input file
void readfile()
{
    string input;
    string lastquery;
    //degree of B+ tree is 11 (can be changed) : num of keys+1
    BPlusTree tree(11);
    cout<<" please select queries from :*insert, *search, *delete, *snapshot, *update, *list and exit !"<<endl;
    while(getline(cin, input))
    {
        while(input =="")
        {
            cout<<"please enter valid queries"<<endl;
            getline(cin,input);
        }
        string updateditem ="";
        std::transform(input.begin(), input.end(), input.begin(), ::toupper);
        if(input  == "EXIT")
        {
            cout<<"Good Bye!"<<endl;
            return;
        }
        else if(input[0] == '*')
        {
            lastquery =input.substr(1,input.length()-1);
            if(lastquery=="SNAPSHOT"|| lastquery=="LIST")
                callfunctions(lastquery,"" ,"", tree);
        }
        else
        {
            if(lastquery == "UPDATE")
            {
                updateditem = input;
                getline(cin,input);
                while(input =="")
                {
                    cout<<"please enter valid queries"<<endl;
                    getline(cin,input);
                }
                callfunctions(lastquery, updateditem, input , tree);
            }
            else
                callfunctions(lastquery, "", input , tree);
        }
    }
}

int main(int argc, const char * argv[]) {
    
   /* BPlusTree tree(3);
    tree.insert_to_BTree("b");

    tree.insert_to_BTree("h");
    
    tree.insert_to_BTree("c");
    
    tree.insert_to_BTree("i");
    
    tree.insert_to_BTree("z");
    
    tree.insert_to_BTree("x");
   
    tree.insert_to_BTree("a");
    tree.snapshot();

    tree.insert_to_BTree("zeinab");
    Node* root= tree.getroot();
    cout<<tree.getlevel()<<endl;
    tree.insert_to_BTree("ebi");
    
    root= tree.getroot();
    tree.traverse(root);
    tree.insert_to_BTree("zeinab2");
     root= tree.getroot();
    tree.traverse(root);
    tree.insert_to_BTree("ebi2");
     root= tree.getroot();
    tree.traverse(root);
    cout<<tree.getlevel()<<endl;
    tree.insert_to_BTree("zeinab3");
     root= tree.getroot();
    tree.traverse(root);
    tree.insert_to_BTree("ebi3");
    root= tree.getroot();
    tree.traverse(root);
    tree.insert_to_BTree("zeinab4");
    cout<<tree.getlevel()<<endl;
    tree.insert_to_BTree("ebi4");
    root= tree.getroot();
    tree.traverse(root);
    tree.insert_to_BTree("zeinab5");
    tree.insert_to_BTree("ebi5");
    tree.insert_to_BTree("zeinab6");
    root= tree.getroot();
    tree.traverse(root);
    tree.insert_to_BTree("ebi6");
    tree.insert_to_BTree("ebi3");
    cout<<tree.search("zeinab4",root, false, "")<<endl;
    cout<<tree.search("zein",root, false, "")<<endl;
    cout<<tree.search("ebi",root, false, "")<<endl;
    cout<<tree.getlevel()<<endl;
    tree.update("zein","student");
    tree.update("zeinab6","student");
    cout<<tree.getnumrecords()<<endl;
    tree.snapshot();
    */
    readfile();
    return 0;
}
