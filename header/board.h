#ifndef BOARD_H
#define BOARD_H

#pragma once

#include <array>
#include <cstdint>
#include <ostream>
#include <optional>

#include "point2d.h"

namespace fiftheenpuzzle
{
    class Board
    {
    public:
        enum class BoardTile : std::int8_t
        {zero, one, two, three, four, five, six, seven, eight, nine, ten, eleven, twelve, thirteen, fourteen, fiftheen};

        using FourXFourGrid = std::array<std::array<std::int8_t, 4U>, 4U>;

    private:

        FourXFourGrid m_board{};

    public:
        Board();

        Board(const Board&) = default;
        Board(Board&&) = default;

        Board& operator=(const Board&) = default;
        Board& operator=(Board&&) = default;

        ~Board() = default;

        //Returns the position of a BoardTile enum object if present on the board.
        //If not present it returns a misc::Point2d object holding absurd values x = -100.f and y = -100.f.
        misc::Point2d toPoint2d(BoardTile n) const;
        /*Function slideTile(misc::Point2d::Direction) dosen't actaully slide tiles, it just checks if it is possible to slide a tile in a
        given direction. And How it does this is by return a std::optional<BoardTile> object that contains a Tile object
        if slidable and nothing otherwise.*/
        std::optional<misc::Point2d> slideTile(misc::Point2d::Direction direction);
        //Randomize a given board's state in solvable way.
        Board& randomize();

    private:
        bool isPointValid(const misc::Point2d& point) const;

        friend std::ostream& operator<<(std::ostream& out, const Board& temp);
        friend bool operator==(const Board& a, const Board& b);
        friend class Tile;
    };

    class Tile
    {
    private:
        //Holds a pointer reference to an individual element tile of a board.
        std::int8_t* m_ptr_tile{};
        Board::BoardTile m_boardTile{};
        misc::Point2d m_tilePosition{};

    public:
        /*Constructs a Tile object from a Board::BoardTile object and the Board itself[Favoured].*/
        Tile(Board& board, Board::BoardTile boardTile = Board::BoardTile::zero);
        /*Constructs a Tile object from a tile's position on a board[Unfavoured].
        Prefer the Tile(Board::BoardTile, Board&) constructor if you desire to construct a Tile from a specific Board.
        Reason: Different Boards may/will have varying positons for an individual Tile element, 
        Example: The position of Tile[1] in Board_A is in the misc::Point2d(0, 0) while the position in Board_B is in themisc::Point2d(2, 3). 
        Only to be used in conjuction with non-static public member function Board::slideTile(misc::Point2d::Direction),
        as it is specific to a given board: Tile exampleTile{Board&, slideTile(Direction::up).value()};
        Warning Note: 
        If you do use this constructor without the aformentioned function and tile's position is not valid, 
        it will result in undefined behaviour, most likely a program crash.*/
        Tile(Board& board, const misc::Point2d& tilePosition);
        /*Default constructs a Tile to an Empty Tile.*/

        Tile(const Tile&) = default;
        Tile(Tile&&) = default;

        Tile& operator=(const Tile&) = default;
        Tile& operator=(Tile&&) = default;

        ~Tile() = default;

        //Swaps a Tile with another.
        Tile& swap(Tile& tile);
        //Returns a Tile's number.
        int getTileNumber() const;
        //Returns a Tile's position on the board.
        const misc::Point2d& getTilePosition() const;
    };

    std::ostream& operator<<(std::ostream& out, const Tile& temp);

}

#endif