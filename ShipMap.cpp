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
	unsigned offset = ( mOnXAxis ? mEnd.x - mStart.x : mEnd.y - mStart.y );
	mHits[ offset ] = 1u;

	// If there's no 0 found in the hits (i.e. no unhit spots), we're sunk.
	// could also do a sum of all values and ensure it's < mHits.size()
	if( std::find( mHits.begin(), mHits.end(), 0 ) == mHits.end() )
		return true;

	return false;
}

Map::Map()
{
	// Init the ships and map status
	initShips();
	initMapStatus();
}

HitType Map::checkShot( Point2D coords )
{
	auto &status = mPoints[ coords.x ][ coords.y ];
	
	// early out: repeats
	
	switch( status )
	{
	case PointStatus::PICKED:
		return HitType::REPEAT;
	case PointStatus::EMPTY:
	{
		//mPoints[ coords.x ][ coords.y ] = PICKED;
		status = PointStatus::PICKED;
		return HitType::MISS;
	}
	case PointStatus::HAS_SHIP:
	{
		status = PointStatus::PICKED;

		// Need to know which ship is being hit, and if it sinks.
		for( auto &ship : mShips )
		{
			//bool sunk = ship.hit( coords );
			bool sunk = ship.isHit( coords );
			return sunk ? HitType::SUNK : HitType::HIT;
		}
	}
	}
	
	// should return error here somehow
	return HitType::MISS;
}

void Map::initShips()
{

}

void Map::initMapStatus()
{
	// Initial: empty

	// Add ship points

}