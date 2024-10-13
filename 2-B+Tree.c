#include <stdio.h>
#include<stdlib.h>

/*backgroud
task 3 order B+ tree:
    initialize, insert (with splitting) and search
    print out itself.
    root has 2-M=3 children,nonleaf node hase2-3 children
    every leaf node has 2-3 keys
frame
input N<=10^4m,positive
output Key X is duplicated(during the insert)
top down level order traversal of the B+ tree
methods
现在print和search写完了，还剩插入和split的部分*/

//struct
typedef struct node* tree;
struct node{
    int n;//number of keys,navigator or key 
    int leaf;//1 if leaf,0 if not
    int key[4];
    tree child[4];//
};
//variebles
int number;
int nextnumber;
int front = 0,rear = 0;
tree queue[10000];
void enqueue(tree node){
    queue[rear++] = node;
}
tree dequeue(){
    return queue[front++];
}

int top =0;
tree stack[10000];
int chnum[10000];
void push(tree node){
    stack[top++] = node;
}
tree pop(){
    return stack[--top];
}
//functions
tree Insert(tree root,int key);
tree Search(tree root,int key);
tree Print(tree root);
tree Creatleaf()
{
    tree temp = (tree)malloc(sizeof(struct node));
    temp->n = 1;
    temp->leaf = 1;
    temp->child[0] = NULL;
    temp->child[1] = NULL;
    temp->child[2] = NULL;
    temp->child[3] = NULL;
    temp->key[0] = temp->key[1] = temp->key[2] = temp->key[3] = 0;
    return temp;
}
tree Creatnode()
{
    tree temp = (tree)malloc(sizeof(struct node));
    temp->n = 1;
    temp->leaf = 0;
    temp->child[0] = NULL;
    temp->child[1] = NULL;
    temp->child[2] = NULL;
    temp->child[3] = NULL;
    temp->key[0] = temp->key[1] = temp->key[2] = temp->key[3] = 0;
    return temp;
}
//ascending order for 3(正常的3个值，或者3个溢出的索引
//ascending order for 4（溢出的4个值）



tree split(tree node);

int main(){
    int n;
    tree root = NULL;
    scanf("%d",&n);

    for (int i = 0; i < n; i++)
    {
        int key;
        scanf("%d",&key);
        root = Insert(root,key);
        front=rear=0;
    }   
    number = 1;
    nextnumber = 0;
    Print(root); 
    return 0;
}

//插入一定会是一个递归的过程
tree Insert(tree root,int key){
    //insert
    if (root == NULL)
    {
       tree firstleaf = Creatleaf();
       firstleaf->key[0] = key;
       return firstleaf;
    }
    //搜索前的准备
    top = 0;
    if (!root->leaf)
        push(root);
    tree node = Search(root,key);
    if (node == NULL)
        return root;
        
    //能不能先统一插入，然后再split，从stack里面去值
    //问题：如何更新索引的值?
   
    //需要split的情况-如何管理索引值，还要改变leaf值，是递归的
    else
    {
        //在合适位置插入,用插入排序
        node->n++;
        int i;
        for (i=node->n-1;i>0&&node->key[i-1]>key;i--)
            node->key[i]=node->key[i-1];   
        node->key[i]= key;
        if (node->n == 4 )
        {
            tree newnode = split(node);
            if (newnode)
                root = newnode;
        }
    }



    return root;
}   

tree split(tree node){

    int index;
    tree nextnode;

    /*printf("tobe split:leaf=%d,element=",node->leaf);
    for (int i=0;i<node->n;i++)
        printf("%d ",node->key[i]);
    printf("\n");
*/
    if (!node->leaf)
    {
        //进入这个阶段的一定是有3个索引的，而且已经排好了
        tree newnode = Creatnode();
        node->n = newnode->n = 1;
        newnode->key[0] = node->key[2];
        newnode->child[0] = node->child[2];
        newnode->child[1] = node->child[3]; 
        node->child[2]=NULL; node->child[3]=NULL;
        //第二个值加到父亲，要可以递归的往上
        index = node->key[1];
        nextnode = newnode;
        
    }
    //只有第一次可能是叶子结点
    else 
    {
        tree newleaf= Creatleaf();
        //已经排好序了
        node->n = newleaf->n = 2;
        newleaf->key[0] = node->key[2];
        newleaf->key[1] = node->key[3];
        index = newleaf->key[0];
        nextnode = newleaf;
    }
   //得到index：索引，和nextnode 
    //如果已经推到root，则新建，且必只有两个
    if (top == 0)
    {
        tree newroot = Creatnode();
        newroot->key[0] = index;
        newroot->child[0] = node;
        newroot->child[1] = nextnode;
        return newroot;
    }
    else
    {
        
        tree parent = pop();
        /*for (int i = 0;i<parent->n;i++)
            printf("%d,",parent->key[i]);
        printf("\n");*/
        ++(parent->n);
        //不管怎样，nextnode都在原node的右边，这样可以保证child的排序是没问题的
        //这里利用栈来把parent里的序排好
        for (int i = parent->n-1; i > chnum[top]; i--)
        {

            parent->child[i+1] = parent->child[i];
            parent->key[i] = parent->key[i-1];
        }
        parent->child[chnum[top]+1] = nextnode;
        parent->key[chnum[top]] = index;
        if (parent->n == 3)
            return split(parent);
    }
    return NULL;
}
tree Search(tree root,int key){
    //search,用栈表示每次的路径
    if (root == NULL) 
        return NULL;
    if (!root->leaf)
    {
        //在非leaf里找到了
        for (int i = 0; i <root->n; i++)
        {
            if (key == root->key[i])
            {
                printf("Key %d is duplicated\n",key);
                return NULL;
            }
            //可以保证尽量往左边走
            else if (key < root->key[i])
            {
                chnum[top-1] = i;
                if (!root->child[i]->leaf)
                    push(root->child[i]);
                return Search(root->child[i],key);
            }
        }
        //如果key比所有索引都大
        chnum[top-1] = root->n;
        if (!root->child[root->n]->leaf)
            push(root->child[root->n]);
        return Search(root->child[root->n],key);
    }
    else if (root->leaf)
    {
        //在leaf里找到了
        for (int i = 0; i < root->n; i++)
        {
        
            if (root->key[i] == key)
            {
                printf("Key %d is duplicated\n",key);
                return NULL;//无需插入
            }
        }
        //到了leaf但是没找到
        return root;//待插入的节点
    }
}

tree Print(tree root){
    if (root == NULL)
        return NULL; 

    //无论什么都需要输出
    printf("[");    
    for (int i = 0; i < root->n-1; i++)
        printf("%d,",root->key[i]);
    printf("%d]",root->key[root->n-1]);
    //如果是最后一个child
    number--;
    
    //是节点则需要塞child
    if (!root->leaf)
    {
        nextnumber+=root->n+1;
        for (int i = 0; i <= root->n; i++)
            enqueue(root->child[i]);
    }
   if (number ==0)
   {
        number = nextnumber;
        printf("\n");
        nextnumber= 0;
   } 
    

    while (front < rear)
        Print(dequeue());
}


