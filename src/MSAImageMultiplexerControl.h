/*
 *  MSAImageMultiplexerControl.h
 *  UK Summer Games
 *
 *  Created by Mehmet Akten on 18/08/2010.
 *  Copyright 2010 MSA Visuals Ltd. All rights reserved.
 *
 */

#pragma once

#include "MSAImageMultiplexer.h"
#include "MSAGuiControl.h"

namespace MSA {
	
	class ImageMultiplexerControl : public GuiControl {
	public:
		
		ImageMultiplexer *parent;
		Vec2i			drawPos;
		Vec2i			mouseOffset;
		
		ImageMultiplexerControl(ImageMultiplexer *parent, string name);
		
		void loadFromXML(ofxXmlSettings &xml);
		void saveToXML(ofxXmlSettings &xml);
	
		void onPress(int x, int y, int button);
		void onPressOutside(int x, int y, int button);
		void onRelease(int x, int y, int button);
		void onReleaseOutside(int x, int y, int button);
		void onMouseMove(int x, int y);
		void onDragOver(int x, int y, int button);
		void onDragOutside(int x, int y, int button);
		
		void onKeyUp();
		void onKeyDown();
		void onKeyLeft();
		void onKeyRight();
		void onKeyEnter();
		
		void draw(float x, float y);
	};
	
}