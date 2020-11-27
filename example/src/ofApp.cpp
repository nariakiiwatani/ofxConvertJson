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
	
	Value(data)
	.castTo<Object>()
	.pick("key2", [](const ofJson &src) {
		return Array(src).toObject();
	})
	.effect(Save("obj", 2))
	.effect(SaveObjEach("obj_", 2))
	.convert(ToArray("newkey"))
	.effect(Save("array", 2))
	.effect(SaveArrayEach("array_", 2))
	.effect(Print(2))
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
