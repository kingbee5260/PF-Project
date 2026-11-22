#include <iostream> //sorry yar, i have a household sadly, they drain me alot :D 
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp> 
#include <SFML/Window.hpp>

using namespace sf;
using namespace std;

int screen_x = 1136;
int screen_y = 896;

void display_level(RenderWindow& window, char**lvl, Texture& bgTex,Sprite& bgSprite,Texture& blockTexture,Sprite& blockSprite, const int height, const int width, const int cell_size)
{
	window.draw(bgSprite);

	for (int i = 0; i < height; i += 1)
	{
		for (int j = 0; j < width; j += 1)
		{

			if (lvl[i][j] == '#')
			{
				blockSprite.setPosition(j * cell_size, i * cell_size);
				window.draw(blockSprite);
			}
		}
	}

}

void crawler_gravity(char** lvl, float& crawlerOffsetY, float&crawlerVelocityY, bool& crawleronGround, const float& gravity, float& terminal_Velocity, float& crawler_x, float& crawler_y, const int cell_size, int& crawlerHeight, int& crawlerWidth){
  crawlerOffsetY = crawler_y;
  crawlerOffsetY += crawlerVelocityY;
  int tileRow = static_cast<int>(crawlerOffsetY + crawlerHeight) / cell_size;

  char bottom_left_down = lvl[static_cast<int>(crawlerOffsetY + crawlerHeight) / cell_size][static_cast<int>(crawler_x) / cell_size];
  char bottom_mid_down = lvl[static_cast<int>(crawlerOffsetY + crawlerHeight) / cell_size][static_cast<int>(crawler_x + crawlerWidth / 2) / cell_size];
  char bottom_right_down = lvl[static_cast<int>(crawlerOffsetY + crawlerHeight) / cell_size][static_cast<int>(crawler_x + crawlerWidth) / cell_size];

  if(bottom_left_down == '#' || bottom_mid_down == '#' || bottom_right_down == '#') {
		crawleronGround = true;
		crawler_y = (tileRow * cell_size) - crawlerHeight;
	}
  else {
		crawler_y = crawlerOffsetY;
		crawleronGround = false;
	}

  if (!crawleronGround) {
		crawlerVelocityY += gravity;
		if (crawlerVelocityY >= terminal_Velocity)
			crawlerVelocityY = terminal_Velocity;
	}
  else {
		crawlerVelocityY = 0;
	}

}

void player_gravity(char** lvl, float& offset_y, float& velocityY, bool& onGround, const float& gravity, float& terminal_Velocity, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth)
{
	offset_y = player_y;
	offset_y+= velocityY;
	int tileRow = static_cast<int>(offset_y + Pheight) / cell_size;
	
	char bottom_left_down = lvl[static_cast<int>(offset_y + Pheight)/ cell_size][static_cast<int>(player_x)/cell_size];
	char bottom_mid_down = lvl[static_cast<int>(offset_y + Pheight)/ cell_size][static_cast<int>(player_x + Pwidth/2)/cell_size];
	char bottom_right_down = lvl[static_cast<int>(offset_y + Pheight)/ cell_size][static_cast<int>(player_x + Pwidth)/cell_size];
	if(bottom_left_down == '#' || bottom_mid_down == '#' || bottom_right_down== '#'){
		onGround = true;
		player_y = (tileRow * cell_size) - Pheight; 
	}
	else{
		player_y = offset_y;
		onGround= false;
	}
	if(!onGround){
		velocityY+= gravity;
		if(velocityY>= terminal_Velocity)
			velocityY= terminal_Velocity;
		}
	else{
		velocityY=0;
	}
}
//Novemeber 22, 2026. Adding a function for jumping enemy gravity. 
void blueJumper (char** lvl, float& blueJumper_offset_y, float& blueJumpervelocityY, bool& blueJumperOnGround, float& gravity, float& terminal_Velocity, float& blueJumper_x, float& blueJumper_y, const int cell_size, int blueJumper_H, int blueJumper_W)
{
  //
  blueJumper_offset_y = blueJumper_y;
  blueJumper_offset_y += blueJumpervelocityY;

  int tileRow = static_cast <int> (blueJumper_offset_y + blueJumper_H) / cell_size; 
  //line 40-42 for reference.
  int columnLeft = static_cast <int> (blueJumper_x) / cell_size;
  int columnMid = static_cast <int> (blueJumper_x + blueJumper_W / 2) / cell_size;
  int columnRight = static_cast <int> (blueJumper_x + blueJumper_W) / cell_size;
  
  if (columnLeft < 0) 
  columnLeft = 0;
  if (columnRight >= 18) 
  columnRight = 17;
  
  char bottom_left_down = lvl [tileRow] [columnLeft];
  char bottom_mid_down = lvl [tileRow] [columnMid];
  char bottom_right_down = lvl [tileRow] [columnRight];
  
  //Now check whether that purple bona enemy is touching '#'
  if (bottom_left_down == '#' || bottom_mid_down == '#' || bottom_right_down == '#')
  {
      blueJumperOnGround = true;
      blueJumper_y = (tileRow * cell_size) - blueJumper_H;
  }
  
  else 
  {
      blueJumper_y = blueJumper_offset_y;
      blueJumperOnGround = false;
  }
  
  if (!blueJumperOnGround)
  {
      blueJumpervelocityY += gravity;
      if (blueJumpervelocityY >= terminal_Velocity)
      blueJumpervelocityY = terminal_Velocity;
  }
  
  else
  {
      blueJumpervelocityY = 0;
  }
}
//Done. 

//Function for Player Movement Animation
void walk_animation(Sprite& PlayerSprite, int walkX[], int Frames, int& animIndex, Clock& animClock, bool isMoving) {
    if (isMoving) {
		// During Movement
        if (animClock.getElapsedTime().asMilliseconds() >= 100) {
            animIndex = (animIndex + 1) % Frames;
            animClock.restart();
        }

        PlayerSprite.setTextureRect(IntRect(walkX[animIndex], 8, 28, 40));

    }
    else {           //When Idle
        animIndex = 0;
        animClock.restart();
        PlayerSprite.setTextureRect(IntRect(9, 8, 28, 40));

    }
} 

void attack_animation(Sprite& PlayerSprite, int atkPlayer[], int atkFrames, int& atkAnimIndex, Clock& atkAnimClock, bool& isAttacking){
	if(isAttacking && atkAnimIndex < atkFrames){
		PlayerSprite.setTextureRect(IntRect(atkPlayer[atkAnimIndex], 120, 34, 60));
		if(atkAnimClock.getElapsedTime().asMilliseconds() >=35){
			atkAnimIndex++;
			atkAnimClock.restart();
		}
	}
	else{
		isAttacking= false;
		atkAnimIndex=0;
		atkAnimClock.restart();
	}
}

void crawler_animation(Sprite& crawlerSprite, int crawleranim[], int& crawlerIndex, Clock& crawlerClock, bool crawling){
    if (crawling){
        if(crawlerClock.getElapsedTime().asMilliseconds() >=129){
            crawlerIndex = (crawlerIndex+1)% 2;
            crawlerClock.restart();
    	}
	crawlerSprite.setTextureRect(IntRect(crawleranim [crawlerIndex], 16, 67, 52));
	}
}


int main()
{
	//Making Screen
	RenderWindow window(VideoMode(screen_x, screen_y), "VICE: Project Doom", Style::Resize);
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);

	//level specifics
	const int cell_size = 64;
	const int height = 14;
	const int width = 18;
	char** lvl;

	//level and background textures and sprites
	Texture bgTex;
	Sprite bgSprite;
	Texture blockTexture;
	Sprite blockSprite;

	bgTex.loadFromFile("Data/bg.png");
	bgSprite.setTexture(bgTex);
	bgSprite.setPosition(0,0);

	blockTexture.loadFromFile("Data/block1.png");
	blockSprite.setTexture(blockTexture);

	//Music initialisation
	Music lvlMusic;

	lvlMusic.openFromFile("Data/mus.ogg");
	lvlMusic.setVolume(20);
	lvlMusic.play();
	lvlMusic.setLoop(true);

	//player data
	float player_x = 780;
	float player_y = 60;
	float speedx= 5.5f;

	const float gravity = 1;

	Texture PlayerTexture;
	Sprite PlayerSprite;

	bool onGround = false;
	bool isJumping = false;
	bool isCrouching= false;
	bool isAttacking= false;

	float offset_y = 0;
	float velocityY = 0;

	float terminal_Velocity = 20;

	int PlayerHeight = 108;
	int PlayerWidth = 54;
	bool facingLeft = false;

	PlayerTexture.loadFromFile("Data/hart.png");
	PlayerSprite.setTexture(PlayerTexture);
	PlayerSprite.setTextureRect(IntRect(15, 10, 16, 38));
	PlayerSprite.setScale(3,3);
	PlayerSprite.setPosition(player_x, player_y);
	
	//Walk Animation
	const int Frames = 4;
	int walkX[Frames] = { 42, 76, 111, 145 };
	int animIndex=0;
	Clock animClock;

	//Attack Animation:
	const int atkFrames = 5;
	int atkPlayer[atkFrames]= {9, 44, 83, 123, 159};
	int atkAnimIndex= 0;
	Clock atkAnimClock;
	

	int crawleranim[2] = {8,76};
  int crawlerIndex=0;
  Clock crawlerClock;

	Texture crawlerTexture;
	Sprite crawlerSprite;
	crawlerTexture.loadFromFile("Custom Textures/crawler.png");
	crawlerSprite.setTexture(crawlerTexture);
    int crawlerHeight= 71;
    int crawlerWidth = 90;

	crawlerSprite.setTextureRect(IntRect(8, 16, 67, 52 ));
	crawlerSprite.setScale(2, 1.5);

    float crawler_x[5] = { 100, 300, 500, 700, 900 };
	float crawler_y[5] = { 0, 0, 0, 0, 0 };
	int moving_right[5] = { 1, 0, 1, 0, 1 };
	bool crawleronGround[5]= {false, false, false, false, false};
	float crawlerOffsetY=0;
	float crawlerVelocityY[5]={0,0,0,0,0};

	//creating level array
	lvl = new char* [height];
	for (int i = 0; i < height; i += 1)
	{
		lvl[i] = new char[width];
	}

	for (int j = 0; j < width; j += 1)
	{
		lvl[13][j] = '#';
	}

	Event ev;
	//main loop
	while (window.isOpen())
	{

		while (window.pollEvent(ev))
		{
			if (ev.type == Event::Closed)
			{
				window.close();
			}

			if (ev.type == Event::KeyPressed)
			{
				if(ev.key.code==Keyboard::X && !isAttacking){
					isAttacking= true;
					atkAnimIndex=0;
					atkAnimClock.restart();
				}
			}

		}
		bool isMoving = false;

		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}
	
		if (Keyboard::isKeyPressed(Keyboard::Left) && !(Keyboard::isKeyPressed(Keyboard::Right)) && !(Keyboard::isKeyPressed(Keyboard::Down)))
		{
			if (player_x > 0)
				player_x -= speedx;

			PlayerSprite.setScale(-3, 3);

			facingLeft = true;
			isMoving = true;
			isCrouching = false;
		}

		if (Keyboard::isKeyPressed(Keyboard::Right) && !(Keyboard::isKeyPressed(Keyboard::Left)) && !(Keyboard::isKeyPressed(Keyboard::Down)))
		{
			if (player_x < screen_x - PlayerWidth)
				player_x += speedx;

			PlayerSprite.setScale(3, 3);

			facingLeft = false;
			isMoving = true;
			isCrouching = false;
		}

		if (Keyboard::isKeyPressed(Keyboard::Up) && onGround)
		{
			velocityY = -20;
			onGround = false;
			isJumping = true;
			isCrouching = false;
		}

		isCrouching = false;
		if (Keyboard::isKeyPressed(Keyboard::Down) && onGround && !isMoving)
		{
			isCrouching = true;
		}

		window.clear();
		display_level(window, lvl, bgTex, bgSprite, blockTexture, blockSprite, height, width, cell_size);
		player_gravity(lvl,offset_y,velocityY,onGround,gravity,terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth);

		if(onGround){
			isJumping = false;
		}
		float crouchOffset = 0.0f;
		if (isAttacking){
			attack_animation(PlayerSprite, atkPlayer, atkFrames, atkAnimIndex, atkAnimClock, isAttacking);
		}
		else{
			walk_animation(PlayerSprite, walkX, Frames, animIndex, animClock, isMoving);
			if(isJumping){
				PlayerSprite.setTextureRect(IntRect(177 ,4 ,30 ,40));
			}

        	if (isCrouching) {
            	PlayerSprite.setTextureRect(IntRect(204, 19, 31, 29));
            	crouchOffset = 33.0f;
        	}
		}
        if (facingLeft) {
            PlayerSprite.setPosition(player_x + 84, player_y + crouchOffset);
        }
        else {
            PlayerSprite.setPosition(player_x, player_y + crouchOffset);
        }	
        window.draw(PlayerSprite);

		bool crawling = true;
		for (int i = 0; i < 5; i++)
		{
		    crawler_gravity(lvl, crawlerOffsetY, crawlerVelocityY[i], crawleronGround[i], gravity, terminal_Velocity, crawler_x[i], crawler_y[i], cell_size, crawlerHeight, crawlerWidth);

			if (moving_right[i] == 1)
			{
				crawler_x[i] = crawler_x[i] + 2.5;
				crawlerSprite.setScale(2, 1.5);
				crawlerSprite.setPosition(crawler_x [i], crawler_y [i]);

				if (crawler_x[i] >= 1050)
				{
					moving_right[i] = 0;
				}
			}
			else if (moving_right[i] == 0)
			{
				crawler_x[i] = crawler_x[i] - 2.5;
				crawlerSprite.setScale(-2, 1.5);
				crawlerSprite.setPosition(crawler_x [i] + crawlerWidth, crawler_y [i]);
				if (crawler_x[i] <= 0)
				{
					moving_right[i] = 1;
				}
			}
			crawler_animation(crawlerSprite, crawleranim, crawlerIndex, crawlerClock, crawling);
			window.draw(crawlerSprite);

		}
        window.display();
    }
	//stopping music and deleting level array
	lvlMusic.stop();
	for (int i = 0; i < height; i++)
	{
		delete[] lvl[i];
	}
	delete[] lvl;

	return 0;
}