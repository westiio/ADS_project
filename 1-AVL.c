
/*task���ڸ���һ���������У���Ӧ�ø������ɵ�AVL���ĸ���
input ��һ�а���һ��������N(��20)������Ҫ����ļ���������
Ȼ������һ�и���N����ͬ����������һ���е�����������һ���ո������
output ��һ���д�ӡ���AVL���ĸ���
methods �ṹ�壬��ʵ����*/

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
int HeigtofNode(tree root);//�����õ�һ������

int max(int a,int b){
    return a > b ? a : b;
}
//����������������������ϵõ�LR��RL
//��ת��Ҫ���ı�ת�ĵ㣨finder��ĳ��child���ĸ߶�
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

tree insert(tree root,tree tnode);//������Insert
tree CreateNode(int);//����һ���µĽڵ�

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
//�ݹ��Ŵ������ϣ�ֱ���ҵ���һ��trouble finderʱ��ִ����ת
tree insert(tree root,tree tnode)
{
    if (root == NULL)
        return tnode;//һֱ�嵽����Ϊֹ

    if (tnode->data < root->data)
    {
		    //�ݹ�ز��룬Ҫ����root�����
        root->left = insert(root->left,tnode);
				//��root��finder
        if (HeigtofNode(root->left) - HeigtofNode(root->right) == 2)
        {
            //LL
            if (tnode->data < root->left->data)
                root = RightRotate(root);  
                //����õ�һ���ֲ���root�������Ϊchild���ظ���һ��
            //LR����һ��������һ������
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
    
    //ÿ�β��룬��Ҫ����һ�θ߶ȡ�root�ĺ���Ѿ���һor������ת�и�����
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