#ifndef DATASTRUCTURES_H_
#define DATASTRUCTURES_H_

#include <iostream>
#include <cstdio>
#include "helpers.h"

using namespace std;


bool isCommand(String);
int32_t isOperand(String);
int32_t isNumOrVar(String);
int32_t getNum(String);

String prevToken;
extern bool didItRead;
bool fileDone = false;
map<String,int32_t> variables;													// map to contain variables


struct expVector {																// vector to contain BLIP statements
private:
	int cap;
public:
	int len;
	String* words;

	/* Default constructor */
	expVector() {
		this->len = 0;
		this->cap = 10;
		words = new String[this->cap];
	}

	/* Default destructor */
	~expVector() {
		delete[] words;
	}


	/**
	 * Checks capacity of the vector of strings 'words.' If the vector is full,
	 * use amortized doubling. Otherwise, checkCap() does nothing.
	 *
	 * param: N/A
	 * return: N/A
	 */
	void checkCap() {
		if (this->len == this->cap) {
			this->cap *= 2;
			String* newWords = new String[this->cap];

			for (int32_t index = 0; index < this->len; index += 1) {
				newWords[index] = words[index];
			}

			delete[] this->words;

			this->words = newWords;
		}
	}


	/*
	 * Puts 'String' type 'word' into the vector of Strings. Updates vector's
	 * length. Every putWord() call should have a corresponding checkCap() call.
	 *
	 * param word: String type 'word' that is stored into the vector of Strings.
	 * return: N/A
	 */
	void putWord(String word) {
		this->words[this->len] = word;
		this->len += 1;
	}
};

int32_t computeRec(expVector*);

struct statementTree {
public:
	expVector* expression;

	/* Default constructor */
	statementTree() {
		this->expression = 0;
	}

	/* Default destructor */
	~statementTree() {
		//printf("destructor statementTree\n");
	}


	/**
	 * Evalutes the BLIP statement for 'text,' 'var,' 'set,' and 'output'
	 * commands.
	 *
	 * Assumptions:
	 * Do nothing further than printing out a warning for the following:
	 * 		(1). Uninitialized variable is set.
	 *		(2). Uninitialized/initialized variable is being
	 *			 assigned to an uninitialized variable.
	 *		(3). Re-initializing a variable.
	 *
	 */
	void executeStatement(expVector* blipVec)
	{
		String token1 = String((blipVec->words + 1)->c_str());
		String token2 = String((blipVec->words + 2)->c_str());

		// text command
		if (String("text") == (String(blipVec->words->c_str()))) {
			blipVec->words += 1;													// don't want to output the command itself
			cout << blipVec->words->c_str();
			blipVec->words -= 1;													// in case you're executing do loop, want it to be in proper place again
		}

		// output command
		else if (String("output") == (String(blipVec->words->c_str()))) {

			if ((isOperand(token1) == 1) ||	(isOperand(token1) == 2))           // if upcoming token is a binary or unary operand,
			{
				cout << computeRec(blipVec);								// then recursively compute computation and output result
				blipVec->words -= (blipVec->len - 1);							// to properly delete
			}

			else if (isNumOrVar(token1) == 1) {									// output a simple number
				cout << getNum(token1);
			}

			else if (isNumOrVar(token1) == 2) {
				cout << variables[token1];										// find variable and output it
			}
		}

		// var command
		else if (String("var") == (String(blipVec->words->c_str()))) {

			// check if variable already exists first.
			if (variables.find(token1) != variables.end())						// if the variable already resides in map,
			{
				cout << "variable " << (blipVec->words + 1)->c_str();			// throw warning
				cout << " incorrectly re-initialized" << endl;
			}

			// if it reaches here, variable hasn't been created yet.
			else if (isOperand(token2) != 0) {									// if first token after 'var' is an operand,
				variables[token1] = computeRec(blipVec);					// make variable and assign a computed value to it
			}

			else if (isNumOrVar(token2) == 1) {									// if first token after 'var' is a number,
				variables[token1] =	getNum(token2);								// make variable and assign the given number to it
			}

			else if (isNumOrVar(token2) == 2) {									// if first token after 'var' is another variable,
				if (variables.find(token2) == variables.end()) {				// check if assigning variable exists
					cout << "variable " << (blipVec->words + 2)->c_str();
					cout << " not declared" << endl;
				}
				else {
					variables[token1] =	variables[token2];						// assigning variable exists
				}
			}
		}

		// set command
		else if (String("set") == (String(blipVec->words->c_str()))) {

			if (variables.find(token1) == variables.end())						// if the variable doesn't exist yet,
			{
				cout << "variable " << (blipVec->words + 1)->c_str();			// throw warning
				cout << " not declared" << endl;
			}

			else if ((isOperand(token2) == 1) || (isOperand(token2) == 2))      // if upcoming token is a binary or unary operand,
			{
				blipVec->words += 1;
				variables[token1] = computeRec(blipVec);					// recursively compute the value to set variable to
				blipVec->words -= (blipVec->len - 1);							// for do loops: set pointer back to start so it'll eval correctly
			}

			else if (isNumOrVar(token2) == 1) {									// variable is set to a number only
				variables[token1] = getNum(token2);
			}

			else if (isNumOrVar(token2) == 2) {									// variable is set to a variable only
				if (variables.find(token2) == variables.end()) {				// check if assigning variable exists
					cout << "variable " << (blipVec->words + 2)->c_str();
					cout << " not declared" << endl;
				}
				else {															// assigning variable exists
					variables[token1] = variables[token2];
				}
			}
		}

		else {
			cout << "Invalid input. Exiting program.";
			exit(1);
		}
	}

	/**
	 * Fills 'blipVec' with String type tokens for a BLIP statement. For
	 * example, 'var x 1' is a 'blipVec' of three Strings var, x, and 1.
	 * If function finds the end of file, it sets the global bool 'fileDone' to
	 * true and returns. If function finds a symbol, it skips it and returns
	 * because you can't have comments between a BLIP statement.
	 *
	 * param blipVec: pointer to a pointer of type expVector 'blipVec.' Holds
	 * BLIP statement.
	 * return: N/A
	 */
	void fillStatementTree(expVector** blipVec)
	{
		(*blipVec)->putWord(String(next_token()));
		(*blipVec)->checkCap();

		if (isCommand(String(peek_next_token()))) {
			return;
		}

		read_next_token();

		if (String(next_token()) == String("//")) {
			skip_line();
			return;
		}

		if (next_token_type == END)
		{
			fileDone = true;
			return;
		}

		fillStatementTree(blipVec);
	}

};


/**
 * Holds conditionals for do, if, and else commands.
 */
struct conditionalTree {
	expVector* conditionals;
	int32_t numCond;

	/* Default constructor */
	conditionalTree() {
		this->conditionals = 0;
		this->numCond = 0;
	}

	/* Default destructor */
	~conditionalTree() {
		//cout << "destructor conditionalTree" << endl;
	}

	void fillConditionalTree(expVector** condVec)
	{
		(*condVec)->putWord(String(next_token()));
		(*condVec)->checkCap();

		read_next_token();

		if (isCommand(String(next_token()))) {
			this->numCond += 1;
			return;
		}

		if (String(next_token()) == String("//")) {
			skip_line();
			return;
		}

		if (next_token_type == END)
		{
			fileDone = true;
			return;
		}

		fillConditionalTree(condVec);
	}
};


// Holds a block of statementTree expressions. Useful for BLIP do loops.
struct blockVector {
private:
	int len;
	int cap;
public:
	statementTree* doBlock;

	int blockLen() {
		return this->len;
	}

	/* Default constructor */
	blockVector() {
		this->len = 0;
		this->cap = 10;
		doBlock = new statementTree[this->cap];
	}

	/* Default destructor */
	~blockVector() {
		cout << "destructor blockVector" << endl;
		delete[] doBlock;
	}

	void checkCap() {
		if (this->len == this->cap) {
			this->cap *= 2;
			statementTree* newDoBlock = new statementTree[this->cap];

			for (int32_t index = 0; index < this->len; index += 1) {
				newDoBlock[index] = doBlock[index];
			}

			delete[] this->doBlock;
			this->doBlock = newDoBlock;
		}
	}

	// There should be a 1:1 ratio of putStatement() calls and checkCap() calls.
	void putStatement(statementTree tree) {
		this->doBlock[this->len] = tree;
		this->len += 1;
	}

	void fillBlock(String sentinel) {
		while (String(next_token()) != sentinel) {							// until you hit the end of the body,

			if (String(next_token()) == String("else") &&
			   (sentinel != String("od")))
			{																	// when not lone if statement and filling do block, break
				read_next_token();
				read_next_token();
				break;
			}

			statementTree* bodyStatement = new statementTree;
			expVector* tokens = new expVector;
			bodyStatement->fillStatementTree(&tokens);							// tokens are a single statement within do loop body
			bodyStatement->expression = tokens;

			this->putStatement(*bodyStatement);									// put the statement into the doBlock vector
			this->checkCap();
		}
	}

	void evalIfElseInDo(expVector* block) {
		// get conditional statement
		expVector condition;
		int index = 0;
		while (isCommand(*(block->words)) != 0) {
			condition.words[index] = block->words[index];
			index += 1;
		}

		// get if block


	}


	void evalDoLoop(conditionalTree* condition) {
		while (computeRec(condition->conditionals)) {							// conditionals are always boolean, so evaluating will always produce 0 or 1
			(condition->conditionals->words) -=									// checking condition doesn't reset pointer to start of condition
			(condition->conditionals->len - 1);									// reset pointer to condition to the start of the condition to properly check condition again

			for (int index = 0; index < this->len; index ++) {
				statementTree currentStatement = this->doBlock[index];
				if (*(currentStatement.expression->words) == String("if")) {
					evalIfElseInDo(currentStatement.expression);
				}
				else {
					this->doBlock->executeStatement(currentStatement.expression);
				}
			}
		}
	}
};


#endif /* DATASTRUCTURES_H_ */
