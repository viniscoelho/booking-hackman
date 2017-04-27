#ifndef DEFINITIONS_H
#define DEFINITIONS_H
#endif

typedef long long int64;
typedef pair<int, int> ii;
typedef priority_queue<ii> moveList;

vector<ii> graph[MAX];
//Vertices
int V;
//Point to vertex
map<ii, int> bib;
//Vertex to point
vector<ii> rev_bib(MAX);
//'Strategic' positions
map<ii, int> strategic;
//-----------------------------------------------------------------------------
struct node{
    int type, dist, numAssets, dir;
    node(int type = 0, int dist = 0, int numAssets = 0, int dir = 0) : 
        type(type), dist(dist), numAssets(numAssets), dir(dir) {}

     bool operator<(const node& at) const{
        //snippets have preference over weapons
        if (type != at.type){
            return at.type < type;
        }
        //same type
        else if (type == at.type){
            //order:
            //smallest distance from the object, greatest number of objects
            if (at.dist < dist) return true;
            else if (at.dist == dist){
                return (at.numAssets >= numAssets);
            }
            else return false;
        }
     }
};
//-----------------------------------------------------------------------------
class Point {
public:
    int x;
    int y;
    Point(){ }

    Point(int x, int y){
        this->x = x;
        this->y = y;
    }
};
//-----------------------------------------------------------------------------
class Player : public Point {
public:
    string name;
    int id;
    int snippets;
    bool has_weapon;
    bool is_paralyzed;
};
//-----------------------------------------------------------------------------
int width, height;
vector<vector<int>> is_wall;
int timebank, time_per_move, time_remaining;
int max_rounds, current_round;
Player me, enemy;
vector<Point> snippets, weapons, prev_position;
map<ii, int> bugs;
//-----------------------------------------------------------------------------
//Bugs' previous position
map<ii, int> prev_bugs;
//Bugs' move direction
vector<string> tracked;
bool isInitialized = false, isBehind = false;
//-----------------------------------------------------------------------------
int dx[4] = {-1, 0, 1, 0};
int dy[4] = {0, -1, 0, 1};
string moves_dir[4] = {"up", "left", "down", "right"};

int dist[MAX];
int visited[MAX], visited_bfs[MAX], ok, seen;
int dontMove[4];

void do_move();
