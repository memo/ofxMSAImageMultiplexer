/*
 *  ImageMultiplexer.cpp
 *  UK Summer Games
 *
 *  Created by Memo Akten on 15/08/2010.
 *  Copyright 2010 MSA Visuals Ltd. All rights reserved.
 *
 */


#include "MSAImageMultiplexer.h"
#include "ofxSimpleGuiToo.h"
#include "MSAImageMultiplexerControl.h"

namespace MSA {
	
	ImageMultiplexer::ImageMultiplexer() {
		inputImage		= NULL;
		cvInputImage	= NULL;
		cvOutputImage	= NULL;
		regionsDirty	= false;

		control			= NULL;
	}
	
	void ImageMultiplexer::setup(ofBaseImage *inputImage, bool inputIsColor) {
		this->inputImage	= inputImage;
		this->inputIsColor	= inputIsColor;
		
		if(inputIsColor) {
			cvInputImage	= new ofxCvColorImage;
			cvOutputImage	= new ofxCvColorImage;
		} else {
			cvInputImage	= new ofxCvGrayscaleImage;
			cvOutputImage	= new ofxCvGrayscaleImage;
		}
		
		inputSize = Vec2f(inputImage->getWidth(), inputImage->getHeight());
		
		cvInputImage->allocate(inputSize.x, inputSize.y);
		cvOutputImage->allocate(inputSize.x, inputSize.y);
		
		DelPointer(control);
		control = new ImageMultiplexerControl(this, "ImageMultiplexer");
		
		gui.saveToXML();
		gui.addPage("IMAGE MULTIPLEXER").setXMLName("settings/ImageMultiplexer/ImageMultiplexer.xml");
		gui.addToggle("enabled", enabled);
		gui.addButton("doAddRegion", doAddRegion);
		gui.addButton("doDeleteRegion", doDeleteRegion);
		gui.addControl(*control);
		gui.loadFromXML();

		clearActive();
	}
	
	void ImageMultiplexer::addRegion(Vec2f p1, Vec2f p2) {
		printf("ImageMultiplexer::addRegion\n");
		points.push_back(Vec2i());
		points.push_back(Vec2i());
		setRegion(points.size()-2, p1, p2);
	}
	
	void ImageMultiplexer::setRegion(int i, Vec2f p1, Vec2f p2) {
		printf("ImageMultiplexer::setRegion\n");
		points[i].set(p1.x * inputSize.x, p1.y * inputSize.y);
		points[i+1].set(p2.x * inputSize.x, p2.y * inputSize.y);
		regionsDirty = true;
		updateRegions();
	}
	
	float ImageMultiplexer::getWidth() {
		return MAX(inputSize.x, cvOutputImage->getWidth());
	}
	
	float ImageMultiplexer::getHeight() {
		return inputSize.y + cvOutputImage->getHeight();
	}
	
	
	void ImageMultiplexer::update() {
		if(!enabled) return;
		
		if(doAddRegion) {
			doAddRegion = false;
			addRegion(Vec2f(0.25, 0.25), Vec2f(0.75, 0.75));
		}
		
		if(doDeleteRegion) {
			doDeleteRegion = false;
			points.pop_back();
			points.pop_back();
			regionsDirty = true;
		}

		updateRegions();
		
		cvInputImage->setFromPixels(inputImage->getPixels(), inputSize.x, inputSize.y);
		
		int startX = 0;
		Vec2f outputSize(0, 0);
		for(int i=0; i<points.size(); i+=2) {
			Vec2i regionSize(points[i+1].x - points[i].x + 1, points[i+1].y - points[i].y + 1);
			outputSize.y	= max(outputSize.y, (float)regionSize.y);
			
			cvInputImage->setROI(points[i].x, points[i].y, regionSize.x, regionSize.y);
			cvOutputImage->setROI(startX, 0, regionSize.x, regionSize.y);
			cvOutputImage->setRoiFromPixels(cvInputImage->getRoiPixels(), regionSize.x, regionSize.y);
			startX += regionSize.x;
		}
		
		cvInputImage->resetROI();
		cvOutputImage->resetROI();
		
		cvOutputImage->flagImageChanged();
	}
	
	void ImageMultiplexer::updateRegions() {
		if(points.size() == 0) addRegion(Vec2f(0, 0), Vec2f(1, 1));
		
		if(regionsDirty == false) return;
		
		Vec2i outputSize(0, 0);
		for(int i=0; i<points.size(); i+=2) {
			Vec2i &p1 = points[i];
			Vec2i &p2 = points[i+1];
			
			p1.x = ofClamp(p1.x, 0, inputSize.x-4);
			p1.y = ofClamp(p1.y, 0, inputSize.y-4);
			
			p2.x = ofClamp(p2.x, p1.x+2, inputSize.x-1);
			p2.y = ofClamp(p2.y, p1.y+2, inputSize.y-1);
			
			outputSize.x	+= p2.x - p1.x + 1;
			outputSize.y	= max(outputSize.y, p2.y - p1.y + 1);
		}
		
		cvOutputImage->allocate(outputSize.x, outputSize.y);
		cvOutputImage->set(0);
		printf("allocating cvOutputImage %i %i\n", outputSize.x, outputSize.y);
		
		regionsDirty = false;
	}
	
	void ImageMultiplexer::drawInput(float x, float y) {
		glColor3f(1, 1, 1);
		inputImage->draw(x, y);
	}
	
	void ImageMultiplexer::drawRegions(float x, float y) {
		glPushMatrix();
		glTranslatef(x, y, 0);
		
		glColor4f(1, 1, 1, 1);
		
		glLineWidth(2);
		
		ofNoFill();
		for(int i=0; i<points.size(); i++) {
			Vec2i &p1 = points[i];
			
			if(i%2 == 0) {
				if(i == activeRegion) {
					glColor3f(1, 0, 0);
				} else {
					glColor3f(0, 1, 0);
				}
				ofRect(p1.x, p1.y, (points[i+1].x - p1.x), (points[i+1].y - p1.y));
			}
		}
		
		glPopMatrix();
	}
	
	void ImageMultiplexer::drawHandles(float x, float y) {
		glPushMatrix();
		glTranslatef(x, y, 0);
		
		glColor4f(1, 1, 1, 1);
		
		glLineWidth(2);
		
		ofNoFill();
		for(int i=0; i<points.size(); i++) {
			Vec2i &p1 = points[i];
			
			if(i == activePoint) {
				glColor3f(1, 0, 0); 
				string s = "(" + ofToString(p1.x, 0) + ", " + ofToString(p1.y) + ")";
				if(i%2 == 1) s+= " (" + ofToString(p1.x - points[i-1].x, 0) + ", " + ofToString(p1.y - points[i-1].y) + ")";
				ofDrawBitmapString(s, p1.x + 10, p1.y - 5);
			} else {
				glColor3f(0, 1, 0);
			}
			ofCircle(p1.x, p1.y, 10);
		}
		
		glPopMatrix();
	}
	
	void ImageMultiplexer::drawOutput(float x, float y) {
		glColor3f(1, 1, 1);
		cvOutputImage->draw(x, y);//, w, h);
		
		//		ofNoFill();
		//		ofRect(x, y, cvOutputImage->getWidth(), cvOutputImage->getHeight());
		//		ofFill();
	}
	
	void ImageMultiplexer::draw(float x, float y) {
		drawInput(x, y);
		drawOutput(x, y + inputSize.y);
	}
	
	ofBaseImage *ImageMultiplexer::getInputImage() {
		return inputImage;
	}
	
	ofBaseImage *ImageMultiplexer::getOutputImage() {
		return enabled ? cvOutputImage : inputImage;
	}
	
	Vec2i &ImageMultiplexer::getActivePoint() {
		return points[activePoint];
	}
	
	Vec2i &ImageMultiplexer::getActiveRegion() {
		return points[activeRegion];
	}
	
	void ImageMultiplexer::clearActive() {
		activePoint		= -1;
		activeRegion	= -1;
		
	}
	
	void ImageMultiplexer::movePointTo(int i, Vec2i newPos) {
		points.at(i) = newPos;
		regionsDirty = true;
	}
	
	void ImageMultiplexer::movePointBy(int i, Vec2i diff) {
		points.at(i) += diff;
		regionsDirty = true;
	}
	
	
	void ImageMultiplexer::moveActivePointTo(Vec2i newPos) {
		if(activePoint >= 0) {
			movePointTo(activePoint, newPos);
		}
	}
	
	void ImageMultiplexer::moveActivePointBy(Vec2i diff) {
		if(activePoint >= 0) {
			movePointBy(activePoint, diff);
		}
	}
	
	
	void ImageMultiplexer::moveActiveRegionTo(Vec2i newPos) {
		if(activeRegion >=0) {
			Vec2i diff = newPos - points[activeRegion];
			moveActiveRegionBy(diff);
		}
	}

	void ImageMultiplexer::moveActiveRegionBy(Vec2i diff) {
		if(activeRegion >=0) {
			movePointBy(activeRegion, diff);
			movePointBy(activeRegion+1, diff);
		}
	}
	
	
	void ImageMultiplexer::moveActiveBy(Vec2i diff) {
		moveActivePointBy(diff);
		moveActiveRegionBy(diff);
	}

	void ImageMultiplexer::findActivePoint(Vec2i targetPos) {
		activePoint		= -1;
		activeRegion	= -1;
		
		for(int i=0; i<points.size(); i++) {
			if((targetPos-points[i]).lengthSquared()<100) {
				activePoint = i;
				return;
			}
		}
		
		for(int i=0; i<points.size(); i+=2) {
			Vec2i &p1 = points[i];
			Vec2i &p2 = points[i+1];
			
			if(ofInRange(targetPos.x, p1.x, p2.x) && ofInRange(targetPos.y, p1.y, p2.y)) {
				activeRegion = i;
				return;
			}
		}
	}
	
	void ImageMultiplexer::loadFromXML(ofxXmlSettings &xml) {
		xml.pushTag("points");
		
		int numPts = xml.getNumTags("pt");
		
		points.clear();
		
		for(int i=0; i<numPts; i+=2) {
			float x1  = xml.getValue("pt:x", 0.0f, i);
			float y1  = xml.getValue("pt:y", 0.0f, i);
			
			float x2  = xml.getValue("pt:x", 0.0f, i+1);
			float y2  = xml.getValue("pt:y", 0.0f, i+1);
			
			addRegion(Vec2f(x1, y1), Vec2f(x2, y2));
		}
		xml.popTag();

		regionsDirty = true;
		updateRegions();
	}
	
	void ImageMultiplexer::saveToXML(ofxXmlSettings &xml) {
		xml.addTag("points");
		xml.pushTag("points");
		for(int i=0; i<points.size(); i++) {
			xml.setValue("pt:x", points[i].x * 1.0f/inputSize.x, i);
			xml.setValue("pt:y", points[i].y * 1.0f/inputSize.y, i);
		}
		xml.popTag();
	}
	
}



