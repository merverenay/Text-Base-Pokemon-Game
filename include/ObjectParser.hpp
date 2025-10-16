#ifndef OBJECT_PARSER_HPP
#define OBJECT_PARSER_HPP

#include "ObjectDescription.hpp"
#include <vector>
#include <string>

std::vector<ObjectDescription> parseObjectFile(const std::string &filepath);

#endif
