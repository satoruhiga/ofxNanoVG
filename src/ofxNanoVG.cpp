#include "ofxNanoVG.h"

#define NANOVG_GL2_IMPLEMENTATION
#include "nanovg_gl.h"

OFX_NANOVG_BEGIN_NAMESPACE

#pragma mark - FrameBuffer

class FrameBuffer
{
public:
	
	FrameBuffer(int w, int h)
	: width(w)
	, height(h)
	, framebuffer(0)
	, color(0)
	, stencil(0)
	{
		{
			glGenTextures(1, &color);
			
			glEnable(GL_TEXTURE_RECTANGLE);
			
			glBindTexture(GL_TEXTURE_RECTANGLE, color);
			glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER,
							GL_NEAREST);
			glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER,
							GL_NEAREST);
			glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S,
							GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T,
							GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, w, h, 0, GL_RGBA,
						 GL_UNSIGNED_BYTE, 0);
			glBindTexture(GL_TEXTURE_RECTANGLE, 0);
			
			glDisable(GL_TEXTURE_RECTANGLE);
			
			checkError();
		}
		
		{
			glGenRenderbuffers(1, &stencil);
			glBindRenderbuffer(GL_RENDERBUFFER, stencil);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX, w, h);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			
			checkError();
		}
		
		{
			glGenFramebuffers(1, &framebuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
									   GL_TEXTURE_RECTANGLE, color, 0);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
										  GL_RENDERBUFFER, stencil);
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			
			checkError();
		}
	}
	
	~FrameBuffer()
	{
		if (framebuffer)
		{
			glDeleteFramebuffers(1, &framebuffer);
			framebuffer = 0;
		}
		
		if (color)
		{
			glDeleteTextures(1, &color);
			color = 0;
		}
		
		if (stencil)
		{
			glDeleteRenderbuffers(1, &stencil);
			stencil = 0;
		}
	}
	
	void clear(float r, float g, float b, float a = 1)
	{
		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}
	
	void bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	}
	
	void unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	void draw(float x, float y, float w, float h)
	{
		glPushMatrix();
		{
			glTranslatef(x, y, 0);
			
			bool e = glIsEnabled(GL_TEXTURE_RECTANGLE);
			glEnable(GL_TEXTURE_RECTANGLE);
			
			glBindTexture(GL_TEXTURE_RECTANGLE, color);
			glBegin(GL_QUADS);
			glTexCoord2f(0, height);
			glVertex2f(0, 0);
			
			glTexCoord2f(width, height);
			glVertex2f(w, 0);
			
			glTexCoord2f(width, 0);
			glVertex2f(w, h);
			
			glTexCoord2f(0, 0);
			glVertex2f(0, h);
			glEnd();
			glBindTexture(GL_TEXTURE_RECTANGLE, 0);
			
			if (!e) glDisable(GL_TEXTURE_RECTANGLE);
		}
		glPopMatrix();
	}
	
public:
	
	float getWidth() const { return width; }
	float getHeight() const { return height; }
	
	GLuint getFrameBufferID() const { return framebuffer; }
	GLuint getColorID() const { return color; }
	GLuint getStencilID() const { return stencil; }
	
private:
	GLuint framebuffer;
	GLuint color;
	GLuint stencil;
	
	int width, height;
	
	void checkError()
	{
		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
		{
			cout << glewGetErrorString(err) << endl;
			throw;
		}
	}
};

#pragma mark - Paint

void LinearGradient::fill(Canvas& canvas) const
{
	NVGcontext* c = canvas.getContext();
	NVGpaint o = nvgLinearGradient(c, from.position.x, from.position.y, to.position.x, to.position.y, *(NVGcolor*)&from.color.r, *(NVGcolor*)&to.color.r);
	nvgFillPaint(c, o);
}

void LinearGradient::stroke(Canvas& canvas) const
{
	NVGcontext* c = canvas.getContext();
	NVGpaint o = nvgLinearGradient(c, from.position.x, from.position.y, to.position.x, to.position.y, *(NVGcolor*)&from.color.r, *(NVGcolor*)&to.color.r);
	nvgStrokePaint(c, o);
}

void BoxGradient::fill(Canvas& canvas) const
{
	NVGcontext* c = canvas.getContext();
	NVGpaint o = nvgBoxGradient(c, rect.x, rect.y, rect.width, rect.height, corner_radius, feather, *(NVGcolor*)&inner.r, *(NVGcolor*)&outer.r);
	nvgFillPaint(c, o);
}

void BoxGradient::stroke(Canvas& canvas) const
{
	NVGcontext* c = canvas.getContext();
	NVGpaint o = nvgBoxGradient(c, rect.x, rect.y, rect.width, rect.height, corner_radius, feather, *(NVGcolor*)&inner.r, *(NVGcolor*)&outer.r);
	nvgStrokePaint(c, o);
}

void RadialGradient::fill(Canvas& canvas) const
{
	NVGcontext* c = canvas.getContext();
	NVGpaint o = nvgRadialGradient(c, center.x, center.y, inner.radius, outer.radius, *(NVGcolor*)&inner.color.r, *(NVGcolor*)&outer.color.r);
	nvgFillPaint(c, o);
}

void RadialGradient::stroke(Canvas& canvas) const
{
	NVGcontext* c = canvas.getContext();
	NVGpaint o = nvgRadialGradient(c, center.x, center.y, inner.radius, outer.radius, *(NVGcolor*)&inner.color.r, *(NVGcolor*)&outer.color.r);
	nvgStrokePaint(c, o);
}

Image::~Image()
{}

void Image::upload(NVGcontext* ctx) const
{
	if (image) nvgDeleteImage(ctx, image);
	if (tex->getTextureData().textureTarget != GL_TEXTURE_2D)
		ofLogError("ofxNanoVG::Image") << "texture target should be GL_TEXTURE_2D";
		
	image = nvglCreateImageFromHandle(ctx, tex->getTextureData().textureID, tex->getWidth(), tex->getHeight(), flags);
}

void Image::fill(Canvas& canvas) const
{
	NVGcontext* c = canvas.getContext();
	upload(c);
	
	NVGpaint o = nvgImagePattern(c, rect.x, rect.y, rect.width, rect.height, angle, image, alpha);
	nvgFillPaint(c, o);
}

void Image::stroke(Canvas& canvas) const
{
	NVGcontext* c = canvas.getContext();
	upload(c);
	
	NVGpaint o = nvgImagePattern(c, rect.x, rect.y, rect.width, rect.height, angle, image, alpha);
	nvgStrokePaint(c, o);
}

#pragma mark - Canvas

void Canvas::allocate(int width, int height)
{
	this->width = width;
	this->height = height;

	release();
	
	vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
	
	FrameBuffer *o = new FrameBuffer(width, height);
	framebuffer = shared_ptr<FrameBuffer>(o);
	
	background_color.set(0, 0);
	
	framebuffer->bind();
	framebuffer->clear(background_color.r, background_color.g, background_color.b, background_color.a);
	framebuffer->unbind();
}

void Canvas::release()
{
	if (vg)
	{
		nvgDeleteGL2(vg);
		vg = NULL;
	}
	
	framebuffer.reset();
}

void Canvas::begin()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	ofPushView();
	ofViewport(0, ofGetViewportHeight() - height, width, height);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	
	framebuffer->bind();
	framebuffer->clear(background_color.r, background_color.g, background_color.b, background_color.a);

	nvgBeginFrame(vg, width, height, 1);
	
	resetState();
	
	textLineHeight(1);
	fillColor(ofColor(127));
	strokeColor(ofColor(127));
}

void Canvas::end()
{
	nvgEndFrame(vg);
	
	// {{{ quick fix for nanovg vbo unbind bug
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// }}}

	framebuffer->unbind();
	
	ofPopView();
	glPopAttrib();
}

void Canvas::draw(float x, float y, float w, float h)
{
	if (w == 0) w = width;
	if (h == 0) h = height;
	
	framebuffer->draw(x, y, w, h);
}

void Canvas::fillColor(const ofFloatColor& c)
{
	nvgFillColor(vg, nvgRGBAf(c.r, c.g, c.b, c.a));
}

void Canvas::strokeColor(const ofFloatColor& c)
{
	nvgStrokeColor(vg, nvgRGBAf(c.r, c.g, c.b, c.a));
}

void Canvas::lineWidth(float w)
{
	nvgStrokeWidth(vg, w);
}

void Canvas::lineCap(LineCap::Type type)
{
	nvgLineCap(vg, type);
}

void Canvas::lineJoin(LineCap::Type type)
{
	nvgLineJoin(vg, type);
}

//

void Canvas::resetState()
{
	nvgReset(vg);
}

void Canvas::push()
{
	nvgSave(vg);
}

void Canvas::pop()
{
	nvgRestore(vg);
}

void Canvas::identity()
{
	nvgResetTransform(vg);
}

//

void Canvas::beginPath()
{
	nvgBeginPath(vg);
}

void Canvas::closePath()
{
	nvgClosePath(vg);
}

void Canvas::fillPath()
{
	nvgFill(vg);
}

void Canvas::fillPath(const PaintStyle& paint)
{
	paint.fill(*this);
	nvgFill(vg);
}

void Canvas::strokePath()
{
	nvgStroke(vg);
}

void Canvas::strokePath(const PaintStyle& paint)
{
	paint.stroke(*this);
	nvgStroke(vg);
}

void Canvas::moveTo(float x, float y)
{
	nvgMoveTo(vg, x, y);
}

void Canvas::lineTo(float x, float y)
{
	nvgLineTo(vg, x, y);
}

void Canvas::bezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y)
{
	nvgBezierTo(vg, c1x, c1y, c2x, c2y, x, y);
}

void Canvas::arcTo(float x1, float y1, float x2, float y2, float radius)
{
	nvgArcTo(vg, x1, y1, x2, y2, radius);
}

//

void Canvas::arc(float cx, float cy, float r, float a0, float a1, int dir)
{
	nvgArc(vg, cx, cy, r, a0, a1, dir);
}

void Canvas::rect(float x, float y, float w, float h)
{
	nvgRect(vg, x, y, w, h);
}

void Canvas::rect(const ofRectangle& rect)
{
	nvgRect(vg, rect.x, rect.y, rect.width, rect.height);
}

void Canvas::roundedRect(float x, float y, float w, float h, float r)
{
	nvgRoundedRect(vg, x, y, w, h, r);
}

void Canvas::roundedRect(const ofRectangle& rect, float r)
{
	nvgRoundedRect(vg, rect.x, rect.y, rect.width, rect.height, r);
}

void Canvas::circle(float cx, float cy, float r)
{
	nvgEllipse(vg, cx, cy, r, r);
}

void Canvas::circle(const ofPoint& p, float r)
{
	nvgEllipse(vg, p.x, p.y, r, r);
}

void Canvas::ellipse(float cx, float cy, float rx, float ry)
{
	nvgEllipse(vg, cx, cy, rx, ry);
}

void Canvas::ellipse(const ofPoint& p, float rx, float ry)
{
	nvgEllipse(vg, p.x, p.y, rx, ry);
}

//

void Canvas::translate(float x, float y)
{
	nvgTranslate(vg, x, y);
}

void Canvas::rotate(float angle)
{
	nvgRotate(vg, ofDegToRad(angle));
}

void Canvas::scale(float x, float y)
{
	nvgScale(vg, x, y);
}

//

bool Canvas::loadFont(const string& path, const string& name)
{
	int n = nvgCreateFont(vg, name.c_str(), ofToDataPath(path).c_str());
	if (n == -1)
	{
		ofLogError("Canvas") << "font not found: " << path;
	}
	return n != -1;
}

bool Canvas::fontStyle(const FontStyle& font_style)
{
	if (!textFont(font_style.getName())) return false;
	textSize(font_style.getSize());
	textBlur(font_style.getBlur());
	textLetterSpaceing(font_style.getLetterSpaceing());
	textLineHeight(font_style.getLineHeight());
	textAlign(font_style.getAlign());
	return true;
}

bool Canvas::textFont(const string& name)
{
	if (nvgFindFont(vg, name.c_str()) == -1)
	{
		ofLogError("Canvas") << "font not loaded: " << name;
		return false;	
	}
	nvgFontFace(vg, name.c_str());
	return true;
}

void Canvas::text(const string& text, float x, float y, float line_break_width)
{
	if (line_break_width == 0)
	{
		nvgText(vg, x, y, text.c_str(), text.c_str() + text.size());
	}
	else
	{
		nvgTextBox(vg, x, y, line_break_width, text.c_str(), text.c_str() + text.size());
	}
}

ofRectangle Canvas::textBounds(const string& text, float x, float y, float line_break_width)
{
	float r[4];
	if (line_break_width == 0)
	{
		nvgTextBounds(vg, x, y, text.c_str(), text.c_str() + text.size(), r);
	}
	else
	{
		nvgTextBoxBounds(vg, x, y, line_break_width, text.c_str(), text.c_str() + text.size(), r);
	}
	return ofRectangle(r[0], r[1], r[2] - r[0], r[3] - r[1]);
}

void Canvas::textSize(float size)
{
	nvgFontSize(vg, size);
}

void Canvas::textBlur(float blur)
{
	nvgFontBlur(vg, blur);
}

void Canvas::textLetterSpaceing(float letter_spaceing)
{
	nvgTextLetterSpacing(vg, letter_spaceing);
}

void Canvas::textLineHeight(float line_height)
{
	nvgTextLineHeight(vg, line_height);
}

void Canvas::textAlign(int align)
{
	nvgTextAlign(vg, align);
}





OFX_NANOVG_END_NAMESPACE