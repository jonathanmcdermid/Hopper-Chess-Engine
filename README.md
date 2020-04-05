# Chess-Engine

This is an amateur chess engine, and still in development. It has an ELO of ~ 2000. UCI compatibility for Arena is limited, but functional for standard play. The engine can run locally, so an external program like Arena is not required to play. 

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
hashing
transposition tables
zobrist keys
null moves
quiescent search
dynamic aspiration windows
pinned piece detection
```
Better UCI compatibility, killer moves, and time management are at the top of the to do list.
