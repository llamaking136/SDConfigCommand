#include "SDConfigCommand.h"

char* SDConfigCommand::getCmd() {
	return cmd;
}

String SDConfigCommand::getCmdS() {
	return cmd;
}

char* SDConfigCommand::getValue() {
	return value;
}

String SDConfigCommand::getValueS() {
	return value;
}

int SDConfigCommand::getValueInt() {
	return atoi(value);
}

float SDConfigCommand::getValueFloat() {
	return atof(value); 
}
