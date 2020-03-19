# Chess-Engine

This is an amateur chess engine, and still in development. UCI compatibility for Arena is currently limited to Player vs Engine. The engine runs smoothly locally, so an external program like Arena is not required to play. 

## Playing In Arena

To play the engine in Arena, compile a binary file using the source material and follow the standard process of loading the Engine in arena. *limited functionality at the moment*

## Playing Locally

To play the engine without Arena, generate and run .exe file. Enter "local" in the cmd window. The board should subsequently be printed, and moves can be input in standard UCI format (a2a4).

## Engine Features

The engine utilizes the following methods and more.  

```
negamax algorithm
material evaluation
mobility evaluation
pruning
iterative deepening 
principle variations
hashtables
zobrist keys
null moves
quiescent search
dynamic alpha beta windows
```
