#include "ofApp.h"
#include "ofxConvertJson.h"

using namespace convjson::helpers;
using namespace std;

//--------------------------------------------------------------
void ofApp::setup(){
	ofJson data = {
		{"key1", {{"arr1", 1},{"arr2", 2}}},
		{"key2", {"arr3", 2}},
	};
	
	Value(data)
	.castTo<Object>()
//	.modify(CherryPick({"key2"}))
	.modify(ToArray([](const std::string &key, const ofJson &item, const ofJson &src) {
		return item;
	}))
	.effect(Print())
	.effect(Save("obj"))
	.effect(SaveObjEach("obj_"))
//	.convert(ToArray("newkey"))
	.convert(Dispatch(2,
		[](std::size_t index, const ofJson &src) { return ofJson{{"wrap_"+ofToString(index), src}}; }
	))
	.effect(Save("array"))
	.effect(SaveArrayEach("array_"))
	.convert(ToObj([](std::size_t index, const ofJson &item, const ofJson &src) {
		return make_pair("item_"+ofToString(index), item);
	}))
	.effect(Print())
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
