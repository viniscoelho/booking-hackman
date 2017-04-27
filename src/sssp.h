#ifndef SSSP_H
#define SSSP_H
#endif

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
//-----------------------------------------------------------------------------
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
