# Space Cowboys

## Members
Period 4 \
Maxwell Zen, Jennifer Sun, Alyssa Choi

## Description
A multiplayer hide-and-seek game. Players will join and set their username, and wait for the server to get 4 players. Then, the seeker will see within a limited radius of their position and within a certain direction, and their job is to find and catch the hiders. If the hiders survive past a certain time limit, they win, otherwise if they are all eliminated in the time limit the seeker wins.

Below is a sample map that all the players will share. There will be obstacles placed throughout the map that blocks players from going through. There is one seeker and multiple hiders. In the seeker's perspective, the map will be blacked out except for a limited front view representing the seeker's flashlight. The seeker will have slightly faster speed than the hiders. If the seeker "tags" a hider, the hider will be eliminated and removed from the map. After all of the hiders are eliminated, a scoreboard will be displayed with the rank of the hiders. 

X = Seeker 

O = Hider 

![alt text](https://raw.githubusercontent.com/MaxwellZen/HideAndSeek/main/game_model.png)

We will use the ncurses library to create the user interface, and other standard libraries, but nothing else.

This project will use:
- Allocating memory (to create structs that represent players / obstacles)
- Working with files, finding information about files (username corresponds with an account and a game history that can be loaded)
- Processes (server that waits for players will fork when enough players join)
- Signals (after ctrl+c sockets have to be closed and ncurses mode will have to be exited)
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

## Instructions
First, set up the server at any known IP address
```
git clone git@github.com:MaxwellZen/SpaceCowboys.git
cd SpaceCowboys
make serverside
./server
```
Then, perform the client side commands on four terminals / computers, and make sure that the terminal window is 80 columns wide and 24 rows tall (any other dimensions may produce unintended behavior)
```
git clone git@github.com:MaxwellZen/SpaceCowboys.git
cd SpaceCowboys
make
./game
```
The program will first prompt the user for an IP address - if the IP address is incorrect, the program will end and you can run the program again to enter a correct address.

After connecting to the server, there will be a couple straightforward prompts asking for whether you'd like to login to an existing account or create a new account, as well as what you'd like your username to be. If you accidentally pick Login instead of Create Account (or vice versa), don't worry - there will be an option to switch modes if the server determines you've entered an incorrect username. Once those are completed, you will be taken to a waiting screen to wait for other players to join.

Once other players have joined, you will be taken to a game screen. At the bottom, there is a message indicating whether you are a hider or a seeker. You can use the arrow keys to move around one tile at a time, the z key to rotate your flashlight clockwise, and the x key to rotate your flashlight counterclockwise. As a seeker, reaching any square on or adjacent to a hider will eliminate that hider, and as a hider, the goal is to avoid the seeker.

After the game ends, results will be printed on to the terminal and you will be prompted to decide whether you want to continue to a new game or end the program.

## Devlog
```
Jan 11, 11:14 AM - Maxwell - started project proposal
Jan 12, 10:46 AM - Alyssa - revised project proposal
Jan 15, 2:17 PM - Maxwell - created files and added basic structure
Jan 15, 2:45 PM - Jennifer - added libraries
Jan 15, 3:57 PM - Maxwell - updated makefile, implemented select to connect with clients
Jan 15, 4:46 PM - Jennifer - wrote ncurses setup; made map array with obstacles; worked on fgets phase 1; made struct user with username, stats
Jan 15, 6:41 PM - Alyssa - finished phase 3 (game setup) client communication; started phase 4 (key signal) client communication
Jan 15, 6:48 PM - Maxwell - implemented phase 2 and phase 3 server communication and game setup
Jan 15, 7:02 PM - Alyssa - added client handshake from networking.h
Jan 15, 9:52 PM - Maxwell - implemented phase 4 server communication
Jan 15, 10:41 PM - Jennifer - wrote login system functions, resized map
Jan 16, 10:19 AM - Alyssa - removed trailing characters so only current cursor is displayed in the game map 
Jan 16, 10:35 AM - Jennifer - added emojis, changed text and map colors; made cursor disappear in the game but appear after exiting with signal handling
Jan 16, 10:48 AM - Alyssa - added game_display in game.h
Jan 16, 12:17 PM - Alyssa - fixed game map display by adding borders
Jan 16, 1:19 PM - Maxwell - integrated game up to phase 4
Jan 16, 1:52 PM - Alyssa - started flashlight view by restricting player view within a certain radius
Jan 16, 1:55 PM - Maxwell - added label with time left
Jan 16, 3:00 AM - Jennifer - wrote phase 5
Jan 16, 3:28 PM - Maxwell - implemented transition from phase 4 to 5
Jan 16, 5:18 PM - Maxwell - implemented flashlight view, added bottom text and hiders left text, various fixes
Jan 20, 1:38 AM - Maxwell - added undo_game_setup method to exit ncurses mode, added method for entering new game
Jan 21, 3:26 PM - Alyssa - added install section in README.md
Jan 23, 5:26 PM - Jennifer - changed image name and text formatting
Jan 23, 8:41 PM - Maxwell - added ncurses mode for phase 2 that can view game history
Jan 23, 11:54 PM - Jennifer - fixed login system
Jan 24, 4:47 AM - Maxwell - added game history file system and server communication
Jan 24, 11:02 AM - Maxwell - added prompt for server ip address
```
