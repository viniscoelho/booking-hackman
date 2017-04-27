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

#include "src/definitions.h"
#include "src/sssp.h"
#include "src/choose.h"
#include "src/track.h"

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
