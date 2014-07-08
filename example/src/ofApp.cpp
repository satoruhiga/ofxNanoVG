#include "ofMain.h"

#include "ofxNanoVG.h"

class ofApp : public ofBaseApp
{
public:
	ofxNanoVG::Canvas canvas;

	void setup()
	{
		ofSetVerticalSync(true);
		ofBackground(0);

		canvas.allocate(1280, 720);

		assert(canvas.loadFont("Roboto-Regular.ttf", "sans"));
	}

	void update()
	{
		ofxNanoVG::Canvas& c = canvas;

		c.begin();

		ofColor color;
		if (ofGetMousePressed())
			color.set(255, 0, 0);
		else
			color.set(255);

		c.fillColor(color);
		c.strokeColor(color);

		canvas.textFont("sans");

		c.textSize(140);
		c.textAlign(ofxNanoVG::TextAlign::LEFT | ofxNanoVG::TextAlign::TOP);
		string text = " TEXT";

		float X = ofGetMouseX();
		float Y = ofGetMouseY();

		ofRectangle r = c.textBounds(text, 0, 0, 400);

		c.translate(X, Y); // offset mouse pos
		c.rotate(ofGetElapsedTimef() * 10);
		c.translate(r.width * -0.5, r.height * -0.5); // offset text rectangle
		c.text(text, 0, 0, 400);

		r.scaleFromCenter(1.2);

		c.beginPath();
		{
			c.roundedRect(r, 30);
			c.lineWidth(5 + sin(fmodf(ofGetElapsedTimef() * 5, PI)) * 10);
		}
		c.strokePath();
		
		c.beginPath();
		{
			c.lineCap(ofxNanoVG::LineCap::ROUND);
			c.lineJoin(ofxNanoVG::LineCap::ROUND);
			
			c.lineWidth(20);
			
			float t = ofGetElapsedTimef() * 2;
			for (int i = 0; i < 64; i++)
			{
				float x = ofSignedNoise(1, 0, t + i * 0.02) * 500;
				float y = ofSignedNoise(0, 1, t + i * 0.02) * 500;
				
				if (i == 0) c.moveTo(x, y);
				else c.lineTo(x, y);
			}
		}
		c.strokePath();

		c.end();
	}

	void draw() { canvas.draw(0, 0); }

	void keyPressed(int key) {}

	void keyReleased(int key) {}

	void mouseMoved(int x, int y) {}

	void mouseDragged(int x, int y, int button) {}

	void mousePressed(int x, int y, int button) {}

	void mouseReleased(int x, int y, int button) {}

	void windowResized(int w, int h) {}
};

#include "ofAppGLFWWindow.h"
int main(int argc, const char** argv)
{
	ofAppGLFWWindow window;
	window.setNumSamples(0);
	ofSetupOpenGL(&window, 1280, 720, OF_WINDOW);
	ofRunApp(new ofApp);
	return 0;
}
