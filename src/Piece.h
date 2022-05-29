#pragma once

#include <iostream>
#include <vector>
#include "Move.h"

namespace Hopper
{

    class Piece {
    public:
        explicit Piece(position* posi, int p, bool s, role_enum re);
        virtual std::vector<Move> getAllMoves() = 0;
        virtual std::vector<Move> getQuietMoves() = 0;
        virtual std::vector<Move> getCaptureMoves() = 0;
        role_enum getRoleVal() const { return roleVal; }
    protected:
        position* myPosition;
        unsigned pos;
        bool side;
        role_enum roleVal;
    };

    class Empty : public Piece {
    public:
        using Piece::Piece;
        std::vector<Move> getAllMoves();
        std::vector<Move> getQuietMoves();
        std::vector<Move> getCaptureMoves();
    };

    class Pawn : public Piece {
    public:
        using Piece::Piece;
        std::vector<Move> getAllMoves();
        std::vector<Move> getQuietMoves();
        std::vector<Move> getCaptureMoves();
    };

    class Knight : public Piece {
    public:
        using Piece::Piece;
        std::vector<Move> getAllMoves();
        std::vector<Move> getQuietMoves();
        std::vector<Move> getCaptureMoves();
    };

    class Bishop : public Piece {
    public:
        using Piece::Piece;
        std::vector<Move> getAllMoves();
        std::vector<Move> getQuietMoves();
        std::vector<Move> getCaptureMoves();
    };

    class Rook : public Piece {
    public:
        using Piece::Piece;
        std::vector<Move> getAllMoves();
        std::vector<Move> getQuietMoves();
        std::vector<Move> getCaptureMoves();
    };

    class Queen : public Piece {
    public:
        using Piece::Piece;
        std::vector<Move> getAllMoves();
        std::vector<Move> getQuietMoves();
        std::vector<Move> getCaptureMoves();
    };

    class King : public Piece {
    public:
        using Piece::Piece;
        std::vector<Move> getAllMoves();
        std::vector<Move> getQuietMoves();
        std::vector<Move> getCaptureMoves();
    };
}