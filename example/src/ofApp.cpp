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
	ofJson result = helpers::Value(data)
	.castTo<helpers::Object>()
	.pick({"key1"}, [](const ofJson &src) -> ofJson {
		return helpers::Object(src)
		.pick({"arr1"}, Set({"newvalue", 100}));
	})
	.pick({"key2"}, [](const ofJson &src) -> ofJson {
		return helpers::Array(src)
		.toObject();
	})
	.println(2)
	.save("all.json", 2)
	.saveEach("obj", 2)
	.toArray("key")
	.copy(copy)
	.saveEach([copy](const std::size_t &index, const ofJson&, const ofJson&) {
		return copy[index]["key"].get<string>()+".json";
	}, 2)
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
