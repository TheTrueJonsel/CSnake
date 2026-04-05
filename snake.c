#include "raylib.h"
#include <stdlib.h>

#define MENU_COLOR CLITERAL(Color){ 20, 20, 20, 255 }
#define MENU_BUTTON_COLOR CLITERAL(Color){ 190, 190, 190, 255 }
#define GAME_COLOR CLITERAL(Color){ 0, 0, 40, 255 }

typedef enum {
    ST_GAME,
    ST_MENU
} GameState;

typedef enum {
    DIR_NORTH,
    DIR_EAST,
    DIR_SOUTH,
    DIR_WEST
} Direction;

const int   screenWidth     = 600, 
            screenHeight    = 600;
const char *titleMessage    = "Snake";
GameState currentState = ST_MENU; 
int highScore = 0;
int currentScore = 0;
Vector2 mousePosition = {0.0f, 0.0f};
bool shouldExit = false;
const int gameBoardLength = 40;
Vector2 currentFood;
float timer = 0.0f;

// this array holds all of the snake segment, the head is alway at snake[0]
// initializing the array with two elements. the head and the first tail
Vector2 snake[256];
int segmentCount;

// lower value means higher movement speed
const float movementSpeed = 0.2f;

// the current direction of the snake head
Direction currentDirection;

// the scale used to draw the board
const float tileScale = 15.0f;

void spawnFood(){
    currentFood.x = rand() % (gameBoardLength + 1);
    currentFood.y = rand() % (gameBoardLength + 1);
}

void initGame(){

    // updating the highscore
    if(currentScore > highScore) highScore = currentScore;

    // resetting the segment count
    segmentCount = 2;
    
    // resetting the current score
    currentScore = 0;

    // putting the snake into the default position
    snake[0].x = 20;
    snake[0].y = 19;
    snake[1].x = 20;
    snake[1].y = 20;

    // initializing the snakes direction
    currentDirection = DIR_NORTH;

    // initializing the first food 
    spawnFood();
}

void updateGame(){
    // change snake head direction
    if (IsKeyDown(KEY_RIGHT) && (currentDirection == DIR_NORTH || currentDirection == DIR_SOUTH)){currentDirection = DIR_EAST;}
    else if (IsKeyDown(KEY_LEFT) && (currentDirection == DIR_NORTH || currentDirection == DIR_SOUTH)){currentDirection = DIR_WEST;}
    else if (IsKeyDown(KEY_UP) && (currentDirection == DIR_EAST || currentDirection == DIR_WEST)){currentDirection = DIR_NORTH;}
    else if (IsKeyDown(KEY_DOWN) && (currentDirection == DIR_EAST || currentDirection == DIR_WEST)){currentDirection = DIR_SOUTH;}

    timer += GetFrameTime();
    if(timer >= movementSpeed){

        // loop through the entire snake array, except the head, and move its posittion the next segment, closer to the snake head
        for(int i = segmentCount - 1; i > 0; i-- ){
            snake[i].x = snake[i-1].x;
            snake[i].y = snake[i-1].y;
        }

        // move the snake head one step towards its direction
        switch(currentDirection){
            case DIR_NORTH:
                snake[0].y -= 1;
                break;
            case DIR_EAST:
                snake[0].x += 1;
                break;
            case DIR_SOUTH:
                snake[0].y += 1;
                break;
            case DIR_WEST:
                snake[0].x -= 1;
                break;
        }

        timer = 0.0f;
    }

    

    

    // food collision
    if(snake[0].x == currentFood.x && snake[0].y == currentFood.y){
        // adding a tail segment
        snake[segmentCount].x = snake[segmentCount-1].x;
        snake[segmentCount].y = snake[segmentCount-1].y;

        currentScore++;
        segmentCount++;
        spawnFood();
    }

    // self collision
    for(int i = 1; i < segmentCount; i++){
        if(snake[0].x == snake[i].x && snake[0].y == snake[i].y){
            currentState = ST_MENU;
            initGame();
        }
    }

    // wall collisions
    if(snake[0].x < 0 || snake[0].x > gameBoardLength || snake[0].y < 0 || snake[0].y > gameBoardLength){
        currentState = ST_MENU;
        initGame();
    }
}

void drawGame(){
    ClearBackground(GAME_COLOR);

    // loop through the entire snake array, except the head, and draw it
    for(int i = segmentCount - 1; i > 0; i--){
        DrawRectangleRounded((Rectangle){snake[i].x*tileScale, snake[i].y*tileScale, tileScale, tileScale}, 0.5f, 1, GREEN);
    }

    // draw the snake head
    DrawRectangleRounded((Rectangle){snake[0].x*tileScale, snake[0].y*tileScale, tileScale, tileScale}, 0.5f, 1, RED);

    // draw the food
    DrawRectangleRounded((Rectangle){currentFood.x*tileScale, currentFood.y*tileScale, tileScale, tileScale}, 0.2f, 1, YELLOW);
}

void drawMenu(){
    mousePosition = GetMousePosition();

    const int   buttonWidth     = 200, 
                buttonHeight    = 80,
                fontSize        = 50,
                fontSizeScore   = 20;

    // defining the rectangles used for drawing the button backgrounds
    Rectangle   highScoreButton   = {screenWidth / 2 - buttonWidth / 2, screenHeight / 4 - buttonHeight / 2, buttonWidth, buttonHeight},
                startButton = {screenWidth / 2 - buttonWidth / 2, screenHeight * 2 / 4 - buttonHeight / 2, buttonWidth, buttonHeight},
                exitButton  = {screenWidth / 2 - buttonWidth / 2, screenHeight * 3 / 4 - buttonHeight / 2, buttonWidth, buttonHeight};

    ClearBackground(MENU_COLOR);

    // drawing the button backgrounds
    DrawRectangle(highScoreButton.x, highScoreButton.y, highScoreButton.width, highScoreButton.height, MENU_BUTTON_COLOR);
    DrawRectangle(startButton.x, startButton.y, startButton.width, startButton.height, MENU_BUTTON_COLOR);
    DrawRectangle(exitButton.x, exitButton.y, exitButton.width, exitButton.height, MENU_BUTTON_COLOR);

    // determining the label text sizes
    int scoreLabelWidth = MeasureText("HIGHSCORE", fontSizeScore);
    int startLabelWidth = MeasureText("Start", fontSize);
    int exitLabelWidth = MeasureText("Exit", fontSize);
    int currentScoreLabelWidth = MeasureText(TextFormat("%d", highScore), fontSize);

    // drawing highcore / label
    DrawText("HIGHSCORE", screenWidth / 2 - scoreLabelWidth / 2, screenHeight / 4 - fontSizeScore - buttonHeight / 2 , fontSizeScore, GREEN);
    DrawText(TextFormat("%d", highScore), screenWidth / 2 - currentScoreLabelWidth / 2, screenHeight / 4 - fontSize / 2, fontSize, BLACK);

    // dawing the button labels
    DrawText("Start", screenWidth / 2 - startLabelWidth / 2, screenHeight * 2 / 4 - fontSize / 2, fontSize, BLACK);
    DrawText("Exit", screenWidth / 2 - exitLabelWidth / 2, screenHeight * 3 / 4 - fontSize / 2, fontSize, BLACK);

    // checking if the user clicked the start button and switching to the "game" state, if they did
    if(CheckCollisionPointRec(mousePosition, startButton) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
        currentState = ST_GAME;

    }

    // checking if the user clicked the exit button and exiting the game, if they did
    if(CheckCollisionPointRec(mousePosition, exitButton) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
        shouldExit = true;
    }
}


int main(){
    InitWindow(screenWidth, screenHeight, titleMessage);
    SetTargetFPS(60);

    initGame();

    // main game loop, executed every frame
    while(!WindowShouldClose() && !shouldExit){
        BeginDrawing();
            switch(currentState){
                case ST_GAME:
                    drawGame();
                    updateGame();
                    break;
                case ST_MENU:
                    drawMenu();
                    break;
            }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}