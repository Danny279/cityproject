/* 
 * possessionfilemaker.h
 *
 *  Created on: 6 Aug 2019
 *      Author: danny
 */

#ifndef POSSESSIONFILEMAKER_H_
#define POSSESSIONFILEMAKER_H_

#include <iostream>
#include<vector>
#include"possessionfile.h"
#include<array>
#include<iterator>
class PossessionFileMaker{

private:
	std::vector<PossessionFile*> possessionFiles;
	std::vector<std::array<int,2>> matchStarts; 
	//class to turn possesion fil for every game into seperate files for each game
public:
	bool readFile();
	void fileWriter();
	void gameFrameMap();
	PossessionFile * fileSpecificReader(int mid);
   	int findMatchStart(int mid);
   	std::vector<PossessionFile*> getPosFiles();
};



#endif /* POSSESSIONFILEMAKER_H_ */
