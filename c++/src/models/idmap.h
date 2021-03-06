#ifndef idmap
#define idmap
#include <array>
#include<vector>
#include<string>
class Idmap {
	//stores a map of player number and team to an unused int
	//findid and addid find and add new players respectively
	//they both take a 2 array in form teamid,playernum
	//findid returns -1 if player is not found or the mapped int if it is
	private:
		std::vector<std::array<int, 2>> theMaps;
	public:
		int findid(std::array<int,2>);
		std::string getid(int);
		int getLength();
};
#endif
