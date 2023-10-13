#if !defined(CUSTOMTYPES_H)
#define CUSTOMTYPES_H

#pragma once

#include "includes.h"
#include "Random.h"

enum Color
{red, green, blue, };
enum PuzzleCommands
{up, down, left, right, quit};

inline std::ostream& operator<<(std::ostream& stream, PuzzleCommands display)
{
    switch (display)
    {
        case up:    stream << "up";     return stream;
        case down:  stream << "down";   return stream;
        case left:  stream << "left";   return stream;
        case right: stream << "right";  return stream;
        default:                        return stream;
    }
    return stream;
}

enum class CRanks 
{c2, c3, c4, c5, c6, c7, c8, c9, c10, Jack, Queen, King, Ace, };
enum class CSuits 
{clubs, diamonds, hearts, spades, };

using Sizeofarray = unsigned int;
using Sizeof2Darray_m  = unsigned int;
using Sizeof2Darray_i = unsigned int;
using Predicate_Raw = bool (*) (int, int);
using Predicate = std::function<bool (int, int)>;
using ArithmeticFunction_Raw = int (*) (int, int);
using ArithmeticFunction = std::function<int (int, int)>;

struct Card
{
    CRanks rank{};
    CSuits suit{};
};

struct Empolyee
{
    int id{};
    int age{};
    double wage{};
};

struct Students
{
    std::string name{};
    int grade{};
};

struct Company
{
    int numberOfEmployees{};
    Empolyee CEO{};
};

class Vector3d;

class Point3d
{
    double x{1.0};
    double y{2.0};
    double z{3.0}; 

public:
    Point3d(double a, double b, double c = 3) 
    : x{a}, y{b}, z{c}
    {} 
    Point3d() = default;
    void print() const;
    bool isEqual(const Point3d& obj);
    void moveByVector(const Vector3d& v);
    Point3d& firstPointAddedTo_others();
};

class Point2d
{
    double x{0.0};
    double y{0.0};
    
public:
    Point2d(double a, double b) 
    : x{a}, y{b}
    {} 
    Point2d() = default;
    friend std::ostream& operator<<(std::ostream& stream, const Point2d& display);
    Point2d getAdjacentPoint(PuzzleCommands direction) const;
    Point2d& swapPoints() 
    {
        std::swap(x, y);
        return *this;
    }
    // this member function is soley for the Board class indexing in relation to the Tile class
    size_t getTile_X_FromPoints() const
    {
        return static_cast<size_t>(x);
    }
    // this member function is soley for the Board class indexing in relation to the Tile class
    size_t getTile_Y_FromPoints() const
    {
        return static_cast<size_t>(y);
    }
    friend double distanceFrom(Point2d& a, Point2d& b);
    friend bool operator==(const Point2d& a, const Point2d& b);
    friend bool operator!=(const Point2d& a, const Point2d& b);
};

class Vector3d
{
    double x{1.0};
    double y{};
    double z{};
public:
    Vector3d(double a, double b, double c = 3) 
    : x{a}, y{b}, z{c}
    {}
    Vector3d() = default;
    void print() const;
    friend void Point3d::moveByVector(const Vector3d& v);
};

class ArrayOFInts
{
    int* ptr{};
    size_t length; 
public:
    ArrayOFInts(size_t a);
    ArrayOFInts(const ArrayOFInts& source); 
    ~ArrayOFInts(); 
    size_t size();
    int& operator[](size_t index);
    int operator[](size_t index) const;
    ArrayOFInts& operator=(const ArrayOFInts& source);
    void deepCopy(const ArrayOFInts& source);
    friend std::ostream& operator<<(std::ostream& out, const ArrayOFInts& arr);
};

class Fraction_c
{
public:
    struct Fraction_s
    {
        int numerator{};
        int denominator{};
    };
private:
    Fraction_s fractionTemplate{1, 1};    
public:
    Fraction_c(int numerator, int denominator = 1)
    : fractionTemplate{numerator, denominator} 
    {
        reduce(); 
    } 
    Fraction_c() = default;
    // supports chainning 
    Fraction_c reduce(); 
    Fraction_c operator-();
    
    friend Fraction_c operator*(const Fraction_c& a, const Fraction_c& b); 
    friend Fraction_c operator*(const Fraction_c& a, const int value); 
    friend Fraction_c operator*(const int value, const Fraction_c& b); 
    friend Fraction_c operator+(const Fraction_c& a, const Fraction_c& b); 
    friend Fraction_c operator+(const Fraction_c& a, const int value); 
    friend Fraction_c operator+(const int value, const Fraction_c& b); 
    friend Fraction_c operator-(const Fraction_c& a, const Fraction_c& b); 
    friend Fraction_c operator-(const Fraction_c& a, const int value); 
    friend Fraction_c operator-(const int value, const Fraction_c& b);
    
    friend bool operator==(const Fraction_c& a, const Fraction_c& b); 
    friend bool operator!=(const Fraction_c& a, const Fraction_c& b); 
    friend bool operator>(const Fraction_c& a, const Fraction_c& b); 
    friend bool operator<(const Fraction_c& a, const Fraction_c& b); 
    friend bool operator>=(const Fraction_c& a, const Fraction_c& b); 
    friend bool operator<=(const Fraction_c& a, const Fraction_c& b); 
    
    friend std::ostream& operator<<(std::ostream& c_out, const Fraction_c& b); 
    friend std::istream& operator>>(std::istream& c_in, Fraction_c& b); 
};

class Grademap
{
    std::vector<std::pair<std::string, char>> m_map{};
    static inline std::pair<std::string, char> element{};
public:
    char& operator[](std::string key) 
    {
        auto find_if{std::ranges::find_if(m_map, [&](const std::pair<std::string, char>& pair) -> bool 
        {
            return pair.first == key;
        } 
        )};
        if(find_if != m_map.end())
        {
            auto index{std::distance(m_map.begin(), find_if)};
            return m_map[(size_t)index].second;
        }
        else
        {
            element = {key, '\0'}; 
            m_map.push_back(element);
            return (m_map.back()).second;
        }
    }
};

class Mystring
{
    std::string m_string{};
    static inline std::string_view non_copy{}; 
public:
    Mystring(const std::string& string) : m_string{string}
    {}
    Mystring() = default;
    std::string_view operator()(size_t offset, size_t length)
    {
        non_copy = m_string.substr(offset, length);
        return non_copy;
    }
    friend std::ostream& operator<<(std::ostream& out, const Mystring& string)
    {
        out << string.m_string;   
        return out;    
    }  
};

class Average
{
    std::int32_t m_sumOfIntegers{};
    std::int8_t m_numOfIntegers{};
public:
    Average& operator+=(int num)
    {
        m_sumOfIntegers += num;
        ++m_numOfIntegers;
        return *this;
    }
    friend std::ostream& operator<<(std::ostream& out, const Average& ave)
    {
        out << ave.m_sumOfIntegers/static_cast<double>(ave.m_numOfIntegers);
        return out;
    }
};

class Tile
{
    char& m_tile;
public:
    Tile(char& tile) : m_tile{tile}
    {}
    Tile() = default; 
    bool isEmpty()
    {
        if(m_tile == '\0' || m_tile == 0)
        return true;

        return false;
    }
    int getNum() const
    {
        return m_tile;
    }
    Tile& operator=(char ch)
    {
        m_tile = ch;
        return *this;
    }
    // better to implement the overload ()operator on my Tile class(though ambiguous) than a swap member function
    void operator()(Tile& b)
    {
        std::swap(m_tile, b.m_tile);
    }
    friend std::ostream& operator<<(std::ostream& stream, const Tile& display)
    {
        if(display.m_tile != 0)
        stream << int(display.m_tile) << '\t';
        else
        stream << display.m_tile << '\t';
        
        return stream;
    }
};

class Board
{
    std::array<std::array<char, 4>, 4> m_board{};
    
    void deepCopy(const std::array<std::array<char, 4>, 4>& source)
    {
        for (size_t main_array{0}; main_array < std::size(source); ++main_array)
        {
            for (size_t inner_array{0}; inner_array < std::size(source[0]); ++inner_array)
            {
                m_board[main_array] [inner_array] = source[main_array][inner_array];
                
            }
        }
    } 
public: 
    Board() = default; 
    Board(const std::array<std::array<char, 4>, 4>& origin)
    {
        deepCopy(origin);
    } 
    Board& fill()
    { 
        int num{1};
        std::uint16_t lastElement{3};
        for (size_t main_array{0}; main_array < std::size(m_board); ++main_array)
        {
            for (size_t inner_array{0}; inner_array < std::size(m_board[0]); ++inner_array)
            {
                m_board[main_array] [inner_array] = num;
                ++num;

                if(m_board[main_array] [inner_array] == m_board[lastElement] [lastElement])
                m_board[main_array] [inner_array] = '\0';
            }
        }
        return *this;
    }
    friend bool operator==(const Board& a, const Board& b)
    {
        for (size_t main_array{0}; main_array < std::size(a.m_board); ++main_array)
        {
            for (size_t inner_array{0}; inner_array < std::size(a.m_board[0]); ++inner_array)
            {
                if(a.m_board[main_array] [inner_array] != b.m_board[main_array][inner_array])
                return false;
            }
        }
        return true;
    }
    friend bool operator!=(const Board& a, const Board& b)
    {
        return !(a == b);
    }
    void randomize()
    {
        std::ranges::shuffle(m_board, random::random_numbers);
    }
    Point2d findIndexOfTile(char tile) const
    {
        for (size_t main_array{0}; main_array < 4; ++main_array)
        {
            for (size_t inner_array{0}; inner_array < 4; ++inner_array)
            {
                if(m_board[main_array][inner_array] == tile)
                {
                    return {double(main_array), double(inner_array)};
                    break;
                }
            }
        }
        assert(false && "Error! Tile Could not be found.");
        return {0.0, 0.0};
    } 
    Point2d getPointsOfTile(char tile = 0) const
    {
        return Point2d{findIndexOfTile(tile)}.swapPoints();
    }
    auto& operator[](size_t index)
    {
        constexpr size_t length{4};
        assert( ( int(index) >= 0 || index < length) && "invalid index" );
        auto ptr{&m_board[0]};
        auto element{ptr + index};
        return *element;
    }
    auto operator[](size_t index) const
    {
        constexpr size_t length{4};
        assert( ( int(index) >= 0 || index < length) && "invalid index" );
        auto ptr{&m_board[0]};
        auto element{ptr + index};
        return *element;
    }
    friend std::ostream& operator<<(std::ostream& stream, const Board& display)
    {
        for (size_t main_array{0}; main_array < std::size(display.m_board); ++main_array)
        {
            for (size_t inner_array{0}; inner_array < std::size(display.m_board[0]); ++inner_array)
            {
                if(display.m_board[main_array] [inner_array] != 0)
                stream << int(display.m_board[main_array] [inner_array]) << '\t';
                else
                stream << display.m_board[main_array] [inner_array] << '\t';
            }
            stream << '\n';
        }
        return stream;
    }
};

class Creature
{
protected:
    std::string m_name{}; 
    char m_symbol{};
    int m_health{};
    int m_damagePerAttack{};
    int m_goldAmount{};

public:
    Creature(std::string name, char symbol, int health, int damagePerAttack, int goldAmount):
    m_name{name}, m_symbol{symbol}, m_health{health}, m_damagePerAttack{damagePerAttack}, m_goldAmount{goldAmount} 
    {}
    Creature() = default;
    
    const std::string& getName() const
    {
        return m_name;
    }
    char getSymbol() const
    {
        return m_symbol;
    }
    int getHealth() const
    {
        return m_health;
    }
    int get_damagePerAttack() const
    {
        return m_damagePerAttack;
    }
    int get_goldAmount() const
    {
        return m_goldAmount;
    }

    void reduceHealth(int amount)
    {
        m_health -= amount;
    }
    bool isDead()
    {
        return (m_health <= 0);
    }
    void addGold(int amount)
    {
        m_goldAmount += amount;
    }

    friend std::ostream& operator<<(std::ostream& out, const Creature& display)
    {
        out << display.m_name 
        << " is denoted by symbol " << display.m_symbol 
        << ", has a maximum health of  " << display.m_health 
        << ", a damagePerAttack of " << display.m_damagePerAttack
        << " and a goldAmount of " << display.m_goldAmount;  
        return out;
    }     
};

class Player: public Creature
{
    int m_level{};
public:
    Player(std::string name): Creature{name, '@', 10, 1, 0}, m_level{1} 
    {}
    Player() = default;
    
    void levelUp()
    {
        ++m_level;
        ++m_damagePerAttack;
    }
    int getLevel() const
    {
        return m_level;
    }
    bool hasWon()
    {
        return m_level == 20;
    }
    bool buyHealthWithGold()
    {
        if (m_goldAmount < 10)
        return false;
        else if (m_goldAmount >= 10)
        {
            ++m_health;
            m_goldAmount -= 10;
            return true;
        }
        
        return false;
    }

    friend std::ostream& operator<<(std::ostream& out, const Player& display)
    {
        out << "Player[on Level: " << display.m_level << "]: " << static_cast<Creature>(display);
        return out;
    }
};

class Monster: public Creature
{
    
public:
    enum Type: size_t
    {dragon, orc, slime, maxType};

    friend std::ostream& operator<<(std::ostream& out, Type display)
    {
        switch (display)
        {
        case dragon:
            std::cout << "Dragon";
            break;
        case orc:
            std::cout << "Orc";
            break;
        case slime:
            std::cout << "Slime";
            break;
        
        default:
            assert(false and "Invalid monster type");
            break;
        }
        return out;
    }

    Monster(Type monster): Creature{getMonsterAttributes(monster)}
    {}
    Monster() = default;
private:
    static const Creature& getMonsterAttributes(Type monster)
    {
        static std::array<Creature, 3> attributes
        {{ 
            {"Dragon", 'd', 20, 4, 100},
            {"Orc", 'o', 4, 2, 25},
            {"Slime", 's', 1, 1, 10},
        }};

        switch (monster)
        {
        case dragon:
            return attributes.at(dragon);
            break;
        case orc:
            return attributes.at(orc);
            break;
        case slime:
            return attributes.at(slime);
            break;
        
        default:
            assert(false and "Invalid monster type");
            break;
        }
        return attributes.at(0);
    }

public:
    friend std::ostream& operator<<(std::ostream& out, const Monster& display)
    {
        out << "Monster: " << static_cast<Creature>(display);
        return out;
    }

    static Type generateRandomMonster()
    {
        return static_cast<Type>(random::get(0, maxType - 1));
    }

};


#endif // CUSTOMTYPES_H
