//
// Created by zeroc00l on 11/4/23.
//

#include "../include/math.hpp"

void MathParser::RegisterVariable(std::string_view key, int value) {
    variables.insert(std::make_pair(std::string{key},value));
}

void MathParser::UnregisterVariable(std::string_view key, int value) {
    auto findVariable(variables.find(std::string{key}));
    if(findVariable != variables.end()) {
        variables.erase(findVariable);
    }
}

void MathParser::UpdateValue(std::string_view key, int value) {
    auto findVariable(variables.find(std::string{key}));
    if(findVariable != variables.end()) {
        findVariable->second = value;
    }
}

int MathParser::EvaluateExpression(Expression& expression) {
    std::string workingCopy(expression.text);

    expression.success = true;
    expression.error.id = OperationErrorID::NoErrors;
    expression.error.name = OperationErrorNames[(int)expression.error.id];
    expression.error.description = GenerateErrorDescription(expression.error.id, 0, workingCopy);
    expression.result = 0;
    expression.simplifiedText = workingCopy;

    // First, replace all instances of variables with integers
    workingCopy = ReplaceVariables(workingCopy);

    // All non-numeric characters should be removed from the expression at this point.
    // Let's confirm this and flag an error if we detect any such characters:
    int errorPosition( DetectIllegalCharacters(workingCopy) );
    if(errorPosition >= 0) {
        expression.success = false;
        expression.error.id = OperationErrorID::IllegalCharacterFound;
        expression.error.name = OperationErrorNames[(int)expression.error.id];
        expression.error.description = GenerateErrorDescription(expression.error.id, errorPosition, workingCopy);
        return 0;
    } // Illegal character

    // To find a starting point, calculate parenthesis depth:
    int parenthesisCount = CountParenthesis(workingCopy);
    if(parenthesisCount < 0) {
        errorPosition = -1 * parenthesisCount;
        expression.success = false;
        expression.error.id = OperationErrorID::UnmatchedParenthesis;
        expression.error.name = OperationErrorNames[(int)expression.error.id];
        expression.error.description = GenerateErrorDescription(expression.error.id, errorPosition, workingCopy);
    } // Unmatched parenthesis

    // Now simplify all parenthesis-enclosed parts of the expression, beginning with the deepest:
    do {
        expression.parenthesisDepth = CountParenthesis(workingCopy);
        if(expression.parenthesisDepth == 0) {
            expression.containsParenthesis = false;
        } else {
            expression.containsParenthesis = true;
            std::string deepestParenthetical = GetParentheticalExpression(workingCopy, expression.parenthesisDepth);
            std::size_t parentheticalPosition = workingCopy.find(deepestParenthetical);
            SimpleExpression simpleExpression = GenerateSimpleExpression(deepestParenthetical);
            std::string simpleParenthetical = simpleExpression.text;
            simpleExpression.text = workingCopy.substr(0, parentheticalPosition);
            simpleExpression.text.append(simpleParenthetical);
            for(int parenthIndex = 0; parenthIndex < expression.parenthesisDepth; ++parenthIndex) {
                simpleExpression.text.append(")");
            }
            simpleExpression.text.append(workingCopy.substr(parentheticalPosition + deepestParenthetical.length() + 1,
                                                            workingCopy.length() - parentheticalPosition + deepestParenthetical.length()));
            if(simpleExpression.success) {
                while(FindOperator(simpleExpression.text, true) >= 0) {
                    simpleExpression.text = SimplifyExpression(simpleExpression.text);
                }
                if(simpleExpression.success) {
                    workingCopy = simpleExpression.text;
                    expression.text = workingCopy;
                    expression.simplifiedText = workingCopy;
                    expression.result = simpleExpression.result;
                } else {
                    expression.error = simpleExpression.error;
                }
            } else {
                expression.error = simpleExpression.error;
            }
        }
    } while(expression.containsParenthesis);

    expression.simplifiedText = workingCopy;
    if(FindOperator(workingCopy, true) < 0) {
        expression.result = std::atoi(workingCopy.c_str());
    }

    // Now we just locate remaining operators and solve them in order of precedence:
    if(expression.success) {
        while(FindOperator(workingCopy, true) >= 0) {
            workingCopy = SimplifyExpression(workingCopy);
            expression.simplifiedText = workingCopy;
            if(FindOperator(workingCopy, true) < 0) {
                expression.result = std::atoi(expression.simplifiedText.c_str());
            }
        } // while operator found
    } // if no errors
    return expression.result;
}

int MathParser::EvaluateSimpleExpression(SimpleExpression& expression) {
    switch(expression.operatorID) {
        case OperatorID::Division:
            // Check for division by zero first!
            if(expression.operand2 == 0) {
                expression.success = false;
                expression.error.id = OperationErrorID::DivisionByZero;
                expression.error.name = OperationErrorNames[(int)expression.error.id];
                expression.error.description = GenerateErrorDescription(expression.error.id, FindOperator(expression.text, true), expression.text);
            } else {
                expression.result = expression.operand1 / expression.operand2;
                expression.success = true;
            }
            break;
        case OperatorID::Multiplication:
            expression.result = expression.operand1 * expression.operand2;
            expression.success = true;
            break;
        case OperatorID::Addition:
            expression.result = expression.operand1 + expression.operand2;
            expression.success = true;
            break;
        case OperatorID::Subtraction:
            expression.result = expression.operand1 - expression.operand2;
            expression.success = true;
            break;
        case OperatorID::Unknown:
        case OperatorID::OpenParenthesis:
        case OperatorID::CloseParenthesis:
        case OperatorID::TotalNumOperatorIDs:   default:
            break;
    }
    return expression.result;
}

std::string MathParser::GenerateErrorDescription(OperationErrorID errorID, std::size_t errorPosition, std::string_view expression) {
    std::string part1, part2;
    switch(errorID) {
        case OperationErrorID::NoErrors:
            part1 = "All operations were successful!";
            part2 = "No errors reported.";
            break;
        case OperationErrorID::IllegalCharacterFound:
            part1 = "Illegal character detected at position ";
            part2 = " of this expression: ";
            break;
        case OperationErrorID::SyntaxError:
            part1 = "Syntax error detected at position ";
            part2 = " of this expression: ";
            break;
        case OperationErrorID::DivisionByZero:
            part1 = "Division by zero attempted at position ";
            part2 = " of this expression: ";
            break;
        case OperationErrorID::UnmatchedParenthesis:
            part1 = "Unmathced parenthesis found at position ";
            part2 = " of this expression: ";
            break;
        case OperationErrorID::TotalNumOperationErrorIDs: default: break;
    }
    std::string description(part1);
    description.append(std::to_string(errorPosition));
    description.append(part2);
    description.append(expression);
    return description;
}

std::string MathParser::SimplifyExpression(std::string_view expression) {
    if(FindOperator(expression, true) < 0) {
        return std::string{expression};
    }
    SimpleExpression simplified;
    simplified.text = expression;
    std::size_t operatorPosition(FindOperator(expression, true));
    for(int operatorIndex = (int)OperatorID::Division; operatorIndex <= (int)OperatorID::Subtraction; ++operatorIndex) {
        char c(expression[operatorPosition]);
        if(c == OperatorSymbols[operatorIndex]) {
            simplified.operatorID = (OperatorID)operatorIndex;
            break;
        }
    } // find first operator

    std::string op1String(GetPrecedingTerm(expression, operatorPosition));
    std::string op2String(GetNextTerm(expression, operatorPosition));
    std::string firstHalfExpression{ expression.substr(0, operatorPosition) };
    std::size_t op1Position( firstHalfExpression.rfind(op1String) );
    std::size_t op2Position( expression.find(op2String, operatorPosition + 1) );
    simplified.operand1 = std::atoi(op1String.c_str());
    simplified.operand2 = std::atoi(op2String.c_str());

    switch(simplified.operatorID) {
        case OperatorID::Division:
            // Check for division by zero first!
            if(simplified.operand2 == 0) {
                simplified.success = false;
                simplified.error.id = OperationErrorID::DivisionByZero;
                simplified.error.name = OperationErrorNames[(int)simplified.error.id];
                simplified.error.description = GenerateErrorDescription(simplified.error.id, FindOperator(simplified.text, true), simplified.text);
            } else {
                simplified.result = simplified.operand1 / simplified.operand2;
                simplified.success = true;
            }
            break;
        case OperatorID::Multiplication:
            simplified.result = simplified.operand1 * simplified.operand2;
            simplified.success = true;
            break;
        case OperatorID::Addition:
            simplified.result = simplified.operand1 + simplified.operand2;
            simplified.success = true;
            break;
        case OperatorID::Subtraction:
            simplified.result = simplified.operand1 - simplified.operand2;
            simplified.success = true;
            break;
        case OperatorID::Unknown:
        case OperatorID::OpenParenthesis:
        case OperatorID::CloseParenthesis:
        case OperatorID::TotalNumOperatorIDs:   default:
            break;
    }

    std::string preamble("");
    if(op1Position > 0) {
        preamble = expression.substr(0, op1Position);
    }
    simplified.text = preamble;
    std::string resultString = std::to_string(simplified.result);
    simplified.text.append(resultString);
    int lengthReduction = (op1String.length() + op2String.length() + 1) - resultString.length();
    int remainingCharacters = expression.length() - preamble.length() - resultString.length() - lengthReduction;
    bool operatorsRemaining(false);
    if(FindOperator(simplified.text, false) >= 0) {
        operatorsRemaining = true;
    }
    if(remainingCharacters > 0 && operatorsRemaining) {
        std::size_t remainderPos = op2Position + op2String.length();
        std::string remainder = std::string{expression.substr(remainderPos, remainingCharacters)};
        simplified.text.append(remainder);
    }
    return simplified.text;
}

SimpleExpression MathParser::GenerateSimpleExpression(std::string_view expression) {
    SimpleExpression simpleExpression;

    int operatorPosition(FindOperator(expression,true));
    if(operatorPosition >= 0) {
        const char operatorSymbol(expression.at(operatorPosition));
        simpleExpression.operatorID = GetOperatorID(operatorSymbol);
        std::string operand1String(GetPrecedingTerm(expression, operatorPosition));
        std::string operand2String(GetNextTerm(expression, operatorPosition));
        simpleExpression.operand1 = std::atoi(operand1String.c_str());
        simpleExpression.operand2 = std::atoi(operand2String.c_str());
        simpleExpression.text = expression;
        simpleExpression.success = true;
    } else {
        simpleExpression.success = false;
        simpleExpression.error.id = OperationErrorID::SyntaxError;
        simpleExpression.error.name = OperationErrorNames[(int)simpleExpression.error.id];
        simpleExpression.error.description = GenerateErrorDescription(simpleExpression.error.id, 0, expression);
    } // Syntax Error

    return simpleExpression;
}

std::string MathParser::ReplaceVariables(std::string_view expression) {
    std::string workingCopy(expression);
    std::string simplifiedExpression("");

    for(auto variable = variables.begin(); variable != variables.end(); ++variable) {
        std::string key(variable->first);
        int value (variable->second);
        std::size_t keyPosition(0);
        do {
            keyPosition = workingCopy.find(key);
            if(keyPosition != std::string::npos) {
                std::string segment1(workingCopy.substr(0, keyPosition));
                simplifiedExpression = segment1;
                simplifiedExpression.append(std::to_string(value));
                if (workingCopy.length() > keyPosition + key.length()) {
                    simplifiedExpression.append(
                            workingCopy.substr(keyPosition + key.length() + 1, workingCopy.length() - keyPosition - key.length() - 1));
                }
                workingCopy = simplifiedExpression;
            }
        } while(keyPosition != std::string::npos);
    } // for each variable

    return workingCopy;
}

int MathParser::DetectIllegalCharacters(std::string_view expression) {
    bool illegalCharacter(false);
    std::size_t errorPosition(0);

    for(int index = 0; !illegalCharacter && index < expression.length(); ++index) {
        char c(expression[index]);
        if(std::isalpha(c)) {
            illegalCharacter = true;
            errorPosition = index;
        } else if(std::ispunct(c)) {
            bool symbolFound(false);
            for(auto opIndex = 0; !symbolFound && opIndex < (int)OperatorID::TotalNumOperatorIDs; ++opIndex) {
                if(c == OperatorSymbols[opIndex]) {
                    symbolFound = true;
                }
            }
            if(!symbolFound) {
                illegalCharacter = true;
                errorPosition = index;
            }
        }
    }
    if(!illegalCharacter) {
        return -1;
    }
    return errorPosition;
}

int MathParser::CountParenthesis(std::string_view expression) {
    int openCount(0), closeCount(0);
    std::size_t lastOpenPosition(0);
    std::size_t lastClosePosition(0);
    bool allMatched(true);
    for(int index = 0; index < expression.length(); ++index) {
        char c(expression[index]);
        if(c == OperatorSymbols[(int)OperatorID::OpenParenthesis]) {
            ++openCount;
            lastOpenPosition = index;
        } else if(c == OperatorSymbols[(int)OperatorID::CloseParenthesis]) {
            ++closeCount;
            lastClosePosition = index;
        }
    }
    if(openCount > closeCount) {
        return (-1 * lastOpenPosition);
    } else if(openCount < closeCount) {
        return (-1 * lastClosePosition);
    }
    return openCount;
}

std::string MathParser::GetParentheticalExpression(std::string_view expression, int depth) {
    std::string parenthetical("");

    int currentDepth(0);
    int openCount(0), closeCount(0);
    std::size_t beginPosition(0), endPosition(0);
    for(int index = 0; currentDepth < depth && index < expression.length(); ++index) {
        if(expression[index] == OperatorSymbols[(int)OperatorID::OpenParenthesis]) {
            ++openCount;
            ++currentDepth;
            if(currentDepth == depth) {
                beginPosition = index + 1;
                endPosition = expression.find(OperatorSymbols[(int)OperatorID::CloseParenthesis], index + 1);
            }
        } else if(expression[index] == OperatorSymbols[(int)OperatorID::CloseParenthesis]) {
            ++closeCount;
            --currentDepth;
        }
    }
    parenthetical = expression.substr(beginPosition, endPosition - beginPosition);
    return parenthetical;
}

int MathParser::FindOperator(std::string_view expression, bool excludeParens) {
    int operatorPosition(-1);
    char OperatorSubset[(int)OperatorID::TotalNumOperatorIDs - 2];
    for(int index = 0; index < (int)OperatorID::TotalNumOperatorIDs - 2; ++index) {
        OperatorSubset[index] = OperatorSymbols[index+2];
    }
    OperatorID operatorFound;

    for(int index = 0; index < expression.length(); ++index) {
        char c(expression[index]);
        for(int operatorIndex = 0; operatorIndex < (int)OperatorID::TotalNumOperatorIDs; ++operatorIndex) {
            if(excludeParens) {
                if(c == OperatorSubset[operatorIndex]) {
                    if(operatorPosition == -1 || (int)operatorFound > operatorIndex){
                        operatorPosition = index;
                        operatorFound = (OperatorID)(operatorIndex + 2);
                    }
                }
            } else if(c == OperatorSymbols[operatorIndex]) {
                if(operatorPosition == -1 || (int)operatorFound > operatorIndex){
                    operatorPosition = index;
                    operatorFound = (OperatorID)operatorIndex;
                }
            }
        } // for each operator
    } // for each character in expression

    return operatorPosition;
}

OperatorID MathParser::GetOperatorID(char symbol) const {
    OperatorID opID(OperatorID::Unknown);
    for(int operatorIndex = 0; opID == OperatorID::Unknown && operatorIndex < (int)OperatorID::TotalNumOperatorIDs; ++operatorIndex) {
        if(symbol == OperatorSymbols[(int)operatorIndex]) {
            opID = (OperatorID)operatorIndex;
        }
    }
    return opID;
}

std::string MathParser::GetPrecedingTerm(std::string_view expression, std::size_t position) {
    std::size_t beginPosition(position);
    int length(0);
    bool done(false);
    bool operatorFound(false);
    bool isTerm(false);
    for(int index = position - 1; !done && index >= 0; --index) {

        char c(expression[index]);
        for(int opIndex = 0; !operatorFound && opIndex < (int)OperatorID::TotalNumOperatorIDs; ++opIndex) {
            if(c == OperatorSymbols[opIndex]) {
                operatorFound = true;
            }
        }
        if(!operatorFound && c != ' ') {
            isTerm = true;
        } else {
            isTerm = false;
        }
        if(isTerm) {
            ++length;
        } else {
            if(length > 0) {
                done = true;
            } else {
                operatorFound = false;
                beginPosition = index;
            }
        }

    }
    std::string precedingTerm = std::string{expression.substr(beginPosition - length, length)};
    return precedingTerm;
}

/*
std::string MathParser::GetPrecedingTerm(std::string_view expression, std::size_t position) {
    std::size_t beginPosition(position - 1), endPosition(0);
    bool termFound(false);
    bool operatorFound(false);
    // Find first whitespace before position
    for(int index = position - 1; endPosition == 0 && index >= 0; --index) {
        char c(expression[index]);
        for(int opIndex = 0; !operatorFound && opIndex < (int)OperatorID::TotalNumOperatorIDs; ++opIndex) {
            if(c == OperatorSymbols[opIndex]) {
                    operatorFound = true;
            }
        }
        if(c == ' ') {
            if(!termFound) {
                beginPosition = index;
                operatorFound =  false;
            } else {
                endPosition = index;
            }
        } else if(operatorFound) {
            if(termFound) {
                endPosition  = index;
            } else {
                operatorFound = false;
            }
        } else {
            termFound = true;
        }
    }
    if(endPosition <= beginPosition) {
        if(endPosition == 0) {
            beginPosition = endPosition;
            endPosition = position;
        } else {
            std::size_t tmpPosition = endPosition;
            beginPosition = endPosition;
            endPosition = tmpPosition;
        }
    }
    std::string precedingTerm = std::string{expression.substr(beginPosition, endPosition - beginPosition)};
    return precedingTerm;
}
*/
std::string MathParser::GetNextTerm(std::string_view expression, std::size_t position) {
    std::string nextTerm("");
    bool done(false);
    bool operatorFound(false);
    for(int index = position + 1; !done && index < expression.length(); ++index) {
        char c(expression[index]);
        for(int opIndex = 0; !operatorFound && opIndex < (int)OperatorID::TotalNumOperatorIDs; ++opIndex) {
            if(c == OperatorSymbols[opIndex]) {
                operatorFound = true;
            }
        }
        if(c == ' ' || operatorFound) {
            if(nextTerm.length() > 0) {
                done = true;
            } else {
                operatorFound = false;
            }
        } else {
            nextTerm += c;
        }
    }
    return nextTerm;
}
