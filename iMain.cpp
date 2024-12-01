#include<bits/stdc++.h>
# include "iGraphics.h"
// # include "inc.h"
// # include "paddle.h"
using namespace std;

#define gp " "
#define DEBG

#define debug(n)
#define debugc(a)
#define debugcc(a)
#ifdef DEBG
#define debug(n) cout<<__LINE__<<gp<<#n<<gp<<n<<endl;
#define debugc(a) cout<<__LINE__<<gp<<#a<<gp<<'['<<gp;for(auto el:a){cout<<el<<gp;}cout<<']'<<endl;
#define debugcc(a) cout<<__LINE__<<gp<<#a<<gp<<'['<<gp;for(auto el:a){cout<<'{'<<gp<<el.ff<<','<<el.ss<<gp<<'}'<<gp;}cout<<']'<<endl;
#endif



const double PI = 3.14159;

#define screenwidth 1280
#define screenheight 760

// const double delTime1 = 20;
const double delTime1 = 15;

double frame = 0;


std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;
time_t currenttime;

int currentState=1;
int gamePlaying = 0;


struct Color{
	int r,g,b;
	void set(int red,int green,int blue){
		r = red;
		g = green;
		b = blue;

	}
	
};


vector<vector<int>> level2grid = 
{
	{1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1},
	{0,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,0},
	{0,0,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,0,0},
	{0,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,1,0,0,0}
};
vector<vector<int>> level1grid = 
{
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,3,3,3,1,3,0,3,1,3,3,3,1,3,3,3,1,1,1},
	{1,1,3,0,3,1,3,0,3,1,3,0,0,1,1,3,1,1,1,1},
	{1,1,3,3,3,1,3,0,3,1,3,3,3,1,1,3,1,1,1,1},
	{1,1,3,0,3,1,3,0,3,1,3,0,0,1,1,3,1,1,1,1},
	{1,1,3,3,3,1,3,3,3,1,3,3,3,1,1,3,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};



Color currentColor;

void mySetColor(Color col){
	iSetColor(col.r, col.g,col.b);
}

int x = 300, y = 300, r = 20;
/*
	function iDraw() is called again and again by the system.

	*/

int reward = 100;


struct Brick{
	int posX;
	int posY;
	int type;
	int width;
	int height;
	Color color1;//lower half
	Color color2;//upper half
	int health=1;
	void settype(){
		switch (health)
		{
		case 1:
			type = 1;
			break;
		
		case 2:
			type = 2;
			break;
		
		default:
			if(health>10)type = 3;
			break;
		}
	}
	void setColor(){
		switch (type)
		{
		case 1:
			color1 = {178,34,34};
			color2 = {210,105,30};
			break;
		
		case 2:
			color1 = {178,34,34};
			color2 = {210,105,30};
			break;
		
		case 3:
			color1 = {255, 223, 63};
			color2 = {218, 165, 32};
			break;
		
		default:
			break;
		}
	}
	

};

vector<Brick>currentBricks;
vector<Brick>::iterator hitbrick(vector<Brick>::iterator hittedbrick);


struct Paddle{
	double initposX = screenwidth/2;
	double posX = screenwidth/2;
	double initposY = screenheight * 0.1;
	double posY = screenheight * 0.1;

	double width = 150;
	double height = 15;
	Color color = {50,130,255};

	void setInitialPos(){
		posX = initposX;
		posY = initposY;
	}

	void Render(){
		double x = posX - width/2;
		double y = posY - height/2;
		mySetColor(color);
		iFilledRectangle(x,y,width, height);
		mySetColor(currentColor);
	}
	void UpdatePositon(double x){
		if(x + width/2 <=screenwidth && x-width/2 >=0)posX = x;
		else if(x+width/2 > screenwidth){
			posX = screenwidth - width/2;
		}else if(x-width/2 < 0){
			posX = width/2;
		}
	}


};


Paddle paddle;

struct Ball{

	double posX;
	double posY;
	double radius = 10;
	bool initSet = 0;
	// Color color = {255,102,0};
	// Color color = {0,255,128};
	Color color = {255,255,255};
	pair<double,double> velocity = {0,0};
	double velocityMag = 0;
	double velocityAngle = 0;
	bool launched = 0;
	
	// void setColor()
	
	void setInitialPos(){
		posX = paddle.posX;
		posY = paddle.posY + paddle.height/2+radius;
		
	}
	void setVelocity(double VMag, double VAngle){
		velocityMag = VMag;
		velocityAngle = VAngle;
		velocity.first = velocityMag * cos(velocityAngle);
		velocity.second = velocityMag * sin(velocityAngle);
	}
	void setVelocity_byxy(double velX, double velY){
		velocity.first = velX;
		velocity.second = velY;
		velocityMag = sqrt(velX*velX + velY*velY);
		velocityAngle = atan2(velY, velX);
		if(velocityAngle<0){
			velocityAngle += 2* PI;
		}

	}
	void checkScreen(){
		if(posX+radius>screenwidth){
			posX = screenwidth - radius;
			velocityAngle = PI - velocityAngle;
		}
		if(posX-radius<0){
			posX = 0 + radius;
			velocityAngle = PI - velocityAngle;
		}
		if(posY+radius>screenheight){
			posY = screenheight - radius;
			velocityAngle = -velocityAngle;
		}
		if(posY+radius<0){
			initSet = 0;
		}
		velocityAngle = -floor(velocityAngle/(2*PI))*2*PI + velocityAngle; 
	}

	void checkColWithPaddle(){
		if(!launched)return;
		if(posX>=paddle.posX-paddle.width/2 && posX<=paddle.posX+paddle.width/2){
			if( (posY-radius) < (paddle.posY+paddle.height/2) && (posY-radius) > (paddle.posY-paddle.height/2) ){
				velocityAngle = -floor(velocityAngle/(2*PI))*2*PI + velocityAngle;
				double velocityAngle2 = -velocityAngle;
				velocityAngle2 = -floor(velocityAngle2/(2*PI))*2*PI + velocityAngle2;
				velocityAngle2 += PI/2;
				velocityAngle2 = -floor(velocityAngle2/(2*PI))*2*PI + velocityAngle2;
				velocityAngle2 = -PI/2 + velocityAngle2;

				posY = paddle.posY+paddle.height/2+radius;

				double alpha = (45*PI/180) / (paddle.width/2) * (posX-paddle.posX);
				velocityAngle2 = velocityAngle2 - 2*alpha;
				velocityAngle2 = min(150*PI/180 , velocityAngle2);
				velocityAngle2 = max(30*PI/180 , velocityAngle2);
				
				velocityAngle = velocityAngle2;
				velocityAngle = -floor(velocityAngle/(2*PI))*2*PI + velocityAngle;
				// if(velocityAngle>=270)velocityAngle = PI/6;
				// if(velocityAngle<=270)velocityAngle = 5*PI/6;
			}
		}
		else if(posY>=paddle.posY-paddle.height/2 && posY<=paddle.posY+paddle.height/2){
			if( (posX+radius) >= (paddle.posX-paddle.width/2) && (posX+radius) <= (paddle.posX+paddle.width/2)){
				posX = paddle.posX-paddle.width/2-radius;
				velocityAngle = PI-velocityAngle;
			}else if( (posX-radius) <= (paddle.posX+paddle.width/2) && (posX-radius) >= (paddle.posX-paddle.width/2) ){
				posX = paddle.posX+paddle.width/2+radius;
				velocityAngle = PI-velocityAngle;
			}
		}

		velocityAngle = -floor(velocityAngle/(2*PI))*2*PI + velocityAngle; 
	}
	void checkColWithBrick(){
		for(auto brit = currentBricks.begin(); brit!=currentBricks.end(); ){
			//brick-left+ball-right
			if(posX+radius>=brit->posX && posX+radius<=brit->posX+20 && brit->posY - brit->height<=posY && posY<=brit->posY){
				velocity.first *= -1;
				setVelocity_byxy(velocity.first,velocity.second);
				posX = brit->posX-radius-1;
				brit = hitbrick(brit);
				continue;
			}
			//brick-right+ball-left
			if(posX-radius<=brit->posX+brit->width && posX-radius>=brit->posX+brit->width-20 && brit->posY - brit->height<=posY && posY<=brit->posY){
				velocity.first *= -1;
				setVelocity_byxy(velocity.first,velocity.second);
				posX = brit->posX + brit->width + radius+1;
				brit = hitbrick(brit);
				continue;
			}
			//brick-top+ball-bottom
			if(posY-radius<=brit->posY && posY-radius>=brit->posY-20 && brit->posX <=posX && posX<=brit->posX+brit->width){
				velocity.second *= -1;
				setVelocity_byxy(velocity.first,velocity.second);
				posY = brit->posY + radius+1;
				brit = hitbrick(brit);
				continue;
			}
			//brick-bottom+ball-top
			if(posY+radius>=brit->posY-brit->height && posY+radius<=brit->posY-brit->height+20 && brit->posX <=posX && posX<=brit->posX+brit->width){
				velocity.second *= -1;
				setVelocity_byxy(velocity.first,velocity.second);
				posY = brit->posY - brit->height - radius-1;
				brit = hitbrick(brit);
				continue;
			}


			//brick-bottom-left-corner
			if(posY+radius>=brit->posY-brit->height && posY<brit->posY-brit->height && brit->posX<=posX+radius && posX<brit->posX){
				if(velocity.first>0)velocity.first *= -1;
				if(velocity.second>0)velocity.second *= -1;
				setVelocity_byxy(velocity.first,velocity.second);
				posY = brit->posY - brit->height - radius;
				posX = brit->posX - radius;
				brit = hitbrick(brit);
				continue;
			}
			//brick-bottom-right-corner
			if(posY+radius>=brit->posY-brit->height && posY<brit->posY-brit->height && brit->posX+brit->width<posX && posX-radius<=brit->posX){
				if(velocity.first<0)velocity.first *= -1;
				if(velocity.second>0)velocity.second *= -1;
				setVelocity_byxy(velocity.first,velocity.second);
				posY = brit->posY - brit->height - radius;
				posX = brit->posX + brit->width + radius;
				brit = hitbrick(brit);
				continue;
			}
			//brick-upper-right-corner
			if(posY-radius<=brit->posY && posY>brit->posY && brit->posX+brit->width<posX && posX-radius<=brit->posX){
				if(velocity.first<0)velocity.first *= -1;
				if(velocity.second<0)velocity.second *= -1;
				setVelocity_byxy(velocity.first,velocity.second);
				posY = brit->posY + radius;
				posX = brit->posX + brit->width + radius;
				brit = hitbrick(brit);
				continue;
			}
			//brick-upper-left-corner
			if(posY-radius<=brit->posY && posY>brit->posY && brit->posX<=posX+radius && posX<brit->posX){
				if(velocity.first>0)velocity.first *= -1;
				if(velocity.second<0)velocity.second *= -1;
				setVelocity_byxy(velocity.first,velocity.second);
				posY = brit->posY + radius;
				posX = brit->posX - radius;
				brit = hitbrick(brit);
				continue;
			}


			brit++;
		}
	}

	void changePos(){
		if(launched){
			if(velocityMag == 0){
				velocityMag = 400;
				velocityAngle = PI/2;
			}
			
			setVelocity(velocityMag, velocityAngle);

			double deltime = 0;
			auto currentTime = std::chrono::high_resolution_clock::now();
			deltime = std::chrono::duration<float>(currentTime - lastTime).count();
			if(deltime > 0.3){
				deltime = 0;
			}
			// debug(currenttime - lasttime)
			lastTime = currentTime;
			// debug(deltime)
			
			posX += velocity.first * deltime;
			posY += velocity.second * deltime;
		}
	}
	
	void Render(){
		
		if(!launched){
			initSet = 0;
		}
		if(initSet == 0){
			setInitialPos();
			initSet = 1;
			launched = 0;
			// velocityMag = 0;
			velocityAngle = PI/2;

		}
		
		
		// changePos();
		checkScreen();
		checkColWithPaddle();
		checkColWithBrick();

		double x = posX;
		double y = posY;

		mySetColor(color);
		iFilledCircle(x,y,radius);
		mySetColor(currentColor);
	}
};



Ball ball;

int numberOfInvincible = 0;


void initBrickgrid(int level){
	currentBricks.clear();
	numberOfInvincible = 0;
	int brickwidth = 60;
	int brickheight = 25;
	int brickgap = 2;
	int startheight = 760-100;
	int startwidth = 0;
	int startrow = startheight;
	vector<vector<int>> grid;

	switch (level)
	{
	case 1:
		grid = level1grid;
		break;
	case 2:
		grid = level2grid;
		break;
	
	default:
		break;
	}
	for(auto row:grid){
		int startcol = 0;
		for(auto col:row){
			Brick temp;
			switch (col)
			{
			case 1:
				temp.health = 1;
				break;
			case 2:
				temp.health = 2;
				break;
			case 3:
				temp.health = 1e9;
				numberOfInvincible++;
				break;
			
			default:
				break;
			}
			if(col){
				temp.settype();
				temp.setColor();
				temp.posX = startcol+brickgap;
				temp.posY = startrow-brickgap;
				temp.type = col;
				temp.width = brickwidth;
				temp.height = brickheight;
				currentBricks.push_back(temp);
			}
			
			startcol += (brickgap*2 + brickwidth);
		}
		startrow -= (brickgap*2 + brickheight);
	}
}

void brickgridRender(){
	for(auto brick:currentBricks){
		mySetColor(brick.color1);
		double xa1[] = {brick.posX, brick.posX+brick.width, brick.posX};
		double ya1[] = {brick.posY, brick.posY-brick.height, brick.posY-brick.height};
		iFilledPolygon(xa1, ya1, 3);

		mySetColor(brick.color2);
		double xa2[] = {brick.posX, brick.posX+brick.width, brick.posX+brick.width};
		double ya2[] = {brick.posY, brick.posY-brick.height, brick.posY};
		iFilledPolygon(xa2, ya2, 3);
		mySetColor(currentColor);
	}
}

vector<Brick>::iterator hitbrick(vector<Brick>::iterator hittedbrick){
	hittedbrick->health--;

	if(hittedbrick->health==0)return currentBricks.erase(hittedbrick);
	hittedbrick->settype();
	hittedbrick->setColor();
	++hittedbrick;
	return hittedbrick;
}


void levelinit(){
	Paddle paddletemp;
	paddle = paddletemp;
	Ball balltemp;
	ball = balltemp;
}


void renderLevel(int level){
	switch (level)
	{
	case 0:
		
		break;
	case 1:
		iShowBMP(0,0,"sprites/Backgrounds/aiback2.bmp");
		if(!gamePlaying){
			initBrickgrid(level);
			levelinit();
			gamePlaying = 1;
		}
		else if(currentBricks.size()-numberOfInvincible==0){
			gamePlaying = 0;
			currentState++;
			
		}
		paddle.Render();
		ball.Render();
		brickgridRender();
		break;
	case 2:
		iShowBMP(0,0,"sprites/Backgrounds/aiback.bmp");
		if(!gamePlaying){
			initBrickgrid(level);
			levelinit();
			gamePlaying = 1;
		}
		else if(currentBricks.size()-numberOfInvincible==0){
			gamePlaying = 0;
			
		}
		paddle.Render();
		ball.Render();
		brickgridRender();
		break;
	
	default:
		break;
	}
	
	
}


void iDraw() {
	frame++;
	//place your drawing codes here
	iClear();

	
	iSetColor(20, 200, 200);
	iFilledCircle(x, y, 50);
	
	//iFilledRectangle(10, 30, 20, 20);
	
	iSetColor(20, 200, 0);
	
	iText(40, 40, "Hi, I am iGraphics");
	iShowBMP2(10, 20, "sprites/ballspr/ball2.bmp",0);
	// iRectangle(100, 100, 100, 20);

	currentColor.set(20,200,0);

	//Level 1
	renderLevel(currentState);
	
}

/*
	function iMouseMove() is called when the user presses and drags the mouse.
	(mx, my) is the position where the mouse pointer is.
	*/
void iMouseMove(int mx, int my) {
	// printf("x = %d, y= %d\n",mx,my);
	//place your codes here

}

void iPassiveMouseMove(int mx, int my){
	paddle.UpdatePositon(mx);

}

/*
	function iMouse() is called when the user presses/releases the mouse.
	(mx, my) is the position where the mouse pointer is.
	*/
void iMouse(int button, int state, int mx, int my) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		//place your codes here
		//	printf("x = %d, y= %d\n",mx,my);
		// x += 10;
		// y += 10;

		ball.launched = 1;
		
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		//place your codes here
		x -= 10;
		y -= 10;
	}
}

/*
	function iKeyboard() is called whenever the user hits a key in keyboard.
	key- holds the ASCII value of the key pressed.
	*/
void iKeyboard(unsigned char key) {
	if (key == 'q') {
		exit(0);
	}
	//place your codes for other keys here
}

/*
	function iSpecialKeyboard() is called whenver user hits special keys like-
	function keys, home, end, pg up, pg down, arraows etc. you have to use
	appropriate constants to detect them. A list is:
	GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6,
	GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11, GLUT_KEY_F12,
	GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN, GLUT_KEY_PAGE UP,
	GLUT_KEY_PAGE DOWN, GLUT_KEY_HOME, GLUT_KEY_END, GLUT_KEY_INSERT
	*/
void iSpecialKeyboard(unsigned char key) {

	if (key == GLUT_KEY_END) {
		exit(0);
	}
	//place your codes for other keys here
}
int i = 0;
void myfunc(){
	
	if(frame!=0){
		if(i==0){
			i++;
			frame = 0;
			return;
		}
		cout<<frame<<endl;
		exit(0);
	}
	
}

void SceneUpdater(){
	if(ball.launched){
		ball.checkScreen();
		ball.checkColWithPaddle();
		ball.checkColWithBrick();
		ball.changePos();
	}
}


void GameManager(){
	
}


int main() {
	//place your own initialization codes here.
	// iSetTimer(1000, myfunc);
	iSetTimer(delTime1, SceneUpdater);
	lastTime = std::chrono::high_resolution_clock::now();
	currentColor.set(255,255,255);
	
	iInitialize(1280, 760, "DX-Ball");

	GameManager();
	return 0;
}
