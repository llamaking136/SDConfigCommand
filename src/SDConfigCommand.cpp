#include "SDConfigCommand.h"

SDConfigCommand::SDConfigCommand() {
	// Empty constructor
}

bool SDConfigCommand::set(char myFilename[], int myCs, void (*myFunction)()) {
	//Put all settings into private values
	strncpy(filename, myFilename, sizeof(filename)-1);
	cs = myCs;
	callback_Function = myFunction;

	// Begin reading SD card
	// Try for TRY amount of times
	for(char i = 0; i < TRY; i++) {
		if (SD.begin(cs)) return true;

		else if (i == TRY-1) {
			Serial.println(F("SD read failed!"));
			return false;
		} else {
			Serial.print(F("Reading SD..."));
			Serial.println(i+1);
		}

	}
}

bool SDConfigCommand::set(String myFilename, int myCs, void (*myFunction)()) {
	// This is the same as the other one, just that this one takes String for myFilename

	char myFilenameCString[13];
	// Use strncopy to limit how much you can copy to avoid overflow
	strncpy(myFilenameCString, myFilename.c_str(), sizeof(myFilenameCString)-1);
	
	return set(myFilenameCString, myCs, myFunction);
}

bool SDConfigCommand::readConfig() {
	// If open file and read successfully, close file
	if (openFile(cFile, filename, FILE_READ) && parseFile()) {
		cFile.close();
		return true;
	} else {
		return false;
	}
}

bool SDConfigCommand::openFile(File &myFile, char* myFilename, char rw) {
	// rw should be FILE_READ or FILE_WRITE
	// myFile should be passed by reference
	 
	// Try to open the file for TRY number of times
	for (char i = 0; i < TRY; i++){
		if (myFile=SD.open(myFilename, rw)) return true;
		
		else if (i == TRY-1) {
			Serial.print(myFilename);
			Serial.println(F(" open failed!"));
			return false;
		} else {
			Serial.print(F("Opening "));
			Serial.print(myFilename);
			Serial.print(F("..."));
			Serial.println(i+1);
		}	
	}
}

bool SDConfigCommand::parseFile() {
	// Default function to read in config by Character Analysis
	// This section follows this flow chart: 
	// https://drive.google.com/file/d/1bJ0vlCzHEXuG8vAbBYMGmATn-_yRA_Rs/view?usp=sharing
	// You can open the file with draw.io or app.diagrams.net or view the included image files
	// It is easier to read the flow chart to understand the logic
 
	while (cFile.available() > 0) {
		if (this->commentMode) {
			char myChar = cFile.read();
			
			if (myChar=='\n' || myChar=='\r') {
				this->commentMode = false;
				continue;
			}
		} else {
			if (cFile.available() > 1) {
				char myChar = cFile.read();
				if (myChar == '/') {
					if (cFile.peek() == '/'){ 
						this->commentMode = true; 
						continue;
					} else { 
						if (!writeCmdValue(myChar)) return false;	
					}
				} else if (myChar == '='){ 
					this->readMode = READ_VALUE; 
					continue;
				} else if (myChar=='\n' || myChar=='\r' ) {
					if (strcmp(cmd, " ") == 0 || strcmp(cmd, "") == 0 ) continue;
					
					else { 
						callbackSequence(); 
						continue;
					}
				} else {
					if (!writeCmdValue(myChar)) return false;
				}
			} else {
				char myChar = cFile.read();
				if (myChar=='\n' || myChar=='\r') {	callbackSequence(); }

				else{
					writeCmdValue(myChar);
					callbackSequence();
				}
			}	
		}
	}
	
	//Serial.println("End of File");
	return true;
	
}

void SDConfigCommand::callbackSequence() {
	// Call the user defined callback function after each command and value is read
	// and let user decide what to do with them

	callback_Function();

	// Reset all relevant variable for next read
	// cmd and value will be empty string
	this->cmd[0] = '\0'; 
	this->value[0] = '\0';
	this->readMode = READ_CMD;
	this->commentMode = false; 
}

bool SDConfigCommand::writeCmdValue(char myChar) {
	// Concat the incoming char into to form the command or value

	if (readMode == READ_CMD) { 
		char L = strlen(cmd);
		if (L < (CMD_SIZE-2)) { // Make sure there is enough memory, account for NULL and new char
			cmd[L] = myChar; // Append to last space
			cmd[L+1] = '\0'; // Manually write in NULL
			return true;
		} else return false;
	} else { // Switch over the value is readMode is set to READ_VALUE
		char L = strlen(value);
		if (L < (VALUE_SIZE-2)) {
			value[L] = myChar;
			value[L+1] = '\0';
			return true;
		} else return false;
	}
}

bool SDConfigCommand::openInSerial() {
	// Dumps the text in one file all into the Serial monitor

	if (openFile(cFile, filename, FILE_READ)) {
		Serial.print(F("--- "));
		Serial.print(filename);
		Serial.println(F(" ---"));

		while (cFile.available()>0) Serial.write(cFile.read());
			
		Serial.println(F("\n---------"));
		cFile.close();
		
		return true;
	} else {
		Serial.println(F("Cannot open in serial"));
		return false;
	}
}
