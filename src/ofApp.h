#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include  "ofxAssimpModelLoader.h"
#include "Octree.h"
#include "ParticleEmitter.h"
#include <glm/gtx/intersect.hpp>



class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void gotMessage(ofMessage msg);
		void drawAxis(ofVec3f);
		void initLightingAndMaterials();
		void savePicture();
		void toggleWireframeMode();
		void togglePointsDisplay();
		void toggleSelectTerrain();
		void setCameraTarget();
        float getAGL(); //get altitude of lander
		bool mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point);
		bool raySelectWithOctree(ofVec3f &pointRet);
		glm::vec3 getMousePointOnPlane(glm::vec3 p , glm::vec3 n);

		ofEasyCam cam;  // free cam view
        ofCamera sideCam;
        ofCamera topCam;
        ofCamera landerCam;
        ofCamera *theCam = NULL;
    
        ofLight keyLight, fillLight, rimLight, landerLight;
    
        ofImage background;
		ofxAssimpModelLoader mars, lander;
		ofLight light;
		Box boundingBox, landerBounds;
		vector<Box> colBoxList;
		bool bLanderSelected = false;
		Octree octree;
		TreeNode selectedNode;
        TreeNode intersectNode;
		glm::vec3 mouseDownPos, mouseLastPos;
		bool bInDrag = false;
    
    
        ofVec3f heading = ofVec3f(1, 0, 0); //lander's heading
        ParticleEmitter exhaust, explosion;
        Particle p;
        ParticleSystem physicSystem; //handle physic of lander
        GravityForce *gravity = NULL;
        TurbulenceForce *turbulence = NULL;
        ImpulseRadialForce *radial = NULL;
        
        float agl;
		ofxIntSlider numLevels;
		ofxPanel gui;

		bool bAltKeyDown;
		bool bCtrlKeyDown;
		bool bWireframe;
		bool bDisplayPoints;
		bool bPointSelected;
		bool bHide;
		bool pointSelected = false;
		bool bDisplayLeafNodes = false;
		bool bDisplayOctree = false;
		bool bDisplayBBoxes = false;
        bool developerMode = false;
        bool drawHeading = false;
        bool bBackgroundLoaded = false;
        bool gameOver = false;
    
		bool bLanderLoaded;
		bool bTerrainSelected;
	
		ofVec3f selectedPoint;
		ofVec3f intersectPoint;

		vector<Box> bboxList;

		const float selectionRange = 4.0;
};
