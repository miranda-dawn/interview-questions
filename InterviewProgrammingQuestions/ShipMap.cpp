#include "ShipMap.h"

Ship::Ship( Point2D start_coord, Point2D end_coord, std::string name ) : mStart{ start_coord }, mEnd{ end_coord }, mName{ name }
{
	init();
}

bool Ship::isValid() const 
{
	// Both start and end must be colinear with X or Y
	return ( mStart.x == mEnd.x ) || ( mStart.y == mEnd.y );
}

bool Ship::init()
{
	mOnXAxis = ( mStart.y == mEnd.y );
	
	// Swap start/end if out of order (for ease of comparisons later)
	if( mEnd < mStart )
		std::swap( mStart, mEnd );

	auto ship_length = ( mOnXAxis ? mEnd.x - mStart.x : mEnd.y - mStart.y );
	// Number of "hits" is length + 1 (also handles single position ship)
	mHits.resize( ship_length + 1u, 0 );

	return isValid();
}

bool Ship::isHit( Point2D shot ) const
{
	if( shot < mStart )
		return false;

	// fix logic here, there are other non-hit values?
	if( mOnXAxis )
		return shot.y <= mEnd.y;
	else
		return shot.x <= mEnd.x;

	return true;
}

bool Ship::hit( Point2D shot )
{
	if( !isHit( shot ) )
		return false;

	// Update the hit status
	unsigned offset = ( mOnXAxis ? shot.x - mStart.x : shot.y - mStart.y );
	if( offset >= mHits.size() )
		throw std::exception( "offset out of bounds, logic is bad" );
	mHits[ offset ] = 1u;

	// If there's no 0 found in the hits (i.e. no unhit spots), we're sunk.
	// could also do a sum of all values and ensure it's < mHits.size()
	if( std::find( mHits.begin(), mHits.end(), 0 ) == mHits.end() )
		return true;

	return false;
}

Map::Map()
{
	// Init the map status
	initMapStatus();
}

std::tuple< HitType, std::string > Map::checkShot( Point2D coords )
{
	auto &status = mPoints[ coords.x ][ coords.y ];
	
	// early out: repeats
	
	switch( status )
	{
	case PointStatus::PICKED:
		return std::make_tuple< HitType, std::string >( HitType::REPEAT, "" );
	case PointStatus::EMPTY:
	{
		status = PointStatus::PICKED;
		return std::make_tuple< HitType, std::string >( HitType::MISS, "" );
	}
	case PointStatus::HAS_SHIP:
	{
		status = PointStatus::PICKED;

		// Need to know which ship is being hit, and if it sinks.
		for( auto &ship : mShips )
		{
			bool hit = ship.isHit( coords );
			if( !hit )
				continue;
			bool sunk = ship.hit( coords );
			const std::string name = ship.getName();
			return { ( sunk ? HitType::SUNK : HitType::HIT ), name };
		}
	}
	}
	
	// should return error here somehow
	return std::make_tuple< HitType, std::string >( HitType::MISS, "error" );
}

bool Map::addShip( Ship new_ship )
{
	// Disallow invalid ships
	if( !new_ship.isValid() )
		return false;
	const auto &start = new_ship.getStart();
	const auto &end = new_ship.getEnd();
	if( !( start < Point2D( MAP_SIZE, MAP_SIZE ) ) ||
		!( end < Point2D( MAP_SIZE, MAP_SIZE ) ) )
	{
		return false;
	}
	// Check each point for collision. update after
	std::vector< Point2D > points;
	if( new_ship.orthogonalX() )
	{
		// Points have the same Y value, differing in X.
		for( unsigned i = start.x; i <= end.x; ++i )
		{
			if( mPoints[ i ][ start.y ] != PointStatus::EMPTY )
				return false;
			points.emplace_back( i, start.y );
		}
	}
	else
	{
		// Points have the same X value, differing in Y.
		for( unsigned i = start.y; i <= end.y; ++i )
		{
			if( mPoints[ start.x ][ i ] != PointStatus::EMPTY )
				return false;
			points.emplace_back( start.y, i );
		}
	}

	for( const auto &point : points )
	{
		mPoints[ point.x ][ point.y ] = PointStatus::HAS_SHIP;
	}
	mShips.push_back( new_ship );
	return true;
	
}

void Map::initMapStatus()
{
	// Initialise all points as empty
	for( unsigned i = 0; i < MAP_SIZE; ++i )
	{
		mPoints[ i ].fill( PointStatus::EMPTY );
	}
}