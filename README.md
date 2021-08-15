# terminal-tris
Terminal-based guideline-conforming tetra-block stacking game. Uses ncurses for console printing.

![terminaltris gameplay](assets/matrix.png)

Well, mostly guideline-conforming:
- Uses the SRS rotation system (i know, i know, but T-spin triples are just too good). 
- "inifitt" lock delay resets are possible, but by default only 15 moves are allowed between step resets. Additional options inside the source enable limiting the number of floorkicks each piece can use, though by default this limit is infinite.
- The 3-corner Tspin detection method is not used. Tspins are recognized if the T becomes unable to move where it is locked, known as the "immobile" system. This gets rid of the EZ tspin that is usually caused by SRS.
- Mini Tspins do not exist. Even if a Tspin uses a wallkick if it is immobile it gives full Tspin points.
- Perfect clears do not use some weird score table, rather they simply multiply whatever amount of points you would have gotten by 5.
- Topping out (a block locks above the top edge of the playfield) immediately ends the game. Blocking out (a new piece attempts to spawn but overlaps a block) is impossible because a piece will not drop down from above the playfield if it would overlap.
- The gravity starts out at 1/60 G, or falling one block per second. This remains constant through the first 4 levels because there is not enough resolution to really separate them. This is not a big deal.
- At level 19. gravity reaches 20G. From there on to the maximum level 30, delays get shorter, e.g. lock delay starts out at 0.5 seconds, but shorten down to a ludicrous 0.1 seconds at level 30. Good luck.
- The controls are so far noncustomizable: Left/Right arrows control shifting, Down arrow soft drops (no locking), Up arrow rotates CW, Z rotates CCW, C holds, and the spacebar hard locks.
- The only mode so far is endless marathon. Time-based lineclear objective modes are set for the future.
- There is no music. Sorry!

When compiling make sure to link ncurses:
- [whatever compiler] *.c \-lncurses


You must have ncurses installed, find it ![here](https://invisible-island.net/ncurses/)
