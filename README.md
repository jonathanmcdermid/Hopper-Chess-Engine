![](images/hopperImage.png)

This is an amateur chess engine, and still in development. UCI compatibility for Arena is limited, but functional for standard play. The engine can run locally, so an external program like Arena is not required to play. 

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
alpha-beta pruning
iterative deepening 
principle variations
transposition tables
pawn hash table
zobrist keys
null moves 
zugswang detection
quiescent search
dynamic aspiration windows
pinned piece detection
double check detection
threatened squares
killer moves
perft
principal variation search
static exchange evaluation
```