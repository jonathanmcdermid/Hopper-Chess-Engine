# Chess-Engine

This is an amateur chess engine, and still in development. UCI compatibility for Arena is currently limited to Player vs Engine. The engine runs smoothly locally, so an external program like Arena is not required to play. 

## Playing In Arena

To play the engine in Arena, generate a binary file using the source material and follow the standard process of installing an Engine in Arena. *limited functionality at the moment*

## Playing Locally

To play the engine without Arena, generate and run a binary file. Enter "local" in the cmd window. The board should subsequently be printed, and moves can be input in standard UCI format (a2a4).

## Engine Features

The engine utilizes the following methods and more.  

```
negamax algorithm
forsyth–edwards notation
material evaluation
mobility evaluation
pruning
iterative deepening 
principle variations
hashing
transposition tables
zobrist keys
null moves
quiescent search
dynamic aspiration windows
```
Better UCI compatibility, time management, FEN string saving, and a rare castling exception (king moves across attacked square) are at the top of the to do list.
