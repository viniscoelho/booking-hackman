
## Booking.com Hack-man

In this game, your own bot will play against a single other bot, and the one that collects the most code snippets wins. Of course there is a bit more to it than that, because your bot has to consider things such as bugs (enemies), weapons and the maze-like playing field. Upload a bot on the "Your Bot" page. For more information about how to create a bot, have a look at the "Getting Started" page. After uploading and successfully compiling, set your bot to active and it will automatically start to play matches against other bots in the leaderboard.

## Game basics
As stated above, your bot will be in a maze-like field against another bot. The characters are Booking.com's Bixies, both running around in the gamified head office of Booking.com in Amsterdam! Code snippets will randomly spawn on the field, the goal is to collect those and your bot will have to be faster at doing this than your opponent, while avoiding bugs. Touching a bug means some of the collected code snippets get corrupted and your bot loses them. Luckily, a sword may occasionally spawn that can be used to defeat a bug or even on your opponent, so that some of his code snippets get redistributed across the field. The sword breaks after a single use.

## In detail
* The game starts with 2 code snippets on the map, a new one will randomly spawn every 8 game steps.
A new bug will appear from the server room (center of the field) every 5 collected code snippets (total of both players).
* For every 8 collected code snippets, a weapon will spawn randomly on the field. A weapon can be used once on a bug or the opponent.
* A collision is defined as two players, or a player and a bug, being in the same position in the same round, or swapping positions in the same round (i.e. they are in the same position half way through the round). Swaps are calculated before same positions.
* If your bot collides with a bug, without a weapon, it loses 4 code snippets. The bug disappears.
* If your bot collides with a bug, with a weapon, the bug will be squashed and no snippets will be lost.
* If your bot collides with the opponent, without a weapon, nothing happens.
* If your bot collides with the opponent, with a weapon, the opponent loses 4 snippets that will be distributed across the field. Additionally the opponent is unable to move for 1 time step. If both players have a weapon, both players lose 4 snippets.
* When a player collides with multiple bugs or opponents at the same time, the collisions will be calculated for each bug/opponent separately. The weapon is only used in one collision. Bug collisions are calculated before opponent collisions.
* Because both players move at the same time each step, it is possible the players both players will be on a snippet, weapon or bug at the same time. When on a weapon or snippet, one of the players will get the item by random chance. When on a bug, the interaction will be calculated for each player as normal.

## Winning the game
The game will end after 200 steps, and the bot with the most code snippets wins the game. If both players have an equal amount of snippets, it's a draw.

It's possible that one of the players loses more snippets, due to a bug or opponent with weapon, than he currently carries. If this happens, that player loses automatically and the other player wins. If both players have a negative amount of snippets at the same time, it's a draw again.
