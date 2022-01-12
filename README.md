# Hide And Seek

## Members
Period 4 \
Maxwell Zen, Jennifer Sun, Alyssa Choi

## Description
A multiplayer hide-and-seek game. Players will join and set their username, and wait for the server to get 4 players. Then, the seeker (or seekers) will see within a limited radius of their position, and their job is to find the hiders and catch them. If the hiders survive past a certain time limit, they win, otherwise if they are all eliminated in the time limit the seeker wins. We can also create a flashlight for seekers that only shows the area in the direction the flashlight is pointing, and potentially sound cues for hiders when a seeker is nearby.

Below is a sample map that all the players will share. There will be walls placed throughout the map that blocks players from going through. There is one seeker and multiple hiders. In the seeker's perspective, the map will be blacked out except for a limited front view representing the seeker's flashlight. The seeker will have slightly faster speed than the hiders. If the seeker "tags" a hider, the hider will be eliminated and removed from the map. After all of the hiders are eliminated, a scoreboard will be displayed with the rank of the hiders. 

S = Seeker 
H = Hider 
 _________________________
|                         |                      
|    H        ===         |
|                   H     |
|              /          |
|======      S  --        |
|              \          |
|     ===                 |
|                         |
|       H              ===|
|_________________________|


We will use the ncurses library to create the user interface, and other standard libraries, but nothing else.

This project will use:
- Allocating memory (to create structs that represent players / obstacles)
- Working with files, finding information about files (username corresponds with an account and a game history that can be loaded)
- Processes (server that waits for players will fork when enough players join)
- Signals (after ctrl+c other players will be alerted that one player has left)
- Sockets (to implement server communication)

We will split the work as follows:
- Maxwell: sockets and updating game information through the server
- Jennifer: login system, accessing data files
- Alyssa: signals, keystrokes, and the user interface
- Everyone: game logic and design

Our timeline will be:
- Jan 13: users can login through the user interface, server waits for 4 people to fork
- Jan 16: a basic version of the game will be implemented
- Jan 19: user interface will be improved, signals will be implemented, flashlights will be added
- Beyond: add new features / touch up old ones
