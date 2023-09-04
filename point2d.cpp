#include <cassert>
#include <algorithm>
#include <format>

#include "point2d.h"

namespace Myfcn
{
    Point2d::Point2d(float x, float y): m_x{x}, m_y{y}
    {}

    Point2d Point2d::getAdjacentPoint(Direction direction)
    {
        switch (direction)
        {
        case Direction::up:    return Point2d{m_x, m_y - 1.f};
        case Direction::down:  return Point2d{m_x, m_y + 1.f};
        case Direction::left:  return Point2d{m_x - 1.f, m_y};
        case Direction::right: return Point2d{m_x + 1.f, m_y};
        default: assert(false and "Not a Direction!\n");
        std::cerr << "Not a Direction\n";
        std::exit(1); 
        break;
        }
        return {};
    }

    Point2d Point2d::swapAxis() const
    {
        return {m_y, m_x};
    }

    float Point2d::getX() const
    {
        return m_x;
    }

    float Point2d::getY() const
    {
        return m_y;
    }

    std::ostream& operator<<(std::ostream& out, const Point2d& temp)
    {
        out << std::format("Point2d({}, {})", temp.m_x, temp.m_y);
        return out;
    }    
} // namespace Myfcn


