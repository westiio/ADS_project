/*10.20
�Ż���

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

#define RIGHT 0
#define DOWN 1
#define LEFT 2
#define UP 3

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
    int junction; // �����������
    //0-3�ֱ������������
    int dir[4];
    Connector neighbor[4];
    Connector next;

    //��¼���ڻ��ж��ٸ�û���ϡ�
}; 

// �������飺��������, �ͼ�¼·��������ʹ��
int dx[] = { 0, 1, 0,-1};
int dy[] = { 1, 0,-1, 0};

//��ת����
int Opposite(int dir) {
    return (dir+2)%4;
}
//�Ƿ��ڻ�԰��
bool InGarden(int x, int y,Garden garden) {
    if (x >= 0 && x < garden->height && y >= 0 && y < garden->width)
        return true;
    return false;
}
/*���˳����ж��Ƿ����е����������Ѿ�����
bool AllConnected(Garden garden) {
    if (garden->unvisited==0)
        return true;
}*/
//�ڳ���֮ǰ����֮����£�1������-1�Ǽ�
void PrintStatus(Connector temp) {
    printf("junction=%d\nUP=%d Down=%d left=%d right=%d\n",temp->junction,temp->dir[3],temp->dir[1],temp->dir[2],temp->dir[0]);
}
void PrintConnector(Connector temp) {
    printf("%d %d ",temp->x+1,temp->y+1);
    printf("%d %d %d %d\n",temp->dir[3],temp->dir[1],temp->dir[2],temp->dir[0]);
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

bool DirAvailable(Connector temp,Connector neighbor,int dir) {
    //���ܴ�������NULL
    if (!neighbor)
        return false;
    if (temp->dir[dir]==0&&neighbor->dir[Opposite(dir)]==0&&neighbor->junction>0)
        return true;
    return false;
}    

//�ж������ǲ�������
int TwoDirAble(Connector temp) {
    int count=0;
    for (int i=0;i<2;i++){
        if (DirAvailable(temp,temp->neighbor[i],i))
            count++;
    }
    //Ψһ���������junction�������з�������ô�Ѿ���ǰһ������ռ����
    if (temp->junction==count)
        return 1;
    //ֻ������1 2 ��������
    else if (temp->junction>count<=count)
        return 0;
    //�޽�
    else 
        return -1;
}
int FourDirAble(Connector temp) {
    int count=0;
    for (int i=0;i<4;i++){
        if (DirAvailable(temp,temp->neighbor[i],i))
            count++;
    }
    //Ψһ����
    if (temp->junction==count)
        return 1;
    //�ܶ�������
    else if (temp->junction>count<=count)
        return 0;
    //�޽�
    else 
        return -1;
    
}
//�ж�ĳ��������û�п������ӵ��ھ�

//�ҵ�����������ھӣ�ֻ��ȫ�ֵ�ʱ��ִ��һ��
Connector FindDir(Garden garden,Connector temp,int dir)
{
    //�������ڵ��Ѿ��ҹ��ˣ�ֱ�ӷ���
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
    //ֱ�Ӹĵ����ָ��
    neighbor->dir[Opposite(dir)]=1;
    neighbor->junction--;
    //�Ҿ�������Ӧ��Ĭ�����������£����ǳ����Ͳ�һ����
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
    temp->junction++;
    neighbor->dir[Opposite(dir)]=0;
    neighbor->junction++;
    for(int xi=temp->x+dx[dir], yi=temp->y+dy[dir];xi!=x||yi!=y;xi+=dx[dir],yi+=dy[dir]){
        garden->fence[xi][yi]=BLANK;
    }
}
//���ĸ�������ھӣ�ֻ��ȫ�ֵ�ʱ��ִ��һ��
void FindNeighbors(Garden garden,Connector temp) {


    //���ڴ����ң����ϵ���ɨ�衣ֻҪ���Һ������Ҿ���
    for (int dir = 0; dir < 4; dir++) {
        //����neighbor,ֻҪ�оʹ��룬��ô��ʵֻҪ�ڳ�ʼ�������������С�֮����Ҫ��ͳ��count
        temp->neighbor[dir]=FindDir(garden,temp,dir);
    }
    /*���������ڳ�ʼ����ʱ��ֱ�ӽ���Ԥ����������Χ���ǲ��ɳ����ģ�
    ��Ϊ�����ܳ�Ϊ�κ�һ����������neighbor��Ҳ���ٲ���backtracking
    */
}
//�����·����е�ȷ�ϵ�����
void R_D_link(Garden garden,Connector temp){
    for (int i=0;i<2;i++){
        if (DirAvailable(temp,temp->neighbor[i],i))
            link(garden,temp,i);
    }
}
bool PreProcess(Garden garden){
    //����ÿ�����������ҵ����е��ھ�
    //1˵��ÿһ������ȷ�ϵģ�-1˵���нڵ��޽�
    Connector temp=garden->head->next;
    while (temp) {
        //�ҵ�4�������neighbor
        FindNeighbors(garden,temp);
        //�ж��ĸ������������������ܽ������ӡ�ע�ⰴ�մ����ң����ϵ��µ�˳��
        //�п���һ������û�б�R_D_link����������ڵ���ȷ��junction����ֻ��һ�����ӷ�������ôҲҪ�ʵ�����
        int state4=FourDirAble(temp);
        if (state4==-1)  return false;
        if (state4==1){
            //�����ĸ����������
            for (int dir=2;dir<4;dir++){
                Connector neighbor=temp->neighbor[dir];
                if (DirAvailable(temp,neighbor,dir))
                    //Ϊ�˱��ַ���һ���ԣ������ң����ϵ�������
                    link(garden,neighbor,Opposite(dir));
            }
            R_D_link(garden,temp);
            //�����ĸ�����Ͳ��ý���2������ļ�����
            continue;
        }
        /*Ԥ����׶Σ�����û�����ϣ����Բ��ܸ���junction��state2��ֵ����*/
        temp=temp->next;
    }
    return true;
}
/*����PreProcess�Ľ����
�ĸ������������������Ҫ��Ķ�����0��ȷ�ϵĶ��Ѿ�����*/

// ���ݺ�����x, y�ǵ�ǰ��������λ�ã�direction�ǵ�ǰ���Ե����췽��
//����ķ���ֵ����һ�����Ƿ�ľ������ķ���
/*�����Ǵ��ϵ��� �����ң�һ���ڵ���Ϻ�������ǰ������ģ����ɸı䡣������findneighborֻҪ�������ҾͿ�����

����Ψһ������ظ���������Һ���Ϊ�գ������ᱻ�ظ���������*/
//��comfirm�������£����һ���ڵ��function=neighborcount

/*����汾��backtrack�У�ÿ���ܱ���һ���ڵ�*/
bool backtrack( Garden garden,Connector temp) {  
    //������нڵ㶼���꣬
    /*�����������������������������ˣ���ô���ǳɹ��ˡ������ĳһ��ʱԤ������û�����ˣ�����ʧ��
    ʧ�ܵ�������ǿ϶��нڵ�������*/
    if (!temp) 
        return true;

    #ifdef DEBUG
    printf ("\n backtracking (%d,%d)\n",temp->x,temp->y);
    PrintStatus(temp);
    #endif
    //����Ԥ����,�ҵ�������neighbor
    int state = TwoDirAble(temp);
    //�޽�ȼ���ĳ���ڵ㲻���ܱ�����
    #ifdef DEBUG
    printf("state=%d\n",state);
    #endif
    //����junction=2��neighbor=0,1,��junction=1��neighbor=0��������޽�
    if (state==-1)
        return false;
    //����junction=2��neighbor=2,��junction=1��neighbor=1�����������Ψһ
    if (state==1){
        //����ڵ�����¶���Ҫ������ô��Ȼֻ��һ������
        R_D_link(garden,temp);
        return backtrack(garden,FindNext(garden));
    }
    //ʣ�µ�����ǣ�jucntion=1�����Һ��¶�����
    /*�����ң��������0����ô�϶�ֻ������һ�����������������Ҳ����0��˵��������������У���Ҫ���Ϸ��ص��ϸ��ڵ�*/
    //ע��functionֻ����link��cancel��ʱ��ᱻ�ı䡣
    link(garden,temp,RIGHT);
    #ifdef DEBUG
    printf("(%d,%d) link <RIGHT> ,finish\n",temp->x,temp->y);
    #endif
    //���ֱ�����ʽ�����������нڵ�
    if (backtrack(garden,FindNext(garden)))     return true;
    //���ַ������оͳ���
    //�п��ܳ�����������ڵ�������ķ���
    Cancel(garden,temp,RIGHT);
    printf("(%d,%d) cancel <RIGHT>,finish",temp->x,temp->y);


    link(garden,temp,DOWN);
    #ifdef DEBUG
    printf("(%d,%d) link <DOWN> ,finish\n",temp->x,temp->y);
    #endif
    //���ֱ�����ʽ�����������нڵ�
    if (backtrack(garden,FindNext(garden)))     return true;
    //���ַ������оͳ���
    //�п��ܳ�����������ڵ�������ķ���
    Cancel(garden,temp,DOWN);
    #ifdef DEBUG
    printf("(%d,%d) cancel <DOWN>,finish",temp->x,temp->y);
    #endif
    return false;
    
}
int main() {
    bool state_able;
    int solution_exist=0;
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
    //��һ��ֻ����ȷ���ĵ㣬�ǲ��ɳ����ģ�Ҳ�����ٲ����κ�backtracking��
    //���Ԥ��������Ͳ��û�����
    state_able=PreProcess(garden);
    #ifdef DEBUG
        printf("PreProcess finish\n");
        printf("state_able=%d\n",state_able);
    #endif
    if (state_able==true)
        solution_exist=backtrack(garden,FindNext(garden));
    #ifdef DEBUG
        printf("backtrack finish\n");
        printf("solution_exist=%d\n",solution_exist);
    #endif
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
