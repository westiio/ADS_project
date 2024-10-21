/*10.20
优化：

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

//需要修改初始化和输出
typedef struct garden* Garden;
typedef struct connector* Connector; 
struct garden {
    int height, width; // 花园的高度、宽度
    int unvisited; // 未访问的连接器数量
    int fence[MAX][MAX]; // 栅栏的二维数组，0表示空，1表示连接器，2表示水平围栏，3表示竖直围栏
    Connector head,tail;
    Connector con[MAX][MAX];//连接器的二维数组
};

struct connector{
    int x;
    int y;
    int junction; // 总体连接情况
    //0-3分别代表右下左上
    int dir[4];
    Connector neighbor[4];
    Connector next;

    //记录现在还有多少个没练上。
}; 

// 方向数组：右下左上, 和记录路径情况配合使用
int dx[] = { 0, 1, 0,-1};
int dy[] = { 1, 0,-1, 0};

//反转方向
int Opposite(int dir) {
    return (dir+2)%4;
}
//是否在花园里
bool InGarden(int x, int y,Garden garden) {
    if (x >= 0 && x < garden->height && y >= 0 && y < garden->width)
        return true;
    return false;
}
/*在退出是判断是否所有的连接器都已经连接
bool AllConnected(Garden garden) {
    if (garden->unvisited==0)
        return true;
}*/
//在撤销之前，加之后更新，1是增，-1是减
void PrintStatus(Connector temp) {
    printf("junction=%d\nUP=%d Down=%d left=%d right=%d\n",temp->junction,temp->dir[3],temp->dir[1],temp->dir[2],temp->dir[0]);
}
void PrintConnector(Connector temp) {
    printf("%d %d ",temp->x+1,temp->y+1);
    printf("%d %d %d %d\n",temp->dir[3],temp->dir[1],temp->dir[2],temp->dir[0]);
}

// 打印连接器
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
// 初始化花园
//fence记录围栏的情况。
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
            //是连接器
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
//找到下一个没满的连接器
Connector FindNext(Garden garden) {
    Connector temp=garden->head->next;
    //找到第一个非0的连接器
    while (temp&&temp->junction==0)
        temp = temp->next;
    return temp;
}

bool DirAvailable(Connector temp,Connector neighbor,int dir) {
    //可能传进来是NULL
    if (!neighbor)
        return false;
    if (temp->dir[dir]==0&&neighbor->dir[Opposite(dir)]==0&&neighbor->junction>0)
        return true;
    return false;
}    

//判断右下是不是满足
int TwoDirAble(Connector temp) {
    int count=0;
    for (int i=0;i<2;i++){
        if (DirAvailable(temp,temp->neighbor[i],i))
            count++;
    }
    //唯一连法。如果junction在左上有分量，那么已经被前一次连接占用了
    if (temp->junction==count)
        return 1;
    //只可能是1 2 两种连法
    else if (temp->junction>count<=count)
        return 0;
    //无解
    else 
        return -1;
}
int FourDirAble(Connector temp) {
    int count=0;
    for (int i=0;i<4;i++){
        if (DirAvailable(temp,temp->neighbor[i],i))
            count++;
    }
    //唯一连法
    if (temp->junction==count)
        return 1;
    //很多种连法
    else if (temp->junction>count<=count)
        return 0;
    //无解
    else 
        return -1;
    
}
//判断某个方向有没有可以连接的邻居

//找到各个方向的邻居，只在全局的时候执行一次
Connector FindDir(Garden garden,Connector temp,int dir)
{
    //如果这个节点已经找过了，直接返回
    //在预处理时，有neighbor就要放入结构体数组中。但如果方向被占用，就不用计数
    int nx = temp->x + dx[dir]; // 下一个位置的坐标
    int ny = temp->y + dy[dir];
    while (InGarden(nx, ny,garden) && garden->fence[nx][ny] == BLANK) {
        nx += dx[dir];
        ny += dy[dir];
    }
    //如果neighbor是连接器，这样保证了取消的时候neighbor数组中不为NULL
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
    //直接改掉这个指针
    neighbor->dir[Opposite(dir)]=1;
    neighbor->junction--;
    //我觉得这里应该默认连的是右下，但是撤销就不一定了
    //更新围栏，两个连接器不用更改
    for(int xi=temp->x+dx[dir], yi=temp->y+dy[dir];xi!=x||yi!=y;xi+=dx[dir],yi+=dy[dir]){
        garden->fence[xi][yi]=FENCE;
    }
}
//Cancel和link都不用改变neigborcount，因为每个节点被再次访问时会重新计算
void Cancel(Garden garden, Connector temp,int dir) {
    // 回溯，撤销之前的围栏
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
//找四个方向的邻居，只在全局的时候执行一次
void FindNeighbors(Garden garden,Connector temp) {


    //由于从左到右，从上到下扫描。只要向右和向下找就行
    for (int dir = 0; dir < 4; dir++) {
        //更新neighbor,只要有就存入，那么其实只要在初始化结束后存入就行。之后主要是统计count
        temp->neighbor[dir]=FindDir(garden,temp,dir);
    }
    /*！！考虑在初始化的时候直接进行预处理，这样的围栏是不可撤销的，
    因为不可能成为任何一个连接器的neighbor，也不再参与backtracking
    */
}
//在右下方进行的确认的连接
void R_D_link(Garden garden,Connector temp){
    for (int i=0;i<2;i++){
        if (DirAvailable(temp,temp->neighbor[i],i))
            link(garden,temp,i);
    }
}
bool PreProcess(Garden garden){
    //对于每个连接器，找到所有的邻居
    //1说明每一步都是确认的，-1说明有节点无解
    Connector temp=garden->head->next;
    while (temp) {
        //找到4个方向的neighbor
        FindNeighbors(garden,temp);
        //判断四个方向的连接情况，可能进行连接。注意按照从左到右，从上到下的顺序
        //有可能一个方向没有被R_D_link，但是这个节点在确定junction数下只有一种连接方法，那么也要适当连接
        int state4=FourDirAble(temp);
        if (state4==-1)  return false;
        if (state4==1){
            //进行四个方向的连接
            for (int dir=2;dir<4;dir++){
                Connector neighbor=temp->neighbor[dir];
                if (DirAvailable(temp,neighbor,dir))
                    //为了保持方向一致性，从左到右，从上到下连接
                    link(garden,neighbor,Opposite(dir));
            }
            R_D_link(garden,temp);
            //连了四个方向就不用进行2个方向的检验了
            continue;
        }
        /*预处理阶段，可能没有连上，所以不能根据junction和state2的值连接*/
        temp=temp->next;
    }
    return true;
}
/*经过PreProcess的结果：
四个方向和两个方向不满足要求的都返回0，确认的都已经连接*/

// 回溯函数：x, y是当前连接器的位置，direction是当前尝试的延伸方向
//这里的返回值代表一个点是否耗尽了它的方向。
/*由于是从上到下 从左到右，一个节点的上和左是由前面决定的，不可改变。理论上findneighbor只要向右下找就可以了

所以唯一会产生重复的情况是右和下为空，这样会被重复遍历两次*/
//在comfirm的条件下，如果一个节点的function=neighborcount

/*这个版本的backtrack中，每次能遍历一个节点*/
bool backtrack( Garden garden,Connector temp) {  
    //如果所有节点都用完，
    /*如果在满足所有条件的情况下用完了，那么就是成功了。如果到某一步时预处理发现没法练了，就是失败
    失败的情况就是肯定有节点连不上*/
    if (!temp) 
        return true;

    #ifdef DEBUG
    printf ("\n backtracking (%d,%d)\n",temp->x,temp->y);
    PrintStatus(temp);
    #endif
    //进行预处理,找到所有用neighbor
    int state = TwoDirAble(temp);
    //无解等价于某个节点不可能被连上
    #ifdef DEBUG
    printf("state=%d\n",state);
    #endif
    //包括junction=2，neighbor=0,1,和junction=1，neighbor=0的情况，无解
    if (state==-1)
        return false;
    //包括junction=2，neighbor=2,和junction=1，neighbor=1的情况，连法唯一
    if (state==1){
        //这个节点的右下都需要连，那么必然只有一种连法
        R_D_link(garden,temp);
        return backtrack(garden,FindNext(garden));
    }
    //剩下的情况是：jucntion=1，但右和下都可用
    /*先连右，如果返回0，那么肯定只有向下一种连法。如果向下连也返回0，说明这个点连法不行，需要向上返回到上个节点*/
    //注：function只有在link和cancel的时候会被改变。
    link(garden,temp,RIGHT);
    #ifdef DEBUG
    printf("(%d,%d) link <RIGHT> ,finish\n",temp->x,temp->y);
    #endif
    //这种遍历方式可以用完所有节点
    if (backtrack(garden,FindNext(garden)))     return true;
    //这种方法不行就撤销
    //有可能撤销不是这个节点所引入的方向
    Cancel(garden,temp,RIGHT);
    printf("(%d,%d) cancel <RIGHT>,finish",temp->x,temp->y);


    link(garden,temp,DOWN);
    #ifdef DEBUG
    printf("(%d,%d) link <DOWN> ,finish\n",temp->x,temp->y);
    #endif
    //这种遍历方式可以用完所有节点
    if (backtrack(garden,FindNext(garden)))     return true;
    //这种方法不行就撤销
    //有可能撤销不是这个节点所引入的方向
    Cancel(garden,temp,DOWN);
    #ifdef DEBUG
    printf("(%d,%d) cancel <DOWN>,finish",temp->x,temp->y);
    #endif
    return false;
    
}
int main() {
    bool state_able;
    int solution_exist=0;
    //计时开始double run_time;
	double run_time;
	LARGE_INTEGER time_start;	//开始时间
	LARGE_INTEGER time_over;	//结束时间
	double dqFreq;		//计时器频率
	LARGE_INTEGER f;	//计时器频率
	QueryPerformanceFrequency(&f);
	dqFreq=(double)f.QuadPart;
	QueryPerformanceCounter(&time_start);	//计时开始


    //初始化
    Garden garden = InitGarden();
    //这一步只连接确定的点，是不可撤销的，也不会再参与任何backtracking中
    //如果预处理出错，就不用回溯了
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
    QueryPerformanceCounter(&time_over);	//计时结束
	run_time=1000000*(time_over.QuadPart-time_start.QuadPart)/dqFreq;
	//乘以1000000把单位由秒化为微秒，精度为1000 000/（cpu主频）微秒
	printf("\nrun_time:%f us \n",run_time);
    return 0;
}
