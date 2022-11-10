#include <iostream>
#include <SDL2/SDL.h>
#include <effolkronium/random.hpp>
#include <vector>
#include <algorithm>
#include <deque>
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <time.h>

using Random = effolkronium::random_static;

#define WIDTH 800
#define HEIGHT 600

bool running, fullscreen;

SDL_Renderer* renderer;
SDL_Window* window;

int frameCount, timerFPS, lastFrame, fps;

enum Direction
{
    DOWN,
    LEFT,
    RIGHT,
    UP
};
int dir = -1;
float speed = 3;

//Posisi Kepala Uler Saat Awal
//SDL_Rect head {(WIDTH/2), (HEIGHT/2), 10, 10};
SDL_Rect head {500, 500, 10, 10};
//

//Badan uler
std::deque<SDL_Rect> rq;
int size = 1;
//

//Makanan
std::vector<SDL_Rect> makanan;
//

void update() {
    if(fullscreen) SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    if(!fullscreen) SDL_SetWindowFullscreen(window, 0);
}

void input() {
    SDL_Event e;
    while(SDL_PollEvent(&e)){
        if(e.type == SDL_QUIT) {running = false;}

        if(e.type == SDL_KEYDOWN){
            if(e.key.keysym.sym == SDLK_DOWN) {dir = DOWN;}
            if(e.key.keysym.sym == SDLK_UP) {dir = UP;}
            if(e.key.keysym.sym == SDLK_LEFT) {dir = LEFT;}
            if(e.key.keysym.sym == SDLK_RIGHT) {dir = RIGHT;}
        }
    }

    //Gerakin Uler Open

        switch (dir) {
            case DOWN:
                    head.y += speed; break;
            case UP:
                    head.y -= speed; break;
            case LEFT:
                    head.x -= speed; break;
            case RIGHT:
                    head.x += speed; break;
        }
    //

    const Uint8 *keystates = SDL_GetKeyboardState(NULL);
    if(keystates[SDL_SCANCODE_ESCAPE]) {running = false;}
    if(keystates[SDL_SCANCODE_F11]) {fullscreen = !fullscreen;}
}

bool check_collision( SDL_Rect A, SDL_Rect B )
{
    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = A.x;
    rightA = A.x + A.w;
    topA = A.y;
    bottomA = A.y + A.h;
        
    //Calculate the sides of rect B
    leftB = B.x;
    rightB = B.x + B.w;
    topB = B.y;
    bottomB = B.y + B.h;
    //If any of the sides from A are outside of B
    if( bottomA <= topB )
    {
        return false;
    }
    
    if( topA >= bottomB )
    {
        return false;
    }
    
    if( rightA <= leftB )
    {
        return false;
    }
    
    if( leftA >= rightB )
    {
        return false;
    }
    
    //If none of the sides from A are outside B
    return true;
}

void collision(){
    //Collision Makanan
    /*std::for_each(makanan.begin(), makanan.end(), [&](auto& makan){
        if(head.x == makan.x && head.y == makan.y){
            size += 10;
            speed += 0.1;
            makan.x = -10;
            makan.y = -10;
        }
    });*/

    std::for_each(makanan.begin(), makanan.end(), [&](auto& makan){
        if(check_collision(head, makan)){
            size += 5;
            speed += 0.1;
            makanan.clear();
            std::cout << "Makan" << '\n' << std::flush;
            //makan.x = -10;
            //makan.y = -10;
        }
    });

    //Collision Kepala
    std::for_each(rq.begin(), rq.end(), [&](auto& snake_segment){
        if(head.x == snake_segment.x && head.y == snake_segment.y){
            size = 1;
            speed = 3;
        }
    });

    rq.push_front(head);

    while (rq.size() > size) {
        rq.pop_back();
    }
}

void draw() {
    SDL_SetRenderDrawColor(renderer, 40, 43, 200, 255);
    SDL_Rect rect;
    rect.x = rect.y = 0;
    rect.w = WIDTH;
    rect.h = HEIGHT;
    SDL_RenderFillRect(renderer, &rect);

    frameCount++;
    int timerFPS = SDL_GetTicks() - lastFrame;
    if(timerFPS < (1000/60)){
        SDL_Delay((1000/60) - timerFPS);
    }

    //Makanan
    SDL_SetRenderDrawColor(renderer,255,0,0,255);
    std::for_each(makanan.begin(), makanan.end(), [&](auto& makan){
        SDL_RenderFillRect(renderer,&makan);
    });
    //

    //Tubur Dan Badan Ular
    SDL_SetRenderDrawColor(renderer,255,255,255,255);
    std::for_each(rq.begin(), rq.end(), [&](auto& snake_segment){
        SDL_RenderFillRect(renderer,&snake_segment);
    });
    //

    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]){
    running=1;
    fullscreen = 0;
    static int lastTime = 0;
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) std::cout << "Failed at STD_Init()" << std::flush;
    if(SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0) std::cout << "Failed At SDL_CreateWindowAndRenderer" << std::flush;
    SDL_SetWindowTitle(window, "Name Window");
    SDL_ShowCursor(1);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

    while (running)
    {
        lastFrame = SDL_GetTicks();
        if(lastFrame >= (lastFrame + 1000)){
            lastTime = lastFrame;
            fps = frameCount;
            frameCount = 0;
        }
        //std::cout << fps << '\n' << std::flush;

        //Naruh Makanan di Map
        if(makanan.size() == 0){
            for(int i = 0; i < 1; i++){
            //std::cout << rand()%100*10 << std::flush;
            //makanan.emplace_back(rand()%100*10, rand()%100*10, 10, 10);
            //makanan.push_back(SDL_Rect(rand()%100*10, rand()%100*10, 10, 10));
            auto valX = Random::get(WIDTH, 0);
            auto valY = Random::get(HEIGHT, 0);
            //std::vector<SDL_Rect> v{{valX, valY, 10, 10}};
            makanan.push_back(SDL_Rect{valX, valY, 10, 10});
            //makanan.push_back(SDL_Rect{rand()%100*10, rand()%100*10, 10, 10});
            std::cout << "Buat Makanan! " << "X : " << valX << ", Y : " << valY << '\n' << std::flush;
            //makanan.emplace_back(std::move(v));
            //makanan.push_back(v);
            }
        }
        //

        update();
        input();
        collision();
        draw();
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}