#include<cmath>
#include<algorithm>
#include "pressureprocessor.h"
#include <iostream>
#include <iterator>
#include "../../STATS/runningstats.h"

PressureProcessor::PressureProcessor(double tframeWeighting, std::string tfilePath){
	previousFid = -1;
	frameWeighting = tframeWeighting;
	filePath = tfilePath;
}
void PressureProcessor::addPressure(std::array<double,5> info){
	/*TESTSTUFF
	tempPossession.push_back(info[2]);
	timeToPrint --;
	if(timeToPrint == 0){
		count ++;
		std::cout << "C" << count << std::endl;
		std::cout << "Pos: ";
		for(auto i : tempPossession){
			std::cout << i << ",";
		}
		std::cout << std::endl;
	}
	if(tempPossession.size()>7) {
		tempPossession.erase(tempPossession.begin());
	}*/
	if(info[0] != previousFid+1){
		sections.push_back(section);
		section.clear();
		std::array<double,5> temp;
		for (int i = 0; i < 5; i++){
			temp[i] = info [i];
		}
		section.push_back(temp);
		previousFid = info[0];
	//TEST	timeToPrint = 3;
	}
	else{
		std::array<double,5> temp;
		for (int i = 0; i < 5; i++){
			temp[i] = info [i];
		}
		section.push_back(temp);
		previousFid = info[0];
	}
}
void PressureProcessor::addFinalPressure(){
	sections.push_back(section);
}
std::array<int,3> PressureProcessor::lengthThreshold(double timeLength, double size, bool increasing, bool savePhases, int lookieLookie,int interestingTeam, bool testing){
	sections.erase(sections.begin());
	std::ofstream unsucFrameId;
	std::ofstream sucFrameId;
	unsucFrameId.open("../data/unsucFrameId.txt");
	sucFrameId.open("../data/sucFrameId.txt");
	std::vector<double> prevPressure;
	int zcount{0}, ycount{0}, xcount{0};
	double lowestPressure;
	int prevAttacking = -1;
	std::array<int,3> number = {0,0,0};
	bool largeEnough = false;
	bool possessionChange = false;
	if(increasing){
		for (auto sectionsit = sections.begin();sectionsit<sections.end();++sectionsit){
			ycount ++;
		//	std::cout <<  (*sectionsit).size() << std::endl;
		}
		std::cout << "TFB"<< ycount << std::endl;
		for (auto sectionsit = sections.begin();sectionsit<sections.end();++sectionsit){
			prevPressure.clear();
			largeEnough = false;
			int consecutive = 0;
			for(auto sectionit = (*sectionsit).begin();sectionit<(*sectionsit).end(); ++sectionit){
				//CASE FOR END OF SECTION (BALL HAS GONE OUT OF PLAY)
				/*
				if(sectionit == (*sectionsit).end()-1){
					if(testing){
					//	std::cout << "econ " << consecutive << std::endl;
						zcount = zcount+ consecutive+1;
						sizesb.push_back(consecutive + 1);
					}
					if(largeEnough){
						bool interested{false};
						if(interestingTeam != 2){
							if(interestingTeam != prevAttacking){
								interested = true;
							}
						}
						else{interested = true;}
			//			std::cout << "length of sphase"<< consecutive << std::endl;
						if(interested){
							number[2] += 1;
							if(savePhases == true){
								std::vector<std::array<double,5>> tphase;
								for(auto dit = sectionit-consecutive;dit<sectionit+1;++dit){
									tphase.push_back(*dit);
								}
								if(tphase.size() == consecutive +1){
					//				std::cout << "y" << std::endl;
								}
								else{std::cout << "n" << std::endl;}
								ePhases.push_back(tphase);
							}
						}
						else {std::cout << "uhuh" << std::endl;}
					}
				}
				else{*/
					//OTHERWISE
					if(prevPressure.size()>0){
						lowestPressure = *std::min_element(std::begin(prevPressure),std::end(prevPressure));
					}
					else{lowestPressure = 0;}
					if((*sectionit)[1] > lowestPressure&&(*sectionit)[2]==prevAttacking) {
						consecutive += 1;
					}
					else{
						//	std::cout << lowestPressure << std::endl;

						if((*sectionit)[1]>lowestPressure&&largeEnough == true){
							bool interested{false};
							if(interestingTeam != 2){
								if(interestingTeam != prevAttacking){
									interested = true;
								}
							}
							else{interested = true;}
				//			std::cout << "length of sphase"<< consecutive << std::endl;
							if(interested){
								number[1] += 1;
								if(savePhases == true){
									sucFrameId << 	(*(sectionit-consecutive))[0] << "," << consecutive <<std::endl;
									std::vector<std::array<double,5>> tphase;
									for(auto dit = sectionit-consecutive;dit<sectionit+1;++dit){
										tphase.push_back(*dit);
									}
									sPhases.push_back(tphase);
								}
							}
						}
						if((*sectionit)[1]<=lowestPressure && largeEnough == true){
				//			std::cout << "length of uphase" << consecutive << std::endl;
							bool interested{false};
							if(interestingTeam != 2){
								if(interestingTeam != prevAttacking){
									interested = true;
								}
							}
							else{interested = true;}
				//			std::cout << "length of sphase"<< consecutive << std::endl;
							if(interested){
								number[0] += 1;
								if(savePhases == true){
									unsucFrameId << 	(*(sectionit-consecutive))[0] << "," << consecutive << std::endl;
									std::vector<std::array<double,5>> tphase;
									for(auto dit = sectionit-consecutive;dit<sectionit + 1;++dit){
										tphase.push_back(*dit);
									}
									uPhases.push_back(tphase);
								}
							}
						}
						if(largeEnough == false){
							xcount += consecutive + 1;
						//	std::cout << consecutive << std::endl;
						}
						largeEnough = false;
						consecutive = 0;
					}
					if(consecutive >= timeLength*5&&(*sectionit)[1]>=size){
						largeEnough = true;
					}
					prevPressure.push_back((*sectionit)[1]);
					if(prevPressure.size() ==  lookieLookie){
						prevPressure.erase(prevPressure.begin());
					}
					prevAttacking = (*sectionit)[2];

	//			}
			}
		}
//		sucFrameId.close();
//		unsucFrameId.close();
		return number;
	}
/*	else{
		for (auto sectionsit = sections.begin();sectionsit<sections.end();++sectionsit){
			for(auto sectionit = (*sectionsit).begin();sectionit<(*sectionsit).end(); ++sectionit){
				if((*sectionit)[0] < prevPressure) {
					consecutive += 1;
				}
				else{
					if(largeEnough == true){
						number += 1;
						if(savePhases == true){
							std::vector<std::array<double,2>> tphase;
							for(auto dit = sectionit-1-consecutive;dit<sectionit;++dit){
								tphase.push_back(*sectionit);
							}
							phases.push_back(tphase);
						}
						largeEnough = false;
					}
					consecutive = 0;
				}
				if(consecutive >= timeLength*5&&(*sectionit)[0]<size){
					largeEnough = true;
				}
				prevPressure = (*sectionit)[0];
			}
		}
		return number;
	}*/
}
void PressureProcessor::addRates(){
	for(auto phaseit = sPhases.begin();phaseit<sPhases.end();++phaseit){
		std::vector<std::array<double,5>>::iterator start = (*phaseit).begin();
		std::vector<std::array<double,5>>::iterator mid = (*phaseit).begin() + 1;
		std::vector<std::array<double,5>>::iterator end = (*phaseit).begin() + 2;
		(*phaseit)[0][0] = 0;
		(*phaseit)[(*phaseit).size() - 1][0] = 0;
		while(end!=(*phaseit).end()){
			(*mid)[0] = (*end)[1] - (*start)[1];
			std::advance(start,1);
			std::advance(mid,1);
			std::advance(end,1);
		}
	}
}
void PressureProcessor::calcPressure(bool write, bool success, bool usuccess, bool esuccess, bool splitByDef, int fromStart, int length){
	if(!write){
		goalDistributions.clear();
		goalDistributions.resize(bins.size());
	}
	double uframes{0}, sframes{0}, eframes{0};
	if(usuccess){
		for (auto phaseit = uPhases.begin();phaseit<uPhases.end();++phaseit){
			std::array<double,2> summedPressure;
			for(int i = 0;i<2;i++){
				summedPressure[i] = 0;
			}
			int count = (*phaseit).size();
			//get minimum number of defenders over threshold velocity in final two seconds
			std::vector<std::array<double,5>>::iterator startit;
			if((*phaseit).end()-(*phaseit).begin()<12){
			       startit = (*phaseit).begin();
			}
	 		else{
				startit = std::prev(((*phaseit).end()),10);
			}
			std::vector<double> defNums;
			for (auto pointit = startit;pointit<(*phaseit).end();++pointit){
				defNums.push_back((*pointit)[4]);
			}
			double lowestDefNum = *std::min_element(defNums.begin(),defNums.end());
			int lowestDefInt = lowestDefNum;
			
			int z = 0;
			bool stopcon = true;
			while(stopcon){
				z = z + 2;
				if(z>10){
					stopcon = false;
				}
				else{
					if (lowestDefInt < z){
						stopcon = false;
						lowestDefInt = z;
					}
				}
			}
			std::vector<std::array<double,5>>::iterator fit = (*phaseit).begin();
		//	std::cout << (*phaseit).size() << std::endl;
			double changeInGoalDistance{0};
			int acount{0};
		//	if((*phaseit).size() < fromStart + length +1){
		//		std::cout << "nooooo" << std::endl;
		//	}
			int check{0};
			bool successPhase{false};
			std::cout << "YYYY" << std::endl;
			for (auto pointit = (*phaseit).begin();pointit<(*phaseit).end();++pointit){
				uframes += 1;
				if((*phaseit).end()-pointit == 1){
					if((*pointit)[3] = -1){
						successPhase = true;
					}
					changeInGoalDistance += (*pointit)[3];
			//		std::cout << (*pointit)[3];
					check++;
				}
				if((*phaseit).end()-pointit-1 == fromStart){
					changeInGoalDistance -= (*pointit)[3];
			//		std::cout << "," << (*pointit)[3] << std::endl;
					check++;
				}
				summedPressure[0] += pow(frameWeighting+count,-1)*(*pointit)[1];
				count --;
				acount ++;
			}
			if(check!=2){
				std::cout << "Error: did not locate start and final d to g" << std::endl;
			}
			if(splitByDef){
				os[lowestDefInt] << summedPressure[0] << "," << changeInGoalDistance << ",0" << std::endl;	
				std::array<double,2> info = {summedPressure[0], changeInGoalDistance};
				getInTheBin(info,lowestDefInt);
			}
			else{
				if(write){
					for(int i = 0;i<1;i++){
						os[i] << changeInGoalDistance << ",0" << std::endl;
					}
				}
				else{
				//	if(!successPhase){
						std::array<double,2> info = {summedPressure[0], changeInGoalDistance};
					//	std::cout << "YYYY" << std::endl;
						getInTheBin(info);
				//	}
				//	else{
				//		std::cout << "Extra Success" <<std::endl;
				//	}
				}
			}
		}
	}
	if(success){
		for (auto phaseit = sPhases.begin();phaseit<sPhases.end();++phaseit){
			std::array<double,2> summedPressure;
			for(int i = 0;i<2;i++){
				summedPressure[i] = 0;
			}
			double changeInGoalDistance{0};
			int acount{0};
			int count = (*phaseit).size();
		//	if((*phaseit).size() < fromStart + length +1){
		//		std::cout << "nooooo" << std::endl;
		//	}
			int check{0};
			for (auto pointit = (*phaseit).begin();pointit<(*phaseit).end();++pointit){
				sframes +=1;
				if((*phaseit).end()-pointit == 1){
					changeInGoalDistance += (*pointit)[3];
			//		std::cout << (*pointit)[3];
					check++;
				}
				if((*phaseit).end()-pointit-1 == fromStart){
				//	changeInGoalDistance -= (*pointit)[3];
			//		std::cout << "," << (*pointit)[3] << std::endl;
					check++;
				}
				summedPressure[0] += pow(frameWeighting+count,-1)*(*pointit)[1];
				count --;
				acount ++;
			}
			if(check!=2){
				std::cout << "Error: did not locate start and final d to g" << std::endl;
			}
			if(write){
				for(int i = 0;i<1;i++){
					os[i] << changeInGoalDistance << ",1" << std::endl;
				}
			}
			else{
				std::array<double,2> info = {summedPressure[0], (*phaseit)[(*phaseit).size()-1][3]};
				getInTheBin(info);
			}
		}
	}
	if(esuccess){
		for (auto phaseit = ePhases.begin();phaseit<ePhases.end();++phaseit){
			sizes.push_back((*phaseit).size());
			for(auto pointit = (*phaseit).begin(); pointit < (*phaseit).end(); ++pointit){
				eframes += 1;
			}
		}
	}
	std::cout << "Phase switch ratio: " << (sframes+eframes)/(sframes+eframes+uframes) << std::endl;
	/*TEST
	std::cout << "length of succ was " << sframes << " ,unsuc " << uframes << " esuc " << eframes << std::endl;
	int g{0},h{0};
	for(auto i : sizesb){
		std::cout << i << ",";
		h+=i;
	}
	std::cout << std::endl;
	for(auto i : sizes){
		std::cout << i << ",";
		g+=i;
	}
	std::cout << std::endl;
	std::cout << g << "," << h << std::endl;
	*/
	
}
void PressureProcessor::openStreams(){
	for (int i = 0;i<12;i++){
		os[i].open(filePath + "_" + std::to_string(i) + ".txt");
	}
}
void PressureProcessor::closeStreams(){
	for (int i = 0;i<12;i++){
		os[i].close();
	}
}
void PressureProcessor::clearPhases(){
	sPhases.clear();
	uPhases.clear();
	ePhases.clear();
	sections.clear();
	section.clear();	
}

void PressureProcessor::getInTheBin(std::array<double,2> rubbish,int defs){
	if(goalDistributionss[defs].size()!=binss[defs].size()){
		goalDistributionss[defs].resize(binss[defs].size());
	}
	std::vector<std::vector<double>>::iterator distrit = goalDistributionss[defs].begin();
	bool stop{true};
	int count = 0;
	if(binss[defs][1]!=99999999){
		for (auto binit = binss[defs].begin();binit<binss[defs].end();++binit){
			if (rubbish[0]<(*binit)){
				(*distrit).push_back(rubbish[1]);
				return;
			}
			else{++distrit;}
		}
		std::cout << "value higher than top bin" << std::endl;
		return;
	}
	else{
		distrSorters[defs].push_back(rubbish);}
	}
void PressureProcessor::getInTheBin(std::array<double,2> rubbish){
	if(goalDistributions.size()!=bins.size()){
		goalDistributions.resize(bins.size());
	}
	std::vector<std::vector<double>>::iterator distrit = goalDistributions.begin();
	bool stop{true};
	int count = 0;
	if(bins[1]!=99999999){
		for (auto binit = bins.begin();binit<bins.end();++binit){
			if (rubbish[0]<(*binit)){
				(*distrit).push_back(rubbish[1]);
				return;
			}
			else{++distrit;}
		}
		std::cout << "value higher than top bin" << std::endl;
		return;
	}
	else{distrSorter.push_back(rubbish);}
}

void PressureProcessor::setBins(bool nd){
	for (auto it=binss.begin();it<binss.end();++it){
		*it = {0,99999999};
	}
//	binss[0] = {0,99999999,3,3,3,3};
}
void PressureProcessor::setBins(){
	bins = {0,99999999};
	return;
}

std::vector<std::array<double,6>> PressureProcessor::getStats(int defs, std::ofstream& off){
	std::vector<double>::iterator binit = binss[defs].begin();
	std::vector<std::array<double,6>> info;
	for (auto distrit = goalDistributionss[defs].begin();distrit<goalDistributionss[defs].end();++distrit){
		std::cout <<"P <  " << (*binit) << ": ";
		off << "For pressure less than " << (*binit) << ": ";
		RunningStats statcalc;
		for (auto rubbishit = (*distrit).begin(); rubbishit<(*distrit).end();++rubbishit){
			statcalc.Push((*rubbishit));
		}		
		std::cout << "me: "<< statcalc.Mean() << ",";
	
		std::cout << "std: " << statcalc.StandardDeviation() << ",";
		std::cout << "kurt: "<< statcalc.Kurtosis() << "," ;
		std::cout << "sk: "<< statcalc.Skewness() << ",";
		off << "Mean-" << statcalc.Mean() << ", Skew-" << statcalc.Skewness() << ", STD-" << statcalc.StandardDeviation() << std::endl;
		std::array<double, 6> tempinfo = {(*binit),(*distrit).size(),statcalc.Mean(),statcalc.StandardDeviation(),statcalc.Kurtosis(),statcalc.Skewness()};
		++binit;
		info.push_back(tempinfo);
	}
	return info;
}
std::vector<std::array<double,6>> PressureProcessor::getStats(std::ofstream& off){
	std::vector<double>::iterator binit = bins.begin();
	std::vector<std::array<double,6>> info;
	for (auto distrit = goalDistributions.begin();distrit<goalDistributions.end();++distrit){
		off << "For pressure less than " << (*binit) << ": ";
		RunningStats statcalc;
		for (auto rubbishit = (*distrit).begin(); rubbishit<(*distrit).end();++rubbishit){
			statcalc.Push((*rubbishit));
		//	std::cout << (*rubbishit) << std::endl;
		}		
		std::cout << "me: "<< statcalc.Mean() << ",";
		std::cout << "std: " << statcalc.StandardDeviation() << ",";
		std::cout << "kurt: "<< statcalc.Kurtosis() << "," ;
		std::cout << "sk: "<< statcalc.Skewness() << ",";
		off << "Mean-" << statcalc.Mean() << ", Skew-" << statcalc.Skewness() << ", STD-" << statcalc.StandardDeviation() << std::endl;
		std::array<double, 6> tempinfo = {(*binit),(*distrit).size(),statcalc.Mean(),statcalc.StandardDeviation(),statcalc.Kurtosis(),statcalc.Skewness()};
		++binit;
		info.push_back(tempinfo);
	}
	return info;
}

void PressureProcessor::printBinSize(int def){
	std::vector<double>::iterator binit = binss[def].begin();
	for (auto it = goalDistributionss[def].begin(); it<goalDistributionss[def].end(); ++it){
		std::cout << "<" << (*binit) << ": " << (*it).size() << std::endl;
		++binit;
	}
	for(auto it = binss[def].begin();it<binss[def].end();++it){
		std::cout << (*it) << std::endl;
	}
}
void PressureProcessor::printBinSize(){
	std::vector<double>::iterator binit = bins.begin();
	for (auto it = goalDistributions.begin(); it<goalDistributions.end(); ++it){
		std::cout << "<" << (*binit) << ": " << (*it).size() << std::endl;
		++binit;
	}
}
bool PressureProcessor::autoBins(int n,int def){
	binss[def].clear();
	goalDistributionss[def].clear();
	if(distrSorters[def].size()==0){
		std::cout << "Empty Bin" << std::endl;
		return false;
	}
	std::sort(distrSorters[def].begin(),distrSorters[def].end(),[] (const std::array<double,2> &a, const std::array<double,2> &b){
		return a[0] < b[0];
	});
	int binLength = distrSorters[def].size()/(1+n);
	std::cout <<"Bin Size: " << binLength << std::endl;
	for(int i = 1; i < n;i++){
		std::vector<double> binedVec;
		binss[def].push_back((*(distrSorters[def].begin()+i*binLength))[0]);
		for(auto it = distrSorters[def].begin() + (i-1)*binLength;it<distrSorters[def].begin()+i*binLength;++it){
			binedVec.push_back((*it)[1]);
		}
		goalDistributionss[def].push_back(binedVec);
	}
	return true;
}
void PressureProcessor::autoBins(int n){
	bins.clear();
	goalDistributions.clear();
	std::sort(distrSorter.begin(),distrSorter.end(),[] (const std::array<double,2> &a, const std::array<double,2> &b){
		return a[0] < b[0];
	});
		std::cout << "Empty Bin" << std::endl;
	int binLength = distrSorter.size()/(1+n);
	for(int i = 1; i < n;i++){
		std::vector<double> binedVec;
		bins.push_back((*(distrSorter.begin()+i*binLength))[0]);
		for(auto it = distrSorter.begin() + (i-1)*binLength;it<distrSorter.begin()+i*binLength;++it){
			binedVec.push_back((*it)[1]);
		}
		goalDistributions.push_back(binedVec);
	}
		std::cout << "Empty Bin" << std::endl;
}	
void PressureProcessor::printBins(){
	int i = 0;
	std::vector<double>::iterator binit = bins.begin();
	for (auto distrit = goalDistributions.begin();distrit<goalDistributions.end();++distrit){
		os[i] << (*binit) << std::endl;
		for (auto rubbishit = (*distrit).begin(); rubbishit<(*distrit).end();++rubbishit){
			os[i] << *rubbishit << std::endl;
		}
		++binit;
		++i;
	}
}

