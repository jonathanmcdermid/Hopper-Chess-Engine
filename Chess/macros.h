#pragma once

enum castle_enum { WKINGSIDE = 0, WQUEENSIDE, BKINGSIDE, BQUEENSIDE };

enum role_enum { EMPTY = 0, B_KING = -20005, B_QUEEN = -904, B_ROOK = -503, B_BISHOP = -322, B_KNIGHT = -301, B_PAWN = -100, W_PAWN = 100, W_KNIGHT = 301, W_BISHOP = 322, W_ROOK = 503, W_QUEEN = 904, W_KING = 20005 };

enum role_index_enum { PINDEX = 0, NINDEX, BINDEX, RINDEX, QINDEX, KINDEX };

enum move_enum { STANDARD = 0, DOUBLEPUSH = 1, KCASTLE = 2, QCASTLE = 3, CAPTURE = 4, ENPASSANT = 5, NULLFLAGS = 7, NPROMOTE = 8, BPROMOTE = 9, RPROMOTE = 10, QPROMOTE = 11, NPROMOTEC = 12, BPROMOTEC = 13, RPROMOTEC = 14, QPROMOTEC = 15 };

enum generation_enum { GENPV = 0, GENHASH, GENWINCAPS, GENKILLS, GENWINNONCAPS, GENLOSENONCAPS, GENLOSECAPS, GENEND };

enum hash_enum { HASHEXACT = 1, HASHBETA, HASHALPHA };

#define STARTFEN	"rnbqkbnr/pppppppp/11111111/11111111/11111111/11111111/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define DIAGslide(from, to) (abs((to) % WIDTH - (from) % WIDTH) - abs((to) / WIDTH - (from) / WIDTH)) ? false : true
#define NESWslide(from, to) (((to) - (from)) % BOARD_SOUTHWEST && ((to) - (from)) % BOARD_NORTHEAST) ? false : true
#define NWSEslide(from, to) (((to) - (from)) % BOARD_SOUTHEAST && ((to) - (from)) % BOARD_NORTHWEST) ? false : true
#define NSslide(from, to) ((from) % WIDTH - (to) % WIDTH) ? false : true
#define EWslide(from, to) ((from) / WIDTH - (to) / WIDTH) ? false : true

#define BLACK		false
#define WHITE		true

#define HASHSIZE	24000

#define UPPERLIMIT	30000
#define LOWERLIMIT	-30000
#define MATE		25000
#define CONTEMPT	-100

#define SPACES		64
#define WIDTH		8
#define MAXDEPTH	64
#define NOCHECK		-1

#define BOARD_NORTH			-8
#define BOARD_SOUTH			8
#define BOARD_EAST			1
#define BOARD_WEST			-1
#define BOARD_NORTHEAST		-7
#define BOARD_NORTHWEST		-9
#define BOARD_SOUTHEAST		9
#define BOARD_SOUTHWEST		7
#define BOARD_LEAP			0

#define NULLMOVE	28672

#define MEMORY		512

#define PAWN_PASSED					5
#define PAWN_ROOK_ON_PASSED			25
#define PAWN_ISOLATED				-20
#define PAWN_DOUBLED				-30
#define PAWN_TRIPLED				-100
#define PAWN_CONNECTED				10
#define PAWN_BACKWARD				-50
#define PAWN_PHALANX				10
#define PAWN_KNIGHT_OUTPOST			50
#define PAWN_BISHOP_OUTPOST			25

#define BONUS_BISHOP_PAIR			15
#define BONUS_ROOK_OPEN_FILE		50
#define BONUS_ROOK_HALF_OPEN_FILE	30
#define BONUS_ROOK_ON_KING_FILE		15
#define BONUS_SPOILED_CASTLE		-20
#define BONUS_QUEEN_SUPPORT			15