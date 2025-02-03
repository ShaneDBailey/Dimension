
#ifndef LOADER_H
#define LOADER_H

#include "Model.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

class Loader {
public:
    static Model load_obj(const std::string& obj_file_path, const std::string& mtl_file_path);
};

#endif 
