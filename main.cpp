#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

using namespace std;

Color green = Color{173 , 204 , 96 , 255};
Color dark_green = Color{43 , 51 , 24 , 255};
// defined colours


int cellSize = 30;
int cellCount = 25;
int offset = 75;

double lastUpdateTime = 0;

bool eventTriggered(double interval)
{
    double currentTime = GetTime();
    if(currentTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

bool ElementInDeque(Vector2 element , deque<Vector2> deque)
{
    for(unsigned int i = 0 ; i < deque.size() ; i++)
    {
        if(Vector2Equals(deque[i] , element))
        {
            return true;
        }
    }
    return false;
}
class Snake
{
    public:
    deque<Vector2> body = {Vector2{6,9} , Vector2{5,9} , Vector2{4,9}};
    Vector2 direction = {1,0};
    bool addSegment = false;

    void Draw()
    {
        for (unsigned int i = 0 ; i < body.size() ; i++)
        {
            float x = body[i].x;
            float y = body[i].y;
            Rectangle segment = Rectangle{offset + x*cellSize , offset + y*cellSize , (float)cellSize , (float)cellSize};
            DrawRectangleRounded(segment, 0.5 , 6 , dark_green);
        }
    }

    void Update()
    {
        body.push_front(Vector2Add(body[0] , direction));
        if (addSegment == true)
        {
            addSegment = false;
        }
        else
        {
            body.pop_back();
        }
        
    }

    void Reset()
    {
        body = {Vector2{6,9} , Vector2{5,9} , Vector2{4,9}};
        direction = {1,0};
    }
    
};

class Food
{
    public:
    Vector2 position = {5,6};
    Texture2D texture;

    Food()
    {
        Image image = LoadImage("Graphics/apple1.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomCell();
    }

    ~Food()
    {
        UnloadTexture(texture);
    }

    void Draw()
    {
        DrawTexture(texture , offset + position.x*cellSize , offset + position.y*cellSize ,  WHITE);
    }

    Vector2 GenerateRandomCell()
    {
        float x = GetRandomValue(0 , cellCount - 1);
        float y = GetRandomValue(0 , cellCount - 1);
        Vector2 position = {x,y};
        return position;
    }

    Vector2 GenerateRandomPos(deque<Vector2> snakeBody)
    {
        float x = GetRandomValue(0 , cellCount - 1);
        float y = GetRandomValue(0 , cellCount - 1);
        Vector2 position = {x,y};
        while (ElementInDeque(position , snakeBody))
        {
            return GenerateRandomCell();
        }
        return GenerateRandomCell();
        
    }
};

class Game
{
    public:
    Snake snake;
    Food food;
    bool running = true;
    int score = 0;

    void GameOver()
    {
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        running = false;
    }

    void Draw()
    {
        food.Draw();
        snake.Draw();
    }

    void Update()
    {
        if(running)
        {
            CheckCollisionWithEdges();
            CheckCollisionWithFood();
            CollisionWithBody();
            snake.Update();
        }
        
        
    }

    void CheckCollisionWithFood()
    {
        if(Vector2Equals(snake.body[0] , food.position))
        {
            food.position = food.GenerateRandomPos(snake.body);
            snake.addSegment = true;
            score++;
        }
    }

    void CheckCollisionWithEdges()
    {
        if(snake.body[0].x == cellCount || snake.body[0].x == -1)
        {
            GameOver();
        }
        else if (snake.body[0].y == cellCount || snake.body[0].y == -1)
        {
            GameOver();
        }
    }

    void CollisionWithBody()
    {
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if(ElementInDeque(snake.body[0] , headlessBody))
        {
            GameOver();
        }
    }
};
int main()
{
    InitWindow(2* offset + cellSize*cellCount , 2*offset + cellSize*cellCount , "Snake");
    Game game;
    SetTargetFPS(60);
    while(WindowShouldClose() == false)
    {
        if(eventTriggered(0.1))
        {
            game.Update();
        }

        
        if (IsKeyDown(KEY_W) && game.snake.direction.y != 1)
        {
            game.snake.direction = {0,-1};
            game.running = true;
        }
        else if (IsKeyDown(KEY_A) && game.snake.direction.x != 1)
        {
            game.snake.direction = {-1,0};
            game.running = true;
        }
        else if (IsKeyDown(KEY_S) && game.snake.direction.y != -1)
        {
            game.snake.direction = {0,1};
            game.running = true;
        }
        else if (IsKeyDown(KEY_D) && game.snake.direction.x != -1)
        {
            game.snake.direction = {1,0};
            game.running = true;
        }

        BeginDrawing();
        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{(float)offset-5 , (float)offset - 5 , (float)cellSize*cellCount + 10 , (float)cellSize*cellCount + 10} , 5 , dark_green);
        DrawText("Snake Badmos" , offset-5 , 20 , 40 , dark_green);
        DrawText(TextFormat("%i" , game.score) ,offset - 5 , offset + cellSize*cellCount + 10 , 40 , dark_green);

        game.Draw();
        EndDrawing();
    }
    CloseWindow(); 
    return 0;
}

