#ifndef allclosest
#define allclosest
#include <array>
#include <vector>
#include <fstream>
#include <iostream>
#include <iterator>
#include "../frame.h"
#include "../idmap.h"
struct findiftro {
	//finite difference calculator.
	//there is one of these for every player
	std::array<std::vector<double>,28> closest;
	//closest is an array for every player, with each point having a vector that stores the last three distances to that player
	std::array<int,28> consecutive;
	//consecutive is an integer that keeps track of how long its been since a frame break
	//if it is less that 3 frames since a frame break then the velocity as calculated will be meaningless. consecutive provides 
	//this information so velocity can be set to 0.it also catches cases where a player was not inside radius the frames before
	void addDist(double temporary_distance, int pid);
	//addDis takes distance provided by external function and adds to closest
	std::array<double,2> passPair(int pid);
	//returns an array of {dist, change in distance} for given player id
};
//not implementd, used for storing data(current implementation writes straight away)
struct frameInfo {
	int fid;
	int attacking;
	std::vector<std::array<double,2>> framePlayerPressures;
};

struct closeplayer {
	//represents a player
	std::ofstream playerPressures;
	//stream player info is written to
	findiftro  trolley;
	frameInfo tempInfo;
	//not implemented
	std::array<double,2> pullPair(int pid);
	//not implemented
	std::array<double,2> pullWritePassPair(int pid);
	//pulls info from trolley of distance to player with pid, writes this for its info file 
	//and also returns info so it can bewritten to other players info file	
	void writePair(std::array<double,2> info_pair);
	//writes info_pair to players output stream
};
class AllClosest {
//class to find closest player distance and rate of change. should be created outside a frame loop. openstreams should be called before frame loop and close streams
//after frameloop. needs to be a passed a frame iterator,previous frame id, previous attacking team id and distance threshold. previous because so it can write the velocity it is always writing information for the previous frame.
//Note:: playerid here is a mappedplayer id where each player gets assigned a unique lowest possible integer (actually just gets placed in vector and player id is vector place)
	private:
		int distanceThreshold;
		std::array<closeplayer,28> allPlayers;
		std::vector<Player*> homePlayers;
		std::vector<Player*> awayPlayers;	
	public:
		AllClosest(int pdistanceThreshold);
		void addPlayers(std::vector<Frame*>::iterator currentFrameit,int previousFid, int prevAttackingTeamid);
		//gets home and away player vectors for given frame
		//calculates all distances and if less than distance Threshold adds information to players trolley
		//this is so rate of change can be calculated in same loop 
		void closeStreams();
		void openStreams(int matchid, Idmap id_mappings);
};
#endif