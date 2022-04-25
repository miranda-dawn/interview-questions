#ifndef SHIP_MAP_H
#define SHIP_MAP_H

#include <string>
#include <unordered_set>
#include <array>
#include <vector>
#include <tuple>
#include <ostream>

// Given a 2D array( think map ) with "ships" of varying sizes occupying spaces in the array.
//Create an algorithm which takes in two coordinates as parameters and determine if the coordinates "Miss", "Hit <ship name>", "Already Selected", 
// or, if the "last spot" of a ship was hit, then "Sunk <ship name>".

// This seems to lend itself nicely to an object-oriented paradigm. I'll do a 2D unsigned int array to represent the board, with 0 for empty and 1 for hit?

// Convenience struct for affine 2D point (no weight)
struct Point2D
{
	unsigned x = 0;
	unsigned y = 0;
};

inline bool operator==( Point2D a, Point2D b )
{
	return a.x == b.x && a.y == b.y;
}

inline bool operator<( Point2D a, Point2D b )
{
	// not sure if this version works, something something demorgan's law probably
	//return !( a.x > b.x || a.y > b.y );

	return ( a.x < b.x || ( a.x == b.x && a.y < b.y ) );
}

class Ship
{
	Ship() = delete;
	
public: 

	Ship( Point2D start_coord, Point2D end_coord, std::string name );

	// Returns whether this ship is valid, i.e. the start and end orthogonal
	// i.e. no diagonal placement. 
	bool isValid() const;

	// Swap if mStart > mEnd, calc hits (0 for unhit, 1 for hit) 
	bool init();
	bool isHit( Point2D shot ) const;

	// Perform the shot, if it's a hit.
	// returns whether this ship is sunk
	bool hit( Point2D shot );

	const Point2D &getStart() const { return mStart; }
	const Point2D &getEnd() const { return mEnd; }
	const std::string &getName() const { return mName; }

	// Returns whether the ship is orthogonal in X, i.e. all
	// points have the same y value.
	bool orthogonalX() const { return mOnXAxis; }

private:
	Point2D mStart, mEnd;
	std::vector< unsigned > mHits;
	std::string mName;
	bool mOnXAxis = false;
};

enum class PointStatus : unsigned char
{
	EMPTY = 0,
	HAS_SHIP = 1,
	PICKED
};

enum class HitType : unsigned char
{
	MISS,
	REPEAT,
	HIT,
	SUNK
};

inline std::ostream &operator<<( std::ostream &out, HitType hit )
{
	switch( hit )
	{
	case HitType::MISS: out << "Miss!"; break;
	case HitType::REPEAT: out << "Already selected!"; break;
	case HitType::HIT: out << "Hit:"; break;
	case HitType::SUNK: out << "Sunk:"; break;
	}
	return out;
}

inline std::ostream &operator<<( std::ostream &out, std::tuple< HitType, std::string > hit_tuple )
{
	HitType hit;
	std::string name;
	std::tie( hit, name ) = hit_tuple;
	out << hit << " " << name;
	return out;
}

constexpr unsigned MAP_SIZE = 10;

class Map
{
public:
	Map();

	// Returns false if the ship couldn't be added (outside board, collision, etc).
	bool addShip( Ship new_ship );

	// Checks whether the coordinates hit, and returns whether it was a hit, miss, or repeat.
	// Also updates the map status.
	std::tuple< HitType, std::string > checkShot( Point2D coords );
private:
	std::vector< Ship > mShips;
	std::array< std::array< PointStatus, MAP_SIZE >, MAP_SIZE > mPoints;

	void initMapStatus();
};

#endif