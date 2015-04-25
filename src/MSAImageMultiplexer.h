/*
 *  ImageMultiplexer.h
 *  UK Summer Games
 *
 *  Created by Memo Akten on 15/08/2010.
 *  Copyright 2010 MSA Visuals Ltd. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "MSACore.h"
#include "ofxXmlSettings.h"
#include "MSAGuiControl.h"

namespace MSA {
	
	class ImageMultiplexer {
	public:
		bool enabled;
		
		ImageMultiplexer();
		
		void setup(ofBaseImage *inputImage, bool inputIsColor);
		void setRegion(int i, Vec2f p1, Vec2f p2);	// takes normalized input, i: index of first point
		void addRegion(Vec2f p1, Vec2f p2);	// takes normalized input
		
		void update();
		
		void drawInput(float x, float y);
		void drawOutput(float x, float y);
		void drawRegions(float x, float y);
		void drawHandles(float x, float y);
		void draw(float x, float y);
		
		void loadFromXML(ofxXmlSettings &xml);
		void saveToXML(ofxXmlSettings &xml);
		
		float getWidth();
		float getHeight();
		
		Vec2i			&getActivePoint();
		Vec2i			&getActiveRegion();		// returns top left of region
		void			clearActive();

		ofBaseImage		*getInputImage();
		ofBaseImage		*getOutputImage();
		
		void			findActivePoint(Vec2i targetPos);
		
		void			movePointTo(int i, Vec2i newPos);
		void			movePointBy(int i, Vec2i diff);
		
		void			moveActivePointTo(Vec2i newPos);
		void			moveActivePointBy(Vec2i diff);
		
		void			moveActiveRegionTo(Vec2i newPos);
		void			moveActiveRegionBy(Vec2i diff);
			
		void			moveActiveBy(Vec2i diff);
		
		int				activePoint;
		int				activeRegion;
		
	protected:
		ofBaseImage		*inputImage;
		bool			inputIsColor;
		
		Vec2i			inputSize;
		ofxCvImage		*cvInputImage;
		ofxCvImage		*cvOutputImage;
		
		bool			regionsDirty;
		
		bool			doAddRegion;
		bool			doDeleteRegion;
		
		vector<Vec2i>	points;
		
		GuiControl *control;
		
		void updateRegions();
		
		void moveActive(Vec2i offset);
	};
}