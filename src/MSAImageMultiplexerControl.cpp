/*
 *  MSAImageMultiplexerControl.cpp
 *  UK Summer Games
 *
 *  Created by Mehmet Akten on 18/08/2010.
 *  Copyright 2010 MSA Visuals Ltd. All rights reserved.
 *
 */


#include "MSAImageMultiplexerControl.h"

namespace MSA {
	
	ImageMultiplexerControl::ImageMultiplexerControl(ImageMultiplexer *parent, string name) : GuiControl(name) {
		printf("----------------------------------------------\n");
		controlType		= "ImageMultiplexerControl";
		this->parent	= parent;
		setPos(0, 0);
		setSize(parent->getWidth(), 2 * parent->getHeight());
		printf("SIZE IS : %f %f\n", width, height);
		drawPos.set(0, 0);
		setup();
	}
	
	void ImageMultiplexerControl::loadFromXML(ofxXmlSettings &xml) {
		parent->loadFromXML(xml);
	}

	void ImageMultiplexerControl::saveToXML(ofxXmlSettings &xml) {
		parent->saveToXML(xml);
	}
	
	
	void ImageMultiplexerControl::onPress(int x, int y, int button) {
		Vec2i mousePos(x, y);
		
		parent->findActivePoint(mousePos - drawPos);
		
		if(parent->activeRegion >= 0) mouseOffset = mousePos - parent->getActiveRegion();
	}
	
	void ImageMultiplexerControl::onPressOutside(int x, int y, int button) {
		onPress(x, y, button);
	}

	
	void ImageMultiplexerControl::onRelease(int x, int y, int button) {
		parent->clearActive();
	}
	
	void ImageMultiplexerControl::onReleaseOutside(int x, int y, int button) {
		onRelease(x, y, button);
	}
	
	void ImageMultiplexerControl::onMouseMove(int x, int y) {
		Vec2i mousePos(x, y);
		
		parent->findActivePoint(mousePos - drawPos);
	}
	
	void ImageMultiplexerControl::onDragOver(int x, int y, int button) {
		Vec2i mousePos(x, y);
		
		parent->moveActivePointTo(mousePos - drawPos);
		parent->moveActiveRegionBy(mousePos - parent->getActiveRegion() - mouseOffset);
	}
	
	void ImageMultiplexerControl::onDragOutside(int x, int y, int button) {
		onDragOver(x, y, button);
	}
	
	
	void ImageMultiplexerControl::onKeyUp() {
		parent->moveActiveBy(Vec2i(0, -1));
	}
	
	void ImageMultiplexerControl::onKeyDown() {
		parent->moveActiveBy(Vec2i(0, 1));
	}
	
	void ImageMultiplexerControl::onKeyLeft() {
		parent->moveActiveBy(Vec2i(-1, 0));
	}

	void ImageMultiplexerControl::onKeyRight() {
		parent->moveActiveBy(Vec2i(1, 0));
	}
	
	void ImageMultiplexerControl::onKeyEnter() {
	}
	
	
	void ImageMultiplexerControl::draw(float x, float y) {
		setPos(x, y);
		drawPos.set(x, y);
		
		if(config->bAlignRight) {
			setSize(parent->getInputImage()->getWidth() * 2, parent->getHeight());
//			else bottomx += width - parent->getOutputImage()->getWidth().x);
		} else {
			setSize(parent->getWidth(), parent->getHeight());
		}
		

		parent->drawInput(x, y);
		parent->drawRegions(x, y);
		parent->drawHandles(x, y);
		parent->drawOutput(x, y + parent->getInputImage()->getHeight());
	}

}