#include <iostream>
#include <numeric>
#include <span>
#include <map>
#include <list>
#include <vector>
#include <set>
#include <format>

#include "ShipMap.h"

using namespace std;

int calcSum( std::span< int > vals )
{
	int sum = 0;
	for( const auto &val : vals )
		sum += val;
	return sum;
}

bool testSum( ostream &out )
{
	// Test against std::accumulate
	try
	{
		int sequence[] = { 1, 2, 3, 4, 5, 6, 7 };
		int init_sum = 0;
		out << calcSum( sequence ) << ", std::accumulate returned: " << accumulate( sequence, sequence + 7, init_sum ) << endl;
		if( calcSum( sequence ) != accumulate( sequence, sequence + 7, init_sum ) )
			throw std::exception( "Mismatch between calcSum and std::accumulate, my function is likely wrong." );

		// Make sure there's no crash on integer overflow
		// Could add in exception handling to catch over-/under-flow
		int int_overflow[] = { INT_MAX, 1, 1 };
		init_sum = 0;

		out << calcSum( int_overflow ) << ", std::accumulate returned: " << accumulate( int_overflow, int_overflow + 3, init_sum ) << endl;
		if( calcSum( int_overflow ) != accumulate( int_overflow, int_overflow + 3, init_sum ) )
			throw std::exception( "Mismatch between calcSum and std::accumulate, my function is likely wrong." );



	}
	catch( std::exception e )
	{
		out << "Test failed: " << e.what() << endl;
		return false;
	}

	return true;
}

using Dictionary = map< string, list< string > >;

// Recursive function that returns true if a cycle was encountered. Uses lots of references in order to track things; this could
// be simplified by making this part of a class that saved that data instead. 
bool track_cycle( const Dictionary &dict, const string &start_key, const list< string > &dependencies, set< string > &cyclical_nodes, set< string > &traversed_keys )
{
	if( std::find( dependencies.begin(), dependencies.end(), start_key ) != dependencies.end() )
	{
		cyclical_nodes.insert( start_key );
		return true;
	}

	bool cycle = false;
	for( const auto &dep : dependencies )
	{
		if( !dict.contains( dep ) )
			continue;
		if( traversed_keys.contains( dep ) )
		{
			cyclical_nodes.insert( dep );
			cyclical_nodes.insert( start_key );
			return true;
		}
		auto new_deps = dict.at( dep );
		// Tracking the keys here breaks infinite loops where the start key isn't directly involved in the loop.
		traversed_keys.insert( dep );
		cycle |= track_cycle( dict, start_key, new_deps, cyclical_nodes, traversed_keys );
	}
	traversed_keys.insert( start_key );
	return cycle;
}

/// Returns an ordered set of all nodes involved in circular references. 
set< string > getCircularDependencies( Dictionary dict )
{
	set< string > circular_nodes;

	for( const auto &[ key, deps ] : dict )
	{
		if( circular_nodes.contains( key ) )
			continue;
		// could increase efficiency by putting this outside the loop and skipping keys that have been traversed
		// as part of other keys
		set< string > traversed_keys;
		track_cycle( dict, key, deps, circular_nodes, traversed_keys );
	}

	return circular_nodes;
}


bool testCircularDependencies( ostream &out )
{
	Dictionary test_dict =
	{
		{"A", {"C"}},
		{"B", {"C", "D"}},
		{"D", {"E"}},
		{"E", {"F", "Q"}},
		{"F", {"D"}},
		{"G", {"L"}},
		{"C", {"M"}},
		{"M", {"A"}}

	};

	try
	{
		auto circular_nodes = getCircularDependencies( test_dict );
		for( const auto &node : circular_nodes )
		{
			out << std::format( "\"{}\": ", node ) << endl;
		}
	}
	catch( std::exception e )
	{
		out << "Test 2 failed: " << e.what() << endl;
		return false;
	}

	return true;
}

bool testShipSinking( ostream &out )
{
	try
	{
		Map testMap;
		Ship nina( Point2D{ 0, 0 }, Point2D{ 2, 0 }, "Nina" );
		testMap.addShip(nina);
		out << "Result of (1,1): " << testMap.checkShot( { 1, 1 } ) << endl;
		out << "Result of (1,1): " << testMap.checkShot( { 1, 1 } ) << endl;
		out << "Result of (2,0): " << testMap.checkShot( { 2, 0 } ) << endl;
		out << "Result of (1,0): " << testMap.checkShot( { 1, 0 } ) << endl;
		out << "Result of (0,0): " << testMap.checkShot( { 0, 0 } ) << endl;


	}
	catch( std::exception e )
	{
		out << "Test 3 failed: " << e.what() << endl;
		return false;
	}

	return true;
}

int main()
{
	cout << "Question 1: Write a function that iterates through an integer array and returns the sum of the values in the array." << endl;
	// Using non-zero return codes here as a quick way to indicate failure
	if( !testSum( cout ) )
		return 1;

	cin.get();
	cout << "Question 2: Given a set of dependent services, write code which detects circular references and returns the node(s) where circular references occur." << endl;
	if( !testCircularDependencies( cout ) )
		return 2;

	cin.get();
	cout << "Create an algorithm which takes in two coordinates as parameters and determines whether it misses, hits, already selected, or sunk the ship." << endl;
	if( !testShipSinking( cout ) )
		return 3;

	return 0;
}
