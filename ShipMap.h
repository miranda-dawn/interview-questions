#ifndef SHIP_MAP_H
#define SHIP_MAP_H

#include <string>
#include <set>
#include <array>
#include <vector>

// Given a 2D array( think map ) with “ships” of varying sizes occupying spaces in the array.
//Create an algorithm which takes in two coordinates as parameters and determine if the coordinates “Miss”, “Hit <ship name>”, “Already Selected”, 
// or , if the “last spot” of a ship was hit, then “Sunk <ship name>”.

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

constexpr Point2D X_AXIS = { 1, 0 };
constexpr Point2D Y_AXIS = { 0, 1 };

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

constexpr unsigned MAP_SIZE = 10;

class Map
{
public:
	Map();

	// Postcondition: the map has been updated.
	// should change this to also return the ship name (tuple with optional string?)
	// also does the "shot".
	HitType checkShot( Point2D coords );
private:
	std::set< Ship > mShips;
	std::array< std::array< PointStatus, MAP_SIZE >, MAP_SIZE > mPoints;

	void initShips();
	void initMapStatus();
};









#endif