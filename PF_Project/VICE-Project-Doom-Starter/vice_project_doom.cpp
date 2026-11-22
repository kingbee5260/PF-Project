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

void player_gravity(char** lvl, float& offset_y, float& velocityY, bool& onGround, const float& gravity, float& terminal_Velocity, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth)
{
	offset_y = player_y;
	offset_y+= velocityY;
	
	char bottom_left_down = lvl[static_cast<int>(offset_y + Pheight)/ cell_size][static_cast<int>(player_x)/cell_size];
	char bottom_mid_down = lvl[static_cast<int>(offset_y + Pheight)/ cell_size][static_cast<int>(player_x + Pwidth/2)/cell_size];
	char bottom_right_down = lvl[static_cast<int>(offset_y + Pheight)/ cell_size][static_cast<int>(player_x + Pwidth)/cell_size];
	if(bottom_left_down == '#' || bottom_mid_down == '#' || bottom_right_down== '#'){
		onGround = true;
        int tileRow = static_cast<int>(offset_y + Pheight) / cell_size; // <-- ADD HERE
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
void walk_animation(Sprite& PlayerSprite, int walkX[], int Frames, int& animIndex, Clock& animClock, bool isMoving) {
    if (isMoving) {
        if (animClock.getElapsedTime().asMilliseconds() >= 120) {
            animIndex = (animIndex + 1) % Frames;
            animClock.restart();
        }

        PlayerSprite.setTextureRect(IntRect(walkX[animIndex], 8, 28, 40));

    }
    else {
        animIndex = 0;
        animClock.restart();
        PlayerSprite.setTextureRect(IntRect(9, 8, 28, 40));

    }
}


int main()
{

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

	float offset_y = 0;
	float velocityY = 0;

	float terminal_Velocity = 20;

	int PlayerHeight = 114;
	int PlayerWidth = 54;
	bool facingLeft = false;

	PlayerTexture.loadFromFile("Data/hart.png");
	PlayerSprite.setTexture(PlayerTexture);
	PlayerSprite.setTextureRect(IntRect(15, 10, 16, 38));
	PlayerSprite.setScale(3,3);
	PlayerSprite.setPosition(player_x, player_y);
	
	const int Frames = 4;
	int walkX[Frames] = { 42, 76, 111, 145 };

	int animIndex=0;
	Clock animClock;

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
			}

		}

		//presing escape to close
		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}
		bool isMoving = false;

		if (Keyboard::isKeyPressed(Keyboard::Right))
		{
    		player_x += speedx;

    		if (player_x > screen_x - PlayerWidth)
        		player_x = screen_x - PlayerWidth;

    		PlayerSprite.setScale(3, 3);

    		facingLeft = false;
    		isMoving = true;
			isCrouching= false;
		}

		if (Keyboard::isKeyPressed(Keyboard::Left))
		{
   			 player_x -= speedx;

    		if (player_x < 0)
      		  player_x = 0;

    		PlayerSprite.setScale(-3, 3);

    		facingLeft = true;
   			isMoving = true;
			isCrouching=false;
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
		walk_animation(PlayerSprite, walkX, Frames, animIndex, animClock, isMoving);
		if(isJumping){
			PlayerSprite.setTextureRect(IntRect(177 ,4 ,30 ,40));
		}

        float crouchOffset = 0.0f;

        if (isCrouching) {
            PlayerSprite.setTextureRect(IntRect(204, 19, 31, 29));
            crouchOffset = 33.0f;
        }

        if (facingLeft) {
            PlayerSprite.setPosition(player_x + PlayerSprite.getGlobalBounds().width, player_y + crouchOffset);
        }
        else {
            PlayerSprite.setPosition(player_x, player_y + crouchOffset);
        }

        window.draw(PlayerSprite);
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