#include<bits/stdc++.h>
#include<cstdlib>
#include <windows.h>
# include "iGraphics.h"

using namespace std;

#define gp " "
#define DEBG

#define debug(n)
#define debugc(a)
#define debugcc(a)
#ifdef DEBG
#define debug(n) cout<<__LINE__<<gp<<#n<<gp<<n<<'\n';
#define debugc(a) cout<<__LINE__<<gp<<#a<<gp<<'['<<gp;for(auto el:a){cout<<el<<gp;}cout<<']'<<'\n';
#define debugcc(a) cout<<__LINE__<<gp<<#a<<gp<<'['<<gp;for(auto el:a){cout<<'{'<<gp<<el.first<<','<<el.second<<gp<<'}'<<gp;}cout<<']'<<'\n';
#endif



const double PI = 3.14159;

#define screenwidth 1280
#define screenheight 760
#define basePaddleWidth 150
#define basePaddleHeight 15
#define baseBallVelocity 400

const double delTime1 = 15;

double frame = 0;


std::chrono::time_point<std::chrono::high_resolution_clock> lastTimeGlobal;

int currentState=0;
int gamePlaying = 0;
int won = 0;
int lives = 3;
int dead = 0;
int score = 0;
int reward = 100;
double factor = 1;
int numberOfLeaders = 3;
string playerName = "";
int maxPlayerNameLength = 6;
int soundOn = 1;
int backgroundOn = 1;
int musicOn = 1;
int musicPlaying = 0;

vector<char*>gameplaybackground={
	"", // for 1-based indexing
	"sprites/Backgrounds/aiback.bmp",
	"sprites/Backgrounds/aiback.bmp",
	"sprites/Backgrounds/aiback2.bmp",
};
vector<char*>gamemenubackground={
	"sprites/Backgrounds/menuback.bmp"
};
vector<char*>menusprites={
	"sprites/Menuspr/Menu.bmp",
	"sprites/Menuspr/Levels.bmp",
	"sprites/Menuspr/Levelcomplete.bmp",
	"sprites/Menuspr/GameOver.bmp",
	"sprites/Menuspr/AboutPage.bmp",
	"sprites/Menuspr/Leaderboard.bmp"
};
vector<char*>iconsprites={
	"sprites/Menuspr/Quit2.bmp"
};
vector<wstring>soundEffects={
	L"music/FireballHit2.wav",
	L"music/metalhit.wav"
};


vector<pair<int,string>> datatext[10];


vector<int>powerUpWeight = {10,10,10,10,10,10,10};

struct Color{
	int r,g,b;
	void set(int red,int green,int blue){
		r = red;
		g = green;
		b = blue;
	}	
};
Color currentColor;

void mySetColor(Color col, double alpha = 1){
	iSetColor(col.r,col.g,col.b, alpha);
}

int randomgen(int lo, int hi){
	// srand(time(0));
	int ans = rand()%(hi-lo+1) + lo;
	return ans;
}

int randomPowerUpGen() {
    int sum = accumulate(powerUpWeight.begin(), powerUpWeight.end(), 0);
    
    int ran = randomgen(0, sum - 1);
    
    int sz = powerUpWeight.size();
    for (int i = 0; i < sz; i++) {
        if (ran < powerUpWeight[i]) {
            return i+1; 
        }
        ran -= powerUpWeight[i];
	}
    throw std::runtime_error("Logic error in randomPowerUpGen");
}

void readData(vector<pair<int, string>>& v, int mode) {
	FILE* datacontent;
	if(mode==1) datacontent = fopen("./GameData/easyLd.txt", "r"); 
	if(mode==2) datacontent = fopen("./GameData/mediumLd.txt", "r"); 
	if(mode==3) datacontent = fopen("./GameData/hardLd.txt", "r"); 
	if (!datacontent) {
        cerr << "Error opening file './GameData/easyLd.txt'" << endl;
        return;
    }

    char buffer[100];
    v.resize(numberOfLeaders);
    for (int i = 0; i < numberOfLeaders; i++) {
        if (fscanf(datacontent, "%s %d", buffer, &v[i].first) == 2) {
            v[i].second = string(buffer); 
        } else {
            cerr << "Error reading data from file!" << endl;
            exit(1); 
        }
    }
	fclose(datacontent);
	sort(v.begin(), v.end(), greater<pair<int,string>>());
}

void updateLocalData(vector<pair<int,string>>&v, pair<int,string>newPlayer){
	v.push_back(newPlayer);
	sort(v.begin(),v.end(),greater<pair<int,string>>());
	while(v.size()>numberOfLeaders)v.erase(--v.end());
}

void writeData(vector<pair<int,string>>&v, int mode){
	FILE* datacontent;
	if(mode==1) datacontent = fopen("./GameData/easyLd.txt", "w"); 
	if(mode==2) datacontent = fopen("./GameData/mediumLd.txt", "w"); 
	if(mode==3) datacontent = fopen("./GameData/hardLd.txt", "w");
	if (!datacontent) {
        cerr << "Error opening file './GameData/easyLd.txt'" << endl;
        return;
    }
    sort(v.begin(), v.end(),greater<pair<int,string>>());
    for (int i = 0; i < numberOfLeaders; i++) {
		fprintf(datacontent, "%s %d\n", v[i].second.c_str(), v[i].first);
    }
	fclose(datacontent);
}

vector<vector<int>> level1grid = 
{
	{1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1},
	{0,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,0},
	{0,0,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,0,0},
	{0,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,1,0,0,0}
};
vector<vector<int>> level2grid = 
{
	{0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0},
	{0,0,0,0,1,1,2,2,1,1,1,2,2,1,1,0,0,0,0,0},
	{0,0,0,1,1,2,2,2,1,1,1,2,2,2,1,1,0,0,0,0},
	{0,0,1,1,2,2,2,2,0,0,0,2,2,2,2,1,1,0,0,0},
	{0,1,1,2,2,1,1,0,0,0,0,0,1,1,2,2,1,1,0,0},
	{0,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0}
};

vector<vector<int>> level3grid = 
{
	{1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2},
	{1,1,3,3,3,1,3,0,3,1,3,3,3,1,3,3,3,1,1,1},
	{1,1,3,0,3,1,3,0,3,1,3,0,0,1,1,3,1,1,1,1},
	{1,1,3,3,3,1,3,0,3,1,3,3,3,1,1,3,1,1,1,1},
	{1,1,3,0,3,1,3,0,3,1,3,0,0,1,1,3,1,1,1,1},
	{1,1,3,3,3,1,3,3,3,1,3,3,3,1,1,3,1,1,1,1},
	{2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1}
};










struct Brick{
	double posX;
	double posY;
	int row;
	int col;
	int type=1;
	double width;
	double height;
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
			color1 = {178, 34, 34};
			color2 = {205, 92, 92};
			break;
		
		case 2:
			color1 = {184, 135, 81};
			color2 = {222, 194, 155};
			break;
		
		case 3:
			color1 = {255, 215, 0};
			color2 = {218, 165, 32};
			break;
		
		default:
			break;
		}
	}
	

};

vector<Brick>currentBricks;
map<pair<int,int>, Brick*>gridToBrickMap;
vector<Brick>::iterator hitbrick(vector<Brick>::iterator& hittedbrick);


struct Paddle{
	double initposX = screenwidth/2;
	double posX = screenwidth/2;
	double initposY = screenheight * 0.1;
	double posY = screenheight * 0.1;

	double width = basePaddleWidth;
	double height = basePaddleHeight;
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


int cc = 0;
struct Ball{

	double posX;
	double posY;
	double radius = 10;
	bool initSet = 0;
	Color colorFire = {255,200,0};
	// Color color = {0,255,128};
	Color colorBase = {255,255,255};
	Color color = colorBase;
	pair<double,double> velocity = {0,0};
	double velocityMag = 0;
	double velocityAngle = 0;
	bool launched = 0;
	int type = 1;
	
	void setColor(){
		if(type==0){
			color = colorBase;
		}else{
			color = colorFire;
		}
	}
	
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
			Paddle paddletemp;
			paddle = paddletemp;
			initSet = 0;
			dead = 1;
		}
		velocityAngle = -floor(velocityAngle/(2*PI))*2*PI + velocityAngle; 
	}

	void checkColWithPaddle(){
		if(!launched)return;
		if(posX>=paddle.posX-paddle.width/2 && posX<=paddle.posX+paddle.width/2){
			if( (posY-radius) < (paddle.posY+paddle.height/2) && (posY-radius) > (paddle.posY-paddle.height*2) ){
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
			if(cc){
				debug("check")
				
			}
			if(brit->health<=0){
				// debug(brit->health)
				brit++;continue;
			}
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
				// debug("hit")
				// debug(currentBricks.size())
				// cc= 5;
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

			if(cc)debug("check")
			brit++;
			// if(cc){
			// 	debug(brit->row)
			// 	debug(brit->col)
				
			// }
		}
		int iii = 0;
		for(auto it=currentBricks.begin(); it!=currentBricks.end();){
			iii++;
			auto itt = it;
			it++;
			if(cc)debug(iii)
			if(itt->health<=0){
				it = currentBricks.erase(itt);
			}else{
				itt->settype();
				itt->setColor();
			}
				
		}
		cc = 0;
		// if(cc)debug("checkout")
	}

	void changePos(double deltime){
		if(launched){
			if(velocityMag == 0){
				velocityMag = baseBallVelocity;
				type = 0;
				velocityAngle = PI/2;
			}
			
			setVelocity(velocityMag, velocityAngle);
			
			
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
			type = 0;
			initSet = 1;
			launched = 0;
			velocityMag = 0;
			velocityAngle = PI/2;
			if(dead){
				factor = 1;
				lives--;
				score/=2;
				dead = 0;
			}

		}
		
		
		// changePos();
		setColor();
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


void doPowerUp(int type){
	switch (type)
	{
	case 1: //Shrink paddle
		{double temp = paddle.width;
		paddle.width=max(paddle.width/1.5, basePaddleWidth/2.25);
		if(abs(temp - paddle.width)>=1e-8){
			factor*=1.5;
		}
		break;}
	case 2: //Expand paddle
		{double temp = paddle.width;
		paddle.width=min(paddle.width*1.5, basePaddleWidth*2.25);
		if(abs(temp - paddle.width)>=1e-8){
			factor/=1.5;
			
		}
		break;}
	case 3:  //Fire Ball
		ball.type=1;
		break;
	case 4:  //Normal Ball
		ball.type=0;
		break;
	case 5:  //Fast Ball
		{double temp = ball.velocityMag;
		ball.velocityMag=min(baseBallVelocity*1.5, ball.velocityMag*2.25);
		if(abs(temp - paddle.width)>=1e-8){
			factor*=1.5;	
		}
		break;}
	case 6:  //Slow Ball
		{double temp = ball.velocityMag;
		ball.velocityMag=max(baseBallVelocity/1.5, ball.velocityMag/2.25);
		if(abs(temp - paddle.width)>=1e-8){
			factor/=1.5;	
		}
		break;}
	case 7:  //kill Ball
		{Paddle paddletemp;
		paddle = paddletemp;
		Ball balltemp;
		ball = balltemp;
		dead = 1;
		ball.initSet = 0;
		break;}

	default:
		break;
	}
}

struct PowerUp{
	double posX;
	double posY;
	int type = 5;
	int isDestroyed = 0;
	double width = 50;
	double height = 20;
	double velY =100; 
	Color color;

	void setColor(){
		switch (type)
		{
		case 1:
			color = {45, 45, 225};
			break;
		case 2:
			color = {25, 250, 25};
			break;
		case 3:
			color = {255, 105, 0};
			break;
		case 4:
			color = {255, 255, 255};
			break;
		case 5:
			color = {239, 255, 58};
			break;
		case 6:
			color = {153, 102, 255};
			break;
		case 7:
			color = {255, 0, 0};
			break;
		
		default:
			break;
		}
	}

	void checkColWithPaddle(){
		if(posY <= paddle.posY+paddle.height/2 && posY+height>=paddle.posY-paddle.height/2 && paddle.posX-paddle.width/2<=posX+width-10 && posX+10<=paddle.posX+paddle.width/2){
			doPowerUp(type);
			isDestroyed = 1;
		}
	}
	void changePos(double deltime){
		checkColWithPaddle();
		posY -= velY * deltime;
		if(posY < 0-height){
			isDestroyed = 1;
		}
	}
};
vector<PowerUp>powerups;

void powerupsRender(){
	for(auto it = powerups.begin(); it!=powerups.end();){
		if(it->isDestroyed){
			it = powerups.erase(it);
			continue;
		}
		mySetColor(it->color);
		iFilledRectangle(it->posX, it->posY, it->width, it->height);
		it++;
	}
}


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
	case 3:
		grid = level3grid;
		break;
	
	default:
		break;
	}

	int i = 0, j = 0;
	for(auto row:grid){
		int startcol = 0;
		j = 0;
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
				temp.row = i;
				temp.col = j;
				temp.type = col;
				temp.width = brickwidth;
				temp.height = brickheight;
				currentBricks.push_back(temp);
				
			}
			
			startcol += (brickgap*2 + brickwidth);
			j++;
		}
		startrow -= (brickgap*2 + brickheight);
		i++;
	}
	
}

void brickgridRender(){
	for(auto it= currentBricks.begin(); it!=currentBricks.end();it++){
		auto brick = *it;
		if(brick.health<=0){
			continue;
		}
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


void scoreUpdate(){
	score += reward * factor;
}

void gameStatRender(){
	if(lives>=1){
		mySetColor(paddle.color);
		iFilledRectangle(1140, 725, 40, 10);
	}
	if(lives>=2){
		mySetColor(paddle.color);
		iFilledRectangle(1080, 725, 40, 10);
	}
	if(lives>=3){
		mySetColor(paddle.color);
		iFilledRectangle(1020, 725, 40, 10);
	}

	iShowBMP2(1200, 715, iconsprites[0], 0);

	iSetColor(229,37,84);
	char scoreboard[100] = "Score : ";
	string scoretxt = "Score : " + to_string(score);
	strcpy(scoreboard, scoretxt.c_str());
	// strcat(scoreboard, itoa(score, scoretxt, 10));
	// iText(100,725,"Score :",  GLUT_BITMAP_TIMES_ROMAN_24);
	iText(50,725,scoreboard,  GLUT_BITMAP_TIMES_ROMAN_24);



}

vector<Brick>::iterator hitbrick(vector<Brick>::iterator& hittedbrick){
	if(hittedbrick->type==3 && soundOn)PlaySound(TEXT("music/metalhit2.wav"), NULL, SND_ASYNC);
	else if(ball.type==1 && soundOn)PlaySound(TEXT("music/FireballHit3.wav"), NULL, SND_ASYNC);
	else if(soundOn)PlaySound(TEXT("music/normalhit.wav"), NULL, SND_ASYNC);
	
	if(hittedbrick->health<=0){
		return ++hittedbrick;
	}
	// debug(hittedbrick->health);
	hittedbrick->health--;
	// debug(hittedbrick->health);
	
	if(ball.type == 1 && hittedbrick->type!=3){			//Fire Ball
		hittedbrick->health--;
		int i = hittedbrick->row;
		int j = hittedbrick->col;
		vector<pair<int,int>> surrounding = {{-1,-1}, {-1,0}, {-1,1}, {0,-1}, {0,1}, {1,-1}, {1,0},{1,1}};
		for(auto el:surrounding){
			for(int ii = 0; ii<currentBricks.size(); ii++){
				auto& brick = currentBricks[ii];
				if(brick.row==i+el.first && brick.col==j+el.second){
					brick.health-=2;
				}
			}
			
		}
	}
	// debug(hittedbrick->health)
	if(hittedbrick->health<=0){
		PowerUp powerup;
		powerup.posX = hittedbrick->posX + hittedbrick->width/2 - powerup.width/2;
		powerup.posY = hittedbrick->posY - powerup.height;
		// srand(time(0));
		powerup.type = randomPowerUpGen();
		powerup.setColor();
		powerups.push_back(powerup);
		scoreUpdate();
		return currentBricks.erase(hittedbrick);
	}else{
		// debug(hittedbrick->health);
		hittedbrick->settype();
		hittedbrick->setColor();
		++hittedbrick;
		return hittedbrick;

	}
}


void levelinit(){
	won = 0;
	lives = 3;
	score = 0;
	factor = 1;
	Paddle paddletemp;
	paddle = paddletemp;
	Ball balltemp;
	ball = balltemp;
	powerups.clear();

}

void renderLeaderBoard(){
	iSetColor(229,37,84);
	char buffer[100];

	strcpy(buffer,  datatext[3][0].second.c_str());
	iText(365, 570,buffer, GLUT_BITMAP_TIMES_ROMAN_24);
	strcpy(buffer,  to_string(datatext[3][0].first).c_str());
	iText(835, 570,buffer, GLUT_BITMAP_TIMES_ROMAN_24);

	strcpy(buffer,  datatext[3][1].second.c_str());
	iText(365, 525,buffer, GLUT_BITMAP_TIMES_ROMAN_24);
	strcpy(buffer,  to_string(datatext[3][1].first).c_str());
	iText(835, 525,buffer, GLUT_BITMAP_TIMES_ROMAN_24);

	strcpy(buffer,  datatext[3][2].second.c_str());
	iText(365, 483,buffer, GLUT_BITMAP_TIMES_ROMAN_24);
	strcpy(buffer,  to_string(datatext[3][2].first).c_str());
	iText(835, 483,buffer, GLUT_BITMAP_TIMES_ROMAN_24);

	strcpy(buffer,  datatext[2][0].second.c_str());
	iText(365, 410,buffer, GLUT_BITMAP_TIMES_ROMAN_24);
	strcpy(buffer,  to_string(datatext[2][0].first).c_str());
	iText(835, 410,buffer, GLUT_BITMAP_TIMES_ROMAN_24);

	strcpy(buffer,  datatext[2][1].second.c_str());
	iText(365, 366,buffer, GLUT_BITMAP_TIMES_ROMAN_24);
	strcpy(buffer,  to_string(datatext[2][1].first).c_str());
	iText(835, 366,buffer, GLUT_BITMAP_TIMES_ROMAN_24);

	strcpy(buffer,  datatext[2][2].second.c_str());
	iText(365, 322,buffer, GLUT_BITMAP_TIMES_ROMAN_24);
	strcpy(buffer,  to_string(datatext[2][2].first).c_str());
	iText(835, 322,buffer, GLUT_BITMAP_TIMES_ROMAN_24);

	strcpy(buffer,  datatext[1][0].second.c_str());
	iText(365, 246,buffer, GLUT_BITMAP_TIMES_ROMAN_24);
	strcpy(buffer,  to_string(datatext[1][0].first).c_str());
	iText(835, 246,buffer, GLUT_BITMAP_TIMES_ROMAN_24);

	strcpy(buffer,  datatext[1][1].second.c_str());
	iText(365, 203,buffer, GLUT_BITMAP_TIMES_ROMAN_24);
	strcpy(buffer,  to_string(datatext[1][1].first).c_str());
	iText(835, 203,buffer, GLUT_BITMAP_TIMES_ROMAN_24);

	strcpy(buffer,  datatext[1][2].second.c_str());
	iText(365, 159,buffer, GLUT_BITMAP_TIMES_ROMAN_24);
	strcpy(buffer,  to_string(datatext[1][2].first).c_str());
	iText(835, 159,buffer, GLUT_BITMAP_TIMES_ROMAN_24);


}

void rendermenu(int state){

	if(backgroundOn)iShowBMP(0,0,gamemenubackground[0]);
	if(musicOn && !musicPlaying){
		musicPlaying = 1;
		PlaySound(TEXT("music/bgMusic.wav"), NULL, SND_LOOP|SND_ASYNC);	
	}
	
	iSetColor(255,255,255);
	switch (state)
	{
	case 0:
		iSetColor(0,0,0,0.5);
		iFilledRectangle(0,0,screenwidth, screenheight);
		iShowBMP2(400,100,menusprites[0], 0);
		break;
	case -1:
		iSetColor(0,0,0,0.5);
		iFilledRectangle(0,0,screenwidth, screenheight);
		iShowBMP2(260,100,menusprites[1], 0);
		break;
	case -2:
		iSetColor(0,0,0,0.5);
		iFilledRectangle(0,0,screenwidth, screenheight);
		iShowBMP2(160,25,menusprites[5], 0);
		
		renderLeaderBoard();
		break;
	case -3:
		iSetColor(0,0,0,0.5);
		iFilledRectangle(0,0,screenwidth, screenheight);
		iShowBMP2(160,100,menusprites[4], 0);
		break;
	
	
	default:
		break;
	}
	
	// iText(800,400, "1245670", GLUT_BITMAP_TIMES_ROMAN_24);
}



void rendergameplay(int level){
	if(backgroundOn)iShowBMP(0,0,gameplaybackground[level]);
	if(!gamePlaying){
		initBrickgrid(level);
		levelinit();
		gamePlaying = 1;
	}
	else if(currentBricks.size()-numberOfInvincible==0 || lives<=0){
		gamePlaying = 0;
		currentState=10*currentState+1;
		if(lives>0)won = 1;
	}
	paddle.Render();
	ball.Render();
	brickgridRender();
	powerupsRender();
	gameStatRender();
}


void renderLevel(int level){
	switch (level)
	{
	case 0: //main menu
	case -1:
	case -2:
	case -3:
		rendermenu(level);
		break;
	
	case 1:  //level1
	case 2:  //level2
	case 3:  //level3
		rendergameplay(level);
		break;
	case 11:
	case 21:
	case 31:
		{if(backgroundOn)iShowBMP(0,0,gameplaybackground[currentState/10]);
		iSetColor(0,0,0,0.5);
		iFilledRectangle(0,0,screenwidth, screenheight);
		
		if(won)iShowBMP2(365,150,menusprites[2], 0);
		else iShowBMP2(365,150,menusprites[3], 0);

		iSetColor(255,255,255);
		char scoretxt[100];
		strcpy(scoretxt, to_string(score).c_str());
		iText(650,430,scoretxt,  GLUT_BITMAP_TIMES_ROMAN_24);

		string nameEditing = playerName+'_';
		char nameEditingChar[100];
		strcpy(nameEditingChar, nameEditing.c_str()); 
		iText(520,355,nameEditingChar,  GLUT_BITMAP_TIMES_ROMAN_24);

		
		break;}
	default:
		break;
	}
	
	
}


void iDraw() {
	frame++;
	//place your drawing codes here
	iClear();

	currentColor.set(20,200,0);

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
		if(soundOn && !musicPlaying)PlaySound(TEXT("music/click.wav"), NULL, SND_ASYNC);
		//place your codes here
		// printf("x = %d, y= %d\n",mx,my);
		double x = mx;
		double y = my;
		switch (currentState)
		{
		case -1:
			if(430<=mx && mx<=540 && 375<=my && my<=495){
				currentState = 1;
			}
			if(590<=mx && mx<=705 && 375<=my && my<=495){
				currentState = 2;
			}
			if(760<=mx && mx<=870 && 375<=my && my<=495){
				currentState = 3;
			}
			if(550<=mx && mx<=715 && 145<=my && my<=225){
				currentState = 0;
			}
			break;
		case -2:
			if(210<=mx && mx<=344 && 58<=my && my<=125){
				currentState = 0;
			}
			break;
		case -3:
			if(195<=mx && mx<=320 && 130<=my && my<=195){
				currentState = 0;
			}
			break;
		case 0:
			if(490<=mx && mx<=775 && 450<=my && my<=520){
				currentState = -1;
			}
			if(490<=mx && mx<=775 && 340<=my && my<=410){
				currentState = -2;
			}
			if(490<=mx && mx<=775 && 220<=my && my<=290){
				currentState = -3;
			}
			if(490<=mx && mx<=775 && 110<=my && my<=180){
				exit(0);
			}
			break;
			

		case 1:
		case 2:
		case 3:
			ball.launched = 1;
			if(1205<=mx && mx<=1230 && 720<=my && my<=745){
				gamePlaying = 0;
				currentState = 0;
				lives = 3;
				score = 0;
				factor = 1;
			}
			break;
		
		case 11:
		case 21:
		case 31:
			if(490<=mx && mx<=785 && 275<=my && my<=335){
				updateLocalData(datatext[currentState/10], make_pair(score, playerName));
				writeData(datatext[currentState/10], currentState/10);
				currentState = -2;
			}
			if(490<=mx && mx<=630 && 215<=my && my<=270){
				currentState /= 10;
			}
			if(640<=mx && mx<=785 && 215<=my && my<=270){
				currentState = 0;
			}
			
			
			break;
		
		default:
			break;
		}
		
		
	}
	// if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
	// 	//place your codes here
	// 	x -= 10;
	// 	y -= 10;
	// }
}

/*
	function iKeyboard() is called whenever the user hits a key in keyboard.
	key- holds the ASCII value of the key pressed.
	*/
void iKeyboard(unsigned char key) {
	// if (key == 'q') {
	// 	gamePlaying = 0;
	// 	currentState = 0;
	// }
	if(currentState>10){
		if(key == '\b'){
			if(playerName.length()>0)playerName.pop_back();
		}else{
			if(playerName.length()<maxPlayerNameLength)playerName.push_back(key);
		}
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

	if (key == GLUT_KEY_F1) {
		musicOn ^= 1;
	}
	if (key == GLUT_KEY_F2) {
		backgroundOn ^= 1;
	}
	if (key == GLUT_KEY_F3) {
		soundOn ^= 1;
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
	static auto lastTime = lastTimeGlobal;
	double deltime = 0;
	auto currentTime = std::chrono::high_resolution_clock::now();
	deltime = std::chrono::duration<float>(currentTime - lastTime).count();
	// if(deltime > 0.3){
	// 	deltime = 0;
	// }
	// debug(currenttime - lasttime)
	lastTime = currentTime;
	// debug(deltime)
	if(gamePlaying){
		if(musicPlaying){
			musicPlaying=0;
			PlaySound(0,0,0);
		}
		if(ball.launched){
			ball.checkScreen();
			ball.checkColWithPaddle();
			ball.checkColWithBrick();
			ball.changePos(deltime);
		}
		for(auto& powerup:powerups){
			powerup.changePos(deltime);
		}
	}
	if(musicOn && !musicPlaying && !gamePlaying){
		PlaySound(TEXT("music/bgMusic.wav"), NULL, SND_LOOP|SND_ASYNC);
		musicPlaying = 1;
	}
	if(!musicOn && musicPlaying){
		PlaySound(0,0,0);
		musicPlaying = 0;
	}
	
}

void timePenalty(){
	if(gamePlaying)score = max(0, score-score/100);
	// cout<<powerups.size()<<endl;
}

void GameManager(){
	
}


int main() {
	//place your own initialization codes here.
	// iSetTimer(1000, myfunc);
	srand(time(0));
	iSetTimer(delTime1, SceneUpdater);
	iSetTimer(1000, timePenalty);
	lastTimeGlobal = std::chrono::high_resolution_clock::now();
	currentColor.set(255,255,255);

	readData(datatext[1], 1);
	readData(datatext[2], 2);
	readData(datatext[3], 3);
	
	
	iInitialize(1280, 760, "DX-Ball");
	GameManager();
	return 0;
}
