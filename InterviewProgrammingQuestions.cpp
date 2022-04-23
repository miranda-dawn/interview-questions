
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

bool testSum()
{
	// Test against std::accumulate
	try
	{
		int sequence[] = { 1, 2, 3, 4, 5, 6, 7 };
		int init_sum = 0;
		std::cout << calcSum( sequence ) << ", std::accumulate returned: " << accumulate( sequence, sequence + 7, init_sum ) << endl;
		if( calcSum( sequence ) != accumulate( sequence, sequence + 7, init_sum ) )
			throw std::exception( "Mismatch between calcSum and std::accumulate, my function is likely wrong." );

		// Make sure there's no crash on integer overflow
		// Could add in exception handling to catch over-/under-flow
		int int_overflow[] = { INT_MAX, 1, 1 };
		init_sum = 0;

		std::cout << calcSum( int_overflow ) << ", std::accumulate returned: " << accumulate( int_overflow, int_overflow + 3, init_sum ) << endl;
		if( calcSum( int_overflow ) != accumulate( int_overflow, int_overflow + 3, init_sum ) )
			throw std::exception( "Mismatch between calcSum and std::accumulate, my function is likely wrong." );



	}
	catch( std::exception e )
	{
		std::cout << "Test failed: " << e.what() << endl;
		return false;
	}

	return true;
}

using Dictionary = map< string, list< string > >;
//using Node = Dictionary::node_type;
using Node = std::pair< std::string, list< string > >;

// language substitution? like in discrete math II
// recursive?
// add each node 

// important to *not* make a copy here!
bool replace_dependencies( Dictionary &dict, string start_key )
{
	auto node = dict.extract( start_key );
	for( const auto &dep : node.mapped() )
		return true;
	return false;
}



vector< Node > getCircularDependencies( Dictionary dict )
{
	vector< Node > circular_nodes;

	auto has_self_dependency = [&]( string key, list<string> deps ) -> bool
	{
		// base case: 
		// If the node includes itself in the dependencies, it's got a cycle.
		if( std::find( deps.begin(), deps.end(), key ) != deps.end() )
			return true;
		for( const auto &dep : deps )
		{

		}
		return false;
	};

	// lambda to apply to each node to follow dependency?
	for( const auto &[key, deps] : dict )
	{
		// can delete nodes from original tree, shrink it
		// move to vector? 
		if( has_self_dependency( key, deps ) )
		{
			circular_nodes.emplace_back( key, deps );
			continue;
		}
		set<string> indirect_dependencies = { key };
		indirect_dependencies.insert( deps.begin(), deps.end() );
		//while( !indirect_dependencies.empty() )
		//{
			for( const auto &depend : indirect_dependencies )
			{
				if( dict.contains( depend ) && has_self_dependency( key, dict[ depend ] ) )
				{
					circular_nodes.emplace_back( depend, dict[ depend ]);
				}
				//indirect_dependencies.insert( dict[ depend ].begin(), dict[ depend ].end() );
			}
//		}
	}


	// Return a copy
	return circular_nodes;
}


bool testCircularDependencies()
{
	Dictionary test_dict =
	{
		{"A", {"A"}},
		{"B", {"C", "D"}},
		{"D", {"E"}},
		{"E", {"F", "Q"}},
		{"F", {"D"}}
	};

	try
	{
		auto circular_nodes = getCircularDependencies( test_dict );
		for( const auto &node : circular_nodes )
		{
			cout << std::format( "\"{}\": ", node.first );
			for( const auto &dep : node.second )
				cout << std::format( "\"{}\", ", dep );
			cout << endl;
		}
	}
	catch( std::exception e )
	{
		std::cout << "Test 2 failed: " << e.what() << endl;
	}

	return true;
}

bool testShipSinking()
{
	Map testMap;
	Ship myShip( Point2D{ 0, 0 }, Point2D{ 5, 0 }, "Nina" );

	cout << "Point [0, 5] should not hit." << endl;
	cout << "isHit: " << myShip.isHit({0, 5}) << endl;
	cout << "isSunk: " << myShip.hit( { 0, 5 } ) << endl;

	cout << "Point [3, 0] should hit" << endl;
	cout << "isHit: " << myShip.isHit({3, 0}) << endl;
	cout << "isSunk: " << myShip.hit( { 3, 0 } ) << endl;

	return true;
}

int main()
{
	cout << "Question 1: Write a function that iterates through an integer array and returns the sum of the values in the array." << endl;
	// Using non-zero return codes here as a quick way to indicate failure
	if( !testSum() )
		return 1;

	cout << "Question 2: Given a set of dependent services, write code which detects circular references and returns the node(s) where circular references occur." << endl;
	if( !testCircularDependencies() )
		return 2;

	cout << "Question 3: Batteship, basically." << endl;
	cout << "Create an algorithm which takes in two coordinates as parameters and determines whether it misses, hits, already selected, or sunk the ship." << endl;
	testShipSinking();

	return 0;
}
