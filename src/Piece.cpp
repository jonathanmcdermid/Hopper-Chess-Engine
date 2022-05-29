#pragma once

#include "Piece.h"
#include "Board.h"

namespace Hopper
{
    Piece::Piece(position* posi, int p, bool s, role_enum re) {
        myPosition = posi;
        pos = p;
        side = s;
        roleVal = re;
        side = s;
    }

    std::vector<Move> Empty::getAllMoves() {
        std::vector<Move> moveVec;
        return moveVec;
    }

    std::vector<Move> Empty::getQuietMoves() {
        std::vector<Move> moveVec;
        return moveVec;
    }

    std::vector<Move> Empty::getCaptureMoves() {
        std::vector<Move> moveVec;
        return moveVec;
    }

    std::vector<Move> Pawn::getAllMoves() {
        std::vector<Move> moveVec;
        int i = (side == BLACK) ? BOARD_SOUTH : BOARD_NORTH;
        if (pos % WIDTH) {
            if (enemyPiece(myPosition->getPieceValAt(pos + i + BOARD_WEST), side)) {
                if ((!side && pos > 15) || (side && pos < 48))
                    moveVec.push_back(Move(pos, pos + i + BOARD_WEST, CAPTURE));
                else {
                    moveVec.push_back(Move(pos, pos + i + BOARD_WEST, QPROMOTEC));
                    moveVec.push_back(Move(pos, pos + i + BOARD_WEST, NPROMOTEC));
                    moveVec.push_back(Move(pos, pos + i + BOARD_WEST, BPROMOTEC));
                    moveVec.push_back(Move(pos, pos + i + BOARD_WEST, RPROMOTEC));
                }
            }
        }
        if (pos % WIDTH != 7) {
            if (enemyPiece(myPosition->getPieceValAt(pos + i + BOARD_EAST), side)) {
                if ((!side && pos > 15) || (side && pos < 48))
                    moveVec.push_back(Move(pos, pos + i + BOARD_EAST, CAPTURE));
                else {
                    moveVec.push_back(Move(pos, pos + i + BOARD_EAST, QPROMOTEC));
                    moveVec.push_back(Move(pos, pos + i + BOARD_EAST, NPROMOTEC));
                    moveVec.push_back(Move(pos, pos + i + BOARD_EAST, BPROMOTEC));
                    moveVec.push_back(Move(pos, pos + i + BOARD_EAST, RPROMOTEC));
                }
            }
        }
        if (myPosition->getPieceValAt(pos + i) == EMPTY) {
            if ((!side && pos > 15) || (side && pos < 48)) {
                moveVec.push_back(Move(pos, pos + i, STANDARD));
                if (((!side && pos > 47) || (side && pos < 16)) && myPosition->getPieceValAt(pos + 2 * i) == EMPTY)
                    moveVec.push_back(Move(pos, pos + 2 * i, DOUBLEPUSH));
            }
            else {
                moveVec.push_back(Move(pos, pos + i, QPROMOTE));
                moveVec.push_back(Move(pos, pos + i, NPROMOTE));
                moveVec.push_back(Move(pos, pos + i, BPROMOTE));
                moveVec.push_back(Move(pos, pos + i, RPROMOTE));
            }
        }
        if (myPosition->getPrevMove().getFlags()   == DOUBLEPUSH &&
          ((myPosition->getPrevMove().getTo()      == pos + BOARD_EAST && pos % WIDTH != 7)||
           (myPosition->getPrevMove().getTo()      == pos + BOARD_WEST && pos % WIDTH)))
        {
            moveVec.push_back(Move(pos, myPosition->getPrevMove().getTo() + i, ENPASSANT));
        }
        return moveVec;
    }

    std::vector<Move> Pawn::getQuietMoves() {
        std::vector<Move> moveVec;
        int i = (side == BLACK) ? BOARD_SOUTH : BOARD_NORTH;
        if (myPosition->getPieceValAt(pos + i) == EMPTY)
        {
            if ((!side && pos > 15) || (side && pos < 48)) {
                moveVec.push_back(Move(pos, pos + i, STANDARD));
                if (((!side && pos > 47) || (side && pos < 16)) && myPosition->getPieceValAt(pos + 2 * i) == EMPTY)
                    moveVec.push_back(Move(pos, pos + 2 * i, DOUBLEPUSH));
            }
            else {
                moveVec.push_back(Move(pos, pos + i, QPROMOTE));
                moveVec.push_back(Move(pos, pos + i, NPROMOTE));
                moveVec.push_back(Move(pos, pos + i, BPROMOTE));
                moveVec.push_back(Move(pos, pos + i, RPROMOTE));
            }
        }
        return moveVec;
    }

    std::vector<Move> Pawn::getCaptureMoves() {
        std::vector<Move> moveVec;
        int i = (side == BLACK) ? BOARD_SOUTH : BOARD_NORTH;
        if (pos % WIDTH && enemyPiece(myPosition->getPieceValAt(pos + i + BOARD_WEST), side)) {
            if ((!side && pos > 15) || (side && pos < 48))
                moveVec.push_back(Move(pos, pos + i + BOARD_WEST, CAPTURE));
            else {
                moveVec.push_back(Move(pos, pos + i + BOARD_WEST, QPROMOTEC));
                moveVec.push_back(Move(pos, pos + i + BOARD_WEST, NPROMOTEC));
                moveVec.push_back(Move(pos, pos + i + BOARD_WEST, BPROMOTEC));
                moveVec.push_back(Move(pos, pos + i + BOARD_WEST, RPROMOTEC));
            }
        }
        if (pos % WIDTH != 7 && enemyPiece(myPosition->getPieceValAt(pos + i + BOARD_EAST), side)) {
            if ((!side && pos > 15) || (side && pos < 48))
                moveVec.push_back(Move(pos, pos + i + BOARD_EAST, CAPTURE));
            else {
                moveVec.push_back(Move(pos, pos + i + BOARD_EAST, QPROMOTEC));
                moveVec.push_back(Move(pos, pos + i + BOARD_EAST, NPROMOTEC));
                moveVec.push_back(Move(pos, pos + i + BOARD_EAST, BPROMOTEC));
                moveVec.push_back(Move(pos, pos + i + BOARD_EAST, RPROMOTEC));
            }
        }
        if (myPosition->getPrevMove().getFlags()  == DOUBLEPUSH &&
           ((myPosition->getPrevMove().getTo()    == pos + BOARD_EAST && pos % WIDTH != 7) ||
            (myPosition->getPrevMove().getTo()    == pos + BOARD_WEST && pos % WIDTH)))
        {
            moveVec.push_back(Move(pos, myPosition->getPrevMove().getTo() + i, ENPASSANT));
        }
        return moveVec;
    }

    std::vector<Move> Knight::getAllMoves() {
        std::vector<Move> moveVec;
        if ((pos + 10) % WIDTH > pos % WIDTH && pos < 54) {
            if (myPosition->getPieceValAt(pos + 10) == EMPTY)
                moveVec.push_back(Move(pos, pos + 10, STANDARD));
            else if (enemyPiece(myPosition->getPieceValAt(pos + 10), side))
                moveVec.push_back(Move(pos, pos + 10, CAPTURE));
        }
        if ((pos + 17) % WIDTH > pos % WIDTH && pos < 47) {
            if (myPosition->getPieceValAt(pos + 17) == EMPTY)
                moveVec.push_back(Move(pos, pos + 17, STANDARD));
            else if (enemyPiece(myPosition->getPieceValAt(pos + 17), side))
                moveVec.push_back(Move(pos, pos + 17, CAPTURE));
        }
        if ((pos - 10) % WIDTH < pos % WIDTH && pos > 9) {
            if (myPosition->getPieceValAt(pos - 10) == EMPTY)
                moveVec.push_back(Move(pos, pos - 10, STANDARD));
            else if (enemyPiece(myPosition->getPieceValAt(pos - 10), side))
                moveVec.push_back(Move(pos, pos - 10, CAPTURE));
        }
        if ((pos - 17) % WIDTH < pos % WIDTH && pos > 16) {
            if (myPosition->getPieceValAt(pos - 17) == EMPTY)
                moveVec.push_back(Move(pos, pos - 17, STANDARD));
            else if (enemyPiece(myPosition->getPieceValAt(pos - 17), side))
                moveVec.push_back(Move(pos, pos - 17, CAPTURE));
        }
        if ((pos + 6) % WIDTH < pos % WIDTH && pos < 58) {
            if (myPosition->getPieceValAt(pos + 6) == EMPTY)
                moveVec.push_back(Move(pos, pos + 6, STANDARD));
            else if (enemyPiece(myPosition->getPieceValAt(pos + 6), side))
                moveVec.push_back(Move(pos, pos + 6, CAPTURE));
        }
        if ((pos + 15) % WIDTH < pos % WIDTH && pos < 49) {
            if (myPosition->getPieceValAt(pos + 15) == EMPTY)
                moveVec.push_back(Move(pos, pos + 15, STANDARD));
            else if (enemyPiece(myPosition->getPieceValAt(pos + 15), side))
                moveVec.push_back(Move(pos, pos + 15, CAPTURE));
        }
        if ((pos - 6) % WIDTH > pos % WIDTH && pos > 5) {
            if (myPosition->getPieceValAt(pos - 6) == EMPTY)
                moveVec.push_back(Move(pos, pos - 6, STANDARD));
            else if (enemyPiece(myPosition->getPieceValAt(pos - 6), side))
                moveVec.push_back(Move(pos, pos - 6, CAPTURE));
        }
        if ((pos - 15) % WIDTH > pos % WIDTH && pos > 14) {
            if (myPosition->getPieceValAt(pos - 15) == EMPTY)
                moveVec.push_back(Move(pos, pos - 15, STANDARD));
            else if (enemyPiece(myPosition->getPieceValAt(pos - 15), side))
                moveVec.push_back(Move(pos, pos - 15, CAPTURE));
        }
        return moveVec;
    }

    std::vector<Move> Knight::getQuietMoves() {
        std::vector<Move> moveVec;
        if ((pos + 10) % WIDTH > pos % WIDTH && pos < 54 && myPosition->getPieceValAt(pos + 10) == EMPTY)
            moveVec.push_back(Move(pos, pos + 10, STANDARD));
        if ((pos + 17) % WIDTH > pos % WIDTH && pos < 47 && myPosition->getPieceValAt(pos + 17) == EMPTY)
            moveVec.push_back(Move(pos, pos + 17, STANDARD));
        if ((pos - 10) % WIDTH < pos % WIDTH && pos > 9 && myPosition->getPieceValAt(pos - 10) == EMPTY)
            moveVec.push_back(Move(pos, pos - 10, STANDARD));
        if ((pos - 17) % WIDTH < pos % WIDTH && pos > 16 && myPosition->getPieceValAt(pos - 17) == EMPTY)
            moveVec.push_back(Move(pos, pos - 17, STANDARD));
        if ((pos + 6) % WIDTH < pos % WIDTH && pos < 58 && myPosition->getPieceValAt(pos + 6) == EMPTY)
            moveVec.push_back(Move(pos, pos + 6, STANDARD));
        if ((pos + 15) % WIDTH < pos % WIDTH && pos < 49 && myPosition->getPieceValAt(pos + 15) == EMPTY)
            moveVec.push_back(Move(pos, pos + 15, STANDARD));
        if ((pos - 6) % WIDTH > pos % WIDTH && pos > 5 && myPosition->getPieceValAt(pos - 6) == EMPTY)
            moveVec.push_back(Move(pos, pos - 6, STANDARD));
        if ((pos - 15) % WIDTH > pos % WIDTH && pos > 14 && myPosition->getPieceValAt(pos - 15) == EMPTY)
            moveVec.push_back(Move(pos, pos - 15, STANDARD));
        return moveVec;
    }

    std::vector<Move> Knight::getCaptureMoves() {
        std::vector<Move> moveVec;
        if ((pos + 10) % WIDTH > pos % WIDTH && pos < 54 && enemyPiece(myPosition->getPieceValAt(pos + 10), side))
            moveVec.push_back(Move(pos, pos + 10, CAPTURE));
        if ((pos + 17) % WIDTH > pos % WIDTH && pos < 47 && enemyPiece(myPosition->getPieceValAt(pos + 17), side))
            moveVec.push_back(Move(pos, pos + 17, CAPTURE));
        if ((pos - 10) % WIDTH < pos % WIDTH && pos > 9 && enemyPiece(myPosition->getPieceValAt(pos - 10), side))
            moveVec.push_back(Move(pos, pos - 10, CAPTURE));
        if ((pos - 17) % WIDTH < pos % WIDTH && pos > 16 && enemyPiece(myPosition->getPieceValAt(pos - 17), side))
            moveVec.push_back(Move(pos, pos - 17, CAPTURE));
        if ((pos + 6) % WIDTH < pos % WIDTH && pos < 58 && enemyPiece(myPosition->getPieceValAt(pos + 6), side))
            moveVec.push_back(Move(pos, pos + 6, CAPTURE));
        if ((pos + 15) % WIDTH < pos % WIDTH && pos < 49 && enemyPiece(myPosition->getPieceValAt(pos + 15), side))
            moveVec.push_back(Move(pos, pos + 15, CAPTURE));
        if ((pos - 6) % WIDTH > pos % WIDTH && pos > 5 && enemyPiece(myPosition->getPieceValAt(pos - 6), side))
            moveVec.push_back(Move(pos, pos - 6, CAPTURE));
        if ((pos - 15) % WIDTH > pos % WIDTH && pos > 14 && enemyPiece(myPosition->getPieceValAt(pos - 15), side))
            moveVec.push_back(Move(pos, pos - 15, CAPTURE));
        return moveVec;
    }

    std::vector<Move> Bishop::getAllMoves() {
        std::vector<Move> moveVec;
        for (int i = pos + BOARD_NORTHEAST; i % WIDTH > pos % WIDTH && i >= 0; i += BOARD_NORTHEAST) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        for (int i = pos + BOARD_NORTHWEST; i % WIDTH < pos % WIDTH && i >= 0; i += BOARD_NORTHWEST) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        for (int i = pos + BOARD_SOUTHEAST; i % WIDTH > pos % WIDTH && i < SPACES; i += BOARD_SOUTHEAST) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        for (int i = pos + BOARD_SOUTHWEST; i % WIDTH < pos % WIDTH && i < SPACES; i += BOARD_SOUTHWEST) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        return moveVec;
    }

    std::vector<Move> Bishop::getQuietMoves() {
        std::vector<Move> moveVec;
        for (int i = pos + BOARD_NORTHEAST; i % WIDTH > pos % WIDTH && i >= 0; i += BOARD_NORTHEAST) {
            if (myPosition->getPieceValAt(i) != EMPTY)
                break;
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        for (int i = pos + BOARD_NORTHWEST; i % WIDTH < pos % WIDTH && i >= 0; i += BOARD_NORTHWEST) {
            if (myPosition->getPieceValAt(i) != EMPTY)
                break;
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        for (int i = pos + BOARD_SOUTHEAST; i % WIDTH > pos % WIDTH && i < SPACES; i += BOARD_SOUTHEAST) {
            if (myPosition->getPieceValAt(i) != EMPTY)
                break;
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        for (int i = pos + BOARD_SOUTHWEST; i % WIDTH < pos % WIDTH && i < SPACES; i += BOARD_SOUTHWEST) {
            if (myPosition->getPieceValAt(i) != EMPTY)
                break;
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        return moveVec;
    }

    std::vector<Move> Bishop::getCaptureMoves() {
        std::vector<Move> moveVec;
        for (int i = pos + BOARD_NORTHEAST; i % WIDTH > pos % WIDTH && i >= 0; i += BOARD_NORTHEAST) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
        }
        for (int i = pos + BOARD_NORTHWEST; i % WIDTH < pos % WIDTH && i >= 0; i += BOARD_NORTHWEST) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
        }
        for (int i = pos + BOARD_SOUTHEAST; i % WIDTH > pos % WIDTH && i < SPACES; i += BOARD_SOUTHEAST) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
        }
        for (int i = pos + BOARD_SOUTHWEST; i % WIDTH < pos % WIDTH && i < SPACES; i += BOARD_SOUTHWEST) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
        }
        return moveVec;
    }

    std::vector<Move> Rook::getAllMoves() {
        std::vector<Move> moveVec;
        for (int i = pos + BOARD_NORTH; i >= 0; i += BOARD_NORTH) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        for (int i = pos + BOARD_SOUTH; i < SPACES; i += BOARD_SOUTH) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        for (int i = pos + BOARD_EAST; i % WIDTH; i += BOARD_EAST) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        for (int i = pos + BOARD_WEST; i % WIDTH != 7 && i >= 0; i += BOARD_WEST) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        return moveVec;
    }

    std::vector<Move> Rook::getQuietMoves() {
        std::vector<Move> moveVec;
        for (int i = pos + BOARD_NORTH; i >= 0; i += BOARD_NORTH) {
            if (myPosition->getPieceValAt(i) != EMPTY)
                break;
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        for (int i = pos + BOARD_SOUTH; i < SPACES; i += BOARD_SOUTH) {
            if (myPosition->getPieceValAt(i) != EMPTY)
                break;
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        for (int i = pos + BOARD_EAST; i % WIDTH; i += BOARD_EAST) {
            if (myPosition->getPieceValAt(i) != EMPTY)
                break;
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        for (int i = pos + BOARD_WEST; i % WIDTH != 7 && i >= 0; i += BOARD_WEST) {
            if (myPosition->getPieceValAt(i) != EMPTY)
                break;
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        return moveVec;
    }

    std::vector<Move> Rook::getCaptureMoves() {
        std::vector<Move> moveVec;
        for (int i = pos + BOARD_NORTH; i >= 0; i += BOARD_NORTH) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
        }
        for (int i = pos + BOARD_SOUTH; i < SPACES; i += BOARD_SOUTH) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
        }
        for (int i = pos + BOARD_EAST; i % WIDTH; i += BOARD_EAST) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
        }
        for (int i = pos + BOARD_WEST; i % WIDTH != 7 && i >= 0; i += BOARD_WEST) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
        }
        return moveVec;
    }

    std::vector<Move> Queen::getAllMoves() {
        std::vector<Move> moveVec;
        for (int i = pos + BOARD_NORTH; i >= 0; i += BOARD_NORTH) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        for (int i = pos + BOARD_SOUTH; i < SPACES; i += BOARD_SOUTH) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        for (int i = pos + BOARD_EAST; i % WIDTH; i += BOARD_EAST) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        for (int i = pos + BOARD_WEST; i % WIDTH != 7 && i >= 0; i += BOARD_WEST) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        for (int i = pos + BOARD_NORTHEAST; i % WIDTH > pos % WIDTH && i >= 0; i += BOARD_NORTHEAST) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        for (int i = pos + BOARD_NORTHWEST; i % WIDTH < pos % WIDTH && i >= 0; i += BOARD_NORTHWEST) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        for (int i = pos + BOARD_SOUTHEAST; i % WIDTH > pos % WIDTH && i < SPACES; i += BOARD_SOUTHEAST) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        for (int i = pos + BOARD_SOUTHWEST; i % WIDTH < pos % WIDTH && i < SPACES; i += BOARD_SOUTHWEST) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        return moveVec;
    }

    std::vector<Move> Queen::getQuietMoves() {
        std::vector<Move> moveVec;
        for (int i = pos + BOARD_NORTH; i >= 0; i += BOARD_NORTH) {
            if (myPosition->getPieceValAt(i) != EMPTY)
                break;
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        for (int i = pos + BOARD_SOUTH; i < SPACES; i += BOARD_SOUTH) {
            if (myPosition->getPieceValAt(i) != EMPTY)
                break;
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        for (int i = pos + BOARD_EAST; i % WIDTH; i += BOARD_EAST) {
            if (myPosition->getPieceValAt(i) != EMPTY)
                break;
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        for (int i = pos + BOARD_WEST; i % WIDTH != 7 && i >= 0; i += BOARD_WEST) {
            if (myPosition->getPieceValAt(i) != EMPTY)
                break;
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        for (int i = pos + BOARD_NORTHEAST; i % WIDTH > pos % WIDTH && i >= 0; i += BOARD_NORTHEAST) {
            if (myPosition->getPieceValAt(i) != EMPTY)
                break;
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        for (int i = pos + BOARD_NORTHWEST; i % WIDTH < pos % WIDTH && i >= 0; i += BOARD_NORTHWEST) {
            if (myPosition->getPieceValAt(i) != EMPTY)
                break;
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        for (int i = pos + BOARD_SOUTHEAST; i % WIDTH > pos % WIDTH && i < SPACES; i += BOARD_SOUTHEAST) {
            if (myPosition->getPieceValAt(i) != EMPTY)
                break;
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        for (int i = pos + BOARD_SOUTHWEST; i % WIDTH < pos % WIDTH && i < SPACES; i += BOARD_SOUTHWEST) {
            if (myPosition->getPieceValAt(i) != EMPTY)
                break;
            else
                moveVec.push_back(Move(pos, i, STANDARD));
        }
        return moveVec;
    }

    std::vector<Move> Queen::getCaptureMoves() {
        std::vector<Move> moveVec;
        for (int i = pos + BOARD_NORTH; i >= 0; i += BOARD_NORTH) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
        }
        for (int i = pos + BOARD_SOUTH; i < SPACES; i += BOARD_SOUTH) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
        }
        for (int i = pos + BOARD_EAST; i % WIDTH; i += BOARD_EAST) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
        }
        for (int i = pos + BOARD_WEST; i % WIDTH != 7 && i >= 0; i += BOARD_WEST) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
        }
        for (int i = pos + BOARD_NORTHEAST; i % WIDTH > pos % WIDTH && i >= 0; i += BOARD_NORTHEAST) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
        }
        for (int i = pos + BOARD_NORTHWEST; i % WIDTH < pos % WIDTH && i >= 0; i += BOARD_NORTHWEST) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
        }
        for (int i = pos + BOARD_SOUTHEAST; i % WIDTH > pos % WIDTH && i < SPACES; i += BOARD_SOUTHEAST) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
        }
        for (int i = pos + BOARD_SOUTHWEST; i % WIDTH < pos % WIDTH && i < SPACES; i += BOARD_SOUTHWEST) {
            if (myPosition->getPieceValAt(i) != EMPTY) {
                if (enemyPiece(myPosition->getPieceValAt(i), side))
                    moveVec.push_back(Move(pos, i, CAPTURE));
                break;
            }
        }
        return moveVec;
    }

    std::vector<Move> King::getAllMoves() {
        std::vector<Move> moveVec;
        if (side == WHITE) {
            if (pos == 60) {
                if (myPosition->getPieceValAt(61) == EMPTY && myPosition->getPieceValAt(62) == EMPTY && myPosition->canCastleWhiteKing())
                    moveVec.push_back(Move(60, 62, KCASTLE));
                if (myPosition->getPieceValAt(59) == EMPTY && myPosition->getPieceValAt(58) == EMPTY && myPosition->getPieceValAt(57) == EMPTY && myPosition->canCastleWhiteQueen())
                    moveVec.push_back(Move(60, 58, QCASTLE));
            }
        }
        else {
            if (pos == 4) {
                if (myPosition->getPieceValAt(5) == EMPTY && myPosition->getPieceValAt(6) == EMPTY && myPosition->canCastleBlackKing())
                    moveVec.push_back(Move(4, 6, KCASTLE));
                if (myPosition->getPieceValAt(3) == EMPTY && myPosition->getPieceValAt(2) == EMPTY && myPosition->getPieceValAt(1) == EMPTY && myPosition->canCastleBlackQueen())
                    moveVec.push_back(Move(4, 2, QCASTLE));
            }
        }
        if ((pos + BOARD_SOUTHEAST) % WIDTH > pos % WIDTH && pos < 55)
            if (!validPiece(myPosition->getPieceValAt(pos + BOARD_SOUTHEAST), side))
                moveVec.push_back(Move(pos, pos + BOARD_SOUTHEAST, myPosition->getPieceValAt(pos + BOARD_SOUTHEAST) == EMPTY ? STANDARD : CAPTURE));

        if ((pos + BOARD_EAST) % WIDTH > pos % WIDTH)
            if (!validPiece(myPosition->getPieceValAt(pos + BOARD_EAST), side))
                moveVec.push_back(Move(pos, pos + BOARD_EAST, myPosition->getPieceValAt(pos + BOARD_EAST) == EMPTY ? STANDARD : CAPTURE));

        if ((pos + BOARD_NORTHWEST) % WIDTH < pos % WIDTH && pos > 8) //bad
            if (!validPiece(myPosition->getPieceValAt(pos + BOARD_NORTHWEST), side))
                moveVec.push_back(Move(pos, pos + BOARD_NORTHWEST, myPosition->getPieceValAt(pos + BOARD_NORTHWEST) == EMPTY ? STANDARD : CAPTURE));

        if ((pos + BOARD_WEST) % WIDTH < pos % WIDTH && pos > 0)
            if (!validPiece(myPosition->getPieceValAt(pos + BOARD_WEST), side))
                moveVec.push_back(Move(pos, pos + BOARD_WEST, myPosition->getPieceValAt(pos + BOARD_WEST) == EMPTY ? STANDARD : CAPTURE));

        if ((pos + BOARD_SOUTHWEST) % WIDTH < pos % WIDTH && pos < 57)
            if (!validPiece(myPosition->getPieceValAt(pos + BOARD_SOUTHWEST), side))
                moveVec.push_back(Move(pos, pos + BOARD_SOUTHWEST, myPosition->getPieceValAt(pos + BOARD_SOUTHWEST) == EMPTY ? STANDARD : CAPTURE));

        if (pos < 56)
            if (!validPiece(myPosition->getPieceValAt(pos + BOARD_SOUTH), side))
                moveVec.push_back(Move(pos, pos + BOARD_SOUTH, myPosition->getPieceValAt(pos + BOARD_SOUTH) == EMPTY ? STANDARD : CAPTURE));

        if ((pos + BOARD_NORTHEAST) % WIDTH > pos % WIDTH && pos > 6)//bad
            if (!validPiece(myPosition->getPieceValAt(pos + BOARD_NORTHEAST), side))
                moveVec.push_back(Move(pos, pos + BOARD_NORTHEAST, myPosition->getPieceValAt(pos + BOARD_NORTHEAST) == EMPTY ? STANDARD : CAPTURE));

        if (pos > 7)//bad
            if (!validPiece(myPosition->getPieceValAt(pos + BOARD_NORTH), side))
                moveVec.push_back(Move(pos, pos + BOARD_NORTH, myPosition->getPieceValAt(pos + BOARD_NORTH) == EMPTY ? STANDARD : CAPTURE));
        return moveVec;
    }

    std::vector<Move> King::getQuietMoves() {
        std::vector<Move> moveVec;
        if (side == WHITE) {
            if (pos == 60) {
                if (myPosition->getPieceValAt(61) == EMPTY && myPosition->getPieceValAt(62) == EMPTY && myPosition->canCastleWhiteKing())
                    moveVec.push_back(Move(60, 62, KCASTLE));
                if (myPosition->getPieceValAt(59) == EMPTY && myPosition->getPieceValAt(58) == EMPTY && myPosition->getPieceValAt(57) == EMPTY && myPosition->canCastleWhiteQueen())
                    moveVec.push_back(Move(60, 58, QCASTLE));
            }
        }
        else {
            if (pos == 4) {
                if (myPosition->getPieceValAt(5) == EMPTY && myPosition->getPieceValAt(6) == EMPTY && myPosition->canCastleBlackKing())
                    moveVec.push_back(Move(4, 6, KCASTLE));
                if (myPosition->getPieceValAt(3) == EMPTY && myPosition->getPieceValAt(2) == EMPTY && myPosition->getPieceValAt(1) == EMPTY && myPosition->canCastleBlackQueen())
                    moveVec.push_back(Move(4, 2, QCASTLE));
            }
        }
        if ((pos + BOARD_SOUTHEAST) % WIDTH > pos % WIDTH && pos < 55 && myPosition->getPieceValAt(pos + BOARD_SOUTHEAST) == EMPTY)
            moveVec.push_back(Move(pos, pos + BOARD_SOUTHEAST, STANDARD));
        if ((pos + BOARD_EAST) % WIDTH > pos % WIDTH && myPosition->getPieceValAt(pos + BOARD_EAST) == EMPTY)
            moveVec.push_back(Move(pos, pos + BOARD_EAST, STANDARD));
        if ((pos + BOARD_NORTHWEST) % WIDTH < pos % WIDTH && pos > 8 && myPosition->getPieceValAt(pos + BOARD_NORTHWEST) == EMPTY)
            moveVec.push_back(Move(pos, pos + BOARD_NORTHWEST, STANDARD));
        if ((pos + BOARD_WEST) % WIDTH < pos % WIDTH && pos > 0 && myPosition->getPieceValAt(pos + BOARD_WEST) == EMPTY)
            moveVec.push_back(Move(pos, pos + BOARD_WEST, STANDARD));
        if ((pos + BOARD_SOUTHWEST) % WIDTH < pos % WIDTH && pos < 57 && myPosition->getPieceValAt(pos + BOARD_SOUTHWEST) == EMPTY)
            moveVec.push_back(Move(pos, pos + BOARD_SOUTHWEST, STANDARD));
        if (pos < 56 && myPosition->getPieceValAt(pos + BOARD_SOUTH) == EMPTY)
            moveVec.push_back(Move(pos, pos + BOARD_SOUTH, STANDARD));
        if ((pos + BOARD_NORTHEAST) % WIDTH > pos % WIDTH && pos > 6 && myPosition->getPieceValAt(pos + BOARD_NORTHEAST) == EMPTY)
            moveVec.push_back(Move(pos, pos + BOARD_NORTHEAST, STANDARD));
        if (pos > 7 && myPosition->getPieceValAt(pos + BOARD_NORTH) == EMPTY)
            moveVec.push_back(Move(pos, pos + BOARD_NORTH, STANDARD));
        return moveVec;
    }

    std::vector<Move> King::getCaptureMoves() {
        std::vector<Move> moveVec;
        if ((pos + BOARD_SOUTHEAST) % WIDTH > pos % WIDTH && pos < 55 && enemyPiece(myPosition->getPieceValAt(pos + BOARD_SOUTHEAST), side))
            moveVec.push_back(Move(pos, pos + BOARD_SOUTHEAST, CAPTURE));
        if ((pos + BOARD_EAST) % WIDTH > pos % WIDTH && enemyPiece(myPosition->getPieceValAt(pos + BOARD_EAST), side))
            moveVec.push_back(Move(pos, pos + BOARD_EAST, CAPTURE));
        if ((pos + BOARD_NORTHWEST) % WIDTH < pos % WIDTH && pos > 8 && enemyPiece(myPosition->getPieceValAt(pos + BOARD_NORTHWEST), side))
            moveVec.push_back(Move(pos, pos + BOARD_NORTHWEST, CAPTURE));
        if ((pos + BOARD_WEST) % WIDTH < pos % WIDTH && pos > 0 && enemyPiece(myPosition->getPieceValAt(pos + BOARD_WEST), side))
            moveVec.push_back(Move(pos, pos + BOARD_WEST, CAPTURE));
        if ((pos + BOARD_SOUTHWEST) % WIDTH < pos % WIDTH && pos < 57 && enemyPiece(myPosition->getPieceValAt(pos + BOARD_SOUTHWEST), side))
            moveVec.push_back(Move(pos, pos + BOARD_SOUTHWEST, CAPTURE));
        if (pos < 56 && enemyPiece(myPosition->getPieceValAt(pos + BOARD_SOUTH), side))
            moveVec.push_back(Move(pos, pos + BOARD_SOUTH, CAPTURE));
        if ((pos + BOARD_NORTHEAST) % WIDTH > pos % WIDTH && pos > 6 && enemyPiece(myPosition->getPieceValAt(pos + BOARD_NORTHEAST), side))
            moveVec.push_back(Move(pos, pos + BOARD_NORTHEAST, CAPTURE));
        if (pos > 7 && enemyPiece(myPosition->getPieceValAt(pos + BOARD_NORTH), side))
            moveVec.push_back(Move(pos, pos + BOARD_NORTH, CAPTURE));
        return moveVec;
    }
}