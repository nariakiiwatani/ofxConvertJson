#include "ofApp.h"
#include "ofxConvertJson.h"
#include "ofxConvertJsonHelper.h"

using namespace ofx::convertjson;

//--------------------------------------------------------------
void ofApp::setup(){
	ofJson data = {
		{"key1", {{"arr1", 1},{"arr2", 2}}},
		{"key2", {"arr3", 2}},
	};
	ofJson copy;
	ofJson result = helpers::Helper(data)
//	.apply<DownStair>("key1")
	.copy(copy)
	.dispatch({
		[](const ofJson &src) -> ofJson {
			return helpers::Helper(src).apply<DownStair>("arr1");
		},
		[](const ofJson &src) -> ofJson {
			return helpers::Helper(src).apply<CherryPick>("arr2");
		},
	})
	.toObject()
//	.apply<CherryPick>(std::vector<std::string>{"arr1"})
//	.apply<DownStair>("arr1")
	.apply<Print>(2)
	;
//	std::cout << copy.dump(2) << std::endl;
//	std::cout << data.dump(2) << std::endl;
//	std::cout << result.dump(2) << std::endl;
	//	data = Chain({DownStair("key1"), ModValueOfKey("data", 3.14)}).convert(data);
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
