#include <cassert>
#include <algorithm>
#include <format>

#include "board.h"
#include "random.h"

/*--------------------------------------Board------------------------------------------*/
Board::Board()
{
    /*An std::int8_t that mimics a static counter.
    Used to assign values to each individual tile element in the board.*/
    std::int8_t tile_value{1};
    constexpr size_t lastindex{3};

    for (size_t i{0}; i < m_board.size(); i++)
    {
        for (size_t j{0}; j < m_board.at(0).size(); j++, tile_value++)
        {
            if (i == lastindex and j == lastindex)
            tile_value = 0;

            m_board.at(i).at(j) = tile_value;
        }
        
    }
}

Myfcn::Point2d Board::toPoint2d(BoardTile n) const
{
    for (size_t i{0}; i < m_board.size(); i++)
    {
        for (size_t j{0}; j < m_board.at(0).size(); j++)
        {
            if ( static_cast<std::int8_t>(n) == m_board.at(i).at(j))
            {
                return {static_cast<float>(j), static_cast<float>(i)};
            }     
        }
    }
    return {-100.f, -100.f};
}

std::optional<Myfcn::Point2d> Board::slideTile(Myfcn::Point2d::Direction direction)
{
    //Locate the empty tile.
    Tile e_t{*this};
    
    //Get the empty tile's position/point on the board.
    auto emptyTilePosition{e_t.getTilePosition()};

    //Get the adjacent position/point, in the opposite direction of the given direction, from the empty tile.
    Myfcn::Point2d adjacentPoint{};

    switch (direction)
    {
    case Myfcn::Point2d::Direction::up: 
    adjacentPoint = emptyTilePosition.getAdjacentPoint(Myfcn::Point2d::Direction::down); 
    break;
    case Myfcn::Point2d::Direction::down: 
    adjacentPoint = emptyTilePosition.getAdjacentPoint(Myfcn::Point2d::Direction::up); 
    break;
    case Myfcn::Point2d::Direction::left: 
    adjacentPoint = emptyTilePosition.getAdjacentPoint(Myfcn::Point2d::Direction::right); 
    break;
    case Myfcn::Point2d::Direction::right: 
    adjacentPoint = emptyTilePosition.getAdjacentPoint(Myfcn::Point2d::Direction::left); 
    break;
    }

    //Then check if the point is valid.
    if (isPointValid(adjacentPoint))
    return adjacentPoint;
    else
    return {};

    return {};
}

Board& Board::randomize()
{
    using namespace Myfcn;

    constexpr size_t maxNumberOfRandomization{1000};

    for (size_t i = 0; i < maxNumberOfRandomization; i++)
    {
        //Get a random direction to slide tiles in.
        auto randomOutput{ Random::get(static_cast<std::int64_t>(Point2d::Direction::up), static_cast<std::int64_t>(Point2d::Direction::right)) };
        Point2d::Direction randomDirection{ static_cast<Point2d::Direction>(randomOutput) };
        //Slide a tile.
        auto maybeTilePosition{slideTile(randomDirection)};

        if (maybeTilePosition.has_value())
        {
            //Empty tile.
            Tile e_t{*this};
            //Unknown valid tile.
            Tile u_t{*this, *maybeTilePosition};
            //Swap the empty tile with the unknown tile.
            e_t.swap(u_t);
        }
    }

    return *this;
}

bool operator==(const Board& a, const Board& b)
{
    std::int8_t element1{};
    std::int8_t element2{};

    /*Since there is no way to alter the size of a given board, 
    using one board as a reference to get a board's size is sufficient.
    Eg: a.m_board.size() and a.m_board.at(0).size()*/
    for (size_t i{0}; i < a.m_board.size(); i++)
    {
        for (size_t j{0}; j < a.m_board.at(0).size(); j++)
        {
            element1 = a.m_board.at(i).at(j);
            element2 = b.m_board.at(i).at(j);

            if (element1 != element2)
            return false;   
        }
    }
    return true;
}

bool Board::isPointValid(const Myfcn::Point2d& point) const
{
    float x{point.getX()};
    float y{point.getY()};

    if ((x >= 0.f and x <= 3.f) and (y >= 0.f and y <= 3.f))
    return true;
    
    return false;
}

std::ostream& operator<<(std::ostream& out, const Board& temp)
{
    std::int8_t element{};
    for (size_t i{0}; i < temp.m_board.size(); i++)
    {
        for (size_t j{0}; j < temp.m_board.at(0).size(); j++)
        {
            element = temp.m_board.at(i).at(j);

            if (element == 0)
            out << element << '\t';
            else
            out << static_cast<int>(element) << '\t';
        }
        out << '\n'; 
    }
    return out;
}

/*--------------------------------------Tile------------------------------------------*/
Tile::Tile(Board& board, Board::BoardTile boardTile): m_boardTile{boardTile}, m_tilePosition{board.toPoint2d(boardTile)}
{
    //Peforms a switch of the tile's position to get the correct index.
    size_t outer_i{static_cast<size_t>(m_tilePosition.getY())};
    //Peforms a switch of the tile's position to get the correct index.
    size_t inner_i{static_cast<size_t>(m_tilePosition.getX())};

    m_ptr_tile = &(board.m_board.at(outer_i).at(inner_i));
}

Tile::Tile(Board& board, const Myfcn::Point2d& tilePosition): m_tilePosition{tilePosition}
{
    if ( !( board.isPointValid(m_tilePosition) ) )
    {
        assert(false and "Point is not valid");
        std::cerr << "Point is not valid\n";
        std::exit(1);
    }
    
    //Peforms a switch of the tile's position to get the correct index.
    size_t outer_i{static_cast<size_t>(m_tilePosition.getY())};
    //Peforms a switch of the tile's position to get the correct index.
    size_t inner_i{static_cast<size_t>(m_tilePosition.getX())};

    m_ptr_tile = &(board.m_board.at(outer_i).at(inner_i));

    m_boardTile = static_cast<Board::BoardTile>(*m_ptr_tile);
}

Tile& Tile::swap(Tile& tile)
{
    if (this == &tile)
    return *this;

    std::swap(*m_ptr_tile, *tile.m_ptr_tile);
    std::swap(m_boardTile, tile.m_boardTile);

    return *this;
}

int Tile::getTileNumber() const
{
    return static_cast<int>(m_boardTile);
}

const Myfcn::Point2d& Tile::getTilePosition() const
{
    return m_tilePosition;
}

std::ostream& operator<<(std::ostream& out, const Tile& temp)
{
    out << std::format("Tile[{}] is in postion: ", temp.getTileNumber()) << temp.getTilePosition();
    return out;
}


