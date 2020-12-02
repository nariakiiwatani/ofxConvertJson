#include "ofxConvjson.h"
#include <iostream>
#include "ofJson.h"
#include "ofUtils.h"

bool convjson::SaveJson(const std::string &path, const Json &json) {
	return ofSavePrettyJson(ofToDataPath(path), json);
}
void convjson::PrintFunc(const Json &json) {
	std::cout << json.dump(2) << std::endl;
}

