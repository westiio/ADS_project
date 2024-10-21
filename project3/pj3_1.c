/*10.16
实操后发现：向一个方向延伸到一个节点之后，另一个节点自带方向
可能connector再增加一个 expdir来记录已经延伸的方向
如果访问到的是4，3，2，1 操作不同 。根据已知的，也不同
unvisit--保留，再看一会
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
    int junction; // 连接器的度数
    //0-3分别代表上右下左
    int dir[4];
    int neighCount;
    int found;
    Connector neighbor[4];
    Connector next;

    //记录现在还有多少个没练上。
}; 

// 方向数组：上右下左, 和记录路径情况配合使用
int dx[] = {-1, 0, 1, 0};
int dy[] = {0,  1, 0,-1};

//是否在花园里
bool InGarden(int x, int y,Garden garden) {
    if (x >= 0 && x < garden->height && y >= 0 && y < garden->width)
        return true;
    return false;
}
//在退出是判断是否所有的连接器都已经连接
bool AllConnected(Garden garden) {
    if (garden->unvisited==0)
        return true;
}
//在撤销之前，加之后更新，1是增，-1是减
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
    node->neighCount=0;
    node->found=0;
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

bool ConAvailable(Connector temp,Connector neighbor,int dir) {
    //可能传进来是NULL
    if (!neighbor)
        return false;
    if (temp->dir[dir]==0&&neighbor->dir[(dir+2)%4]==0&&neighbor->junction>0)
        return true;
    return false;
}    

//找到各个方向的邻居,如果已经链接过了就不计入
//由于回溯会按照 3 2 1 0的顺序撤销。如何解决这个问题
Connector FindNeighbor(Garden garden,Connector temp,int dir)
{
    //如果这个节点已经找过了，直接返回
    if (temp->found)
        return temp->neighbor[dir];
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
    UpdateUnvisit(garden,neighbor,-1);
    //直接改掉这个指针
    neighbor->dir[(dir+2)%4]=1;
    neighbor->junction--;
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
    UpdateUnvisit(garden,temp->neighbor[dir],1);
    temp->junction++;
    neighbor->dir[(dir+2)%4]=0;
    neighbor->junction++;
    for(int xi=temp->x+dx[dir], yi=temp->y+dy[dir];xi!=x||yi!=y;xi+=dx[dir],yi+=dy[dir]){
        garden->fence[xi][yi]=BLANK;
    }
}
int PreProcessConnect(Garden garden,Connector temp) {
    //可能会多次PreProcessConnect一个节点，所以neighbor要更新neighCount要初始化
    temp->neighCount=0;
    for (int dir = 0; dir < 4; dir++) {
        //更新neighbor,只要有就存入，那么其实只要在初始化结束后存入就行。之后主要是统计count
        temp->neighbor[dir]=FindNeighbor(garden,temp,dir);
        //如果这个方向的neighbor没有被占用，那么对于本次连接器来说，可用neighbor数+1
        temp->neighCount+= ConAvailable(temp,temp->neighbor[dir],dir);
    }
    temp->found=1;
    //如果一个节点的未连接度数等于可用邻居数，那么这个节点的neighbor方向都是围栏
    //但是这里的围栏是可撤销的，因为先决条件可能改变
    /*！！考虑在初始化的时候直接进行预处理，这样的围栏是不可撤销的，
    因为不可能成为任何一个连接器的neighbor，也不再参与backtracking
    */
    if (temp->neighCount == temp->junction)
        return 1;
    
    //无解，因为可用邻居不够。这时候已经nosolution了
    if (temp->neighCount < temp->junction)
        return -1;

    //不确定
    return 0;
}
// 回溯函数：x, y是当前连接器的位置，direction是当前尝试的延伸方向
//这里的返回值代表一个点是否耗尽了它的方向。
/*由于是从上到下 从左到右，一个节点的上和左是由前面决定的，不可改变。理论上findneighbor只要向右下找就可以了

所以唯一会产生重复的情况是右和下为空，这样会被重复遍历两次*/
bool backtrack( Garden garden,Connector temp) {  
    //如果所有节点都用完，
    //这里有个疑问 理论上全部节点用完就是成功了（backtrack从头开始可能访问到同一个节点）
    /*如果在满足所有条件的情况下用完了，那么就是成功了。如果到某一步时预处理发现没法练了，就是失败
    失败的情况就是肯定有节点连不上*/
    if (!temp) 
        return true;

    int state;
    #ifdef DEBUG
    printf ("\n visit ");
    PrintStatus(temp);
    #endif
    //进行预处理,找到所有用neighbor
    state=PreProcessConnect(garden,temp);
    //无解等价于某个节点不可能被连上
    #ifdef DEBUG
    printf("state=%d\n",state);
    #endif
    if (state==-1)
        return false;
    //访问了这个节点
    for ( int dir=0; dir<4; dir++) {
        //这个方向没有可用邻居
        Connector neighbor=temp->neighbor[dir];
        //可能是没有neighbor，也可能temp或者neighbor在上次连接时已经被占用
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
        //现在的问题是访问到(0,5)之后输出一个DOWN就没反应了
        // 如果下一个位置是合法的并且是空白，则放置围栏
        //注：function只有在link和cancel的时候会被改变。
        link(garden,temp,dir);
        #ifdef DEBUG
        printf("(%d,%d) link %d to (%d,%d)\n",temp->x,temp->y,dir,neighbor->x,neighbor->y);
        #endif
        //这种遍历方式可以用完所有节点
        if (backtrack(garden,FindNext(garden)))     return true;
        //这种方法不行就撤销
        //有可能撤销不是这个节点所引入的方向
        Cancel(garden,temp,dir);
        #ifdef DEBUG
        printf("(%d,%d) cancel %d to (%d,%d),junction=%d\n",temp->x,temp->y,dir,neighbor->x,neighbor->y,temp->junction);
        #endif
        //如果这个节点的右下都需要连，那么必然只有一种连法。但是在连右之后，返回了false，那么肯定是之前节点失败了
        //因此不需要再连下个方向，返回上个方向继续连即可
        if (temp->junction==2)
            return false;
    }
    //如果耗尽了所有方向都没有return true，那么也是失败，要撤销
    //现在的问题是如果多次访问了一个节点，那么如何保障每次
    #ifdef DEBUG
    printf("(%d,%d) dir out\n",temp->x,temp->y);
    #endif
    return false;
    
}
int main() {
    int solution_exist ;
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
    // 从第一个连接器开始回溯：
    solution_exist=backtrack(garden,FindNext(garden));
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
