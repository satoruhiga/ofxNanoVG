#pragma once

#include "ofMain.h"

#include "nanovg.h"

#define OFX_NANOVG_BEGIN_NAMESPACE namespace ofx { namespace NanoVG {
#define OFX_NANOVG_END_NAMESPACE } }

OFX_NANOVG_BEGIN_NAMESPACE

class FrameBuffer;

struct TextAlign {
	enum {
		LEFT = NVG_ALIGN_LEFT,
		CENTER = NVG_ALIGN_CENTER,
		RIGHT = NVG_ALIGN_RIGHT,
		TOP = NVG_ALIGN_TOP,
		MIDDLE = NVG_ALIGN_MIDDLE,
		BOTTOM = NVG_ALIGN_BOTTOM,
		BASELINE = NVG_ALIGN_BASELINE
	};
};

struct LineCap
{
	enum Type {
		BUTT = NVG_BUTT,
		ROUND = NVG_ROUND,
		SQUARE = NVG_SQUARE,
		BEVEL = NVG_BEVEL,
		MITER = NVG_MITER
	};
};

class FontStyle
{
	string name;
	float size;
	float blur;
	float letter_spaceing;
	float line_height;
	int align;
	
public:
	
	FontStyle(const string& name = "",
			  float size = 14,
			  float blur = 0,
			  float letter_spaceing = 0,
			  float line_height = 0,
			  int align = TextAlign::TOP | TextAlign::LEFT)
	: name(name)
	, size(size)
	, blur(blur)
	, letter_spaceing(letter_spaceing)
	, line_height(line_height)
	, align(align) {}
	
	void setFace(const string& name) { this->name = name; }
	const string& getName() const { return name; }
	
	void setSize(float size) { this->size = size; }
	float getSize() const { return size; }
	
	void setBlur(float blur) { this->blur = blur; }
	float getBlur() const { return blur; }
	
	void setLetterSpaceing(float letter_spaceing) { this->letter_spaceing = letter_spaceing; }
	float getLetterSpaceing() const { return letter_spaceing; }
	
	void setLineHeight(float line_height) { this->line_height = line_height; }
	float getLineHeight() const { return line_height; }
	
	void setAligh(int align) { this->align = align; }
	int getAlign() const { return align; }
};

class Canvas
{
public:
	
	Canvas() : vg(NULL) {}
	
	void allocate(int width, int height);
	
	void resetState();
	
	void begin();
	void end();
	
public:
	
	// draw commands

	void background(float r, float g, float b, float a = 1) { background_color.set(r, g, b, a); }
	
	void fillColor(const ofFloatColor& c);
	void strokeColor(const ofFloatColor& c);
	
	void lineWidth(float w);
	void lineCap(LineCap::Type type);
	void lineJoin(LineCap::Type type);
	
	// path
	
	void beginPath();
	void closePath();
	void fillPath();
	void strokePath();
	

	void arc(float cx, float cy, float r, float a0, float a1, int dir);
	
	void rect(float x, float y, float w, float h);
	void rect(const ofRectangle& rect);
	
	void roundedRect(float x, float y, float w, float h, float r);
	void roundedRect(const ofRectangle& rect, float r);
	
	void circle(float cx, float cy, float r);
	void circle(const ofPoint& p, float r);
	
	void ellipse(float cx, float cy, float rx, float ry);
	void ellipse(const ofPoint& p, float rx, float ry);
	
	void moveTo(float x, float y);
	void lineTo(float x, float y);
	void bezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y);
	void arcTo(float x1, float y1, float x2, float y2, float radius);
	
	// text
	
	bool loadFont(const string& path, const string& name);
	bool textFont(const string& name);
	
	void text(const string& text, float x, float y, float line_break_width = 0);
	ofRectangle textBounds(const string& text, float x, float y, float line_break_width = 0);
	
	void textSize(float size);
	void textBlur(float blur);
	void textLetterSpaceing(float letter_spaceing);
	void textLineHeight(float line_height);
	void textAlign(int align);
	
	bool fontStyle(const FontStyle& font_style);
	
	// transform
	
	void push();
	void pop();
	
	void identity();
	
	void translate(float x, float y);
	void rotate(float angle);
	void scale(float x, float y);

public:
	
	float getWidth() const { return width; }
	float getHeight() const { return height; }
	
	void draw(float x, float y, float w = 0, float h = 0);
	
private:
	
	struct NVGcontext* vg;
	
	float width, height;
	shared_ptr<FrameBuffer> framebuffer;
	
	ofFloatColor background_color;
	
	void release();
};

OFX_NANOVG_END_NAMESPACE

namespace ofxNanoVG = ofx::NanoVG;