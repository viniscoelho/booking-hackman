#ifndef CHOOSE_H
#define CHOOSE_H
#endif

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
        //Let's see which position is safer.
        if (!enemyAvoid.empty()){
            m = enemyAvoid.top().second;
            cout << valid_moves[m] << endl;
        }
        else cout << "pass" << endl;
    }
    //There are bugs and/or enemy and there is no way to avoid them.
    else if (bugs.size() && bugAvoid.empty()){
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
                    m = corners.top().dir;
                    cout << valid_moves[m] << endl;
                }
            }
            //Ops, something went wrong. I am trapped somehow.
            //Is enemy a threat as well?
            else if (!enemyAvoid.empty()){
                m = enemyAvoid.top().second;
                cout << valid_moves[m] << endl;
            }
            //Or just the bugs?
            else if (!mostSafe.empty()){
                m = mostSafe.top().second;
                cout << valid_moves[m] << endl;
            }
            //Or nothing :P
            else cout << "pass" << endl;
        }
        else{
            m = moves.top().dir;
            cout << valid_moves[m] << endl;
        }
    }
    //Oh boy, I'm screwed
    else cout << "pass" << endl;
}
