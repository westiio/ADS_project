
/*task现在给定一个插入序列，您应该告诉生成的AVL树的根。
input 第一行包含一个正整数N(≤20)，这是要插入的键的总数。
然后在下一行给出N个不同的整数键。一行中的所有数字用一个空格隔开。
output 在一行中打印结果AVL树的根。
methods 结构体，真实建树*/

#include<stdio.h>
#include<stdlib.h>

//struct
typedef struct node* tree;
struct node{
	int data;
	int height;
	tree left;
	tree right;
};

//functions
int HeigtofNode(tree root);//迭代得到一棵树高

int max(int a,int b){
    return a > b ? a : b;
}
//左旋与右旋，在这个基础上得到LR与RL
//旋转后要更改被转的点（finder的某个child）的高度
tree RightRotate(tree root){
    tree temp = root->left;
    root->left = temp->right;
    temp->right = root;
    int hl = HeigtofNode(root->left);
    int hr = HeigtofNode(root->right);
    temp->height =  max(hl,hr) + 1;
    return temp;
}
tree LeftRotate(tree root){
    tree temp = root->right;
    root->right = temp->left;
    temp->left = root;
    int hl = HeigtofNode(root->left);
    int hr = HeigtofNode(root->right);
    temp->height =  max(hl,hr) + 1;
    return temp;
}

tree insert(tree root,tree tnode);//迭代地Insert
tree CreateNode(int);//返回一个新的节点

int main(){

		int n;
    int x;
    int i;
    tree tnode = NULL;
    tree root = NULL;

    scanf("%d",&n);
    for (i = 0; i < n; i++){
        scanf("%d",&x);
        tnode = CreateNode(x);
        root = insert(root,tnode);
    }

    printf("%d",root->data);
    return 0;
}
//递归着从下往上，直到找到第一个trouble finder时就执行旋转
tree insert(tree root,tree tnode)
{
    if (root == NULL)
        return tnode;//一直插到空树为止

    if (tnode->data < root->data)
    {
		    //递归地插入，要更改root的左边
        root->left = insert(root->left,tnode);
				//若root是finder
        if (HeigtofNode(root->left) - HeigtofNode(root->right) == 2)
        {
            //LL
            if (tnode->data < root->left->data)
                root = RightRotate(root);  
                //这里得到一个局部的root，最后作为child返回给上一级
            //LR就是一次左旋＋一次右旋
            else
            {
                root->left = LeftRotate(root->left);
                root = RightRotate(root);
            }
        }
        
    }
    else if (tnode->data > root->data)
    {
        root->right = insert(root->right,tnode);
        if (HeigtofNode(root->right) - HeigtofNode(root->left) == 2)
        {
            //RR
            if (tnode->data > root->right->data)
                root = LeftRotate(root);
            else
            //RL
            {
                root->right =RightRotate(root->right);
                root = LeftRotate(root);
            }
        }
    }
    
    //每次插入，都要更新一次高度。root的后继已经在一or两次旋转中更新了
    root->height = max(HeigtofNode(root->left),HeigtofNode(root->right)) + 1;
    return root;
}


tree CreateNode(int data)
{
    tree temp = (tree)malloc(sizeof(struct node));
    temp->left = NULL;
    temp->right = NULL;
    temp->height = 0;
    temp->data = data;
    return temp;
}

int HeigtofNode(tree root){
    if (root == NULL)
        return -1;
    else
    {
        int hl = HeigtofNode(root->left);
        int hr = HeigtofNode(root->right);
        return max(hl,hr) + 1;
    
    }
}