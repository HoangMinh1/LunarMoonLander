
//--------------------------------------------------------------
//
//  Kevin M. Smith
//
//  Octree Test - startup scene
// 
//
//  Student Name:   < Your Name goes Here >
//  Date: <date of last version>


#include "ofApp.h"
#include "Util.h"


//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup(){
	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bLanderLoaded = false;
	bTerrainSelected = true;
//	ofSetWindowShape(1024, 768);
    
    
    // set up lights
    //
    keyLight.setup();
    keyLight.enable();
    keyLight.setAreaLight(1, 1);
    keyLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
    keyLight.setDiffuseColor(ofFloatColor(1, 1, 1));
    keyLight.setSpecularColor(ofFloatColor(1, 1, 1));
    
    keyLight.rotate(45, ofVec3f(0, 1, 0));
    keyLight.rotate(-45, ofVec3f(1, 0, 0));
    keyLight.setPosition(5, 40, 5);

    fillLight.setup();
    fillLight.enable();
    fillLight.setSpotlight();
    fillLight.setScale(.05);
    fillLight.setSpotlightCutOff(15);
    fillLight.setAttenuation(2, .001, .001);
    fillLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
    fillLight.setDiffuseColor(ofFloatColor(1, 1, 1));
    fillLight.setSpecularColor(ofFloatColor(1, 1, 1));
    fillLight.rotate(-10, ofVec3f(1, 0, 0));
    fillLight.rotate(-45, ofVec3f(0, 1, 0));
    fillLight.setPosition(-50, 50, 5);

    rimLight.setup();
    rimLight.enable();
    rimLight.setSpotlight();
    rimLight.setScale(.05);
    rimLight.setSpotlightCutOff(30);
    rimLight.setAttenuation(.2, .001, .001);
    rimLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
    rimLight.setDiffuseColor(ofFloatColor(1, 1, 1));
    rimLight.setSpecularColor(ofFloatColor(1, 1, 1));
    rimLight.rotate(180, ofVec3f(0, 1, 0));
    rimLight.setPosition(0, 20, -70);
    
    
    landerLight.setup();
    landerLight.enable();
    landerLight.setSpotlight();
    landerLight.setScale(.05);
    landerLight.setSpotlightCutOff(50);
    landerLight.setAttenuation(.2, .001, .001);
    landerLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
    landerLight.setDiffuseColor(ofFloatColor(1, 1, 1));
    landerLight.setSpecularColor(ofFloatColor(1, 1, 1));
    //landerLight.rotate(-90, ofVec3f(0, 1, 0));
    //landerLight.rotate(-45, ofVec3f(0, 0, 1));
    landerLight.setPosition(lander.getPosition().x, lander.getPosition().y, lander.getPosition().z);
    
    
    // set up camera
    //
	cam.setDistance(40);
	cam.setNearClip(.1);
	cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
	ofSetVerticalSync(true);
	cam.disableMouseInput();
	ofEnableSmoothing();
	ofEnableDepthTest();
    
    
    sideCam.setPosition(lander.getPosition().x, lander.getPosition().y, lander.getPosition().z + 300);
    sideCam.lookAt(lander.getPosition());
    
    topCam.setPosition(lander.getPosition().x - 40, lander.getPosition().y + 40, lander.getPosition().z);
    topCam.lookAt(lander.getPosition());   //look at lander
    
    landerCam.setPosition(lander.getPosition().x, lander.getPosition().y, lander.getPosition().z);
    landerCam.lookAt(lander.getPosition() + ofVec3f(0, -5, 0), ofVec3f(1, 0, 0)); // look down
    
    theCam = &topCam;

	// setup rudimentary lighting 
	//
	initLightingAndMaterials();

	mars.loadModel("geo/moon-houdini.obj");
	mars.setScaleNormalization(false);
    
    // add lander
    //
    lander.loadModel("geo/lander.obj");
    lander.setScaleNormalization(false);
    bLanderLoaded = true;
    lander.setPosition(0, 80, 0);
    
    glm::vec3 min = lander.getSceneMin() + lander.getPosition();
    glm::vec3 max = lander.getSceneMax() + lander.getPosition();
    
    landerBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
    
    
    // add ParticleEmitter
    //
    exhaust.type = RadialEmitter;
    exhaust.init();
    exhaust.setPosition(lander.getPosition());
    exhaust.setVelocity(ofVec3f(0, -5, 0)); //downward vector
    exhaust.setLifespan(0.5);
    exhaust.setGroupSize(50);
    exhaust.setRate(80);
    
    explosion.type = RadialEmitter;
    explosion.init();
    explosion.setLifespan(4);
    explosion.setGroupSize(400);
    radial = new ImpulseRadialForce(1000.0);
    explosion.sys->addForce(radial);
    explosion.setOneShot(true);
    
    // add Physic to lander
    //
    p.position = lander.getPosition();
    p.lifespan = -1;
    p.damping = 0.999;
    
    physicSystem.add(p);
    gravity = new GravityForce(glm::vec3(0, -0.1625, 0));
    turbulence = new TurbulenceForce(glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1));
    physicSystem.addForce(gravity);
    physicSystem.addForce(turbulence);
    
    // variables init
    //
    agl = -1;
    background.load("geo/background.jpeg");
    bBackgroundLoaded = true;
    
	// create sliders for testing
	//
	gui.setup();
	gui.add(numLevels.setup("Number of Octree Levels", 10, 1, 10));
	bHide = true;

	//  Create Octree for testing.
	//
	
	octree.create(mars.getMesh(0), 20);
	
	cout << "Number of Verts: " << mars.getMesh(0).getNumVertices() << endl;

	//testBox = Box(Vector3(3, 3, 0), Vector3(5, 5, 2));

}
 
//--------------------------------------------------------------
// incrementally update scene (animation)
//
void ofApp::update() {
    if(!developerMode) {
        
        ofVec3f pos = physicSystem.particles[0].position;
        lander.setPosition(pos.x, pos.y, pos.z);
        
        //update landerBounds
        ofVec3f min = lander.getSceneMin() + lander.getPosition();
        ofVec3f max = lander.getSceneMax() + lander.getPosition();

        landerBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
        
        
        exhaust.setPosition(lander.getPosition());
        exhaust.update();
        
        
        
        
        if(lander.getPosition().y > 0) {physicSystem.update();}
        
        if(agl != -1 && agl < 3) {
            // collision
            int exThreshold = 4;
            Particle par = physicSystem.particles[0];
            if(par.velocity.length() < exThreshold) {
                // bouncing
                ofVec3f norm = octree.mesh.getNormal(intersectNode.points[0]);
                physicSystem.addForce(new ImpulseForce(par.velocity, norm));
            }
            else {
                explosion.setPosition(lander.getPosition());
                explosion.start();
                bLanderLoaded = false; // not draw lander
            }
        }
        explosion.update();
            
        
        // update cam
        //
        topCam.setPosition(lander.getPosition().x - 40, lander.getPosition().y + 40, lander.getPosition().z);
        topCam.lookAt(lander.getPosition());   //look at lander
        
        
        sideCam.setPosition(lander.getPosition().x, lander.getPosition().y, lander.getPosition().z + 60);
        sideCam.lookAt(lander.getPosition());
        
        landerCam.setPosition(lander.getPosition().x, lander.getPosition().y, lander.getPosition().z);
        landerCam.lookAt(lander.getPosition() + ofVec3f(0, -5, 0), ofVec3f(1, 0, 0)); // look down
        
        landerLight.setPosition(lander.getPosition().x, lander.getPosition().y, lander.getPosition().z);
    }
}
//--------------------------------------------------------------
void ofApp::draw() {
    
	ofBackground(ofColor::black);

	glDepthMask(false);
	if (!bHide) gui.draw();
	glDepthMask(true);
    
    if (bBackgroundLoaded) {
            ofPushMatrix();
            ofDisableDepthTest();
            ofSetColor(ofColor::white);
            ofScale(0.5, 0.5);
            background.draw(-200, -100);
            ofEnableDepthTest();
            ofPopMatrix();
        }

	theCam->begin();
	ofPushMatrix();
    
    
	if (bWireframe) {                    // wireframe mode  (include axis)
		ofDisableLighting();
		ofSetColor(ofColor::slateGray);
		mars.drawWireframe();
		if (bLanderLoaded) {
			lander.drawWireframe();
			if (!bTerrainSelected) drawAxis(lander.getPosition());
		}
		if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));
	}
	else {
		ofEnableLighting();              // shaded mode
		mars.drawFaces();
		ofMesh mesh;
		if (bLanderLoaded) {
			lander.drawFaces();
			if (!bTerrainSelected) drawAxis(lander.getPosition());
			if (bDisplayBBoxes) {
				ofNoFill();
				ofSetColor(ofColor::white);
				for (int i = 0; i < lander.getNumMeshes(); i++) {
					ofPushMatrix();
					ofMultMatrix(lander.getModelMatrix());
					ofRotate(-90, 1, 0, 0);
					Octree::drawBox(bboxList[i]);
					ofPopMatrix();
				}
			}
            // draw emitter if only lander is there
            exhaust.draw();

			if (bLanderSelected) {

				ofVec3f min = lander.getSceneMin() + lander.getPosition();
				ofVec3f max = lander.getSceneMax() + lander.getPosition();

				landerBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
				ofSetColor(ofColor::white);
				Octree::drawBox(landerBounds);

				// draw colliding boxes
				//
				ofSetColor(ofColor::lightBlue);
				for (int i = 0; i < colBoxList.size(); i++) {
					Octree::drawBox(colBoxList[i]);
				}
			}
		}
	}
	if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));



	if (bDisplayPoints) {                // display points as an option    
		glPointSize(3);
		ofSetColor(ofColor::green);
		mars.drawVertices();
	}

	// highlight selected point (draw sphere around selected point)
	//
	if (bPointSelected) {
		ofSetColor(ofColor::blue);
		ofDrawSphere(selectedPoint, .1);
	}


	// recursively draw octree
	//
	ofDisableLighting();
	int level = 0;
	//	ofNoFill();

	if (bDisplayLeafNodes) {
		octree.drawLeafNodes(octree.root);
		cout << "num leaf: " << octree.numLeaf << endl;
    }
	else if (bDisplayOctree) {
		ofNoFill();
		ofSetColor(ofColor::white);
		octree.draw(numLevels, 0);
        
        //draw bounder for lander
        Octree::drawBox(landerBounds);
	}

	// if point selected, draw a sphere
	//
	if (pointSelected) {
		ofVec3f p = octree.mesh.getVertex(selectedNode.points[0]);
		ofVec3f d = p - cam.getPosition();
		ofSetColor(ofColor::lightGreen);
		ofDrawSphere(p, .02 * d.length());
	}
    
    // draw Heading
    if(drawHeading) {
        ofSetColor(ofColor::red);
        ofDrawLine(lander.getPosition(), lander.getPosition() + 2 * heading);
    }
    
    explosion.draw();
    
	ofPopMatrix();
	theCam->end();
    
    // draw AGL
    ofSetColor(ofColor::white);
    float val = getAGL();
    if(val != -1) agl = val;
    ofDrawBitmapString("AGL: " + std::to_string(agl), ofGetWindowWidth() - 170, 15);
}


// 
// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location) {

	ofPushMatrix();
	ofTranslate(location);

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));
	

	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

	ofPopMatrix();
}


void ofApp::keyPressed(int key) {

	switch (key) {
	case 'B':
	case 'b':
		bDisplayBBoxes = !bDisplayBBoxes;
		break;
	case 'C':   //developer mode
	case 'c':
        if (cam.getMouseInputEnabled()) {
            cam.disableMouseInput();
            developerMode = true;
        }
        else {
            cam.enableMouseInput();
            developerMode = false;
        }
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
        drawHeading = !drawHeading;
		break;
	case 'L':
	case 'l':
		bDisplayLeafNodes = !bDisplayLeafNodes;
		break;
	case 'O':
	case 'o':
		bDisplayOctree = !bDisplayOctree;
		break;
	case 'r':
		cam.reset();
		break;
	case 's':
		savePicture();
		break;
	case 't':
		setCameraTarget();
		break;
	case 'u':   // test explosion
        explosion.setPosition(lander.getPosition());
        explosion.start();
        bLanderLoaded = false;
		break;
	case 'v':
		togglePointsDisplay();
		break;
	case 'V':
		break;
	case 'w':
		toggleWireframeMode();
		break;
	case OF_KEY_ALT:
		cam.enableMouseInput();
		bAltKeyDown = true;
		break;
	case OF_KEY_CONTROL:    // move down
        bCtrlKeyDown = true;
        physicSystem.addForce(new ThrustForce(5, ofVec3f(0, -1, 0)));
        exhaust.start();
		break;
    case ' ':   // move up
        physicSystem.addForce(new ThrustForce(5, ofVec3f(0, 1, 0)));
        exhaust.start();
        break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;
    case OF_KEY_UP: // move forward
        physicSystem.addForce(new ThrustForce(10, ofVec3f(1, 0, 0)));
        exhaust.start();
        break;
    case OF_KEY_DOWN: // move backward
        physicSystem.addForce(new ThrustForce(10, ofVec3f(-1, 0, 0)));
        exhaust.start();
        break;
    case OF_KEY_LEFT: // move left
        physicSystem.addForce(new ThrustForce(10, ofVec3f(0, 0, -1)));
        exhaust.start();
        break;
    case OF_KEY_RIGHT: // move right
        physicSystem.addForce(new ThrustForce(10, ofVec3f(0, 0, 1)));
        exhaust.start();
        break;
            
	default:
		break;
	}
}

void ofApp::toggleWireframeMode() {
	bWireframe = !bWireframe;
}

void ofApp::toggleSelectTerrain() {
	bTerrainSelected = !bTerrainSelected;
}

void ofApp::togglePointsDisplay() {
	bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key) {

	switch (key) {
	
	case OF_KEY_ALT:
		cam.disableMouseInput();
		bAltKeyDown = false;
		break;
	case OF_KEY_CONTROL:
        exhaust.stop();
		bCtrlKeyDown = false;
		break;
	case OF_KEY_SHIFT:
		break;
    case OF_KEY_UP:
    case OF_KEY_DOWN:
    case OF_KEY_RIGHT:
    case OF_KEY_LEFT:
    case ' ':
        exhaust.stop();
        break;
    case OF_KEY_F1: // main cam
        theCam = &topCam;
        break;
    case OF_KEY_F2: // side cam
        theCam = &sideCam;
        break;
    case OF_KEY_F3: // landerCam
        theCam = &landerCam;
        break;
    case OF_KEY_F4: // free cam
        theCam = &cam;
        break;
	default:
		break;

	}
}



//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	
}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	//
	if (cam.getMouseInputEnabled()) return;

	// if rover is loaded, test for selection
	//
	if (bLanderLoaded) {
		glm::vec3 origin = cam.getPosition();
		glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		bool hit = bounds.intersect(Ray(Vector3(origin.x, origin.y, origin.z), Vector3(mouseDir.x, mouseDir.y, mouseDir.z)), 0, 10000);
		if (hit) {
			bLanderSelected = true;
			mouseDownPos = getMousePointOnPlane(lander.getPosition(), cam.getZAxis());
			mouseLastPos = mouseDownPos;
			bInDrag = true;
		}
		else {
			bLanderSelected = false;
		}
	}
	else {
		ofVec3f p;
		raySelectWithOctree(p);
	}
}

bool ofApp::raySelectWithOctree(ofVec3f &pointRet) {
	ofVec3f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(mouse);
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
		Vector3(rayDir.x, rayDir.y, rayDir.z));

	pointSelected = octree.intersect(ray, octree.root, selectedNode);

	if (pointSelected) {
		pointRet = octree.mesh.getVertex(selectedNode.points[0]);
	}
	return pointSelected;
}

float ofApp::getAGL() {
    // make a ray from lander pointing downard
    Vector3 origin = Vector3(lander.getPosition().x, lander.getPosition().y, lander.getPosition().z);
    Vector3 dir = Vector3(0, -1, 0); //downward
    Ray r = Ray(origin, dir);
    
    ofVec3f pointRtn = lander.getPosition(); //in case error, return 0;
    bool isIntersect = octree.intersect(r, octree.root, intersectNode);
    if(isIntersect) {
        pointRtn = octree.mesh.getVertex(intersectNode.points[0]);
        ofVec3f v = pointRtn - lander.getPosition();
        return v.length();
    }
    return -1;
        
    
}


//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	//
	if (cam.getMouseInputEnabled()) return;

	if (bInDrag) {
        
		glm::vec3 landerPos = lander.getPosition();

		glm::vec3 mousePos = getMousePointOnPlane(landerPos, cam.getZAxis());
		glm::vec3 delta = mousePos - mouseLastPos;
        
        
		landerPos += delta;
		lander.setPosition(landerPos.x, landerPos.y, landerPos.z);
        exhaust.setPosition(landerPos);
        physicSystem.particles[0].position = landerPos;
        
		mouseLastPos = mousePos;

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		landerBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

		colBoxList.clear();
		octree.intersect(landerBounds, octree.root, colBoxList);
	
		if (octree.intersect(landerBounds, octree.root, colBoxList)) {
			cout << "overlap" << endl;
		}
		else {
			cout << "OK" << endl;
		}

	}
	else {
		ofVec3f p;
		raySelectWithOctree(p);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
    bLanderSelected = false;
	bInDrag = false;
}



// Set the camera to use the selected point as it's new target
//  
void ofApp::setCameraTarget() {

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
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials() {

	static float ambient[] =
	{ .5f, .5f, .5, 1.0f };
	static float diffuse[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float position[] =
	{5.0, 5.0, 5.0, 0.0 };

	static float lmodel_ambient[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float lmodel_twoside[] =
	{ GL_TRUE };


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);


	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
//	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
} 

void ofApp::savePicture() {
	ofImage picture;
	picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	picture.save("screenshot.png");
	cout << "picture saved" << endl;
}



bool ofApp::mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point) {
	ofVec2f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	return (rayIntersectPlane(rayPoint, rayDir, planePoint, planeNorm, point));
}


//  intersect the mouse ray with the plane normal to the camera 
//  return intersection point.   (package code above into function)
//
glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 planePt, glm::vec3 planeNorm) {
	// Setup our rays
	//
	glm::vec3 origin = cam.getPosition();
	glm::vec3 camAxis = cam.getZAxis();
	glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
	float distance;

	bool hit = glm::intersectRayPlane(origin, mouseDir, planePt, planeNorm, distance);

	if (hit) {
		// find the point of intersection on the plane using the distance 
		// We use the parameteric line or vector representation of a line to compute
		//
		// p' = p + s * dir;
		//
		glm::vec3 intersectPoint = origin + distance * mouseDir;

		return intersectPoint;
	}
	else return glm::vec3(0, 0, 0);
}
