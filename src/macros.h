#pragma once
#define BLACK		0
#define WHITE		1
#define NOCHECK		-1
#define LEAP		0
#define MAXDEPTH	20
#define HASHSIZE	30000
#define HASHEXACT	1
#define HASHBETA	2
#define HASHALPHA	3
#define UPPERLIMIT	30000
#define LOWERLIMIT	-30000
#define MATE		25000
#define CONTEMPT	-100
#define SPACES		64
#define WIDTH		8
#define NORTH		-8
#define SOUTH		8
#define EAST		1
#define WEST		-1
#define NORTHEAST	-7
#define NORTHWEST	-9
#define SOUTHEAST	9
#define SOUTHWEST	7
#define KING		20005
#define QUEEN		904
#define ROOK		503
#define BISHOP		322
#define KNIGHT		301
#define PAWN		100
#define KINDEX		5
#define QINDEX		4
#define RINDEX		3
#define BINDEX		2
#define NINDEX		1
#define PINDEX		0
#define EMPTY		0
#define STANDARD	0
#define DOUBLEPUSH	1
#define KCASTLE		2
#define QCASTLE		3
#define CAPTURE		4
#define ENPASSANT	5
#define NULLMOVE	6
#define FAIL		7	
#define PROMOTE		8
#define NPROMOTE	8
#define BPROMOTE	9
#define RPROMOTE	10
#define QPROMOTE	11
#define PROMOTEC	12
#define NPROMOTEC	12
#define BPROMOTEC	13
#define RPROMOTEC	14
#define QPROMOTEC	15
#define MEMORY		255
#define GENPV		0
#define GENHASH		1
#define GENCAPS		2
#define GENKILLS	3
#define GENNONCAPS	4
#define GENEND		5
#define STARTFEN	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define DIAGslide(from, to) (abs((to) % WIDTH - (from) % WIDTH) - abs((to) / WIDTH - (from) / WIDTH)) ? false : true
#define NESWslide(from, to) (((to) - (from)) % SOUTHWEST && ((to) - (from)) % NORTHEAST) ? false : true
#define NWSEslide(from, to) (((to) - (from)) % SOUTHEAST && ((to) - (from)) % NORTHWEST) ? false : true
#define NSslide(from, to) ((from) % WIDTH - (to) % WIDTH) ? false : true
#define EWslide(from, to) ((from) / WIDTH - (to) / WIDTH) ? false : true