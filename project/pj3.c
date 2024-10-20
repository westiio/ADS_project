/*10.16
ʵ�ٺ��֣���һ���������쵽һ���ڵ�֮����һ���ڵ��Դ�����
����connector������һ�� expdir����¼�Ѿ�����ķ���
������ʵ�����4��3��2��1 ������ͬ ��������֪�ģ�Ҳ��ͬ
unvisit--�������ٿ�һ��
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
//#define DEBUG

#define MAX 50 
#define CONNECT 1
#define BLANK 0
#define FENCE 2

#define UP 0
#define Right 1
#define DOWN 2
#define LEFT 3

//��Ҫ�޸ĳ�ʼ�������
typedef struct garden* Garden;
typedef struct connector* Connector; 
struct garden {
    int height, width; // ��԰�ĸ߶ȡ����
    int unvisited; // δ���ʵ�����������
    int fence[MAX][MAX]; // դ���Ķ�ά���飬0��ʾ�գ�1��ʾ��������2��ʾˮƽΧ����3��ʾ��ֱΧ��
    Connector head,tail;
    Connector con[MAX][MAX];//�������Ķ�ά����
};

struct connector{
    int x;
    int y;
    int junction; // �������Ķ���
    //0-3�ֱ������������
    int dir[4];
    int neighCount;
    int found;
    Connector neighbor[4];
    Connector next;

    //��¼���ڻ��ж��ٸ�û���ϡ�
}; 

// �������飺��������, �ͼ�¼·��������ʹ��
int dx[] = {-1, 0, 1, 0};
int dy[] = {0,  1, 0,-1};

//�Ƿ��ڻ�԰��
bool InGarden(int x, int y,Garden garden) {
    if (x >= 0 && x < garden->height && y >= 0 && y < garden->width)
        return true;
    return false;
}
//���˳����ж��Ƿ����е����������Ѿ�����
bool AllConnected(Garden garden) {
    if (garden->unvisited==0)
        return true;
}
//�ڳ���֮ǰ����֮����£�1������-1�Ǽ�
void UpdateUnvisit(Garden garden,Connector temp,int operation) {
    if (temp->junction==0)
        garden->unvisited+=operation;
}
void PrintStatus(Connector temp) {
    printf("  x=%d y=%d \n  junction=%d ",temp->x,temp->y,temp->junction);
    printf("UP=%d Down=%d left=%d right=%d\n",temp->dir[0],temp->dir[2],temp->dir[3],temp->dir[1]);
}
void PrintConnector(Connector temp) {
    printf("%d %d ",temp->x+1,temp->y+1);
    printf("%d %d %d %d\n",temp->dir[0],temp->dir[2],temp->dir[3],temp->dir[1]);
}

// ��ӡ������
void Print(Garden garden) {
    Connector temp=garden->head->next ;
    while (temp) {
        PrintConnector(temp);
        temp=temp->next;
    }
}
Connector InitConnector(int x,int y,int junction) {
    Connector node = (Connector)malloc(sizeof(struct connector));
    node->x = x;
    node->y = y;
    for (int i=0;i<4;i++){
        node->dir[i]=0;
        node->neighbor[i]=NULL;
    }
    node->junction = junction;
    node->neighCount=0;
    node->found=0;
    node->next=NULL;
    return node;
}
// ��ʼ����԰
//fence��¼Χ���������
Garden InitGarden() {
    int junction;
    Garden garden = (Garden)malloc(sizeof(struct garden));
    garden->head=InitConnector(-1,-1,0);
    garden->tail=garden->head;
    garden->unvisited=0;
    scanf("%d %d", &garden->height, &garden->width);
    for (int i = 0; i < garden->height; i++) {
        for (int j = 0; j < garden->width; j++) {
            scanf("%d", &junction);
            if (junction == 0) //not connector
            {
                garden->fence[i][j] = BLANK;
                garden->con[i][j]=NULL;
            }
            //��������
            else{
                garden->fence[i][j] = CONNECT;
                garden->con[i][j]=InitConnector(i,j,junction);
                garden->unvisited++;
                garden->tail->next=garden->con[i][j];
                garden->tail=garden->con[i][j];
                /*
                int state=PreProcessConnect(garden->con[i][j]);
                if (state==-1)
                    return NULL;
                if (state==1){
                    for (int i=0;i<4;i++){
                        if (garden->con[i][j]->neighbor[i])
                            link(garden,garden->con[i][j],i);
                    }
        
                }
                */
            }
        }

    }
    return garden;
}
//�ҵ���һ��û����������
Connector FindNext(Garden garden) {
    Connector temp=garden->head->next;
    //�ҵ���һ����0��������
    while (temp&&temp->junction==0)
        temp = temp->next;
    return temp;
}

bool ConAvailable(Connector temp,Connector neighbor,int dir) {
    //���ܴ�������NULL
    if (!neighbor)
        return false;
    if (temp->dir[dir]==0&&neighbor->dir[(dir+2)%4]==0&&neighbor->junction>0)
        return true;
    return false;
}    

//�ҵ�����������ھ�,����Ѿ����ӹ��˾Ͳ�����
//���ڻ��ݻᰴ�� 3 2 1 0��˳��������ν���������
Connector FindNeighbor(Garden garden,Connector temp,int dir)
{
    //�������ڵ��Ѿ��ҹ��ˣ�ֱ�ӷ���
    if (temp->found)
        return temp->neighbor[dir];
    //��Ԥ����ʱ����neighbor��Ҫ����ṹ�������С����������ռ�ã��Ͳ��ü���
    int nx = temp->x + dx[dir]; // ��һ��λ�õ�����
    int ny = temp->y + dy[dir];
    while (InGarden(nx, ny,garden) && garden->fence[nx][ny] == BLANK) {
        nx += dx[dir];
        ny += dy[dir];
    }
    //���neighbor����������������֤��ȡ����ʱ��neighbor�����в�ΪNULL
    if (InGarden(nx, ny,garden) && garden->fence[nx][ny] == CONNECT ) {
        #ifdef DEBUG
        printf("dir = %d,neighbor x=%d y=%d\n",dir,nx,ny);
        #endif
        return garden->con[nx][ny];
    }
    #ifdef DEBUG
    printf("dir%d,no neighbor\n",dir);
    #endif
    return NULL;
}


void link(Garden garden,Connector temp,int dir)
{
    Connector neighbor = temp->neighbor[dir];
    int x = neighbor->x;
    int y = neighbor->y;
    //printf("Linking from (%d, %d) to (%d, %d) in direction %d\n", temp->x, temp->y, x, y, dir);
    temp->dir[dir]=1;
    temp->junction--;
    UpdateUnvisit(garden,neighbor,-1);
    //ֱ�Ӹĵ����ָ��
    neighbor->dir[(dir+2)%4]=1;
    neighbor->junction--;
    //����Χ�����������������ø���
    for(int xi=temp->x+dx[dir], yi=temp->y+dy[dir];xi!=x||yi!=y;xi+=dx[dir],yi+=dy[dir]){
        garden->fence[xi][yi]=FENCE;
    }
}
//Cancel��link�����øı�neigborcount����Ϊÿ���ڵ㱻�ٴη���ʱ�����¼���
void Cancel(Garden garden, Connector temp,int dir) {
    // ���ݣ�����֮ǰ��Χ��
    Connector neighbor = temp->neighbor[dir];
    if (neighbor==NULL){
        printf("error,neighbor is NULL\n");
        return;
    }
    int x = neighbor->x;
    int y = neighbor->y;
    temp->dir[dir]=0;
    UpdateUnvisit(garden,temp->neighbor[dir],1);
    temp->junction++;
    neighbor->dir[(dir+2)%4]=0;
    neighbor->junction++;
    for(int xi=temp->x+dx[dir], yi=temp->y+dy[dir];xi!=x||yi!=y;xi+=dx[dir],yi+=dy[dir]){
        garden->fence[xi][yi]=BLANK;
    }
}
int PreProcessConnect(Garden garden,Connector temp) {
    //���ܻ���PreProcessConnectһ���ڵ㣬����neighborҪ����neighCountҪ��ʼ��
    temp->neighCount=0;
    for (int dir = 0; dir < 4; dir++) {
        //����neighbor,ֻҪ�оʹ��룬��ô��ʵֻҪ�ڳ�ʼ�������������С�֮����Ҫ��ͳ��count
        temp->neighbor[dir]=FindNeighbor(garden,temp,dir);
        //�����������neighborû�б�ռ�ã���ô���ڱ�����������˵������neighbor��+1
        temp->neighCount+= ConAvailable(temp,temp->neighbor[dir],dir);
    }
    temp->found=1;
    //���һ���ڵ��δ���Ӷ������ڿ����ھ�������ô����ڵ��neighbor������Χ��
    //���������Χ���ǿɳ����ģ���Ϊ�Ⱦ��������ܸı�
    /*���������ڳ�ʼ����ʱ��ֱ�ӽ���Ԥ����������Χ���ǲ��ɳ����ģ�
    ��Ϊ�����ܳ�Ϊ�κ�һ����������neighbor��Ҳ���ٲ���backtracking
    */
    if (temp->neighCount == temp->junction)
        return 1;
    
    //�޽⣬��Ϊ�����ھӲ�������ʱ���Ѿ�nosolution��
    if (temp->neighCount < temp->junction)
        return -1;

    //��ȷ��
    return 0;
}
// ���ݺ�����x, y�ǵ�ǰ��������λ�ã�direction�ǵ�ǰ���Ե����췽��
//����ķ���ֵ����һ�����Ƿ�ľ������ķ���
/*�����Ǵ��ϵ��� �����ң�һ���ڵ���Ϻ�������ǰ������ģ����ɸı䡣������findneighborֻҪ�������ҾͿ�����

����Ψһ������ظ���������Һ���Ϊ�գ������ᱻ�ظ���������*/
bool backtrack( Garden garden,Connector temp) {  
    //������нڵ㶼���꣬
    //�����и����� ������ȫ���ڵ�������ǳɹ��ˣ�backtrack��ͷ��ʼ���ܷ��ʵ�ͬһ���ڵ㣩
    /*�����������������������������ˣ���ô���ǳɹ��ˡ������ĳһ��ʱԤ������û�����ˣ�����ʧ��
    ʧ�ܵ�������ǿ϶��нڵ�������*/
    if (!temp) 
        return true;

    int state;
    #ifdef DEBUG
    printf ("\n visit ");
    PrintStatus(temp);
    #endif
    //����Ԥ����,�ҵ�������neighbor
    state=PreProcessConnect(garden,temp);
    //�޽�ȼ���ĳ���ڵ㲻���ܱ�����
    #ifdef DEBUG
    printf("state=%d\n",state);
    #endif
    if (state==-1)
        return false;
    //����������ڵ�
    for ( int dir=0; dir<4; dir++) {
        //�������û�п����ھ�
        Connector neighbor=temp->neighbor[dir];
        //������û��neighbor��Ҳ����temp����neighbor���ϴ�����ʱ�Ѿ���ռ��
        if (neighbor==NULL||!ConAvailable(temp,neighbor,dir))
            continue;
        #ifdef DEBUG
        switch (dir)
        {
            case UP:printf("UP\n");break;
            case Right:printf("Right\n");break;
            case DOWN:printf("DOWN\n");break;
            case LEFT:printf("LEFT\n");break;
            default:
                break;
        }
        #endif
        //���ڵ������Ƿ��ʵ�(0,5)֮�����һ��DOWN��û��Ӧ��
        // �����һ��λ���ǺϷ��Ĳ����ǿհף������Χ��
        //ע��functionֻ����link��cancel��ʱ��ᱻ�ı䡣
        link(garden,temp,dir);
        #ifdef DEBUG
        printf("(%d,%d) link %d to (%d,%d)\n",temp->x,temp->y,dir,neighbor->x,neighbor->y);
        #endif
        //���ֱ�����ʽ�����������нڵ�
        if (backtrack(garden,FindNext(garden)))     return true;
        //���ַ������оͳ���
        //�п��ܳ�����������ڵ�������ķ���
        Cancel(garden,temp,dir);
        #ifdef DEBUG
        printf("(%d,%d) cancel %d to (%d,%d),junction=%d\n",temp->x,temp->y,dir,neighbor->x,neighbor->y,temp->junction);
        #endif
        //�������ڵ�����¶���Ҫ������ô��Ȼֻ��һ������������������֮�󣬷�����false����ô�϶���֮ǰ�ڵ�ʧ����
        //��˲���Ҫ�����¸����򣬷����ϸ��������������
        if (temp->junction==2)
            return false;
    }
    //����ľ������з���û��return true����ôҲ��ʧ�ܣ�Ҫ����
    //���ڵ������������η�����һ���ڵ㣬��ô��α���ÿ��
    #ifdef DEBUG
    printf("(%d,%d) dir out\n",temp->x,temp->y);
    #endif
    return false;
    
}
int main() {
    int solution_exist ;
    //��ʱ��ʼdouble run_time;
	double run_time;
	LARGE_INTEGER time_start;	//��ʼʱ��
	LARGE_INTEGER time_over;	//����ʱ��
	double dqFreq;		//��ʱ��Ƶ��
	LARGE_INTEGER f;	//��ʱ��Ƶ��
	QueryPerformanceFrequency(&f);
	dqFreq=(double)f.QuadPart;
	QueryPerformanceCounter(&time_start);	//��ʱ��ʼ
    //��ʼ��
    Garden garden = InitGarden();
    // �ӵ�һ����������ʼ���ݣ�
    solution_exist=backtrack(garden,FindNext(garden));
    if(solution_exist)
        Print(garden);
    else
        printf("no solution\n");
    QueryPerformanceCounter(&time_over);	//��ʱ����
	run_time=1000000*(time_over.QuadPart-time_start.QuadPart)/dqFreq;
	//����1000000�ѵ�λ���뻯Ϊ΢�룬����Ϊ1000 000/��cpu��Ƶ��΢��
	printf("\nrun_time:%f us \n",run_time);
    return 0;
}
