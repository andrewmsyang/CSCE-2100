// Project 1: Controlled Burn
// Date: 2019-06-23
// Author: Andrew Yang ay0123 11229926

// Including header files
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

// Character marks for burn status of a tree
const char CHAR_T = 'T'; //is a tree that is not on fire
const char CHAR_F = 'F'; //is a tree that is on fire
const char CHAR_B = 'B'; //is a tree that has already burnt down
const char CHAR_EMPTY = ' '; //is an empty patch of land

// Character marks for forecast status of a day
const char CHAR_N = 'N'; //indicates the wind is blowing to the North
const char CHAR_S = 'S'; //indicates the wind is blowing to the South
const char CHAR_W = 'W'; //indicates the wind is blowing to the West
const char CHAR_E = 'E'; //indicates the wind is blowing to the East
const char CHAR_R = 'R'; //indicates that it is raining and there is no wind blowing

struct ForestInfo {
	int burnDay; // How many days a tree burns for
	vector< vector<char> > forestVec; // The forest can be any sized rectangle
	vector< vector<int> > burnOutVec;
};

struct ForecastInfo {
	int day;
	char flag;
};

struct WeatherInfo {
	bool lasting;
	char flag;
};

// Load forest data from file
void loadForestData(ForestInfo& info, const string& filePath);

// Load forecast data from file
void loadForecastData(vector<ForecastInfo>& info, const string& filePath);

// Get weather data on one day
WeatherInfo getWeatherInfo(const vector<ForecastInfo>& info, int day);

// Show current status of forest and forecast
void showInfo(int day, WeatherInfo weather, const vector< vector<char> >& forestVec);

// Whether the forest is still on fire on not
bool isOnFire(const vector< vector<char> >& forestVec);

// Update the count of burn day for a tree on its position
void updateBurnStatus(const vector< vector<char> >& tmpVec, ForestInfo& info);

// Simulate on a day
void simulate(ForestInfo& info, const WeatherInfo& weather);

// Fire a tree on position(i, j) if it is a tree
void fire(ForestInfo& info, int i, int j);

// Split a string line by flag
void split(const string& s, vector<string>& sv, const char flag = ' ');

int main(int argc, char* argv[]) {
	// freopen("out.txt", "w", stdout);

	string forestInputFile(argv[1]);
	string forecastInputFile(argv[2]);

	/*cout << "Input the file describing the initial condition of the forest: ";
	cin >> forestInputFile;
	cout << "\n";
	cout << "Input the file describing the weather forecast: ";
	cin >> forecastInputFile;
	cout << "\n";

	forestInputFile = "forest1.txt";
	forecastInputFile = "forecast1.txt";*/

	ForestInfo cForestInfo;
	loadForestData(cForestInfo, forestInputFile);

	vector<ForecastInfo> forcaseInfoVec;
	loadForecastData(forcaseInfoVec, forecastInputFile);

	int cDay = 0;
	WeatherInfo cWeather = getWeatherInfo(forcaseInfoVec, cDay);

	// The initial condition of the forest, as specified in the file, 
	// represents Day 0 of the fire, and should be output in a grid format
	showInfo(cDay, cWeather, cForestInfo.forestVec);

	while (true) {
		// During every day in which there is at least one tree still on fire
		if (isOnFire(cForestInfo.forestVec)) {
			cDay += 1;
			// The weather should first be checked for that day, 
			// if it changed the user should be notified of the change
			cWeather = getWeatherInfo(forcaseInfoVec, cDay);
			simulate(cForestInfo, cWeather);

			// Finally, the current condition of the forest should be output in a grid format, 
			// along with its day number.
			showInfo(cDay, cWeather, cForestInfo.forestVec);
		}
		else {
			break;
		}
	}

	cout << "The forest fire took " << cDay << " days to burn out." << endl;
	cout << "Remember-Only YOU Can Prevent Forest Fires!" << endl;
	cout << "-Smokey Bear" << endl;

	// system("pause");
	return 0;
}

void fire(ForestInfo& info, int i, int j) {
	int rows = info.forestVec.size();
	int cols = info.forestVec[0].size();

	if (i >= 0 && i < rows && j >= 0 && j < cols){
		if (info.forestVec[i][j] == CHAR_T){
			info.forestVec[i][j] = CHAR_F;
		}
	}
}

void simulate(ForestInfo& info, const WeatherInfo& weather) {
	size_t rows = info.forestVec.size();
	size_t cols = info.forestVec[0].size();

	vector< vector<char> > tmpVec;
	for (size_t i = 0; i < rows; ++i) {
		vector<char> charVec;
		for (size_t j = 0; j < cols; ++j) {
			charVec.push_back(info.forestVec[i][j]);
		}
		tmpVec.push_back(charVec);
	}

	// If it is not raining, then the fire should spread according to the direction of the wind.
	switch (weather.flag) {
	case CHAR_N: {
		for (size_t i = 0; i < rows; ++i) {
			for (size_t j = 0; j < cols; ++j) {
				// A tree can only set other trees on fire the day after it was set on fire
				if (tmpVec[i][j] == CHAR_F) {
					fire(info, i - 1, j);
					fire(info, i - 1, j - 1);
					fire(info, i - 1, j + 1);
				}
			}
		}
		break;
	}
	case CHAR_S: {
		for (size_t i = 0; i < rows; ++i) {
			for (size_t j = 0; j < cols; ++j) {
				// A tree can only set other trees on fire the day after it was set on fire
				if (tmpVec[i][j] == CHAR_F) {
					fire(info, i + 1, j);
					fire(info, i + 1, j - 1);
					fire(info, i + 1, j + 1);
				}
			}
		}
		break;
	}
	case CHAR_W: {
		for (size_t i = 0; i < rows; ++i) {
			for (size_t j = 0; j < cols; ++j) {
				// A tree can only set other trees on fire the day after it was set on fire
				if (tmpVec[i][j] == CHAR_F) {
					fire(info, i , j - 1);
					fire(info, i - 1, j - 1);
					fire(info, i + 1, j - 1);
				}
			}
		}
		break;
	}
	case CHAR_E: {
		for (size_t i = 0; i < rows; ++i) {
			for (size_t j = 0; j < cols; ++j) {
				// A tree can only set other trees on fire the day after it was set on fire
				if (tmpVec[i][j] == CHAR_F) {
					fire(info, i, j + 1);
					fire(info, i - 1, j + 1);
					fire(info, i + 1, j + 1);
				}
			}
		}
		break;
	}
	default: {
		break;
	}
	}
	// If it is raining, then no wind is blowing and the fire cannot spread that day

	// After a tree has burned for the specified number of days, 
	// it becomes a ¡®B¡¯ representing a burnt down tree. Day 0 does count as a day for a tree burning
	updateBurnStatus(tmpVec, info);
}

void updateBurnStatus(const vector< vector<char> >& tmpVec, ForestInfo& info) {
	for (size_t i = 0; i < info.forestVec.size(); ++i) {
		vector<char> cVec = tmpVec[i];
		for (size_t j = 0; j < cVec.size(); ++j) {
			if (cVec[j] == CHAR_F) {
				info.burnOutVec[i][j] += 1;
				if (info.burnOutVec[i][j] == info.burnDay) {
					info.forestVec[i][j] = CHAR_B;
				}
			}
		}
	}
}

bool isOnFire(const vector< vector<char> >& forestVec) {
	for (size_t i = 0; i < forestVec.size(); ++i) {
		vector<char> cVec = forestVec[i];
		for (size_t j = 0; j < cVec.size(); ++j) {
			if (cVec[j] == CHAR_F) {
				return true;
			}
		}
	}

	return false;
}

void showInfo(int day, WeatherInfo weather, const vector< vector<char> >& forestVec) {
	if (!weather.lasting) {
		if (weather.flag == CHAR_R) {
			cout << "It is now raining" << endl;
		}
		else {
			cout << "The wind is now blowing " << weather.flag << endl;
		}
	}
	cout << "Current day: " << day << endl;

	for (size_t i = 0; i < forestVec.size(); ++i) {
		vector<char> cVec = forestVec[i];
		for (size_t j = 0; j < cVec.size(); ++j) {
			cout << cVec[j] << " ";
		}
		cout << endl;
	}
	cout << endl;
}

WeatherInfo getWeatherInfo(const vector<ForecastInfo>& info, int day) {
	WeatherInfo weatherInfo;
	for (size_t i = 0; i < info.size(); ++i) {
		ForecastInfo cInfo = info[i];
		if (day == cInfo.day) {
			weatherInfo.lasting = false;
			weatherInfo.flag = cInfo.flag;
			return weatherInfo;
		}
		else if (day < cInfo.day) {
			// If the weather is not specified for a day, then assume it is the same as the previous day
			weatherInfo.lasting = true;
			weatherInfo.flag = info[i - 1].flag;
			return weatherInfo;
		}
	}

	weatherInfo.lasting = true;
	weatherInfo.flag = info[info.size() - 1].flag;
	return weatherInfo;
}

void loadForecastData(vector<ForecastInfo>& info, const string& filePath) {
	ifstream iFile(filePath.c_str());

	// Read forecast info
	string line;
	while (getline(iFile, line)) {
		vector<string> vec;
		split(line, vec, ' ');
		string secStr = vec[1];

		vector<string> secVec;
		split(secStr, secVec, ':');
		ForecastInfo forecastInfo;
		forecastInfo.day = atoi(secVec[0].c_str());
		forecastInfo.flag = secVec[1][0];

		info.push_back(forecastInfo);
	}

	iFile.close();
}

void loadForestData(ForestInfo& info, const string& filePath) {
	ifstream iFile(filePath.c_str());

	// Read how many days a tree burns for
	string firstLine;
	getline(iFile, firstLine);
	vector<string> firstVec;
	split(firstLine, firstVec, ':');
	info.burnDay = atoi(firstVec[1].c_str());

	// Read forest map
	string line;
	while (getline(iFile, line)) {
		vector<string> vec;
		split(line, vec, ',');
		vector<char> charVec;
		vector<int> intVec;
		for (size_t i = 0; i < vec.size(); ++i) {
			charVec.push_back(vec[i].at(0));
			intVec.push_back(0);
		}

		info.forestVec.push_back(charVec);
		info.burnOutVec.push_back(intVec);
	}

	iFile.close();
}

void split(const string& s, vector<string>& sv, const char flag) {
	sv.clear();
	istringstream iss(s);
	string temp;

	while (getline(iss, temp, flag)) {
		sv.push_back(temp);
	}
}
