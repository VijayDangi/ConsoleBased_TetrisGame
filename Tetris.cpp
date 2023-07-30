/**
 * @file Tetris.cpp
 * @author Vijaykumar Dangi
 * 
 * @ref https://www.youtube.com/watch?v=8OK8_tHeCIA&t=2s&ab_channel=javidx9
 */

#include <Windows.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

// function declaration
int RotateTetromino( int px, int py, int r);
bool DoesPieceFit( int tetromino_index, int rotation, int pos_x, int pos_y);

// global variable
std::wstring tetromino[7];

int playing_field_width = 12;
int playing_field_height = 18;

    //0 => represent the empty space, 9 => represent the border
unsigned char *p_field = nullptr;

int screen_width = 80;  // console screen size (columns)
int screen_height = 30; // console screen size (rows)

/**
 * @brief main()
 */
int main()
{
    // code
        // Tetromino array will be 4x4 element
        // "." represent empty space
        // "X" represent a part of shape
    tetromino[0].append(L"..X.")
                .append(L"..X.")
                .append(L"..X.")
                .append(L"..X.");

    tetromino[1].append(L"..X.")
                .append(L".XX.")
                .append(L"..X.")
                .append(L"....");

    tetromino[2].append(L"....")
                .append(L".XX.")
                .append(L".XX.")
                .append(L"....");

    tetromino[3].append(L"..X.")
                .append(L".XX.")
                .append(L".X..")
                .append(L"....");

    tetromino[4].append(L".X..")
                .append(L".XX.")
                .append(L"..X.")
                .append(L"....");

    tetromino[5].append(L".X..")
                .append(L".X..")
                .append(L".XX.")
                .append(L"....");

    tetromino[6].append(L"..X.")
                .append(L"..X.")
                .append(L".XX.")
                .append(L"....");

    // create and initialize play field
    p_field = new unsigned char[ playing_field_width * playing_field_height];
    for( int y = 0; y < playing_field_height; ++y)
    {
        for( int x = 0; x < playing_field_width; ++x)
        {
            p_field[ y * playing_field_width + x] = ( x == 0 || x == playing_field_width - 1 || y == playing_field_height - 1) ? 9 : 0;
        }
    }

    // create console
    wchar_t *screen = new wchar_t[ screen_width * screen_height];
    for( int i = 0; i < screen_width * screen_height; ++i)
    {
        screen[i] = L' ';
    }

    HANDLE hConsole = CreateConsoleScreenBuffer( GENERIC_READ | GENERIC_WRITE, 0, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwByteWritten = 0;

    // game loop
    bool bGameOver = false;

    int current_piece = 1;
    int current_rotation = 0;
    int current_x = playing_field_width / 2;
    int current_y = 0;

    bool b_key[4];
    bool b_rotate_key_hold = false;

    int speed = 20;
    int speed_counter = 0;
    bool b_force_down = false;

    int piece_count = 0;
    int score = 0;

    std::vector<int> vLines;

    while( !bGameOver)
    {
        // GAME TIMING ============================================
        std::this_thread::sleep_for( std::chrono::milliseconds(50));    // 1 game tick

        // INPUT ==================================================
        for( int k = 0; k < 4; ++k)
        {
                                                                //    R   L   D Z
            b_key[k] = ( 0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;
        }

        // GAME LOGIC =============================================
            //Right Arrow Key Pressed
        current_x += ( b_key[0] && DoesPieceFit( current_piece, current_rotation, current_x + 1, current_y)) ? 1 : 0;

            //Left Arrow Key Pressed
        current_x -= ( b_key[1] && DoesPieceFit( current_piece, current_rotation, current_x - 1, current_y)) ? 1 : 0;
        
            //Down Arrow Key Pressed
        current_y += ( b_key[2] && DoesPieceFit( current_piece, current_rotation, current_x, current_y + 1)) ? 1 : 0;

            //Z key pressed (rotation)
        if( b_key[3])
        {
            current_rotation += ( !b_rotate_key_hold && DoesPieceFit( current_piece, current_rotation + 1, current_x, current_y)) ? 1 : 0;
            b_rotate_key_hold = true;
        }
        else
        {
            b_rotate_key_hold = false;
        }
        

            // force down
        speed_counter++;
        b_force_down = ( speed_counter == speed);
        if( b_force_down)
        {
            if( DoesPieceFit( current_piece, current_rotation, current_x, current_y + 1))
            {
                current_y++;
            }
            else
            {
                // Lock the current piece in the field
                for( int py = 0; py < 4; ++py)
                {
                    for( int px = 0; px < 4; ++px)
                    {
                        if( tetromino[ current_piece][RotateTetromino(px, py, current_rotation)] == L'X')
                        {
                            p_field[ (current_y + py) * playing_field_width + ( current_x + px)] = current_piece + 1;
                        }
                    }
                }

                // Level up game
                piece_count++;
                if( piece_count %10 == 0)
                {
                    if( speed >= 10)
                        speed--;
                }

                // Check have we got any horizontal line
                for( int py = 0; py < 4; ++py)
                {
                    if( current_y + py < playing_field_height - 1)
                    {
                        bool bLine = true;
                        for( int px = 1; px < playing_field_width - 1; ++px)
                        {
                            bLine &= (p_field[ (current_y + py) * playing_field_width + px]) != 0;
                        }

                        if( bLine)
                        {
                            // Remove Line, set to =
                            for( int px = 1; px < playing_field_width - 1; ++px)
                            {
                                p_field[ (current_y + py) * playing_field_width + px] = 8;
                            }

                            vLines.push_back( current_y + py);
                        }
                    }
                }

                score += 25;
                if( !vLines.empty())
                {
                    score += ( 1 << vLines.size()) * 100;
                }

                // choose next piece
                current_x = playing_field_width / 2;
                current_y = 0;
                current_rotation = 0;
                current_piece = rand() % 7;

                // if piece down not fit
                bGameOver = !DoesPieceFit( current_piece, current_rotation, current_x, current_y);
            }

            speed_counter = 0;
        }


        // RENDER =================================================

        // Draw Field
        for( int y = 0; y < playing_field_height; ++y)
        {
            for( int x = 0; x < playing_field_width; ++x)
            {
                screen[ (y + 2) * screen_width + ( x + 2)] = L" ABCDEFG=#"[ p_field[ y * playing_field_width + x]];
            }
        }
        
        // Draw Current Piece
        for( int py = 0; py < 4; ++py)
        {
            for( int px = 0; px < 4; ++px)
            {
                if( tetromino[ current_piece][RotateTetromino(px, py, current_rotation)] == L'X')
                {
                    screen[ ( current_y + py + 2) * screen_width + (current_x + px + 2)] = current_piece + 65;
                }
            }
        }

        // Draw Score
        swprintf_s( &screen[1 * screen_width + playing_field_width + 5], 16, L"SCORE: %8d", score);

        if( !vLines.empty())
        {
            // Display Frame ( cheekily to draw lines)
            WriteConsoleOutputCharacter( hConsole, screen, screen_width * screen_height, {0, 0}, &dwByteWritten);
            std::this_thread::sleep_for( std::chrono::milliseconds(400)); // Delay a bit

            for( int &v : vLines)
            {
                for( int py = v; py > 0; --py)
                {
                    for( int px = 1; px < playing_field_width - 1; ++px)
                    {
                        p_field[py * playing_field_width + px] = p_field[ (py - 1) * playing_field_width + px];
                        p_field[(py - 1) * playing_field_width + px] = 0;
                    }
                }
            }

            vLines.clear();
        }

        // Display Frame
        WriteConsoleOutputCharacter( hConsole, screen, screen_width * screen_height, { 0, 0}, &dwByteWritten);
    }


    // Game Over'
    CloseHandle( hConsole);
    std::cout << "Game Over!!!" << std::endl;
    std::cout << "Your Score : " << score << std::endl;

    std::system("pause");

    return 0;
}

/**
 * @brief DoesPieceFit()
 */
bool DoesPieceFit( int tetromino_index, int rotation, int pos_x, int pos_y)
{
    // code
    for( int py = 0; py < 4; ++py)
    {
        for( int px = 0; px < 4; ++px)
        {
            // Get index into piece
            int pi = RotateTetromino( px, py, rotation);

            // Get index into field
            int fi = ( pos_y + py) * playing_field_width + ( pos_x + px);

            if( (pos_x + px >= 0) && (pos_x + px < playing_field_width))
            {
                if( (pos_y + py >= 0) && (pos_y + py < playing_field_height))
                {
                    if( tetromino[tetromino_index][pi] == L'X' && p_field[fi] != 0)
                    {
                        return false;   // fail on first hit
                    }
                }
            }
        }
    }

    return true;
}

/*
 * @brief RotateTetromino()
 *
 * @description
 * 
 * [ 0  1  2  3]  0deg Rotation     [ 0  1  2  3]
 * [ 4  5  6  7]  ================> [ 4  5  6  7]       index => y * 4 + x
 * [ 8  9 10 11]                    [ 8  9 10 11]
 * [12 13 14 15]                    [12 13 14 15]
 * 
 * [ 0  1  2  3]  90deg Rotation    [12  8  4  0]
 * [ 4  5  6  7]  ================> [13  9  5  1]       index => 12 + y - ( x * 4)
 * [ 8  9 10 11]                    [14 10  6  2]
 * [12 13 14 15]                    [15 11  7  3]
 * 
 * [ 0  1  2  3]  180deg Rotation   [15 14 13 12]
 * [ 4  5  6  7]  ================> [11 10  9  8]       index = 15 - 4 * y - x
 * [ 8  9 10 11]                    [ 7  6  5  4]
 * [12 13 14 15]                    [ 3  2  1  0]
 * 
 * [ 0  1  2  3]  270deg Rotation   [ 3  7 11 15]
 * [ 4  5  6  7]  ================> [ 2  6 10 14]       index = 3 + y (4 * x)
 * [ 8  9 10 11]                    [ 1  5  9 13]
 * [12 13 14 15]                    [ 0  4  8 12]
 */
int RotateTetromino( int px, int py, int r)
{
    // code
    switch( r % 4)
    {
        case 0: return (py * 4 + px); //   0 degrees

        case 1: return (12 + py - (4 * px)); //  90 degrees

        case 2: return (15 - (4 * py) - px); // 180 degrees

        case 3: return  (3 - py + ( 4 * px)); // 2700 degrees
    }

    return 0;
}
