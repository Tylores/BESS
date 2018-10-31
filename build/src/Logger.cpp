#include "include/Logger.h"
#include "include/tsu.h"
#include <iostream>
#include <fstream>

Logger::Logger (std::string context) : context_(context) {
	msg_ =  tsu::GetDateTime () + '\t';
}  // end constructor

Logger::~Logger () {
	Logger::ToFile ();
}  // end destructor

void Logger::ToFile  () {
	std::string path = "../../LOGS/";
	std::string device = "test";
	std::string file_name = path + device + "_" + context_ 
		+ "_" + tsu::GetDate () + ".log";
	std::ofstream output_file(file_name, std::ios_base::app);
	if (output_file.is_open()) {
		output_file << msg_ << '\n';
	}
	output_file.close();
}  // end To File
