#include "game.h"
#include <stdlib.h>
#include "frame.h"
#include "ball.h"
#include "player.h"
#include <fstream>
#include <iostream>
#include<sstream>
#include<string>
#include<array>
#include<algorithm>
#include"../ReadTrackingData/include/FOOTBALL/Football.h"
#include"../helpers/functions.h"
//Game::Game(){
//	filepath = "/pc2014-data1/lah/data/data_files/987881";
//}
Game::~Game(){
	for(int i = 0;i<frames.size();i++){
		delete frames[i];
	}
}
Game::Game(int pmid, std::string pfilename): filename{pfilename}, mid{pmid}{};
void Game::addFrame(Frame* pframe){
	frames.push_back(pframe);
}
bool Game::readFile(bool mapped){
	std::ifstream inFile;
	std::string csvfile = filename + std::to_string(mid) + ".csv";
	inFile.open(csvfile);
	if (!inFile) {
		std::cout << "Unable to open file" << std::endl;
		return false;
	}
	std::string bin;
	std::getline(inFile, bin);
	std::string lineSave;
	bool lineSaveFlag=true;
	int fid = 0;
	while (inFile) {
		std::string pline;
		if (lineSaveFlag) {
			std::getline(inFile, pline);
		}
		else {
			pline = lineSave;
			lineSaveFlag = true;
		}
		std::stringstream sline(pline);
		std::size_t found = pline.find("Ball");
		if (found != std::string::npos) {
			Frame* tframe = new Frame;
			bool corFrame = true;
			//ball line so create frame
			double ballx, bally;
			sline >> bin >> fid;
			tframe->addFid(5*fid);
			for (int i = 0; i < 4; i++) {
				sline >> bin;
			}
			sline >> ballx >> bally;
			Ball* tball = new Ball(ballx, bally, 0);
			tframe->addBall(tball);
			while (corFrame) {
				if (inFile) {
					std::getline(inFile, pline);
					std::stringstream sline(pline);
					std::size_t found = pline.find("Ball");
					if (found == std::string::npos) {
						double playx, playy;
						int mnum, num, home;
						for (int i = 0; i < 5; i++) {
							sline >> bin;
						}
						sline >> num >> playx >> playy;

						found = pline.find("Home");
						if (found != std::string::npos) {
							home = 1;
						}
						else { home = 0; }
						if(mapped){
							std::array<int,2> playid={num, home};
							mnum = idmapd.findid(playid);
							if (mnum < 0){
								std::cout << mnum <<std::endl;
							}
						}
						Player* tplayer = new Player(num, playx, playy, home, mnum);
						tframe->addPlayer(tplayer);
					}
					else {
						lineSave = pline;
						addFrame(tframe);
						corFrame = false;
						lineSaveFlag = false;
					}
				}
				else {
					corFrame = false;
				}
			}
		}
	}
	inFile.close();
	std::cout << "Read file: " << frames.size() << std::endl;
	return true;
}
std::array<int,2> Game::output(std::ofstream & names, std::vector<int> prevDone){
	Football::Match ex_match;
	if(ex_match.loadFromFile(filename,mid)== true){
		Football::Frame oneFrame = ex_match.get_frame(0);
		int h = oneFrame.HOMETEAM.get_team_roster().get_team_id();
		int a = oneFrame.AWAYTEAM.get_team_roster().get_team_id();
		bool ab{true},hb{true};
		std::array<int,2> rearr = {-1,-1};
		for (auto i : prevDone){
			if (i == h){
				hb = false;
			}
			if (i == a){
				ab = false;
			}
		}
		if (hb){
			names << oneFrame.HOMETEAM.get_team_roster().get_team_id() << "," << oneFrame.HOMETEAM.get_team_roster().get_team_name() << std::endl;
			rearr[0] = h;
		}
		if(ab){
			names << oneFrame.AWAYTEAM.get_team_roster().get_team_id() << "," << oneFrame.AWAYTEAM.get_team_roster().get_team_name() << std::endl;
			rearr[1] = a;
		}
		return rearr;
	}
}

bool Game::readNewFile(std::vector<int> teamids, int & ateamid, int & hteamid){
	Football::Match ex_match;
		if(ex_match.loadFromFile(filename,mid)== true){
			ex_match.remove_dead_frames();
			Football::Frame oneFrame = ex_match.get_frame(3);
			bool hometeam = false;
			bool awayteam = false;
			if(teamids[0]!=-1){
				for(auto i : teamids){
					if(oneFrame.HOMETEAM.get_team_roster().get_team_id() == i){
						hteamid = i;
						hometeam = true;
					}
					if(oneFrame.AWAYTEAM.get_team_roster().get_team_id() == i){
						ateamid = i;
						awayteam = true;
					}

				}
				if(hometeam != true && awayteam != true){
					std::cout << "Neither away or home team had a matching id" << std::endl;
					return false;
				}
			}
		//	ex_match.mirror_alternate_periods();
			int leftCount = 0;
			int rightCount = 0;
			for(int i=0; i<oneFrame.HOMETEAM.get_playersInTeam().size();i++){
					Football::Player tplayer = oneFrame.HOMETEAM.get_playersInTeam()[i];
					double x = tplayer.get_posX();
		//			std::cout << x << ",";
					if(x<-2000){
						leftCount ++;
					}
					if(x>2000){
						rightCount ++;
					}
			}
		//	std::cout << std::endl;
			if (leftCount>0&&rightCount ==0){
				homeSide = 'L';
			}
			else{
				if (rightCount>0&&leftCount==0){
					homeSide = 'R';
					}
				else{std::cout<<"uhoh could not not work out which side of the pitch everyone was on" << std::endl;
				return false;}
			}
			for(int i = 0; i < ex_match.number_of_frames();i++){
				Football::Frame tframe = ex_match.get_frame(i);
				Frame * myFrame = new Frame();
				for(int i=0; i<tframe.HOMETEAM.get_playersInTeam().size();i++){
					Football::Player tplayer = tframe.HOMETEAM.get_playersInTeam()[i];
		//		for(auto Football:: = tframe.HOMETEAM.get_playersInTeam().begin; nplayerit<tframe.HOMETEAM.get_playersInTeam().end();++nplayerit){
					double x = tplayer.get_posX();
					double y = tplayer.get_posY();
					int num = tplayer.get_shirtNumber();
					int home = 0;
					std::array<int,2> playid={num, home};
					int mnum = idmapd.findid(playid);
					Player* myPlayer = new Player(num, x, y, home, mnum);
					myFrame->addPlayer(myPlayer);
				}
				for(int i=0; i<tframe.AWAYTEAM.get_playersInTeam().size();i++){
			//	for(auto nplayerit = tframe.AWAYTEAM.get_playersInTeam().begin; nplayerit<tframe.AWAYTEAM.get_playersInTeam().end();++nplayerit){
					Football::Player tplayer = tframe.AWAYTEAM.get_playersInTeam()[i];
					double x = tplayer.get_posX();
					double y = tplayer.get_posY();
					int num = tplayer.get_shirtNumber();
					int home = 1;
					std::array<int,2> playid={num, home};
					int mnum = idmapd.findid(playid);
					Player* myPlayer = new Player(num, x, y, home, mnum);
					myFrame->addPlayer(myPlayer);
				}
				double x = tframe.BALL.get_posX();
				double y = tframe.BALL.get_posY();
				double z = tframe.BALL.get_posZ();
				Ball* tball = new Ball(x,y,z);
				myFrame->addBall(tball);
				myFrame->addFid(tframe.BALL.get_frameId());
				int possession;
				if(tframe.BALL.get_owningTeam() == 'H'){
					possession = 0;
				}
				else{
					if(tframe.BALL.get_owningTeam() == 'A'){
						possession = 1;
					}
					else{possession = -1;}
				}
				myFrame->addPossession(possession);
			//	for(auto frameit = frames.begin();frameit<frames.end();++frameit){
		//			std::vector<Player*> players = (->getPlayers();
		//			for(auto playerit = players.begin(); playerit < players.end();++playerit){
		//				std::cout << (*playerit)->getPos()[0]<< std::endl;
		//			}
			//	}
				addFrame(myFrame);
			}
	//	}
	/*	for(auto frameit = frames.begin();frameit<frames.end();++frameit){
			std::vector<Player*> players = (*frameit)->getPlayers();
			for(auto playerit = players.begin(); playerit < players.end();++frameit){
				std::cout << (*playerit)->getPos()[0]<< std::endl;
			}
		}*/ 
			return true;
		}
		else{return false;}
}

void Game::writeFile(std::ofstream & os,int type){
	int count = 0;
	for(std::vector<Frame*>::iterator frameit = frames.begin();frameit<frames.end();++frameit){
		(*frameit)->objectInfo(os,type);
		count++;
	}
	std::cout << "line wrote" << count << std::endl;
}

void Game::findPhases() {
	//find the breaks in game data
	frames[0]->setFirstFrame();
	int previousFid = frames[0]->getFid();
	for (std::vector<Frame*>::iterator frameit = frames.begin(); frameit < frames.end(); ++frameit) {
		if (((*frameit)->getFid()) != previousFid + 1) {
			(*frameit)->setFirstFrame();
		}
		previousFid = ((*frameit)->getFid());
	}
}
int Game::getMapLength(){
	return idmapd.getLength();
}
void Game::addBVelocities(){
	//add velocity data to frames
	//uses current +-2 frames so does not use frames close to new phases
	std::vector<std::array<double,2>> positions;
	//for each team for each player a vector of 2-array co-ordinates
	int previousFid=-1;
	for (std::vector<Frame*>::iterator frameit = frames.begin(); frameit < frames.end(); ++frameit) {
		if ((*frameit)->getFid() != previousFid + 1) {
			for (int i = 0; i < positions.size(); i++) {
					positions.clear();
			}
		}
		Ball * ball = (*frameit)->getBall();
		std::array<double,2> posball = ball->getPos();
			positions.push_back( posball);
			if (positions.size() > 3) {
				positions.erase(positions.begin());
			}
			if (positions.size() == 3) {
				std::array<double,2> velocity;
				for (int j = 0; j < 2; j++) {
			velocity[j] = ((positions[0][j]) 
						 - (positions[2][j]));
				}
				(*(frameit - 1))->getBall()->setVelocity(velocity);
			}
	previousFid = (*frameit)->getFid();
	}
//	std::cout << "done" <<std::endl;
}
void Game::addVelocities(){
	//add velocity data to frames
	//uses current +-2 frames so does not use frames close to new phases
	std::vector<std::vector<std::array<double,2>>> positions;
//	std::cout << idmapd.getLength() << std::endl;
	positions.resize(idmapd.getLength());
	//for each team for each player a vector of 2-array co-ordinates
	int previousFid=-1;
	for (std::vector<Frame*>::iterator frameit = frames.begin(); frameit < frames.end(); ++frameit) {
		if ((*frameit)->getFid() != previousFid + 1) {
			for (int i = 0; i < positions.size(); i++) {
					positions[i].clear();
			}
		}
		std::vector<Player*> players = (*frameit)->getPlayers();
		for (auto playerit = players.begin(); playerit < players.end();
			++playerit) {
			int pid = (*playerit)->getMappedPid();
			std::array<double,2> posb = (*playerit)->getPos();
			positions[pid].push_back( posb);
			if (positions[pid].size() > 5) {
				positions[pid].erase(positions[pid].begin());
			}
			if (positions[pid].size() == 5) {
				std::array<double,2> velocity;
				for (int j = 0; j < 2; j++) {
					velocity[j] = ((positions[pid][0][j]) - 8 * (positions[pid][1][j])
						+ 8 * (positions[pid][3][j]) - (positions[pid][4][j])) / (12 * 0.2*100);
				}
				(*(frameit - 2))->findPid(pid)->setVelocity(velocity);
			}
		}
	previousFid = (*frameit)->getFid();
	}
//	std::cout << "done" <<std::endl;

}
Idmap Game::getMap(){
	return idmapd;
}
/*void Game::setClosestPlayers(){
	for(auto frameit = frames.begin();frameit<frames.end();++frameit){
		(*frameit)->closestPlayers();
	}
}
void Game::writeClosestPlayers(int game){
	for(auto frameit = frames.begin();frameit<frames.end();++frameit){
		const std::vector<Player*>&	playersFrame = (*frameit)->getPlayers();
		for(auto playerit = playersFrame.begin();playerit<playersFrame.end();++playerit){
			std::array<double,2> tinfo;
			tinfo[0] = (*frameit)->getFid();
			tinfo[1] = (*playerit)->getClosestDist();
			int tnum = (*playerit)->getNum();
			int tteam = (*playerit)->getTeam();
			bool found = true;
			for(auto playerGameit = playersGame.begin();playerGameit<playersGame.end();++playerGameit){
				if(found){
					if((*playerGameit)->getNum() == tnum && (*playerGameit)->getTeam() == tteam){
						(*playerGameit)->addDist(tinfo);
						found = false;
					}
				}
			}
			if (found){
					WholePlayer* tWholePlayer = new WholePlayer(tnum, tteam);
					tWholePlayer->addDist(tinfo);
					playersGame.push_back(tWholePlayer);
			}
		}
	}
	for(auto playerit = playersGame.begin();playerit<playersGame.end();++playerit){
		std::vector<std::array<double,2>> stencil;
		std::vector<std::array<double,2>> velocity;
		double tvelocity;
		bool inVec{true};
		int i = 0;
		int previousFid{0};
		while(inVec){
			try{
				std::array<double,2> temp = (*playerit)->getClosestDistance(i);
				if (temp[0] == previousFid + 1){
					stencil.push_back(temp);
					previousFid = temp[0];
					if (stencil.size()>3){
						stencil.erase(stencil.begin());
					}
					if (stencil.size() == 3){
						tvelocity = (stencil[2][1]-stencil[0][1])/0.4;
						std::array<double,2> tvelar = {stencil[1][0], tvelocity};
						velocity.push_back(tvelar);
					}
				}
				else{
					stencil.clear();
					stencil.push_back(temp);
				}
				i++;
			}
			catch (std::out_of_range& e){
				inVec = false;
			}

		}
	}
	for(auto playerit = playersGame.begin();playerit<playersGame.end();++playerit){
		std::string filename = "../data/playerclosestdist/" + std::to_string(game) + "_" + std::to_string((*playerit)->getTeam()) + "-" + std::to_string((*playerit)->getNum()) +".txt";
		std::ofstream os;
		os.open(filename);
		os << "Frame" << "\t" << "Closesplayer" << std::endl;
		int i = 0;
		bool stop = true;
		while (stop){
			try{
				(*playerit)->getClosestDistance(i);
				os  << (*playerit)->getClosestDistance(i)[0] << "\t" << (*playerit)->getClosestDistance(i)[1] << std::endl;
				i++;
			}
			catch (std::out_of_range& e){
				stop = false;				
			}
		}
	os.close();
	}
}*/
bool Game::storeMdata(){
	std::ifstream inFile;
    	std::string mdatafile = filename + std::to_string(mid) + ".mdata";
	inFile.open(mdatafile);
	if (!inFile) {
		std::cout << "Unable to open file";
		return false;
	}
	int bin;
	inFile >> bin >> pitchx >>pitchy >> homeNum >> awayNum;
    return true;
}
std::vector<double> Game::calcVoronoi(){
	std::vector<double> ratios;
	for(auto frameit=frames.begin();frameit<frames.end();++frameit){
		ratios.push_back((*frameit)->getVoronoi(pitchx,pitchy));
	}
	return ratios;
}
void Game::writeVoronoi(){
	std::ofstream os;
	std::string filename = "data/voronoi/" + std::to_string(mid) + ".txt";
	os.open(filename);
	std::vector<double> ratios = calcVoronoi();
	for(auto i = 0; i<ratios.size();i++){
		os << ratios[i] << std::endl;
	}
	os.close();
}
void Game::splitFrames(){
	std::string filename;
	filename = "posdata/" + std::to_string(mid) + ".txt";
	std::ifstream inFile;
	inFile.open(filename);
	if (!inFile) {
		std::cout << "Unable to open possession file for: " << filename << std::endl;
		return;
	}
	while(inFile){
		int startFrame, endFrame, tid;
		inFile >> tid >> startFrame >> endFrame;
		bool stopPlace{true};
		int count{0};
		while(stopPlace&&count<frames.size()){
			if (frames[count]->getFid()>=startFrame && frames[count]->getFid()<=endFrame){
				std::cout << frames[count] << std::endl;
				if (tid ==homeNum){
					homeFrames.push_back(frames[count]);
					count ++;
				}
				else {
					awayFrames.push_back(frames[count]);
					count ++;
				}
			}
			if (frames[count]->getFid()>endFrame){
				stopPlace = false;

			} else{count ++;}
		}
	}
	std::cout << "Frame save ratio: " << (homeFrames.size()+awayFrames.size())/frames.size() << std::endl;
}
/*
void Game::getScalar(){
	for (auto frameit = frames.begin();frameit<frames.end();++frameit){
		(*frameit)->computeScalar(0);
	}
}
*/
std::vector<Frame*> Game::getFrames(){
    return frames;
}
int Game::getHome(){
    return homeNum;
}
int Game::getX(){
	return pitchx;
}
int Game::getY(){
	return pitchy;
}
char Game::getHomeSide(){
	return homeSide;
}
void Game::setFramesPlayersDistance(double radius){
	for(auto frameit = frames.begin();frameit<frames.end();++frameit){
		(*frameit)->setPlayerDistance(radius);
	}
}
void Game::setFramesPlayersVelocity(){
	double previousFid{0};
	std::array<std::array<std::vector<double>,28>,28> allInfo;
	for(auto frameit = frames.begin();frameit<frames.end();++frameit){
		if((*frameit)->getFid() == previousFid + 1){
			std::vector<Player*> aplayers = (*frameit)->getPlayers();
			for(auto playerit = aplayers.begin();playerit<aplayers.end();++playerit){
				if((*playerit)->getTeam()==(*frameit)->getAttacking()){
					int pid = (*playerit)->getMappedPid();
					for(int i = 0; i < 28; i ++){
						if((*playerit)->getPlayerPlayerDistance(i) != 0){
						//	std::cout << pid << "," << i << "," << (*playerit)->getPlayerPlayerDistance(i) << "AAAAA" << std::endl;
							if((*playerit)->getTeam() == 0){
								allInfo[pid][i].push_back((*playerit)->getPlayerPlayerDistance(i));
							}
							else{allInfo[i][pid].push_back((*playerit)->getPlayerPlayerDistance(i));}
						}
						else{
							if((*playerit)->getTeam() == 0){
								allInfo[pid][i].clear();
								}

							else{allInfo[i][pid].clear();}
						}							
					}
				}
			}
		}
		else{
			for(int i = 0;i<28;i++){
				for(int j = 0;j<28;j++){
					allInfo[i][j].clear();
				}
			}
		}
		for(int i = 0;i<28;i++){
			for(int j = 0;j<28;j++){
				if(allInfo[i][j].size() > 3){
					allInfo[i][j].erase(allInfo[i][j].begin());
				}
				if(allInfo[i][j].size() == 3){
					double velocity = allInfo[i][j][0]-allInfo[i][j][2];
				//	std::cout << "AFTER";
				//	std::cout << (*(frameit))->getFid();
				//	std::vector<Player*> playersss = (*frameit)->getPlayers();
				//	for(auto l : playersss){
				//		std::cout << l->getMappedPid() << ",";
				//	}
				//	std::cout << std::endl;
				//	std::cout << "," << frames[frames.size()-1]->getFid() << std::endl;
					if((*frameit)->getAttacking() == (*frameit)->findPid(i)->getTeam()){
						(*(frameit-1))->findPid(i)->setPlayerPlayerVelocity(velocity,j);
					}
					else{
				//		std::cout << "pooo" << std::endl;
						(*(frameit-1))->findPid(j)->setPlayerPlayerVelocity(velocity,i);
					}
				}
			}
		}	
		previousFid = (*frameit)->getFid();

	}
}
void Game::setAllGoalPos(){
	//Comented is a test function for goal side check
//	std::array<double,28> count = {0};
	for(auto it = frames.begin();it<frames.end();++it){
		(*it)->setGoalPos(homeSide);
	/*	std::vector<Player*> players = (*it)->getPlayers();
		double mindist{5000};
		int minPlayer;
		for(auto pit = players.begin();pit<players.end();pit++){
			std::array<double,2> G = {-5250,0};
			if(distance((*pit)->getPos(),G)<mindist){
				mindist = distance((*pit)->getPos(),G);
				minPlayer = (*pit)->getMappedPid();
			}
		}
		count[minPlayer] ++;
	*/}/*
	for(auto i : count){
		std::cout << i << ",";
	}
	std::cout << std::endl;*/
}
std::vector<std::array<int,4>> Game::getPhases(int minDefA, double minDefVel, int minFrames,int postPressDistance,bool errorMsg){
	int previousFid {-1};
	int previousPossession {0};
	int length{0};
	bool postPress{false};
	int timeToGo;
	int endType;
	int startFrame;
	std::vector<std::array<int,4>> startLengthTypes;
	std::array<int,4> startLengthType;
	for(auto frameit = frames.begin();frameit<frames.end();++frameit){
		if(length == 0){
			startFrame = (*frameit)->getFid();
			postPress = false;
		}
		if(!postPress){
			if((*frameit)->getAttacking()==previousPossession&&(*frameit)->getFid()==previousFid+1){
				if((*frameit)->getRunningForward(minDefVel)>minDefA){
					length++;
				}
				else{
					if(length<minFrames){
						length = 0;
					}
					else{
						timeToGo = postPressDistance;
						postPress = true;
					}
				}
			}
			else{
				if(length>=minFrames){
					if((*frameit)->getFid()==previousFid+1){
						startLengthType = {startFrame,length,2,previousPossession};
						startLengthTypes.push_back(startLengthType);
					}
					else{
						startLengthType = {startFrame,length,1,previousPossession};
						startLengthTypes.push_back(startLengthType);
					}
				}
				length = 0;
			}
		}
		else{
			if((*frameit)->getAttacking()==previousPossession){
				if((*frameit)->getFid()!=previousFid+1){
					if(errorMsg){
						std::cout<< "no possession change but frame jump" << previousPossession << "," << (*frameit)->getAttacking() << "," << previousFid << "," << (*frameit)->getFid() << std::endl;
					}
					endType = 1;
					startLengthType = {startFrame,length,endType,previousPossession};
					length = 0;
					startLengthTypes.push_back(startLengthType);
				}
				else{
					if(timeToGo>0){
						timeToGo --;
					}
					else{
						endType = 0;
						startLengthType = {startFrame,length,endType,previousPossession};
						length = 0;
						startLengthTypes.push_back(startLengthType);
					}
				}
			}
			else{
				if((*frameit)->getFid()!=previousFid+1){
					endType = 1;
				}
				else{
					endType = 2;
				}
				startLengthType = {startFrame,length,endType,previousPossession};
				startLengthTypes.push_back(startLengthType);
				length = 0;
			}
		}	
		previousPossession = (*frameit)->getAttacking();
		previousFid = (*frameit)->getFid();
	}
	return startLengthTypes;
}

std::vector<std::array<double,2>> Game::getPhaseInformation(std::vector<std::array<int,2>> startSizes, int startLookingDistance, int lookingLength, double radius, int type, double closePressure, std::array<double, 7> parameters, double playerRadius){
	//
	std::vector<Frame*>::iterator frameit = frames.begin();
	double startBallDistance{0};
	double endBallDistance{0};
	double pressure;
	std::vector<std::array<double,2>> pressureBallDists;
	for(auto phaseit = startSizes.begin(); phaseit<startSizes.end();++phaseit){
		while((*frameit)->getFid()!=(*phaseit)[0]){
			std::advance(frameit,1);
		}
		std::advance(frameit,startLookingDistance);
//		want to make some sort of collective pressure within a phase
//		maybe using the old phase weighting 
//		pressure = (*frameit)->getPressure(radius);
		pressure = (*frameit)->getPressureB(closePressure, parameters, playerRadius);
		if(type == 0){
			startBallDistance = (*frameit)->getBallDistance();
			std::advance(frameit,lookingLength);
			endBallDistance = (*frameit)->getBallDistance();
		}
		std::array<double,2> pressureBallDist = {pressure,endBallDistance - startBallDistance};
		pressureBallDists.push_back(pressureBallDist);
	}
	return pressureBallDists;
}
std::vector<double> Game::getPhasePosition(std::vector<std::array<int,2>> startSizes){
	std::vector<Frame*>::iterator frameit = frames.begin();
	std::vector<double> positions;
	for (auto phaseit = startSizes.begin();phaseit<startSizes.end();++phaseit){
		while((*frameit)->getFid()!=(*phaseit)[0]){
			std::advance(frameit,1);
		}
		std::array<double,2> goal = {(*frameit)->getGoalPos(),0};
		positions.push_back(distance((*frameit)->getBall()->getPos(),goal)/100);
	}
	return positions;
}

std::array<int,2> Game::getPossessionTimes(){
	std::array<int,3> count = {};
	for (auto frameit = frames.begin();frameit<frames.end();++frameit){
		count[(*frameit)->getAttacking()] ++;
	}
	std::array<int,2> results = {count[0],count[1]};
	return results;
}

std::vector<std::vector<std::vector<std::vector<std::array<int,3>>>>>	Game::getAllPhases(std::vector<int> defenders, std::vector<double> velocities, std::vector<int> minimumFrames, std::vector<int> postPressTimes){
	//this function should probably be deleted
	std::vector<std::vector<std::vector<std::vector<std::array<int,3>>>>> numbers;
	for(auto minDef = defenders.begin();minDef<defenders.end();++minDef){
		std::vector<std::vector<std::vector<std::array<int,3>>>> velNumber;
		for(auto minVel = velocities.begin(); minVel< velocities.end(); ++minVel){
			std::vector<std::vector<std::array<int,3>>> frameNumber;
			for(auto minFrames = minimumFrames.begin();minFrames<minimumFrames.end();++minFrames){
				std::vector<std::array<int,3>> postNumber;
				for(auto postPressTime = postPressTimes.begin();postPressTime<postPressTimes.end(); ++postPressTime){
					std::vector<std::array<int,4>> frameStartLengthType;
					frameStartLengthType = getPhases(*minDef,*minVel,*minFrames,*postPressTime,false);
					std::vector<std::array<int,2>> noPossessionChangePhases;
					std::vector<std::array<int,2>> possessionChangePhases;
					std::vector<std::array<int,2>> frameJumpPhases;
					splitByType(frameStartLengthType,noPossessionChangePhases,possessionChangePhases,frameJumpPhases);		
					std::array<int,3> temp = {noPossessionChangePhases.size(),possessionChangePhases.size(),frameJumpPhases.size()};
					postNumber.push_back(temp);
				}
				frameNumber.push_back(postNumber);
			}
			velNumber.push_back(frameNumber);
		}
		numbers.push_back(velNumber);
	}
	return numbers;
}

std::vector<std::vector<std::vector<std::vector<std::array<int,3>>>>>	Game::getAllPhasesB(std::vector<int> defenders, std::vector<double> velocities, int minimumFrames, std::vector<int> postPressTimes, std::vector<int> lengthBins){
	std::vector<std::vector<std::vector<std::vector<std::array<int,3>>>>> numbers;
	for(auto minDef = defenders.begin();minDef<defenders.end();++minDef){
		std::vector<std::vector<std::vector<std::array<int,3>>>> velNumber;
		for(auto minVel = velocities.begin(); minVel< velocities.end(); ++minVel){
			std::vector<std::vector<std::array<int,3>>> postNumber;
			for(auto postPressTime = postPressTimes.begin();postPressTime<postPressTimes.end(); ++postPressTime){
				std::vector<std::array<int,4>> frameStartLengthType;
				frameStartLengthType = getPhases(*minDef,*minVel,minimumFrames,*postPressTime,false);
				std::vector<std::array<int,2>> noPossessionChangePhases;
				std::vector<std::array<int,2>> possessionChangePhases;
				std::vector<std::array<int,2>> frameJumpPhases;
				splitByType(frameStartLengthType,noPossessionChangePhases,possessionChangePhases,frameJumpPhases);		
				std::vector<std::vector<std::array<int,2>>> sortNoPos = lengthSort(noPossessionChangePhases,lengthBins);
				std::vector<std::vector<std::array<int,2>>> sortPos = lengthSort(possessionChangePhases,lengthBins);
				std::vector<std::vector<std::array<int,2>>> sortFrame = lengthSort(frameJumpPhases,lengthBins);
				std::vector<std::array<int,3>> lengthTypeNumber;
				for(int i = 0; i < sortNoPos.size();i++){
					std::array<int,3> typeNumber = {sortNoPos[i].size(),sortPos[i].size(),sortFrame[i].size()};
					lengthTypeNumber.push_back(typeNumber);
				}
				postNumber.push_back(lengthTypeNumber);
			}
			velNumber.push_back(postNumber);
		}
		numbers.push_back(velNumber);
	}
	return numbers;
}

std::vector<std::vector<std::vector<std::array<std::vector<std::array<int,2>>,3>>>> Game::getBinnedPosition(std::vector<int> defenders, std::vector<double> velocities, std::vector<int> minimumFrames, int postPressTime){
	std::vector<std::vector<std::vector<std::array<std::vector<std::array<int,2>>,3>>>> defNumPos;
	for(auto minDef = defenders.begin();minDef<defenders.end();++minDef){
		std::vector<std::vector<std::array<std::vector<std::array<int,2>>,3>>> velPos;
		for(auto minVel = velocities.begin(); minVel< velocities.end(); ++minVel){
			std::vector<std::array<std::vector<std::array<int,2>>,3>> framePos;
			for(auto minFrames = minimumFrames.begin();minFrames<minimumFrames.end();++minFrames){
				std::vector<std::array<int,4>> frameStartLengthType;
				frameStartLengthType = getPhases(*minDef,*minVel,*minFrames,postPressTime,false);
				std::array<std::vector<std::array<int,2>>,3> allPhases;
				std::cout << "Phases: " << *minDef << "," << *minVel << ","<< *minFrames << "," << postPressTime << ": " <<  frameStartLengthType.size() << std::endl;
				splitByType(frameStartLengthType,allPhases[0],allPhases[1],allPhases[2]);		
				std::array<std::vector<double>,3> allPos;
				for(int i = 0;i<3;i++){
					allPos[i] = getPhasePosition(allPhases[i]);
				}
				std::array<std::vector<std::array<int,2>>,3> locationNumber;
				for(int j = 0;j<3;j++){
					for(int i = 0; i<=110; i+=10){
						std::array<int,2> temp = {i,0};
						locationNumber[j].push_back(temp);
					}
					for(double pos : allPos[j]){
						int count = 0;
						while(pos>0){
							pos = pos - 10;
							count ++;
						}
						locationNumber[j][count][1] += 1;
					}	
				}
				framePos.push_back(locationNumber);
			}
			velPos.push_back(framePos);
		}
		defNumPos.push_back(velPos);
	}
	return defNumPos;
}
std::vector<std::vector<std::array<int,2>>> Game::lengthSort(std::vector<std::array<int,2>> lengths, std::vector<int> bins){
	std::vector<std::vector<std::array<int,2>>> vectoredLengths;
	vectoredLengths.resize(bins.size());
	for(auto i : lengths){
		int j{0};
		bool unfound{true};
		while(j<bins.size()&&unfound){
			if(i[1]<bins[j]){
				vectoredLengths[j].push_back(i);
				unfound = false;
			}
			j++;
		}
		if(unfound == true){
			std::cout << "Phaselength greater than final bin" << std::endl;
		}
	}
	return vectoredLengths;
}
	
std::vector<std::vector<double>> Game::getVectoredPhaseInformation(std::vector<std::array<int,2>> startSizes, int startLookingDistance, int lookingLength, int type,int group){
	//
	std::vector<Frame*>::iterator frameit = frames.begin();
	double startBallDistance{0};
	double endBallDistance{0};
	double pressure;
	std::vector<std::vector<double>> pressureBallDists;
	for(auto phaseit = startSizes.begin(); phaseit<startSizes.end();++phaseit){
		while((*frameit)->getFid()!=(*phaseit)[0]){
			std::advance(frameit,1);
		}
		std::advance(frameit,startLookingDistance);
//		want to make some sort of collective pressure within a phase
//		maybe using the old phase weighting 
//		pressure = (*frameit)->getPressure(radius);
//		pressure = (*frameit)->getPressureB(closePressure, parameters, playerRadius);
		std::vector<double> pressures = (*frameit)->getPressureComponents();
		if(type == 0){
			startBallDistance = (*frameit)->getBallDistance();
			std::advance(frameit,lookingLength);
			endBallDistance = (*frameit)->getBallDistance();
		}
		std::vector<double> pressureBallDist;
		pressureBallDist.push_back(1.0*type);
		pressureBallDist.push_back(1.0*group);
		pressureBallDist.push_back((endBallDistance-startBallDistance)/100);
		for(auto i : pressures){
			pressureBallDist.push_back(i);
		}
		pressureBallDists.push_back(pressureBallDist);
	}
	return pressureBallDists;
}
std::vector<std::vector<double>> Game::getOptimisedVectoredPhaseInformation(std::vector<std::array<int,2>> startSizes, int lookingLength, int type,int group){
        //
        std::vector<Frame*>::iterator frameit = frames.begin();
        double startBallDistance{0};
        double endBallDistance{0};
        double pressure;
        std::vector<std::vector<double>> pressureBallDists;
        int noPassPhases{0}, foundPasses{0}, foundPoopy{0};
        for(auto phaseit = startSizes.begin(); phaseit<startSizes.end();++phaseit){
                while((*frameit)->getFid()!=(*phaseit)[0]){
                        std::advance(frameit,1);        
                }
		std::vector<Frame*>::iterator saveit = frameit;
                if(type == 0){
                        startBallDistance = (*frameit)->getBallDistance();
                        std::vector<Frame*>::iterator endit = std::next(frameit,lookingLength);
                        endBallDistance = (*endit)->getBallDistance();
                }
                frameit = findLastPassInFuture(frameit, (*phaseit)[1],noPassPhases,foundPasses,foundPoopy);
		if(frameit != frames.end()){
			std::vector<double> pressures = (*frameit)->getPressureComponents();
			std::vector<double> pressureBallDist;
			pressureBallDist.push_back(1.0*type);
			pressureBallDist.push_back(1.0*group);
			pressureBallDist.push_back((endBallDistance-startBallDistance)/100);
			for(auto i : pressures){        
				pressureBallDist.push_back(i);  
			}
			pressureBallDists.push_back(pressureBallDist);
		}
		else{
			frameit = saveit;
		}

        }
        std::cout << "no pass: " << noPassPhases << std::endl << "poopy pass: " << foundPoopy << std::endl << "pass: " << foundPasses << std::endl;
        return pressureBallDists;       
}
std::vector<Frame*>::iterator Game::findFirstPassInFuture(std::vector<Frame*>::iterator frameit, int phaseLength,int& noPassPhases, int& foundPasses, int& foundPoopy){
        int framesFromStart{0};
	while(distance((*frameit)->closestPlayerToBall()->getPos(),(*frameit)->getBall()->getPos())>150&&framesFromStart < phaseLength){
		std::advance(frameit,1);
		framesFromStart ++;
	}
        int closestAttacker = (*frameit)->closestPlayerToBall()->getMappedPid();
        while( closestAttacker == (*frameit)->closestPlayerToBall()->getMappedPid()&&framesFromStart<phaseLength){
                framesFromStart ++;             
                std::advance(frameit,1);        
        }
        if (framesFromStart==phaseLength){ 
                noPassPhases ++;                
        }
        else{
                std::advance(frameit,-1);       
                framesFromStart -= 1;
                double distanceToBall = distance((*frameit)->closestPlayerToBall()->getPos(),(*frameit)->getBall()->getPos());
                bool closer{true};
                while(closer&&framesFromStart>0){
                        framesFromStart -=1;
                        std::advance(frameit,-1);
                        double tdistanceToBall = distance((*frameit)->closestPlayerToBall()->getPos(),(*frameit)->getBall()->getPos());
                        if(tdistanceToBall<distanceToBall){
                                distanceToBall = tdistanceToBall;
                        }
                        else{
				closer = false;
			}
                }
                std::advance(frameit,1);
                if(distanceToBall < 150){
                        foundPasses ++;
                        return frameit;
                }
                else{
                        foundPoopy ++;
                }
        }
        return frames.end();
}
std::vector<Frame*>::iterator Game::findLastPassInFuture(std::vector<Frame*>::iterator frameit, int phaseLength,int& noPassPhases, int& foundPasses, int& foundPoopy){
        int framesFromEnd{0};
	std::advance(frameit,phaseLength);        
	while(distance((*frameit)->closestPlayerToBall()->getPos(),(*frameit)->getBall()->getPos())>150&&framesFromEnd < phaseLength){
		std::advance(frameit,-1);
		framesFromEnd ++;
	}
        int closestAttacker = (*frameit)->closestPlayerToBall()->getMappedPid();
        while( closestAttacker == (*frameit)->closestPlayerToBall()->getMappedPid()&&framesFromEnd<phaseLength){
                framesFromEnd ++;             
		std::advance(frameit,-1);
        }
        if (framesFromEnd==phaseLength){ 
                noPassPhases ++;                
        }
        else{
                std::advance(frameit,-1);       
                framesFromEnd += 1;
                double distanceToBall = distance((*frameit)->closestPlayerToBall()->getPos(),(*frameit)->getBall()->getPos());
                bool closer{true};
                while(closer&&framesFromEnd<phaseLength){
                        framesFromEnd +=1;
                        std::advance(frameit,-1);
                        double tdistanceToBall = distance((*frameit)->closestPlayerToBall()->getPos(),(*frameit)->getBall()->getPos());
                        if(tdistanceToBall<distanceToBall){
                                distanceToBall = tdistanceToBall;
                        }
                        else{
				closer = false;
			}
                }
                std::advance(frameit,-1);
                if(distanceToBall < 150){
                        foundPasses ++;
                        return frameit;
                }
                else{
                        foundPoopy ++;
                }
        }
        return frames.end();
}





		

	
