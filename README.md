# Tic-Tac-Toe Connection Oriented ![status](https://img.shields.io/badge/Status-Finished-red.svg)
Game Tic-Tac-Toe connection oriented. There is one client and one server playing against each other.

The game is played on a 5x5 grid until one of the players gets a three-mark line, or no more movements are possible.

## How to use it
Follow the next steps to try the script:

1. Download or clone the project.

2. In the same directory that you have the project, compile the `server1.c` file in the terminal.

```
gcc server1.c -o server1
```

3. Execute it assigning a port number, for example the _6000_.

```
./server1 6000
```

4. Now the server is waiting to establish a connection. Let's open a new terminal and compile the `client1.c` file.

```
gcc client1.c -o client1
```

5. Execute it assigning an IP and the same port number as the server. The IP could be for example _192.0.2.1_

```
./client1 192.0.2.1 6000
```

6. Start playing!
