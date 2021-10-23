#pragma once

#define STARTFEN	"rnbqkbnr/pppppppp/11111111/11111111/11111111/11111111/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define WHITE	false
#define BLACK	true

#define PAWN   0
#define KNIGHT 1
#define BISHOP 2
#define ROOK   3
#define QUEEN  4
#define KING   5

#define UPPERLIMIT	30000
#define LOWERLIMIT	-30000
#define MATE		25000
#define CONTEMPT	0

#define PANIC_THRESHOLD 50

#define SEE_THRESHOLD -30

#define NULLMOVE_GAMEPHASE_THRESHOLD 10

#define SPACES		64
#define WIDTH		8
#define MAXDEPTH	32

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

#define MEMORY		128

#define PAWN_ISOLATED				-10
#define PAWN_CONNECTED				5
#define PAWN_BACKWARD				-20
#define PAWN_PHALANX				0

#define BONUS_BISHOP_PAIR			15
#define BONUS_ROOK_OPEN_FILE		30
#define BONUS_ROOK_KING_FILE		15
#define BONUS_QUEEN_SUPPORT			5

#define DIAGslide(from, to) (abs((to) % WIDTH - (from) % WIDTH) - abs((to) / WIDTH - (from) / WIDTH)) ? false : true
#define NESWslide(from, to) (((to) - (from)) % BOARD_SOUTHWEST && ((to) - (from)) % BOARD_NORTHEAST) ? false : true
#define NWSEslide(from, to) (((to) - (from)) % BOARD_SOUTHEAST && ((to) - (from)) % BOARD_NORTHWEST) ? false : true
#define NSslide(from, to) ((from) % WIDTH - (to) % WIDTH) ? false : true
#define EWslide(from, to) ((from) / WIDTH - (to) / WIDTH) ? false : true

enum castle_enum : int { WKINGSIDE, WQUEENSIDE, BKINGSIDE, BQUEENSIDE };

enum role_enum : int { WHITE_PAWN, BLACK_PAWN, WHITE_KNIGHT, BLACK_KNIGHT, WHITE_BISHOP, BLACK_BISHOP, WHITE_ROOK, BLACK_ROOK, WHITE_QUEEN, BLACK_QUEEN, WHITE_KING, BLACK_KING, EMPTY };

enum move_enum : int { STANDARD, DOUBLEPUSH, KCASTLE, QCASTLE, CAPTURE, ENPASSANT, NULLFLAGS = 7, NPROMOTE, BPROMOTE, RPROMOTE, QPROMOTE, NPROMOTEC, BPROMOTEC, RPROMOTEC, QPROMOTEC };

enum generation_enum : int { GENPV, GENHASH, GENWINCAPS, GENKILLPRIMARY, GENKILLSECONDARY, GENNONCAPS, GENLOSECAPS, GENEND };

enum hash_enum : int { HASHEXACT, HASHBETA, HASHALPHA };

static inline bool validPiece(int p, bool t) { return ((t  == (p & 1)) && p != EMPTY); }

static inline bool enemyPiece(int p, bool t) { return (t ^ (p & 1)) && p != EMPTY; }

static inline int pawnPush(int p) { return (p & 1) ? BOARD_SOUTH : BOARD_NORTH; }