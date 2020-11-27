#include "ofApp.h"
#include "ofxConvertJson.h"
#include "ofxConvertJsonHelper.h"

using namespace ofx::convertjson;
using namespace ofx::convertjson::helpers;

//--------------------------------------------------------------
void ofApp::setup(){
	ofJson data = {
		{"key1", {{"arr1", 1},{"arr2", 2}}},
		{"key2", {"arr3", 2}},
	};
	
	ofJson copy;
	ofJson result = Object(data)
	.pick("key1", [](const ofJson &src) -> ofJson {
		return Object(src)
		.pick("arr1", conv::Set({"newvalue", 100}));
	})
	.pick("key2", [](const ofJson &src) -> ofJson {
		return Array(src)
		.toObject();
	})
	.view(Print(2))
	.view(Save("all.json", 2))
	.view(SaveObjEach("obj_", 2))
	.mod(ToArray("key"))
	.effect(Copy(copy))
	.view(Print(2))
	.view(SaveArrayEach([](const std::size_t &index, const ofJson &item, const ofJson &src) {
		return item["key"].get<string>();
	}, 2))
	;
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
