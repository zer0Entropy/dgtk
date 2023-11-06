//
// Created by zeroc00l on 11/4/23.
//

#include <map>
#include <vector>
#include <string>

#ifndef DGTKPROJECT_MATH_HPP
#define DGTKPROJECT_MATH_HPP

enum class OperatorID {
    Unknown = -1, OpenParenthesis, CloseParenthesis, Division, Multiplication, Addition, Subtraction, TotalNumOperatorIDs
};

const char OperatorSymbols[(int)OperatorID::TotalNumOperatorIDs] = { '(', ')', '/', '*', '+', '-' };

enum class OperationErrorID {
    NoErrors, IllegalCharacterFound, SyntaxError, DivisionByZero, UnmatchedParenthesis, TotalNumOperationErrorIDs
};

const std::string OperationErrorNames[(int)OperationErrorID::TotalNumOperationErrorIDs] = {
        {"No Errors"},
        {"Illegal Character Found"},
        {"Syntax Error"},
        {"Division By Zero"},
        {"Unmatched Parenthesis"}
};

struct OperationError {
    OperationErrorID id;
    std::string name;
    std::string description;
};

struct Operation {
    struct Operator {
        OperatorID id;
        char symbol;
    };
    std::vector<int> operands;
};

struct SimpleExpression {
    int operand1;
    int operand2;
    OperatorID operatorID;
    bool success;
    int result;
    OperationError error;
    std::string text;
};

struct Expression {
    std::string text;
    std::string simplifiedText;
    OperationError error;
    bool success;
    int result;
    bool containsParenthesis;
    int parenthesisDepth;
};

class MathParser {
public:
    void RegisterVariable(std::string_view key, int value);
    void UnregisterVariable(std::string_view key, int value);
    void UpdateValue(std::string_view key, int value);
    int EvaluateExpression(Expression& expression);
    int EvaluateSimpleExpression(SimpleExpression& expression);
private:

    std::string GenerateErrorDescription(OperationErrorID errorID, std::size_t errorPosition, std::string_view expression);

    std::string SimplifyExpression(std::string_view expression);

    SimpleExpression GenerateSimpleExpression(std::string_view expression);

    std::string ReplaceVariables(std::string_view expression);

    // Returns -1 if no illegal characters found, or the position of the first illegal character otherwise:
    int DetectIllegalCharacters(std::string_view expression);

    // Returns the number of matched parentheses if all parentheses are matched,
    // or -1 * the position of the first unmatched parenthesis otherwise:
    int CountParenthesis(std::string_view expression);

    std::string GetParentheticalExpression(std::string_view expression, int depth = 1);

    // Returns -1 if no operators found, or the position of the first operator otherwise:
    int FindOperator(std::string_view expression);

    OperatorID GetOperatorID(char symbol) const;

    std::string GetPrecedingTerm(std::string_view expression, std::size_t position);

    std::string GetNextTerm(std::string_view expression, std::size_t position);

    std::map<std::string,int> variables;
};

#endif //DGTKPROJECT_MATH_HPP
