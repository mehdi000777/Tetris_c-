#define UNICODE

#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
#include <vector>

std::wstring tetromino[7];
int boardWidth = 12;
int boardHeight = 18;
unsigned char *gameBoard = nullptr;

int screenWidth = 80;
int screenHeight = 30;

int Rotate(int x, int y, int r)
{
    switch (r % 4)
    {
    case 0:
        return y * 4 + x;
        break;
    case 1:
        return 12 + y - (x * 4);
        break;
    case 2:
        return 15 - (y * 4) - x;
        break;
    case 3:
        return 3 - y + (x * 4);
        break;
    }

    return 0;
}

bool DoesPiceFite(int nTetromino, int rotation, int posX, int posY)
{
    for (int px = 0; px < 4; px++)
    {
        for (int py = 0; py < 4; py++)
        {
            int pi = Rotate(px, py, rotation);

            int fi = (py + posY) * boardWidth + (px + posX);

            if (posX + px >= 0 && posX + px < boardWidth)
            {
                if (posY + py >= 0 && posY + py < boardHeight)
                {
                    if (tetromino[nTetromino][pi] != L'.' && gameBoard[fi] != 0)
                        return false;
                }
            }
        }
    }
    return true;
}

int main()
{
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");

    tetromino[1].append(L"..X.");
    tetromino[1].append(L".XX.");
    tetromino[1].append(L".X..");
    tetromino[1].append(L"....");

    tetromino[2].append(L".X..");
    tetromino[2].append(L".XX.");
    tetromino[2].append(L"..X.");
    tetromino[2].append(L"....");

    tetromino[3].append(L"....");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L"....");

    tetromino[4].append(L"..X.");
    tetromino[4].append(L".XX.");
    tetromino[4].append(L"..X.");
    tetromino[4].append(L"....");

    tetromino[5].append(L"....");
    tetromino[5].append(L".XX.");
    tetromino[5].append(L"..X.");
    tetromino[5].append(L"..X.");

    tetromino[6].append(L"....");
    tetromino[6].append(L".XX.");
    tetromino[6].append(L".X..");
    tetromino[6].append(L".X..");

    gameBoard = new unsigned char[boardWidth * boardHeight];
    for (int x = 0; x < boardWidth; x++)
    {
        for (int y = 0; y < boardHeight; y++)
        {
            gameBoard[y * boardWidth + x] = (x == 0 || x == boardWidth - 1 || y == boardHeight - 1) ? 9 : 0;
        }
    }

    wchar_t *screen = new wchar_t[screenWidth * screenHeight];
    for (int i = 0; i < screenWidth * screenHeight; i++)
        screen[i] = L' ';
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    bool gameOver = false;

    int currentPice = 1;
    int currentRotaion = 0;
    int currentX = boardWidth / 2;
    int currentY = 0;

    bool key[4];
    bool zHold = false;

    bool forceDown = false;
    int speedCounter = 0;
    int speed = 20;

    int piceCount = 0;
    int score = 0;

    std::vector<int> lines;

    while (!gameOver)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        speedCounter++;
        forceDown = (speedCounter == speed);

        for (int k = 0; k < 4; k++)
        {
            key[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;
        }

        if (key[1])
        {
            if (DoesPiceFite(currentPice, currentRotaion, currentX - 1, currentY))
            {
                currentX = currentX - 1;
            }
        }

        if (key[0])
        {
            if (DoesPiceFite(currentPice, currentRotaion, currentX + 1, currentY))
            {
                currentX = currentX + 1;
            }
        }

        if (key[2])
        {
            if (DoesPiceFite(currentPice, currentRotaion, currentX, currentY + 1))
            {
                currentY = currentY + 1;
            }
        }

        if (key[3])
        {
            if (!zHold && DoesPiceFite(currentPice, currentRotaion + 1, currentX, currentY))
            {
                currentRotaion = currentRotaion + 1;
                zHold = true;
            }
            else
            {
                zHold = false;
            }
        }

        if (forceDown)
        {
            if (DoesPiceFite(currentPice, currentRotaion, currentX, currentY + 1))
            {
                currentY = currentY + 1;
            }
            else
            {
                for (int px = 0; px < 4; px++)
                {
                    for (int py = 0; py < 4; py++)
                    {
                        if (tetromino[currentPice][Rotate(px, py, currentRotaion)] == L'X')
                            gameBoard[(py + currentY) * boardWidth + (px + currentX)] = currentPice + 1;
                    }
                }

                piceCount++;
                if (piceCount % 10 == 0)
                    if (speed >= 10)
                        speed--;

                for (int py = 0; py < 4; py++)
                {
                    if (currentY + py < boardHeight - 1)
                    {
                        bool line = true;
                        for (int px = 1; px < boardWidth - 1; px++)
                        {
                            line &= gameBoard[(py + currentY) * boardWidth + px] != 0;
                        }

                        if (line)
                        {
                            for (int px = 1; px < boardWidth - 1; px++)
                            {
                                gameBoard[(py + currentY) * boardWidth + px] = 8;
                            }

                            lines.push_back(currentY + py);
                        }
                    }
                }

                score += 25;
                if (!lines.empty())
                    score += lines.size() * 100;

                currentX = boardWidth / 2;
                currentY = 0;
                currentRotaion = 0;
                currentPice = rand() % 7;

                gameOver = !DoesPiceFite(currentPice, currentRotaion, currentX, currentY);
            }
            speedCounter = 0;
        }

        for (int x = 0; x < boardWidth; x++)
        {
            for (int y = 0; y < boardHeight; y++)
            {
                screen[(y + 2) * screenWidth + (x + 2)] = L" ABCDEFG=#"[gameBoard[y * boardWidth + x]];
            }
        }

        swprintf_s(&screen[2 * screenWidth + boardWidth + 6], 16, L"SCORE: %8d", score);

        for (int px = 0; px < 4; px++)
        {
            for (int py = 0; py < 4; py++)
            {
                if (tetromino[currentPice][Rotate(px, py, currentRotaion)] == L'X')
                    screen[(py + 2 + currentY) * screenWidth + (px + 2 + currentX)] = currentPice + 65;
            }
        }

        if (!lines.empty())
        {
            WriteConsoleOutputCharacter(hConsole, screen, screenWidth * screenHeight, {0, 0}, &dwBytesWritten);
            std::this_thread::sleep_for(std::chrono::milliseconds(400));

            for (auto &v : lines)
            {
                for (int px = 1; px < boardWidth - 1; px++)
                {
                    for (int py = v; py > 0; py--)
                    {
                        gameBoard[(py * boardWidth + px)] = gameBoard[(py - 1) * boardWidth + px];
                    }
                    gameBoard[px] = 0;
                }
            }
            lines.clear();
        }

        WriteConsoleOutputCharacter(hConsole, screen, screenWidth * screenHeight, {0, 0}, &dwBytesWritten);
    }

    CloseHandle(hConsole);
    std::cout << "Game Over!! Score:" << score << std::endl;
    system("pause");

    return 0;
}