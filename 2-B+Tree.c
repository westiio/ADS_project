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
����print��searchд���ˣ���ʣ�����split�Ĳ���*/

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
//ascending order for 3(������3��ֵ������3�����������
//ascending order for 4�������4��ֵ��



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

//����һ������һ���ݹ�Ĺ���
tree Insert(tree root,int key){
    //insert
    if (root == NULL)
    {
       tree firstleaf = Creatleaf();
       firstleaf->key[0] = key;
       return firstleaf;
    }
    //����ǰ��׼��
    top = 0;
    if (!root->leaf)
        push(root);
    tree node = Search(root,key);
    if (node == NULL)
        return root;
        
    //�ܲ�����ͳһ���룬Ȼ����split����stack����ȥֵ
    //���⣺��θ���������ֵ?
   
    //��Ҫsplit�����-��ι�������ֵ����Ҫ�ı�leafֵ���ǵݹ��
    else
    {
        //�ں���λ�ò���,�ò�������
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
        //��������׶ε�һ������3�������ģ������Ѿ��ź���
        tree newnode = Creatnode();
        node->n = newnode->n = 1;
        newnode->key[0] = node->key[2];
        newnode->child[0] = node->child[2];
        newnode->child[1] = node->child[3]; 
        node->child[2]=NULL; node->child[3]=NULL;
        //�ڶ���ֵ�ӵ����ף�Ҫ���Եݹ������
        index = node->key[1];
        nextnode = newnode;
        
    }
    //ֻ�е�һ�ο�����Ҷ�ӽ��
    else 
    {
        tree newleaf= Creatleaf();
        //�Ѿ��ź�����
        node->n = newleaf->n = 2;
        newleaf->key[0] = node->key[2];
        newleaf->key[1] = node->key[3];
        index = newleaf->key[0];
        nextnode = newleaf;
    }
   //�õ�index����������nextnode 
    //����Ѿ��Ƶ�root�����½����ұ�ֻ������
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
        //����������nextnode����ԭnode���ұߣ��������Ա�֤child��������û�����
        //��������ջ����parent������ź�
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
    //search,��ջ��ʾÿ�ε�·��
    if (root == NULL) 
        return NULL;
    if (!root->leaf)
    {
        //�ڷ�leaf���ҵ���
        for (int i = 0; i <root->n; i++)
        {
            if (key == root->key[i])
            {
                printf("Key %d is duplicated\n",key);
                return NULL;
            }
            //���Ա�֤�����������
            else if (key < root->key[i])
            {
                chnum[top-1] = i;
                if (!root->child[i]->leaf)
                    push(root->child[i]);
                return Search(root->child[i],key);
            }
        }
        //���key��������������
        chnum[top-1] = root->n;
        if (!root->child[root->n]->leaf)
            push(root->child[root->n]);
        return Search(root->child[root->n],key);
    }
    else if (root->leaf)
    {
        //��leaf���ҵ���
        for (int i = 0; i < root->n; i++)
        {
        
            if (root->key[i] == key)
            {
                printf("Key %d is duplicated\n",key);
                return NULL;//�������
            }
        }
        //����leaf����û�ҵ�
        return root;//������Ľڵ�
    }
}

tree Print(tree root){
    if (root == NULL)
        return NULL; 

    //����ʲô����Ҫ���
    printf("[");    
    for (int i = 0; i < root->n-1; i++)
        printf("%d,",root->key[i]);
    printf("%d]",root->key[root->n-1]);
    //��������һ��child
    number--;
    
    //�ǽڵ�����Ҫ��child
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


