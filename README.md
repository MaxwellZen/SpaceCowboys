# Hide And Seek

## Members
Period 4 \
Maxwell Zen, Jennifer Sun, Alyssa Choi

## Description
A multiplayer hide-and-seek game. Players will join and set their username, and wait for the server to get 4 players. Then, the seeker (or seekers) will see within a limited radius of their position, and their job is to find the hiders and catch them. If the hiders survive past a certain time limit, they win, otherwise if they are all eliminated in the time limit the seeker wins.

We will use the ncurses library to create the user interface, and other standard libraries, but nothing else.

This project will use:
- Allocating memory (to create structs that represent players / obstacles)
- Working with files, finding information about files (username corresponds with an account and a game history that can be loaded)
- Processes (server that waits for players will fork when enough players join)
- Signals (after ctrl+c other players will be alerted that one player has left)
- Sockets (to implement server communication)
