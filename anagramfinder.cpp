/*******************************************************************************
 * Name        : anagram_finder.cpp
 * Author      : Michael Chunko
 * Date        : November 4, 2019
 * Description : Find the largest anagram class in a given file
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

// Calculates the hash of str using prime numbers
// Calculates the same hash for anagrams regardless of their ordering or case
// Returns -1 if str is non-alphabetical (contains characters except 'a'-'z', 'A'-'Z')
unsigned long int str_hasher(string str) {
	unsigned long int hash_value = 1;
	unsigned long int primes[26] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59,
								61, 67, 71, 73, 79, 83, 89, 97, 101}; // 26 primes
	char temp;

	for(auto it = str.begin(); it < str.end(); it++) {
		temp = tolower(*it);
		if (!(temp >= 'a' && temp <= 'z') && temp != 13) // Check for non-alphabetical (13 is Carriage Return)
			return 0;

		if (temp >= 'a' && temp <= 'z') // Don't know why I need to check again but there's errors if I don't
			hash_value *= primes[temp - 'a'];
	}

	return hash_value;
}

struct Hash_Cell {
	unsigned long int hash;
	vector<string> words;

	Hash_Cell(unsigned long int _hash, string _word) : hash(_hash) {
		words.push_back(_word);
	}

	Hash_Cell() : hash(0) {}

	void add(string word) {
		words.push_back(word);
	}
};

struct Hash_Table {
	unsigned int max;
	vector<unsigned int> max_indices;
	vector<Hash_Cell> cells; // A prime equal to roughly 1.3 * max dictionary size
	unsigned int num_cells = 550163;

	Hash_Table() : max(0) {
		cells.reserve(550163);
	}

	void add(unsigned long int hash_value, string word) {
		unsigned int i = hash_value % num_cells;
		while (!cells[i].words.empty() && cells[i].hash != hash_value) { // Probe until we find the right cell/an empty cell
			i++;
			if (i >= num_cells) // Loop back around
				i = 0;
		}

		if (cells[i].words.empty()) // Empty cell
			cells[i] = Hash_Cell(hash_value, word);
		else                       // Non-empty cell
			cells[i].add(word);

		unsigned int num = cells[i].words.size();
		if (num > max) { // New max
			max = num;
			max_indices.clear();
			max_indices.push_back(i);
		} else if (num == max) // Same as max
			max_indices.push_back(i);
	}
};

Hash_Table anagram_finder(ifstream *dictionary) {
	string line;
	unsigned long int hash_value;
	Hash_Table hash_table = Hash_Table();

	// Iterate through the dictionary
	while (getline(*dictionary, line, '\n')) {
		// Find the hash of the line
		hash_value = str_hasher(line);
		if (hash_value == 0) // Non-alphabetical
			continue;

		hash_table.add(hash_value, line); // Add to the hash table
	}

	return hash_table;
}

void print_anagrams(Hash_Table hash_table) {
	if (hash_table.max <= 1) // No anagrams have been found
		cout << "No anagrams found.\n";
	else {
		cout << "Max anagrams: " << hash_table.max << "\n";

		vector<pair<string, int>> word_index_pairs;

		// Sort the contents of the anagram classes
		for (auto it = hash_table.max_indices.begin(); it < hash_table.max_indices.end(); it++) {
			sort(hash_table.cells[*it].words.begin(), hash_table.cells[*it].words.end());
			word_index_pairs.push_back(make_pair(hash_table.cells[*it].words[0], *it)); // Add them to word_index_pairs
		}

		// Sort the anagram classes relative to each other
		sort(word_index_pairs.begin(), word_index_pairs.end());

		// Print it all out
		for (auto it = word_index_pairs.begin(); it < word_index_pairs.end(); it++) {
			for (auto jt = hash_table.cells[it->second].words.begin(); jt < hash_table.cells[it->second].words.end(); jt++) {
				cout << *jt;
				putchar('\n');
			}
			if (it + 1 != word_index_pairs.end())
				putchar('\n');
		}
	}
}

int main(int argc, char* const argv[]) {
	// Check for correct arguments
	if (argc != 2) {
		cerr << "Usage: ./anagramfinder <dictionary file>\n";
		return 1;
	}

	// Attempt to open the dictionary file
	ifstream dictionary;
	dictionary.open(argv[1]);

	if (!dictionary.is_open()) { // The file does not exist or could not be opened
		cerr << "Error: File '" << argv[1] << "' not found.\n";
		return 1;
	}

	print_anagrams(anagram_finder(&dictionary));

	dictionary.close();

	return 0;
}