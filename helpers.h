/*
 * helpers.h
 *
 *  Created on: Aug 8, 2016
 *      Author: User
 */

#ifndef HELPERS_H_
#define HELPERS_H_

#include <iostream>
#include <cstdio>
#include "dataStructures.h"


/**
* Computes arg1 and/or arg2 with operand 'opType.'
* If unary operator, arg2 is 0.
*
* param arg1: first argument
* param arg2: second argument
* param opType: string holding operand for arguments
* return: result of computation
*/
int32_t computation(int32_t arg1, int32_t arg2, String opType)
{
	// Binary math ops
	if (opType == String("+")) {
		return (arg1 + arg2);
	}
	else if (opType == String("-")) {
		return (arg1 - arg2);
	}
	else if (opType == String("*")) {
		return (arg1 * arg2);
	}
	else if (opType == String("/")) {
		return (arg1 / arg2);
	}
	else if (opType == String("%")) {
		return (arg1 % arg2);
	}

	// Binary logic ops
	else if (opType == String("&&")) {
		return (arg1 && arg2);
	}
	else if (opType == String("||")) {
		return (arg1 || arg2);
	}

	// Binary comparison ops (returns 1 for true, 0 for false)
	else if (opType == String(">")) {
		return (arg1 > arg2);
	}
	else if (opType == String("<")) {
		return (arg1 < arg2);
	}
	else if (opType == String("==")) {
		return (arg1 == arg2);
	}
	else if (opType == String("!=")) {
		return (arg1 != arg2);
	}
	else if (opType == String(">=")) {
		return (arg1 >= arg2);
	}
	else if (opType == String("<=")) {
		return (arg1 <= arg2);
	}

	// Unary ops
	else if (opType == String("!")) {
		return (arg1 == 0);
	}
	else if (opType == String("~")) {
		return (-1 * arg1);
	}

	// invalid operator
	else {
		exit(1);
	}

}


bool isCommand(String command)
{
	if (command == String("var")   ||
	    command == String("text")  ||
	    command == String("output")||
	    command == String("set")   ||
		command == String("do")    ||
		command == String("if")    ||
		command == String("else")  ||
		command == String("od")    ||
		command == String("fi"))
	{
		return true;
	}

	else {
		return false;
	}
}

/**
* Checks if 'command' is an operand.
*
* param command: holds a string representing next token
* return values:
*	(1) - '0' if 'command' is not an operator
* 	(2) - '1' if operand is unary operator
*	(3) - '2' if operand is binary operator (logic, math, comparison)
*/
int32_t isOperand(String command)
{
	if (command == String("+") ||
		command == String("-") ||
		command == String("*") ||
		command == String("/") ||
		command == String("%") ||
		command == String("&&")||
		command == String("||")||
		command == String("<") ||
		command == String(">") ||
		command == String("==")||
		command == String("!=")||
		command == String("<=")||
		command == String(">="))
	{
		return 2;
	}

	else if (command == String("~") || command == String("!"))
	{
		return 1;
	}

	else {
		return 0;
	}
}

/**
 * Converts a string number to its decimal equivalent.
 *
 * param token: String type that holds number
 * return: decimal equivalent of that number
 */
int32_t getNum(String token)
{
	int32_t num = 0;

	for (uint32_t index = 0; index < token.size(); index++) {
		num = (int32_t) ((num * 10) + (token.c_str()[index] - '0'));
	}
	return num;
}


/**
 * Return 0 if 'contender' is neither OR an unitialized variable
 * Return 1 if 'contender' is number.
 * Return 2 if 'contender' is variable.
 */
int32_t isNumOrVar(String contender)
{
	if ((contender.c_str()[0] >= 48) && (contender.c_str()[0] <= 57))
	{
		return 1;
	}
	else if (variables.find(contender) != variables.end()) {
		return 2;
	}
	return 0;
}


/**
 * Recursively computes the BLIP arithmetic expression. If the operand is
 * unary, arg2 is set to 0. This does arithmetic for numbers and variables,
 * assuming the variables already exist.
 *
 * param blipVec: pointer to a 'expVector' type which holds the argument(s)
 * and operand(s).
 * return: the result of the computation of elements in 'blipVec'
 */
int32_t computeRec(expVector* blipVec)
{
	blipVec->words += 1;
	String token = *blipVec->words;

	if (isNumOrVar(token) == 1) {											// number = 1
		return getNum(token);
	}

	else if (isNumOrVar(token) == 2) {										// variable = 2
		return variables[token];											// finds the string type 'token' and returns its corresponding value
	}

	else if ((isOperand(token)) == 2 || (isOperand(token)) == 1)
	{
		String opType = token;												// opType holds operand

		int32_t arg1 = computeRec(blipVec);
		int32_t arg2;

		if (isOperand(token) == 2) {										// binary operands
			arg2 = computeRec(blipVec);
		}
		else {																// unary operands
			arg2 = 0;
		}

		return computation(arg1, arg2, opType);
	}

	else {
		exit(1);
	}
}


/**
 * Creates a statement tree and evaluates it.
 */
void evalStatement()
{
	statementTree blip;							// make new statement tree
	expVector* blipVec = new expVector;										// make new vector to hold statement
	blip.fillStatementTree(&blipVec);										// fill the vector
	blip.expression = blipVec;												// set pointers equal to each other
	blip.executeStatement(blipVec);										// evaluate the statement
	blipVec->~expVector();
}


void evalDoLoop()
{
	// Creates conditional to check
	conditionalTree* condition = new conditionalTree;
	expVector* condVec = new expVector;
	condition->fillConditionalTree(&condVec);							// fills condVec with the condition of do loop
	condition->conditionals = condVec;

	// Creates body of do loop
	blockVector* doBody = new blockVector;
	doBody->fillBlock(String("od"));												//
	doBody->evalDoLoop(condition);
}


void executeBlock(blockVector* block)
{
	for (int index = 0; index < block->blockLen(); index++) {
		statementTree currentStatement = block->doBlock[index];
		block->doBlock->executeStatement(currentStatement.expression);
	}
}


void evalIfElse()
{
	// Creates conditional to check
	conditionalTree condition;
	expVector* condVec = new expVector;
	condition.fillConditionalTree(&condVec);
	condition.conditionals = condVec;

	// Creates body of if block
	blockVector* ifBody = new blockVector;
	ifBody->fillBlock(String("fi"));

	// Cretes body of else block
	blockVector* elseBody = new blockVector;
	elseBody->fillBlock(String("fi"));

	if (computeRec(condVec)) {
		executeBlock(ifBody);
	}
	else {
		executeBlock(elseBody);
	}
}



#endif /* HELPERS_H_ */
