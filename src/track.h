#ifndef TRACK_H
#define TRACK_H
#endif

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
