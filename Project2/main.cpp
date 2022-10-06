// Project 2: MDNA
// Date: 2019-07-14
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
#include <cmath> 
#include <algorithm>

using namespace std;

// ConfigInfo describing the configuration of the analysis
struct ConfigInfo {
	string firstSongFile; // The first line will specify the name of the first song file
	string secondSongFile; // The second line will specify the name of the second song file
	int matchScore; // The third line will specify the score for a match
	int mismatchScore; // The fourth line will specify the score for a mismatch
	int gapScore; // The fifth line will specify the score for a gap
	int threshold; // The sixth line will specify the allowed similarity threshold for copyright infringement
};

// Information for each song
struct SongInfo {
	string name;
	string idNumber;
	string originalMusicalNote;
};

// Load configuration data from file
void loadConfigurationData(ConfigInfo& info, const string& filePath);

// Load song data from file
void loadSongData(SongInfo& info, const string& filePath);

int analyzePercentageSimilarity(const ConfigInfo& configInfo, const SongInfo& songInfo1, const SongInfo& songInfo2);

int calculatePercentageSimilarity(const string& s1, const string& s2);

// Split a string line by flag
void split(const string& s, vector<string>& sv, const char flag = ' ');

string parseConfigurationRow(const string& line);

int main(int argc, char* argv[]) {
	// freopen("out.txt", "w", stdout);

	// Load data
	string configurationFile(argv[1]);

	ConfigInfo configInfo;
	loadConfigurationData(configInfo, configurationFile);

	SongInfo songInfo1;
	SongInfo songInfo2;
	loadSongData(songInfo1, configInfo.firstSongFile);
	loadSongData(songInfo2, configInfo.secondSongFile);

	// Analyze data
	int similarity = analyzePercentageSimilarity(configInfo, songInfo1, songInfo2);

	// The similarity percentage between the two songs, the allowed percentage, and whether copyright
	// infringement has occurred.If the similarity percentage is greater than or equal to the allowed
	// percentage then copyright infringement has occurred, otherwise, copyright infringement has not
	// occurred
	if (similarity >= configInfo.threshold) {
		cout << "The similarity between the two songs was " << similarity <<
			"% and is not below the similarity threshold of " << configInfo.threshold <<
			"%. Thus, copyright infringement has occurred." << endl;
	}
	else {
		cout << "The similarity between the two songs was " << similarity <<
			"% and is below the similarity threshold of " << configInfo.threshold 
			<< "%. Thus, no copyright infringement has occurred." << endl;
	}

	return 0;
}

int calculatePercentageSimilarity(const string& s1, const string& s2) {
	size_t size = s1.size();
	int diff = 0;

	// Calculate Hamming Distance
	for (size_t i = 0; i < size; i++) {
		if (s1[i] == s2[i]) {
			++diff;
		}
	}

	double rate = (double)diff / size;

	// Round the percentage similarity to the nearest whole percent, i.e. 86.5% becomes
	// 87 % and 86.4% becomes 86 %
	int result = (int)round(rate * 100);
	return result;
}

int analyzePercentageSimilarity(const ConfigInfo& configInfo, const SongInfo& songInfo1, const SongInfo& songInfo2) {
	// Generate matrix
	vector< vector<int> > matrix;

	int rows = songInfo1.originalMusicalNote.size();
	int cols = songInfo2.originalMusicalNote.size();
	for (int i = 0; i <= rows; i++) {
		vector<int> cVec;
		for (int j = 0; j <= cols; j++) {
			cVec.push_back(0);
		}
		matrix.push_back(cVec);
	}

	// First your system must fill in the alignment matrix
	// All slots along the top row should be set to the gap penalty times their index, i.e. 0*gap, 1*gap, 2*gap…
	for (int j = 0; j <= cols; j++) {
		matrix[0][j] = j * configInfo.gapScore;
	}

	// All slots along the left column should be set to the gap penalty times their index, i.e. 0*gap, 1*gap, 2*gap…
	for (int i = 0; i <= rows; i++) {
		matrix[i][0] = i * configInfo.gapScore;
	}

	// Starting at the top left most empty slot and working left to right, top to bottom
	for (int i = 1; i <= rows; i++) {
		char c1 = songInfo1.originalMusicalNote[i - 1];
		for (int j = 1; j <= cols; j++) {
			char c2 = songInfo2.originalMusicalNote[j - 1];

			// If the music notes for each song corresponding to the slot’s position are the same, 
			// then the slot’s value should be the diagonal up and left slot’s value plus the match score
			if (c1 == c2) {
				matrix[i][j] = matrix[i - 1][j - 1] + configInfo.matchScore;
			}
			else {
				// If the music notes for each song corresponding to the slot’s position are not the same, 
				// then the slot’s value should be the max of:
				// o The up slot’s value plus the gap penalty
				int value1 = matrix[i - 1][j] + configInfo.gapScore;
				// o The left slot’s value plus the gap penalty
				int value2 = matrix[i][j - 1] + configInfo.gapScore;
				// o The diagonal up and left slot’s value plus the mismatch penalty
				int value3 = matrix[i - 1][j - 1] + configInfo.mismatchScore;
				matrix[i][j] = max(max(value1, value2), value3);
			}
		}
	}
	// Output
	// The complete alignment matrix with appropriate top and left headers
	cout << "Alignment Matrix" << endl;
	// Header
	cout << "       -";
	for (int j = 1; j <= cols; j++) {
		cout << "    " << songInfo2.originalMusicalNote[j - 1];
	}
	cout << " " << endl;

	for (int i = 0; i <= rows; i++) {
		cout << "  ";
		if (i == 0) {
			cout << "-";
		}
		else {
			cout << songInfo1.originalMusicalNote[i - 1];
		}
		for (int j = 0; j <= cols; j++) {
			printf("%5d", matrix[i][j]);			
		}
		cout << " " << endl;
	}
	cout << endl;

	// Next your system should trace back through the matrix to align the songs correctly
	string s1("");
	string s2("");
	int i = rows;
	int j = cols;

	// Start at the bottom right slot in the matrix and work towards the top left slot
	while (i > 0 && j > 0) {
		char c1 = songInfo1.originalMusicalNote[i - 1];
		char c2 = songInfo2.originalMusicalNote[j - 1];

		// If the music notes corresponding to the current slot are the same then move diagonally up 
		// and to the left in the matrix and add both notes to their respective alignments
		if (c1 == c2) {
			s1 += c1;
			s2 += c2;
			--i;
			--j;
		}
		else {
			int currentSlotNumber = matrix[i][j];
			// If the music notes corresponding to the current slot are not the same then you must determine 
			// whether a gap or mismatch has occurred. Your system should check diagonal, up, and left 
			// (in the order) and select the first direction who’s requirements have been fulfilled
			if (matrix[i - 1][j - 1] + configInfo.mismatchScore == currentSlotNumber) {
				// First, if the diagonal up and to the left number minus the mismatch penalty is equal to the 
				// current slot’s number, then move diagonally up and to the left in the matrix and 
				// add both notes to their respective alignments
				s1 += c1;
				s2 += c2;
				--i;
				--j;
			}
			else if (matrix[i - 1][j] + configInfo.gapScore == currentSlotNumber) {
				// Second, if the up number minus the gap penalty is equal to the current slot’s number, 
				// then move up in the matrix, add the note to the left song’s alignment, 
				// and add a “ - “ to the top song’s alignment
				s1 += c1;
				s2 += "-";
				--i;
			}
			else if (matrix[i][j - 1] + configInfo.gapScore == currentSlotNumber) {
				// Finally, if the left number minus the gap penalty is equal to the current slot’s number, 
				// then move left in the matrix, add the note to the top song’s alignment, 
				// and add a “ - “ to the left song’s alignment
				s1 += "-";
				s2 += c2;
				--j;
			}
		}
	}

	// If you ever end up along the left or top side of the matrix, add “-“ to the alignment of the song
	// for which no more notes exist, and add the notes of the other song to the other alignment
	while (i > 0) {
		char c1 = songInfo1.originalMusicalNote[i - 1];
		s1 += c1;
		s2 += "-";
		--i;
	}
	while (j > 0) {
		char c2 = songInfo2.originalMusicalNote[j - 1];
		s1 += "-";
		s2 += c2;
		--j;
	}

	reverse(s1.begin(), s1.end());
	reverse(s2.begin(), s2.end());

	// Output
	// The ID, name, and alignment of each song.
	printf("%-5s%-25s:%s\n", songInfo1.idNumber.c_str(), songInfo1.name.c_str(), s1.c_str());
	printf("%-5s%-25s:%s\n", songInfo2.idNumber.c_str(), songInfo2.name.c_str(), s2.c_str());
	cout << endl;

	return calculatePercentageSimilarity(s1, s2);
}

void loadSongData(SongInfo& info, const string& filePath) {
	ifstream iFile(filePath.c_str());

	string cLine;
	getline(iFile, cLine);
	vector<string> vec;
	split(cLine, vec, '|');
	info.name = vec[0].substr(1);
	info.idNumber = vec[1];

	getline(iFile, info.originalMusicalNote);

	iFile.close();
}

string parseConfigurationRow(const string& line) {
	vector<string> vec;
	split(line, vec, '=');

	return vec[1];
}

void loadConfigurationData(ConfigInfo& info, const string& filePath) {
	ifstream iFile(filePath.c_str());

	string cLine;
	getline(iFile, cLine);
	info.firstSongFile = parseConfigurationRow(cLine);

	getline(iFile, cLine);
	info.secondSongFile = parseConfigurationRow(cLine);

	getline(iFile, cLine);
	info.matchScore = atoi(parseConfigurationRow(cLine).c_str());

	getline(iFile, cLine);
	info.mismatchScore = atoi(parseConfigurationRow(cLine).c_str());

	getline(iFile, cLine);
	info.gapScore = atoi(parseConfigurationRow(cLine).c_str());

	getline(iFile, cLine);
	info.threshold = atoi(parseConfigurationRow(cLine).c_str());

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
