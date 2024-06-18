#ifndef POINT2D
#define POINT2D

#pragma once

#include <iostream>

namespace misc
{
    class Point2d
    {
    public:
        enum class Direction{up, down, left, right};
    private:
        float m_x{};
        float m_y{};
    public:
        Point2d(float x, float y);

        Point2d() = default;

        Point2d(const Point2d&) = default;
        Point2d(Point2d&&) = default;

        Point2d& operator=(const Point2d&) = default;
        Point2d& operator=(Point2d&&) = default;

        ~Point2d() = default;

        /*Gets the point adjacent to the original point in any direction.
        For example:
        1. Point2d{ 1.f, 1.f }.getAdjacentPoint(Direction::up)    == Point2d{ 1.f, 0.f };
        2. Point2d{ 1.f, 1.f }.getAdjacentPoint(Direction::down)  == Point2d{ 1.f, 2.f };
        3. Point2d{ 1.f, 1.f }.getAdjacentPoint(Direction::left)  == Point2d{ 0.f, 1.f };
        4. Point2d{ 1.f, 1.f }.getAdjacentPoint(Direction::right) == Point2d{ 2.f, 1.f };*/
        Point2d getAdjacentPoint(Direction direction);
        //Swaps the xaxis with the yaxis, or vice versa, and returns a new temporary Point2d object.
        Point2d swapAxis() const;
        //Gets the x-coordinate in a 2d plane.
        float getX() const;
        //Gets the y-coordinate in a 2d plane.
        float getY() const;

        friend std::ostream& operator<<(std::ostream& out, const Point2d& temp);
    };
    
} // namespace Myfcn



#endif