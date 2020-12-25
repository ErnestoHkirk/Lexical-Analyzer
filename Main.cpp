/*
 * PROJECT: Lexical Analyzer
 * 
 * Group Memebers:
 * Jacob Rapmund
 * Ernesto Hooghkirk
 * Andy Duong
 * Michael Lau
 * 
 * Professor: Anthony Le
 * 
 */

#include "FileReader.hpp"
#include "readTable.hpp"

#include <vector>
#include <unordered_map>
#include <iomanip>

//List of possible states for the state table
enum tokenState { LETTER, DIGIT, DOLLAR, APOST, LEFTPAR, RIGHTPAR, LEFTCURL, RIGHTCURL, LEFTSQR, RIGHTSQR, COMMA, COLON, SEMICOLON, SPACE, PERIOD, EXCLA, OPER, BACKUP };

//State matrix to be defined by calling the static function in readTable
int stateTable[ROWS][COLUMNS];

//struct for differentiating lexeme types
struct LexemePair
{
	std::string lexeme;
	std::string identifier;
};

//Creates a map that associates characters to states (key = char, value = tokenState)
std::unordered_map<char, tokenState> createStateMap();
//Creates a map to help differentiate between identifiers and keywords (key = string, value = bool)
std::unordered_map<std::string, bool> createKeySet();
//The tokenize function is a lexical analyzer. [s] is the source code string and [tokens] will be manipulated within the function to produce a vector of lexeme pairs
bool lexer(const std::string& s, std::vector<LexemePair>& tokens);
//Prints a set of lexeme pairs in an easy-to-read format on the console given a vector of [tokens]
void printLexerTable(const std::vector<LexemePair>& tokens);

int main()
{
	std::vector<LexemePair> Tokens;

    //extract and populate values for state matrix
    readTable::createMatrix("states.txt");
    //extract source code from file
	std::string fileData = FileReader::readFile("testfile.txt");

    //Perform lexical analysis
	if (!lexer(fileData, Tokens)) printLexerTable(Tokens);
    else std::cout << "ERROR: Lexer could not analyze input file.";

    //uncomment if console closes abruptly 
    //system("pause");

	return 0;
}

std::unordered_map<char, tokenState> createStateMap()
{
	std::unordered_map<char, tokenState> stateMap;

	stateMap['$']  = DOLLAR;
	stateMap['\''] = APOST;
	stateMap['(']  = LEFTPAR;
	stateMap[')']  = RIGHTPAR;
	stateMap['{']  = LEFTCURL;
	stateMap['}']  = RIGHTCURL;
	stateMap['[']  = LEFTSQR;
	stateMap[']']  = RIGHTSQR;
	stateMap[',']  = COMMA;
	stateMap[':']  = COLON;
	stateMap[';']  = SEMICOLON;
    	stateMap['\t'] = SPACE;
    	stateMap['\n'] = SPACE;
    	stateMap[' ']  = SPACE;
    	stateMap['\r']  = SPACE;
	stateMap['.']  = PERIOD;
	stateMap['!']  = EXCLA;
	stateMap['+']  = OPER;
	stateMap['-']  = OPER;
	stateMap['/']  = OPER;
	stateMap['*']  = OPER;
	stateMap['=']  = OPER;
	stateMap['%']  = OPER;
	stateMap['>']  = OPER;
	stateMap['<']  = OPER;

	return stateMap;
}

std::unordered_map<std::string, bool> createKeySet()
{
    std::unordered_map<std::string, bool> keySet;

    keySet["int"] = true;
    keySet["float"] = true;
    keySet["bool"] = true;
    keySet["true"] = true;
    keySet["false"] = true;
    keySet["if"] = true;
    keySet["else"] = true;
    keySet["then"] = true;
    keySet["endif"] = true;
    keySet["while"] = true;
    keySet["whileend"] = true;
    keySet["do"] = true;
    keySet["doend"] = true;
    keySet["for"] = true;
    keySet["forend"] = true;
    keySet["input"] = true;
    keySet["output"] = true;
    keySet["and"] = true;
    keySet["or"] = true;
    keySet["not"] = true;

    return keySet;
}

void printLexerTable(const std::vector<LexemePair>& tokens)
{
    std::ofstream outfile;
    outfile.open("output.txt");
    outfile << std::left << std::setw(15) << "TOKENS" <<  "|" << std::right << std::setw(15) << "Lexemes" << "\n";
    outfile << std::setfill('=') << std::setw(31) << "" << std::setfill(' ') << std::endl;
    for (auto token : tokens) outfile << std::left << std::setw(15) << token.identifier << "=" << std::right << std::setw(15) << token.lexeme << std::endl;
    outfile.close();

    std::cout << std::left << std::setw(15) << "TOKENS" <<  "|" << std::right << std::setw(15) << "Lexemes" << "\n";
    std::cout << std::setfill('=') << std::setw(31) << "" << std::setfill(' ') << std::endl;
    for (auto token : tokens) std::cout << std::left << std::setw(15) << token.identifier << "=" << std::right << std::setw(15) << token.lexeme << std::endl;
}

bool lexer(const std::string& s, std::vector<LexemePair>& tokens)
{
    tokenState tState = LETTER;
    int state = 0;

    std::unordered_map<char, tokenState> stateMap = createStateMap();
    std::unordered_map<std::string, bool> keySet = createKeySet();

    std::string::const_iterator curr = s.cbegin();
    std::string::const_iterator pplace = s.cbegin();

    while (curr != s.cend())
    {
        //check next character
        if      (std::isalpha(*curr)) tState = LETTER;
        else if (std::isdigit(*curr)) tState = DIGIT;
        else if (stateMap.find(*curr) != stateMap.end()) tState = stateMap[*curr];
        else    return 1; // because the character is not valid

        //update state table
        state = stateTable[state][tState] - 1;
	
	//check for valid state
        if(state == -1) return 1;

        //update character pointer
        curr++;

        //update pplace (useful for extra spaces)
        if (state == 0) pplace = curr;

        //check if accepting state and update
        if (state == 2 || state == 4 || state == 7 || state == 8 || state == 9 || state == 11 || curr == s.cend())
        {
            LexemePair npair;
            
            //backup character pointer if necessary 
            if (stateTable[state][BACKUP]) curr--;

            //eof check
            if(!(state == 2 || state == 4 || state == 7 || state == 8 || state == 9 || state == 11)) state = stateTable[state][SPACE] - 1; 

            //if the ending state was because we had a comment, skip creating a pair
            if (state != 7 && state != 11)
            {
                npair.lexeme = s.substr(pplace - s.cbegin(), curr - pplace);

                if      (state == 2) npair.identifier = (keySet.find(npair.lexeme) != keySet.end() ? "KEYWORD" : "IDENTIFIER");
                else if (state == 4) npair.identifier = "INTEGER";
                else if (state == 8) npair.identifier = (tState == OPER ? "OPERATOR" : "SEPARATOR");
                else if (state == 9) npair.identifier = "REAL";
		
		if(npair.lexeme != "" && npair.identifier != "")
                	tokens.push_back(npair);
            }
            //revert back to the initial state and update pplace
            state = 0;
            pplace = curr;
        }
    }
    return 0;
}
