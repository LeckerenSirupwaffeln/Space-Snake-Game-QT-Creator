#ifndef MAINWINDOW_H
#define MAINWINDOW_H

enum {MENU_INDEX, OPTIONS_INDEX, GAME_INDEX};
enum {KEY_MENU, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT};
enum {SOUND_EFFECT_START, SOUND_EFFECT_EAT, SOUND_EFFECT_GAMEOVER};
enum {EMPTY, SNAKE, PLANET};

#define CURRENT_VERSION "alpha 1.0"

#define MUSIC_DIR "/home/js/Documents/QtBuild/SnakeGame/music/"

#define NUM_PLANETS 8
#define BOARD_LENGTH 8
#define BOARD_SIZE BOARD_LENGTH * BOARD_LENGTH
#define RECT_SIZE 64
#define PLANET_SIZE 64
#define XOffset 350
#define YOffset 35

#define SNAKE_HEAD_DOWN "background-image:url(:/game/snake_head_down.png);"
#define SNAKE_HEAD_LEFT "background-image:url(:/game/snake_head_left.png);"
#define SNAKE_HEAD_RIGHT "background-image:url(:/game/snake_head_right.png);"
#define SNAKE_HEAD_UP "background-image:url(:/game/snake_head_up.png);"
#define SNAKE_BODY_HORIZONTAL "background-image:url(:/game/snake_body_horizontal.png);"
#define SNAKE_BODY_VERTICAL "background-image:url(:/game/snake_body_vertical.png);"
#define SNAKE_TAIL_DOWN "background-image:url(:/game/snake_tail_down.png);"
#define SNAKE_TAIL_LEFT "background-image:url(:/game/snake_tail_left.png);"
#define SNAKE_TAIL_RIGHT "background-image:url(:/game/snake_tail_right.png);"
#define SNAKE_TAIL_UP "background-image:url(:/game/snake_tail_up.png);"
#define SNAKE_TURN_LEFT_DOWN "background-image:url(:/game/snake_turn_left_down.png);"
#define SNAKE_TURN_LEFT_UP "background-image:url(:/game/snake_turn_left_up.png);"
#define SNAKE_TURN_RIGHT_DOWN "background-image:url(:/game/snake_turn_right_down.png);"
#define SNAKE_TURN_RIGHT_UP "background-image:url(:/game/snake_turn_right_up.png);"

#define MOVE_RIGHT 1
#define MOVE_LEFT -1
#define MOVE_DOWN BOARD_LENGTH
#define MOVE_UP -BOARD_LENGTH

#define DATA_FILENAME "data.txt"
#define LOG_FILENAME "log.txt"

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QSlider>
#include <QString>
#include <QVector>
#include <QFile>
#include <QTextEdit>

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);


private slots:
    //Global
    void pressed_musictoggle();
    void pressed_terminaltoggle();
    //keys
    void pressed_key_menu();
    void pressed_key_up();
    void pressed_key_down();
    void pressed_key_left();
    void pressed_key_right();

    //game
    void Game_Update();
    void pressed_playpause();
    void pressed_restart();

    //main menu
    void pressed_startgame();
    void pressed_options();
    void pressed_exit();

    //options
    void pressed_changekeymenu();
    void pressed_changekeyup();
    void pressed_changekeydown();
    void pressed_changekeyleft();
    void pressed_changekeyright();


protected:
    void keyPressEvent(QKeyEvent* event) override; //This function has all the key press logic

private:
   //General
   QStackedWidget* stackedWidget;

   QWidget* mainmenuWidget;
   QWidget* optionsWidget;
   QWidget* gameWidget;

   QVector<int*> dataPointerVector;

   int topscore;

   //Console
   QWidget* console;
   QTextEdit* consoleText;
   QTimer* consoleTimer;
   QFile* logFile;
   QTextStream* logStream;

   //GameTimer
   QTimer* gameTimer;
   int gamespeed;

   //Game
   QMovie* movie;
   QLabel* label;


   QVector<int> gameMap;
   QVector<QVector<QString>> gameLogicBucket;
   QVector<QLabel*> gameMapRectangles;
   QVector<QPair<int,int>> gameMapRectanglesPosition;
   QVector<int> snakeBody;
   QVector<QLabel*> gameSnakeParts;
   QVector<QString> planetURLs;

   QLabel* planetBody;
   QPushButton* button_playpause;
   QPushButton* button_restart;

   int nextmove;
   int snakeCurrentDirection;

   //Terminal
   bool terminal_on;

   //Audio
   QMediaPlayer* player;
   QMediaPlaylist* playlist;

   int volume;
   bool music_on;

   //Key
   bool changekey = false;
   int changeKeyIndex = 0; //We'll change these to later, just declare to make sure memory is allocated.
   QVector<int> keyArray = QVector<int>(5, 0); //Just declaring here to prevent possible weird stuff from happening with the key event at start-up.

   //Global buttons
   QPushButton* button_terminaltoggle;
   QPushButton* button_musictoggle;

   //Main Menu
   QPushButton* button_startgame;
   QPushButton* button_options;
   QPushButton* button_exit;

   QLabel* text_scorelabel;
   QLabel* text_score;
   QLabel* text_versionlabel;
   QLabel* text_version;

   //Options
   QSlider* volumeSlider;
   QSlider* gamespeedSlider;

   QPushButton* button_changekeymenu;
   QPushButton* button_changekeyup;
   QPushButton* button_changekeydown;
   QPushButton* button_changekeyleft;
   QPushButton* button_changekeyright;

   //Functions

   void InitDefaultVals();
   void InitGameTimer();
   void InitMediaPlayer();
   void InitMainMenuWidget();
   void InitOptionsWidget();
   void InitGameWidget();
   void InitConsole();

   void UpdateOptions();

   void SaveData();
   int LoadData();

   void AppDebug(QString str);

   void Game_SpawnPlanet();
   void Game_Init();
   void Game_MoveSnake();
   void Game_GrowSnake();
   void Game_SnakeRevisualize();
   void Game_Over();


};
#endif // MAINWINDOW_H
