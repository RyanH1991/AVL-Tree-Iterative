#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <queue> 
#include <ostream>
using namespace std;

struct node{
    int data;
    struct node* left;
    struct node* right;

    struct node* parent;

    int height;
};

struct node* newNode(int data) 
{  
  struct node* node = (struct node*)malloc(sizeof(struct node)); 
  node->data = data; 
  node->left = NULL; 
  node->right = NULL; 

  node->parent = NULL;

  node->height = 0;
  return(node); 
}

struct binarySearchTree{
    struct node *root = newNode(-1);
    binarySearchTree();
    int traverseCounter = 0;
    void insertIter(int value, node* root){
        int height = 0;
        struct node *cream = newNode(value);
        //case where nothing is in BST
        if(root->data == -1){
            root->data = value;
            root->height = height;
            return;
        }
        //cases where value is lesser or greater than root
        node* parent = NULL;
        node* current = root;
        while(current!=NULL){
            parent = current;
            height ++;
            if(current->data < value){
                current = current->right;
                traverseCounter++;
            }
            else{
                current = current->left;
                traverseCounter++;
            }
        }
        if(parent->data < value){
            parent->right = cream;
            cream->height = height;
            cream->parent = parent;
        }
        else{
            parent->left = cream;
            cream->height = height;
            cream->parent = parent;
        }
        //now do necessary rotations all the way up the tree
        node* temp = cream;
        adjustHeight(temp);
        while(temp != NULL){
            
            rotateIter(temp);
            adjustHeight(temp);
            if(temp->parent == NULL){
                this->root = temp;
            }
            temp = temp->parent;
        }

    }

    void deleteIter(int value, node* root){
        //Now I modify this to handle rebalancing
        node* parent = NULL;
        //node* current = root;
        //iterate the the node to be deleted
        while(root->data != value){
            parent = root;
            if(root->data < value){
                root = root->right;
                traverseCounter++;
            }
            else{
                root = root->left;
                traverseCounter++;
            }
        }
    
        if(root->data == value){
            //3 cases
            //leaf case
            if(root->left == NULL && root->right == NULL){
                //if this is root node
                if(root->parent == NULL){
                    root = newNode(-1);
                }
                //not root node
                else if(root->parent != NULL){
                    if(root->parent->data > root->data){
                        root->parent->left = NULL;
                    }
                    if(root->parent->data < root->data){
                        root->parent->right = NULL;
                    }
                }
                //do rotations
                node* temp = root->parent;
                adjustHeight(temp);
                while(temp != NULL){
            
                    rotateIter(temp);
                    adjustHeight(temp);
                    if(temp->parent == NULL){
                        this->root = temp;
                    }
                    temp = temp->parent;
                }
            }
            //one child case
            else if(root->left != NULL && root->right == NULL){
                root->left->parent = root->parent;
                if(root->parent->data > root->data){
                    root->parent->left = root->left;
                }
                if(root->parent->data < root->data){
                    root->parent->right = root->left;
                }
                //rotations
                node* temp = root->left;
                adjustHeight(temp);
                while(temp != NULL){
            
                    rotateIter(temp);
                    adjustHeight(temp);
                    if(temp->parent == NULL){
                        this->root = temp;
                    }
                    temp = temp->parent;
                }
            }
            else if(root->left == NULL && root->right != NULL){
                root->right->parent = root->parent;
                if(root->parent->data > root->data){
                    root->parent->left = root->right;
                }
                if(root->parent->data < root->data){
                    root->parent->right = root->right;
                }
                //rotations
                node* temp = root->right;
                adjustHeight(temp);
                while(temp != NULL){
            
                    rotateIter(temp);
                    adjustHeight(temp);
                    if(temp->parent == NULL){
                        this->root = temp;
                    }
                    temp = temp->parent;
                }
            }
            //two children case
            else if(root->left != NULL && root->right != NULL){
                int successor = findMinIter(root->right);
                //remove this recursive!
                //deleteIter(successor, root->right);
                root->data = successor;
                root = root->right;
                while(root->data != successor ){
                    root->left->parent = root;
                    root = root->left;
                }
                root->parent->left = NULL;
                //rotations
                node* temp = root->parent;
                adjustHeight(temp);
                while(temp != NULL){
            
                    rotateIter(temp);
                    adjustHeight(temp);
                    if(temp->parent == NULL){
                        this->root = temp;
                    }
                    temp = temp->parent;
                }
            }
            return;
        }
        /*else if(value < root->data){
            deleteRec(value, root->left);
        }
        else if(value > root->data){
            deleteRec(value, root->right);
        }*/
        return;
    }

    int findNextIter(node* root){
        int next;
        //ignore edge case where there is no next (no right child and no greater parent)
        //case with right child
        if(root->right != NULL){
            next = findMinIter(root->right);
        }
        //case with no right child, but greater parent exists
        else if(root->right == NULL && (root->parent->data > root->data)){
            next = root->parent->data;
        }
        return next;
    }

    int findPrevIter(node* root){
        int previous;
        //ignore edge case where there is no next (no left child and no lesser parent)
        //case with left child
        if(root->left != NULL){
            previous = findMaxIter(root->left);
        }
        //case with no left child, but lesser parent exists
        else if(root->left == NULL && (root->parent->data < root->data)){
            previous = root->parent->data;
        }
        return previous;
    }

    int findMinIter(node* root){
        while(root->left!=NULL){
            root=root->left;
            traverseCounter++;
        }
        return root->data;
    }
    int findMaxIter(node* root){
        while(root->right!=NULL){
            root=root->right;
            traverseCounter++;
        }
        return root->data;
    }

    void fromArray(int* array, int size, node* root){
        for(int i = 0; i < size; i++){
            insertIter(array[i], this->root);
        }
    }

    void rotateIter(node* cream){
        //first find heights of left and right subtrees, so i can find BF
        int BF = getBF(cream);
        //BF determined, now determine what rotations this demands
        if (BF == 2){
            //left-skewed, right-rotation needed
            //now determine BF of left child
            int BFL = getBF(cream->left);
            if(BFL == -1){
                //right skew, must rotate cream->left left
                rotateLeft(cream->left);
                //must adjust heights
            }
            rotateRight(cream);
            //must adjust heights
        }
        else if (BF == -2){
            //right-skewed, left-rotation needed
            //now determine BF of right child
            int BFR = getBF(cream->right);
            if(BFR == 1){
                //left skew, must rotate cream->right right
                rotateRight(cream->right);
            }
            rotateLeft(cream);
        }
        //try some shit
    }

    void rotateLeft (node *honey){
        node* temp = honey->right->left;
        honey->right->left = honey;
        honey->right->parent = honey->parent;
        honey->parent = honey->right;
        honey->right = temp;
        if(honey->parent->parent!=NULL){
            honey->parent->parent->right=honey->parent;
        }
    }
    void rotateRight (node *honey){
        node* temp = honey->left->right;
        honey->left->right = honey;
        honey->left->parent = honey->parent;
        honey->parent = honey->left;
        honey->left = temp;
        if(honey->parent->parent!=NULL){
            honey->parent->parent->left=honey->parent;
        }
    }

    int getBF (node *bunk){
        //first find heights of left and right subtrees, so i can find BF
        int leftHeight = 0;
        int rightHeight = 0;
        if(bunk->left!=NULL){
            leftHeight = (bunk->left->height + 1);
        }
        if(bunk->right!=NULL){
            rightHeight = (bunk->right->height + 1);
        }
        int BF = leftHeight - rightHeight;
        return BF;
    }
    void adjustHeight(node* poop){
        //utilizing https://www.geeksforgeeks.org/iterative-method-to-find-height-of-binary-tree/
        queue<node *> horn;
        horn.push(poop);
        traverseCounter++;
        int height = -1;
        while (1){
            int nodecount = horn.size();
            if (nodecount == 0){ 
                poop->height = height; 
                return;
            }
            height++;

            while(nodecount>0){
                node *node = horn.front();
                horn.pop();
                if (node->left!=NULL){
                    horn.push(node->left);
                    traverseCounter++;
                }
                if(node->right!=NULL){
                    horn.push(node->right);
                    traverseCounter++;
                }
                nodecount--;
            }
        } 
    }
};

binarySearchTree :: binarySearchTree(){
    //root = NULL;
}

int main(){
    binarySearchTree Poopy;
    /*int inputArray[7] = {1,2,3,4,5,6,7};
    Poopy.fromArray(inputArray,7,Poopy.root);
    //test avl functionality
    cout << "check AVL insert works" << endl;
    cout << Poopy.root->data << endl;
    //cout << Poopy.root->height << endl;
    cout << Poopy.root->left->data << endl;
    //cout << Poopy.root->left->height << endl;
    cout << Poopy.root->right->data << endl;
    //cout << Poopy.root->right->height << endl;
    cout << Poopy.root->left->left->data << endl;
    //cout << Poopy.root->left->left->height << endl;
    cout << Poopy.root->left->right->data << endl;
    //cout << Poopy.root->left->right->height << endl;
    cout << Poopy.root->right->left->data << endl;
    //cout << Poopy.root->right->left->height << endl;
    cout << Poopy.root->right->right->data << endl;
    //cout << Poopy.root->right->right->height << endl;

    //now test delete
    Poopy.deleteIter(4,Poopy.root);
    cout << "check delete 4 works" << endl;
    cout << Poopy.root->data << endl;
    //cout << Poopy.root->height << endl;
    cout << Poopy.root->left->data << endl;
    //cout << Poopy.root->left->height << endl;
    cout << Poopy.root->right->data << endl;
    //cout << Poopy.root->right->height << endl;
    cout << Poopy.root->left->left->data << endl;
    //cout << Poopy.root->left->left->height << endl;
    cout << Poopy.root->left->right->data << endl;
    //cout << Poopy.root->left->right->height << endl;
    //cout << Poopy.root->right->left->data << endl;
    //cout << Poopy.root->right->left->height << endl;
    cout << Poopy.root->right->right->data << endl;
    //cout << Poopy.root->right->right->height << endl;
    Poopy.deleteIter(7,Poopy.root);
    cout << "check delete 7 works" << endl;
    cout << Poopy.root->data << endl;
    //cout << Poopy.root->height << endl;
    cout << Poopy.root->left->data << endl;
    //cout << Poopy.root->left->height << endl;
    cout << Poopy.root->right->data << endl;
    //cout << Poopy.root->right->height << endl;
    cout << Poopy.root->left->left->data << endl;
    //cout << Poopy.root->left->left->height << endl;
    cout << Poopy.root->left->right->data << endl;
    //cout << Poopy.root->left->right->height << endl;
    //cout << Poopy.root->right->left->data << endl;
    //cout << Poopy.root->right->left->height << endl;
    //cout << Poopy.root->right->right->data << endl;
    //cout << Poopy.root->right->right->height << endl;
    Poopy.deleteIter(6,Poopy.root);
    cout << "check delete 6 works" << endl;
    cout << Poopy.root->data << endl;
    //cout << Poopy.root->height << endl;
    cout << Poopy.root->left->data << endl;
    //cout << Poopy.root->left->height << endl;
    cout << Poopy.root->right->data << endl;
    //cout << Poopy.root->right->height << endl;
    //cout << Poopy.root->left->left->data << endl;
    //cout << Poopy.root->left->left->height << endl;
    //cout << Poopy.root->left->right->data << endl;
    cout << Poopy.root->right->left->data << endl;
    */

    /*
    int inputArray[3] = {1,2,3};
    Poopy.fromArray(inputArray,3,Poopy.root);
    cout << Poopy.root->data << endl;
    cout << Poopy.root->height << endl;
    cout << Poopy.root->right->data << endl;
    cout << Poopy.root->right->height << endl;
    cout << Poopy.root->left->data << endl;
    cout << Poopy.root->left->height << endl;
    */
    
    /*int inputArray[7] = {5,3,7,2,4,6,8};
    Poopy.fromArray(inputArray, 7, Poopy.root);
    cout << Poopy.root->height << endl;
    cout << Poopy.root->right->height << endl;
    cout << Poopy.root->left->height << endl;
    cout << Poopy.root->right->right->height << endl;
    Poopy.adjustHeight(Poopy.root);
    cout << Poopy.root->height << endl;
    cout << Poopy.root->right->height << endl;
    cout << Poopy.root->left->height << endl;
    cout << Poopy.root->right->right->height << endl;*/
    int inputArray[15];
    ifstream File;
    File.open("SortedArrayFile.txt");

    int count;
    File >> count;

    for(int a = 0; a < 15; a++){
        File >> inputArray[a];
    }
    /*for(int a = 0; a < 10; a++){
        cout << inputArray[a] << endl;;
    }*/
    Poopy.fromArray(inputArray, 15, Poopy.root);
    //cout << Poopy.root->data << endl;
    //cout << Poopy.root->right->data << endl;
    //cout << Poopy.root->left->data << endl;
    cout << Poopy.traverseCounter << endl;
    return 0;
}