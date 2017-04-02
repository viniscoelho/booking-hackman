#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <ctime>
#include <map>
#include <queue>
#define INF 1000000007
#define MAX 300
#define pb push_back
#define mp make_pair
#define fi first
#define se second

using namespace std;

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

class Point {
    public:
        int x;
        int y;
        Point(){

        }
        Point(int x, int y){
            this->x = x;
            this->y = y;
        }
};

class Player : public Point {
    public:
        string name;
        int id;
        int snippets;
        bool has_weapon;
        bool is_paralyzed;
};

int width;
int height;
vector<vector<int>> is_wall;
int timebank;
int time_per_move;
int time_remaining;
int max_rounds;
int current_round;
Player me;
Player enemy;
vector<Point> snippets, weapons, prev_position;
map<ii, int> bugs;
//Bugs' previous position
map<ii, int> prev_bugs;
//Bugs' move direction
vector<string> tracked;

bool isInitialized = false;
bool isBehind = false;

void do_move();

void process_next_command() {
    string command;
    cin >> command;
    if (command == "settings") {
        string type;
        cin >> type;
        if (type == "timebank") {
            cin >> timebank;
        }
        else if (type == "time_per_move") {
            cin >> time_per_move;
        }
        else if (type == "player_names") {
            string names;
            cin >> names;
            // player names aren't very useful
        }
        else if (type == "your_bot") {
            cin >> me.name;
        }
        else if (type == "your_botid") {
            cin >> me.id;
        }
        else if (type == "field_width") {
            cin >> width;
        }
        else if (type == "field_height") {
            cin >> height;
        }
        else if (type == "max_rounds") {
            cin >> max_rounds;
        }
    }
    else if (command == "update") {
        string player_name, type;
        cin >> player_name >> type;
        if (type == "round") {
            cin >> current_round;
        }
        else if (type == "field") {
            snippets.clear();
            weapons.clear();
            bugs.clear();
            is_wall.clear();
            for (int x = 0; x < height; x++) {
                is_wall.pb(vector<int>(width));
            }
            string s;
            cin >> s;
            int n = s.length();
            int i = 0;
            for (int x = 0; x < height; x++) {
                for (int y = 0; y < width; y++) {
                    Point pt;
                    pt.x = x;
                    pt.y = y;
                    while (true) {
                        if (i == n || s[i] == ',') {
                            i++;
                            break;
                        }
                        char c = s[i++];
                        if (c == 'x') {
                            is_wall[x][y] = 1;
                            // cout << 'x';
                        }
                        else if (c == '.') {
                            //do nothing, is_wall[x][y] == 0 by default
                            // cout << ' ';
                        }
                        else if (c == 'E') {
                            //bugs.pb(pt);
                            int bugNum = bugs.size();
                            bugs[mp(x, y)] = bugNum;
                            // cout << 'e';
                        }
                        else if (c == 'W') {
                            weapons.pb(pt);
                            // cout << 'w';
                        }
                        else if (c == 'C') {
                            snippets.pb(pt);
                            // cout << 'c';
                        }
                        else {
                            // played id
                            int id = c - '0';
                            if (id == me.id) {
                                me.x = x;
                                me.y = y;
                                // cout << 'm';
                            }
                            else {
                                enemy.x = x;
                                enemy.y = y;
                                // cout << 'n';
                            }
                        }
                    }
                }
                // cout << endl;
            }
        }
        else if (type == "snippets") {
            if (player_name == me.name) {
                cin >> me.snippets;
            }
            else {
                cin >> enemy.snippets;
            }
        }
        else if (type == "has_weapon") {
            string value;
            cin >> value;
            if (player_name == me.name) {
                me.has_weapon = (value == "true");
            }
            else {
                enemy.has_weapon = (value == "true");
            }
        }
        else if (type == "is_paralyzed") {
            string value;
            cin >> value;
            if (player_name == me.name) {
                me.is_paralyzed = (value == "true");
            }
            else {
                enemy.is_paralyzed = (value == "true");
            }
        }
    }
    else if (command == "action") {
        string useless_move;
        cin >> useless_move;
        cin >> time_remaining;
        do_move();
    }
}

int main(){
    srand((int)time(0));
    while (true){
        process_next_command();
    }
    return 0;
}

//-----------------------------------------//
//  Improve the code below to win 'em all  //
//-----------------------------------------//

int dx[4] = {-1, 0, 1, 0};
int dy[4] = {0, -1, 0, 1};
string moves_dir[4] = {"up", "left", "down", "right"};

int dist[MAX];
int visited[MAX], visited_bfs[MAX], ok, seen;
int dontMove[4];

void initializeGraph(){
    V = 0;
    for (int x = 0; x < height; x++){
        for (int y = 0; y < width; y++, V++){
            bib[mp(x, y)] = V;
            rev_bib[V] = mp(x, y);
        }
    }

    for (int x = 0; x < height; x++){
        for (int y = 0; y < width; y++){
            if (is_wall[x][y]) continue;
            int st = 0;
            for (int dir = 0; dir < 4; dir++){
                int nextx = x + dx[dir];
                int nexty = y + dy[dir];
                if (nextx < 0 || nextx >= height || nexty < 0 || nexty >= width)
                    continue;
                if (!is_wall[nextx][nexty])
                    graph[bib[mp(x, y)]].pb(mp(bib[mp(nextx, nexty)], 1));
            }
        }
    }
    strategic[mp(4, 2)] = 1;
    strategic[mp(4, 17)] = 1;
    strategic[mp(2, 6)] = 1;
    strategic[mp(2, 13)] = 1;
    strategic[mp(9, 6)] = 1;
    strategic[mp(9, 13)] = 1;
    strategic[mp(11, 4)] = 1;
    strategic[mp(11, 15)] = 1;

    isInitialized = true;
}

void trackBugs(){
    tracked.clear();
    if (!prev_bugs.empty()){
        int qtd = 0;
        for (auto a : bugs){
            int bx = a.fi.fi;
            int by = a.fi.se;
            vector<string> uncertain;
            for (int dir = 0; dir < 4; dir++){
                int nextx = bx + dx[dir];
                int nexty = by + dy[dir];
                if (nextx < 0 || nextx >= height || nexty < 0 || nexty >= width) continue;
                if (!is_wall[nextx][nexty]) uncertain.pb(moves_dir[dir]);
            }
            //Not sure where it might go.
            if (uncertain.empty() || uncertain.size() == 3){
                tracked.pb("uncertain");
                qtd++;
                continue;
            }

            for (auto b : prev_bugs){
                int b2x = b.fi.fi;
                int b2y = b.fi.se;
                //Bug just got out.
                if ((b2x == 6 && b2y == 9) || (b2x == 6 && b2y == 10)){
                    tracked.pb("uncertain");
                    break;
                }
                //Bug tracked! Next move?
                if (abs(b2x-bx) + abs(b2y-by) == 1){
                    //moved up
                    if (b2x > bx){
                        bool found = false;
                        string other;
                        for (auto dir : uncertain){
                            found |= (dir == "up");
                            if (dir != "up" && dir != "down") other = dir;
                        }
                        if (found) tracked.pb("up");
                        else tracked.pb(other);
                    }
                    //moved down
                    else if (b2x < bx){
                        bool found = false;
                        string other;
                        for (auto dir : uncertain){
                            found |= (dir == "down");
                            if (dir != "up" && dir != "down") other = dir;
                        }
                        if (found) tracked.pb("down");
                        else tracked.pb(other);
                    }
                    //moved left
                    else if (b2y > by){
                        bool found = false;
                        string other;
                        for (auto dir : uncertain){
                            found |= (dir == "left");
                            if (dir != "left" && dir != "right") other = dir;
                        }
                        if (found) tracked.pb("left");
                        else tracked.pb(other);
                    }
                    //moved right
                    else if (b2y < by){
                        bool found = false;
                        string other;
                        for (auto dir : uncertain){
                            found |= (dir == "right");
                            if (dir != "left" && dir != "right") other = dir;
                        }
                        if (found) tracked.pb("right");
                        else tracked.pb(other);
                    }
                    //don't know where it is going
                    else tracked.pb("uncertain");
                    break;
                }
            }
            //This bug was not tracked before
            if (qtd == tracked.size()) tracked.pb("uncertain");
            qtd++;
        }
    }
}

int bfs(int from, bool danger, int d){
    seen++;

    queue<ii> bfs;
    bfs.push(mp(from, 0));
    visited_bfs[from] = seen;
    int qtdBugs = 0;
    while (!bfs.empty()){
        ii at = bfs.front(); bfs.pop();
        //May be safe
        if (at.second == d) return qtdBugs;
        //Ops! Enemy with weapon
        if (at.first == bib[mp(enemy.x, enemy.y)] && danger) qtdBugs++;
        //This path is too dangerous
        if (at.second <= 1 && bugs.count(rev_bib[at.first])) qtdBugs++;
        for (int i = 0; i < graph[at.first].size(); i++){
            int v = graph[at.first][i].first;
            if (visited_bfs[v] != seen){
                bfs.push(mp(v, at.second+1));
                visited_bfs[v] = seen;
            }
        }
    }
    return qtdBugs;
}

//ep -> verify enemy position; do not use a path having him
//bp -> verify bug position; do not use a path having it
int dijkstra(int from, int to, bool ep = false, bool bp = false){
    ok++;
    for (int i = 0; i < bib.size(); i++) dist[i] = INF;

    priority_queue<ii> pq;
    pq.push(mp(0, from));
    dist[from] = 0;
    
    while (!pq.empty()){
        ii atual = pq.top(); pq.pop();
        int cost = -atual.first;
        int v = atual.second;
        if (visited[v] == ok) continue;
        visited[v] = ok;
        for (int i = 0; i < graph[v].size(); i++){
            int u = graph[v][i].first;
            int d = graph[v][i].second;
            //-----------------------------------------------------------------
            //Verifications
            //Enemy has a weapon
            if (ep){
                //Can't have the enemy carrying a weapon on my way
                if (u == bib[mp(enemy.x, enemy.y)]) continue;
            }
            //There are bugs and I'm not carrying a weapon. Be careful
            if (bp){
                bool ignore_u = false;
                //Can't have a bug on my way
                if (bugs.count(rev_bib[u])){
                    ii v_p = rev_bib[v];
                    ii u_p = rev_bib[u];
                    int bug_p = bugs[u_p];

                    if (tracked[bug_p] == "uncertain")
                        ignore_u = true;
                    //moving down
                    else if (v_p.fi < u_p.fi && tracked[bug_p] == "up")
                        ignore_u = true;
                    //moving up
                    else if (v_p.fi > u_p.fi && tracked[bug_p] == "down")
                        ignore_u = true;
                    //moving right
                    else if (v_p.se < u_p.se && tracked[bug_p] == "left")
                        ignore_u = true;
                    //moving left
                    else if (v_p.se > u_p.se && tracked[bug_p] == "right")
                        ignore_u = true;
                }
                int threatsClose = bfs(u, enemy.has_weapon, 2);
                if (ignore_u || threatsClose) continue;
            }
            //-----------------------------------------------------------------
            if (dist[u] > cost + d){
                dist[u] = cost + d; 
                pq.push(mp(-dist[u], u));
            }
        }
    }
    return dist[to];
}

void choseMove(moveList& bugAvoid, moveList& enemyAvoid, moveList& mostSafe,
        moveList& chase, priority_queue<node>& moves, vector<string>& valid_moves,
        priority_queue<node>& corners){
    bool enemy_threat = (!me.has_weapon && enemy.has_weapon);
    bool canPass = false;
    for (int k = 0; k < valid_moves.size(); k++) canPass |= dontMove[k];
    int m;
    //Condition 16
    //No bugs, snippets, weapons and enemy threat.
    //OR
    //Condition 8
    //Bugs in a safe distance and no snippets, weapons and enemy threat.
    if ((!bugs.size() || !bugAvoid.empty()) && !snippets.size()
        && !weapons.size() && !enemy_threat){
        // cout << "safe!\n";
        //Go to the corners
        if (!corners.empty()){
            if (corners.top().dist == 0 && !canPass){
                m = corners.top().dir;
                if (strategic.count(mp(me.x, me.y))) cout << "pass" << endl;
                else cout << valid_moves[m] << endl;
            }
            else{
                m = corners.top().dir;
                cout << valid_moves[m] << endl;
            }
        }
        //Chase him!
        else if (!chase.empty()){
            m = chase.top().second;
            cout << valid_moves[m] << endl;
        }
        else cout << "pass" << endl;
    }
    //No bugs, no snippets, no weapons and enemy threat.
    //OR
    //Condition 7
    //Bugs in a safe distance and both no snippets and weapons.
    //But, enemy threat.
    else if ((!bugs.size() || !bugAvoid.empty()) && !snippets.size()
        && !weapons.size() && enemy_threat){
        // cout << "trapped 1!\n";
        //Let's see which position is safer.
        if (!enemyAvoid.empty()){
            m = enemyAvoid.top().second;
            cout << valid_moves[m] << endl;
        }
        else cout << "pass" << endl;
    }
    //There are bugs and/or enemy and there is no way to avoid them.
    else if (bugs.size() && bugAvoid.empty()){
        // cout << "trapped 2!\n";
        //Is enemy a threat as well?
        if (!enemyAvoid.empty()){
            m = enemyAvoid.top().second;
            cout << valid_moves[m] << endl;
        }
        //Or just the bugs?
        else if (!mostSafe.empty()){
            m = mostSafe.top().second;
            cout << valid_moves[m] << endl;
        }
        else cout << "pass" << endl;
    }
    //Conditions 1, 3, 5, 9, 11, 13
    //Conditions 2, 4, 6, 10, 12, 14
    //There are snippets and/or weapons to collect!
    else if (!moves.empty()){
        // cout << "moves!\n";
        if (moves.top().type == -1) cout << "pass" << endl;
        //Snippets too far away. Go to the corners
        else if (moves.top().type == 5 || moves.top().type == 6){
            if (!corners.empty()){
                if (corners.top().dist == 0 && !canPass){
                    m = corners.top().dir;
                    if (strategic.count(mp(me.x, me.y))) cout << "pass" << endl;
                    else cout << valid_moves[m] << endl;
                }
                else{
                    // cout << "moves! corner\n";
                    m = corners.top().dir;
                    cout << valid_moves[m] << endl;
                }
            }
            //Ops, something went wrong. I am trapped somehow.
            //Is enemy a threat as well?
            else if (!enemyAvoid.empty()){
                // cout << "moves! enemy\n";
                m = enemyAvoid.top().second;
                cout << valid_moves[m] << endl;
            }
            //Or just the bugs?
            else if (!mostSafe.empty()){
                // cout << "moves! bugs\n";
                m = mostSafe.top().second;
                cout << valid_moves[m] << endl;
            }
            //Or nothing :P
            else cout << "pass" << endl;
        }
        else{
            // cout << "moves! yeah\n";
            m = moves.top().dir;
            cout << valid_moves[m] << endl;
        }
    }
    //Oh boy, I'm screwed
    else cout << "pass" << endl;
}

void do_move() {
    // cout << current_round << endl;
    if (!isInitialized) initializeGraph();
    trackBugs();

    vector<string> valid_moves;
    vector<Point> valid_positions;
    for (int dir = 0; dir < 4; dir++) {
        int nextx = me.x + dx[dir];
        int nexty = me.y + dy[dir];
        if (nextx < 0 || nextx >= height || nexty < 0 || nexty >= width) continue;
        if (!is_wall[nextx][nexty]) {
            valid_moves.pb(moves_dir[dir]);
            valid_positions.pb(Point(nextx, nexty));
        }
    }
    if (!valid_moves.size()) valid_moves.pb("pass");
    
    int trap = 0;
    moveList bugAvoid, mostSafe, enemyAvoid, chase;
    //params:
    //snippet or weapon, my distance from the snippet,
    //number of snippets close to it, direction
    priority_queue<node> moves, corners;
    bool avoidHim[] = {false, false, false, false};
    bool enemy_threat = (!me.has_weapon && enemy.has_weapon);
    for (int i = 0; i < valid_positions.size(); i++){
        int x = valid_positions[i].x;
        int y = valid_positions[i].y;

        int me_from = bib[mp(x, y)];
        int enemy_from = bib[mp(enemy.x, enemy.y)];
        //The strategy is to avoid the enemy if a movement distance to him
        //is smaller equal than 1, he has a sword and I don't
        if (enemy_threat){
            //I already know that the enemy has a weapon.
            int dist = dijkstra(me_from, enemy_from);
            //He is chasing me! Aaaaaaah!
            if (dist <= 1){
                trap++;
                avoidHim[i] = true;
                continue;
            }
            //I'm still safe... but the enemy might chase me... Run to the hills!
            else if (!weapons.size() && !snippets.size())
                enemyAvoid.push(mp(dist, i));
        }
    }

    for (int i = 0; i < valid_positions.size(); i++){
        int x = valid_positions[i].x;
        int y = valid_positions[i].y;

        int me_from = bib[mp(x, y)];
        int enemy_from = bib[mp(enemy.x, enemy.y)];
        //don't use this move; a position is in observation
        dontMove[i] = 0;
        int observation = 0;

        //All movements are compromised :/
        if (trap == valid_positions.size()) continue;
        //This move is dangerous
        if (avoidHim[i]) continue;
        //Enemy is behind me. Stab him!
        if (isBehind) continue;
        
        //Avoid bugs if a movement distance to it is smaller than 2,
        //unless I have a sword
        moveList tmpBugAvoid;
        int bugsTooClose = 0;
        for (auto b : bugs){
            int bx = b.fi.fi;
            int by = b.fi.se;

            int to = bib[mp(bx, by)];
            //A bug is just being born
            //and it is not under me
            if ((bx == 6 && by == 9)){
                int d1 = dijkstra(me_from, bib[mp(5, 9)], enemy_threat);
                if (d1 <= 1){
                    bugsTooClose += bfs(me_from, enemy.has_weapon, 2);
                    mostSafe.push(mp(-bugsTooClose, i));
                    dontMove[i]++;
                    break;
                }
            }
            //A bug is just being born
            //and it is not under me
            else if (bx == 6 && by == 10){
                int d2 = dijkstra(me_from, bib[mp(5, 10)], enemy_threat);
                if (d2 <= 1){
                    bugsTooClose += bfs(me_from, enemy.has_weapon, 2);
                    mostSafe.push(mp(-bugsTooClose, i));
                    dontMove[i]++;
                    break;
                }
            }

            //Don't chose a path crossing with the enemy carrying a weapon!
            int dist = dijkstra(me_from, to);
            // cout << dist << endl;

            //Too many bugs too close. That's dangerous
            if (dist <= 2) bugsTooClose++;

            //Please, don't use this move. I'm might lose some snippets
            if (!me.has_weapon && dist == 0) dontMove[i]++;
            //If I hold my position, I might get a snippet on the next turn
            else if (!me.has_weapon && dist == 1) observation++;
            //Bug at a safe distance! Or I have a weapon :P
            else tmpBugAvoid.push(mp(-dist, i));
        }
        //There are bugs and I can't avoid them :/
        if (bugs.size() && dontMove[i]) continue;
        if (bugsTooClose >= 2 && !me.has_weapon) continue;

        //There is no need to check this move anymore.
        //It might be a trap.
        if (observation > 1 && !me.has_weapon) continue;

        //Number of bugs too close from me.
        mostSafe.push(mp(-bugsTooClose, i));

        //A safe position! Woohooo!
        while (!tmpBugAvoid.empty()){
            bugAvoid.push(tmpBugAvoid.top());
            tmpBugAvoid.pop();
        }

        //Just one bug is fine.
        if (observation == 1){
            if (!prev_bugs.empty()){
                //If there was a bug at this position before, you may use this move
                if (prev_bugs.count(mp(x, y))) observation = 0;
            }
        }

        //Strategic points
        if (!observation){
            for (auto q : strategic){
                int sx = q.fi.fi;
                int sy = q.fi.se;
                
                int to = bib[mp(sx, sy)];
                int bugsAround = bfs(to, enemy.has_weapon, 2);
                //Not so strategic
                if (bugsAround >= 2) continue;

                int dist = dijkstra(me_from, to, enemy_threat, !me.has_weapon);
                if (dist != INF) corners.push(node(1, dist, 0, i));
            }
        }

        //Now, calculate the distance between each snippet. Go for the closest one
        //which has more snippets close to it.
        map<int, int> closeSnippets, closeWeapons;
        for (int s = 0; s < snippets.size(); s++){
            int d = snippets[s].x;
            int e = snippets[s].y;

            int from = bib[mp(d, e)];
            for (int st = s+1; st < snippets.size(); st++){
                int f = snippets[st].x;
                int g = snippets[st].y;

                int to = bib[mp(f, g)];
                int dist_st = dijkstra(from, to);
                if (dist_st <= 3) closeSnippets[s]++;
            }
            for (int w = 0; w < weapons.size(); w++){
                int f = weapons[w].x;
                int g = weapons[w].y;

                int to = bib[mp(f, g)];
                int dist_st = dijkstra(from, to);
                if (dist_st <= 3){
                    closeSnippets[s]++;
                    closeWeapons[w]++;
                }
            }
        }
        // cout << "no error here! bugs\n";

        //If the distance to a snippet is smaller or equal than the enemy's distance to it,
        //I choose it as a possible movement. 
        for (int s = 0; s < snippets.size(); s++){
            int sx = snippets[s].x;
            int sy = snippets[s].y;

            int to = bib[mp(sx, sy)];
            //No bugs on the way!
            int dist = dijkstra(me_from, to, enemy_threat, !me.has_weapon);
            //Just a few won't hurt, I guess.
            // int distBugs = dijkstra(me_from, to, enemy_threat);

            int distEnemy = dijkstra(enemy_from, to, false, !enemy.has_weapon);
            // int distEnemyBugs = dijkstra(enemy_from, to);

            //Risky, but might be better.
            //ok if: dist <= distEnemy && dist <= distEnemyBugs
            //if: dist > distEnemy && distBugs > distEnemy, disconsider

            //Distance avoiding bugs is greater than enemy's avoiding bugs,
            //but it is smaller equal than enemy's both considering and disconsidering
            // if (!trap && dist > distEnemy && distBugs < distEnemy
            //     && distBugs < distEnemyBugs){
            //     dist = distBugs;
            // }
            // cout << dist << " " << s << endl;
            //Can't reach it. Just ignore.
            if (dist == INF){
                moves.push(node(5, dist, closeSnippets[s], i));
                continue;
            }

            //This is a collecting movement!
            if (dist == 0){
                if (!observation) moves.push(node(0, dist, closeSnippets[s], i));
                //-1 indicates that I will hold. By doing this,
                //I might have a chance to collect the item on the next move,
                //if the enemy is not a threat.
                else if (observation && !trap)
                    moves.push(node(-1, dist, closeSnippets[s], i));
            }
            else if (dist < distEnemy && !observation) moves.push(node(0, dist, closeSnippets[s], i));
            //Better not go after this snippet... the enemy might get it first.
            else if (distEnemy > 3 && !observation) moves.push(node(2, dist, closeSnippets[s], i));
            //Is this really necessary?
            else moves.push(node(5, dist, closeSnippets[s], i));
        }
        // cout << "no error here! snippets\n";
        //If the distance to a weapon is smaller or equal than the enemy's distance to it,
        //I choose it as a possible movement. 
        for (int w = 0; w < weapons.size(); w++){
            int wx = weapons[w].x;
            int wy = weapons[w].y;

            int to = bib[mp(wx, wy)];

            int dist = dijkstra(me_from, to, enemy_threat, !me.has_weapon);
            int distEnemy = dijkstra(enemy_from, to);

            //Cant reach it. Just ignore.
            if (dist == INF){
                moves.push(node(6, dist, closeWeapons[w], i));
                continue;
            }

            //This is a collecting movement!
            if (dist == 0){
                if (!observation) moves.push(node(0, dist, closeWeapons[w], i));
                //-1 indicates that I will hold. By doing this,
                //I might have a chance to collect the item on the next move,
                //if the enemy is not a threat.
                else if (observation && !trap)
                    moves.push(node(-1, dist, closeWeapons[w], i));
            }
            else if (dist+1 < distEnemy && !observation) moves.push(node(0, dist, closeWeapons[w], i));
            //Better not go after this weapon... the enemy might get it first.
            else if (distEnemy > 3 && !observation) moves.push(node(3, dist, closeWeapons[w], i));
            //Is this really necessary?
            else moves.push(node(6, dist, closeWeapons[w], i));
        }
        //I have a weapon and enemy does not
        if (me.has_weapon && !enemy.has_weapon){
            int dist = dijkstra(me_from, enemy_from, false, !me.has_weapon);
            int distActual = dijkstra(bib[mp(me.x, me.y)], enemy_from,
                false, !me.has_weapon);
            //Special case: enemy is behind me
            if (distActual == 1 && prev_position.size()){
                Point prev_me = prev_position.back(); prev_position.pop_back();
                //If his position now is a previous of mine
                if (enemy_from == bib[mp(prev_me.x, prev_me.y)] && dist == 0){
                    isBehind = true;
                    cout << valid_moves[i] << endl;
                }
            }
            chase.push(mp(-dist, i));
        }
    }

    if (!isBehind) choseMove(bugAvoid, enemyAvoid, mostSafe, chase, moves, valid_moves, corners);
    prev_position.clear();
    prev_position.pb(Point(me.x, me.y));
    prev_bugs.clear();
    prev_bugs = bugs;
    isBehind = false;
}
