#include "mainwindow.h"
#include <QCoreApplication>
#include <QApplication>
#include <QMovie>
#include <QDateTime>
#include <QRandomGenerator>
#include <QSlider>
#include <QTimer>
#include <QIcon>
#include <QString>
#include <QSoundEffect>
#include <QFile>
#include <QMediaPlayer>
#include <QAudioOutput>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    InitConsole();
    this->setWindowTitle("PSSSSSSSSSSSSSSSSSS");
    //Define stackedWidget
    stackedWidget = new QStackedWidget;

    //Define the various menu widgets
    mainmenuWidget = new QWidget;
    optionsWidget = new QWidget;
    gameWidget = new QWidget;

    //Add menu widgets to the stackedWidget
    stackedWidget->addWidget(mainmenuWidget);
    stackedWidget->addWidget(optionsWidget);
    stackedWidget->addWidget(gameWidget);

    //Configure stackedWidget further
    this->setCentralWidget(stackedWidget); //Set the CentralwWidget of the mainWindow to stackedWidget
    stackedWidget->setCurrentIndex(0); //Set current index to 0(main menu)

    //Global Widgets
    music_on = true;
    button_musictoggle= new QPushButton("", this);
    button_musictoggle->setGeometry(QRect(QPoint(1100, 25),
                                              QSize(48, 48)));
    button_musictoggle->setStyleSheet("QPushButton{background-color:transparent; outline:none; border:0px;} QPushButton:pressed{background-color:transparent; outline:none; border:0px;}");
    button_musictoggle->setIcon(QIcon(":/icons/music_on.png"));
    button_musictoggle->setIconSize(QSize(48, 48));
    connect(button_musictoggle, SIGNAL(released()), this, SLOT(pressed_musictoggle()));

    terminal_on = true;
    button_terminaltoggle= new QPushButton("", this);
    button_terminaltoggle->setGeometry(QRect(QPoint(1025, 25),
                                                 QSize(48, 48)));
    //button_terminaltoggle->setFlat(true);
    button_terminaltoggle->setStyleSheet("QPushButton{background-color:transparent; outline:none; border:0px;} QPushButton:pressed{background-color:transparent; outline:none; border:0px;}");
    button_terminaltoggle->setIcon(QIcon(":/icons/terminal_on.png"));
    button_terminaltoggle->setIconSize(QSize(48, 48));
    connect(button_terminaltoggle, SIGNAL(released()), this, SLOT(pressed_terminaltoggle()));



    InitDefaultVals();
    InitGameTimer();
    InitMediaPlayer();
    InitMainMenuWidget();
    InitOptionsWidget();
    InitGameWidget();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
   {
        if(changekey == true)
        {
            keyArray[changeKeyIndex] = event->key();
            changekey = false;
            AppDebug("KEY CHANGED!");
            UpdateOptions();
            return; //Lazy way to avoid one more else statement
        }

        if(event->key() == keyArray[KEY_MENU])
        {
            pressed_key_menu();
            return; //Lazy way to avoid one more else statement
        }

        if(stackedWidget->currentIndex() == GAME_INDEX)
        {
            if(event->key() == keyArray[KEY_UP])
            {
                pressed_key_up();
            }

            else if(event->key() == keyArray[KEY_DOWN])
            {
                pressed_key_down();
            }

            else if(event->key() == keyArray[KEY_LEFT])
            {
                pressed_key_left();
            }

            else if(event->key() == keyArray[KEY_RIGHT])
            {
                pressed_key_right();
            }
        }
    }
}

void MainWindow::InitDefaultVals()
{
    //Default configuration of non-defined variables.
    volume = 50;
    gamespeed = 20;
    changeKeyIndex = 0;
    topscore = 0;

    //Default key init
    keyArray[KEY_MENU] = Qt::Key_Escape;
    keyArray[KEY_UP] = Qt::Key_W;
    keyArray[KEY_DOWN] = Qt::Key_S;
    keyArray[KEY_LEFT] = Qt::Key_A;
    keyArray[KEY_RIGHT] = Qt::Key_D;

    dataPointerVector.push_back(&keyArray[KEY_MENU]);
    dataPointerVector.push_back(&keyArray[KEY_UP]);
    dataPointerVector.push_back(&keyArray[KEY_DOWN]);
    dataPointerVector.push_back(&keyArray[KEY_LEFT]);
    dataPointerVector.push_back(&keyArray[KEY_RIGHT]);
    dataPointerVector.push_back(&volume);
    dataPointerVector.push_back(&gamespeed);
    dataPointerVector.push_back(&topscore);

    if(LoadData() == 0) //If load data failed
    {
        AppDebug("No load data found or load failed, trying to save and retry loading");
        SaveData();
        if(LoadData() == 0)
        {
            AppDebug("CRITICAL ERROR: LOAD OR SAVE FAILED");
            exit(0); //If still can't load data after saving it, then there's obviously an issue.
        }
    }

    AppDebug("Data loaded succesfully");
}

void MainWindow::InitGameTimer()
{
    //Init the game timer stuff
    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &MainWindow::Game_Update);
    gameTimer->setInterval((int)20000/gamespeed);
}

void MainWindow::InitMediaPlayer()
{
    QRandomGenerator prng1(QDateTime::currentMSecsSinceEpoch() / 1000); //Seed to make sure starting song is RNG
    music_list = QVector<QUrl>(20);
    media_player = new QMediaPlayer;
    audio_output = new QAudioOutput;
    media_player->setAudioOutput(audio_output);

    for(int i=0; i<20; i++)
    {
        QString tmpString = QApplication::applicationDirPath() + QString(MUSIC_DIR) + "music" + QString::number(i) + ".mp3";
        music_list[i] = QUrl::fromLocalFile(tmpString);
    }

    media_player->setSource(music_list[prng1.generate() % 20]);
    audio_output->setVolume(volume);
    media_player->setLoops(1);
    media_player->play();
    connect(media_player, &QMediaPlayer::loopsChanged, this, &MainWindow::RandomizeSoundEffect);
}

void MainWindow::InitMainMenuWidget()
{
    // MAIN MENU WIDGET
    QMovie* movie=new QMovie(":/images/background.gif");
    QLabel* label = new QLabel(this->mainmenuWidget);
    label->setMovie(movie);
    movie->start();

    button_startgame = new QPushButton("Start Game", this->mainmenuWidget);
    button_startgame->setGeometry(QRect(QPoint(150, 100),
    QSize(150, 75)));
    connect(button_startgame, SIGNAL(released()), this, SLOT(pressed_startgame()));

    button_options = new QPushButton("Options", this->mainmenuWidget);
    button_options->setGeometry(QRect(QPoint(150, 200),
    QSize(150, 75)));
    connect(button_options, SIGNAL(released()), this, SLOT(pressed_options()));

    button_exit = new QPushButton("Exit", this->mainmenuWidget);
    button_exit->setGeometry(QRect(QPoint(150, 300),
    QSize(150, 75)));
    connect(button_exit, SIGNAL(released()), this, SLOT(pressed_exit()));

    // Style only the QPushButtons of mainmenuWidget
    mainmenuWidget->setStyleSheet("QPushButton{background-color: transparent;border: 2px solid #9b009d;color: #fde2fe;padding: 16px 32px;text-align: center;text-decoration: none;font-size: 16px;margin: 4px 2px;} QPushButton:hover{background-color: #9b009d;color: #fde2fe;}");

    //Version display
    text_versionlabel = new QLabel("Version:", this->mainmenuWidget);
    text_versionlabel->setGeometry(QRect(QPoint(20, 0),
    QSize(100, 50)));
    text_versionlabel->setStyleSheet("color: #fde2fe");

    text_version = new QLabel(CURRENT_VERSION, this->mainmenuWidget);
    text_version->setGeometry(QRect(QPoint(80, 0),
    QSize(100, 50)));
    text_version->setStyleSheet("color: #fde2fe");

    //Top score display
    text_scorelabel = new QLabel("Top Score:", this->mainmenuWidget);
    text_scorelabel->setGeometry(QRect(QPoint(20, 15),
    QSize(100, 50)));
    text_scorelabel->setStyleSheet("color: #fde2fe");

    text_score = new QLabel(QString::number(topscore), this->mainmenuWidget);
    text_score->setGeometry(QRect(QPoint(95, 15),
    QSize(100, 50)));
    text_score->setStyleSheet("color: #fde2fe");
}

void MainWindow::InitOptionsWidget()
{
    //OPTIONS WIDGET
    optionsWidget->setObjectName("Options-Menu"); //We need to define a name to only apply border image to one element
    optionsWidget->setStyleSheet("#Options-Menu {border-image: url(:/images/options.jpg);} QPushButton{background-color: transparent;border: 2px solid #9b009d;color: #fde2fe;padding: 16px 32px;text-align: center;text-decoration: none;font-size: 16px;margin: 4px 2px;} QPushButton:hover{background-color: #9b009d;color: #fde2fe;}"); //Apply border image to main menu only, not it's children

    volumeSlider = new QSlider(Qt::Horizontal, this->optionsWidget);
    volumeSlider->setGeometry(QRect(QPoint(170,320),
    QSize(100, 100)));
    volumeSlider->setValue(volume);
    volumeSlider->setMinimum(0);
    volumeSlider->setMaximum(100);
    connect(volumeSlider, &QAbstractSlider::valueChanged, this, [this](int value) { volume = value; audio_output->setVolume(volume); AppDebug("Volume changed");});
    connect(volumeSlider, &QAbstractSlider::sliderReleased, this, &MainWindow::SaveData);

    gamespeedSlider = new QSlider(Qt::Horizontal, this->optionsWidget);
    gamespeedSlider->setGeometry(QRect(QPoint(235,390),
    QSize(100, 100)));
    gamespeedSlider->setValue(gamespeed);
    gamespeedSlider->setMinimum(1);
    gamespeedSlider->setMaximum(100);
    connect(gamespeedSlider, &QAbstractSlider::valueChanged, this, [this](int value) {gamespeed = value; gameTimer->setInterval((int)20000/gamespeed); AppDebug("Gamespeed changed");}); //lambdas ftw, doesn't work normally idk why
    connect(gamespeedSlider, &QAbstractSlider::sliderReleased, this, &MainWindow::SaveData);
    //Do not remove "this" from upper connects, while it may work without, then whatever impacts the slider may also impact the lambda function during execution.

    button_changekeymenu = new QPushButton(QKeySequence(keyArray[KEY_MENU]).toString(), this->optionsWidget);
    button_changekeymenu->setGeometry(QRect(QPoint(350, 100), //150, 200
    QSize(50, 30)));
    connect(button_changekeymenu, SIGNAL(released()), this, SLOT(pressed_changekeymenu()));

    button_changekeyup = new QPushButton(QKeySequence(keyArray[KEY_UP]).toString(), this->optionsWidget);
    button_changekeyup->setGeometry(QRect(QPoint(350, 140),
    QSize(50, 30)));
    connect(button_changekeyup, SIGNAL(released()), this, SLOT(pressed_changekeyup()));

    button_changekeydown = new QPushButton(QKeySequence(keyArray[KEY_DOWN]).toString(), this->optionsWidget);
    button_changekeydown->setGeometry(QRect(QPoint(350, 180),
    QSize(50, 30)));
    connect(button_changekeydown, SIGNAL(released()), this, SLOT(pressed_changekeydown()));

    button_changekeyleft = new QPushButton(QKeySequence(keyArray[KEY_LEFT]).toString(), this->optionsWidget);
    button_changekeyleft->setGeometry(QRect(QPoint(350, 220),
    QSize(50, 30)));
    connect(button_changekeyleft, SIGNAL(released()), this, SLOT(pressed_changekeyleft()));

    button_changekeyright = new QPushButton(QKeySequence(keyArray[KEY_RIGHT]).toString(), this->optionsWidget);
    button_changekeyright->setGeometry(QRect(QPoint(350, 260),
    QSize(50, 30)));
    connect(button_changekeyright, SIGNAL(released()), this, SLOT(pressed_changekeyright()));
}

void MainWindow::InitGameWidget()
{
    movie=new QMovie(":/images/game_background.gif");
    label = new QLabel(this->gameWidget);
    label->setMovie(movie);
    movie->start();

    button_playpause = new QPushButton("", this->gameWidget);
    button_playpause->setGeometry(QRect(QPoint(1100, 100),
                                              QSize(48, 48)));
    button_playpause->setStyleSheet("QPushButton{background-color:transparent; outline:none; border:0px;} QPushButton:pressed{background-color:transparent; outline:none; border:0px;}");
    button_playpause->setIcon(QIcon(":/icons/pause.png"));
    button_playpause->setIconSize(QSize(48, 48));
    connect(button_playpause, SIGNAL(released()), this, SLOT(pressed_playpause()));

    button_restart = new QPushButton("", this->gameWidget);
    button_restart->setGeometry(QRect(QPoint(1025, 100),
                                                 QSize(48, 48)));
    button_restart->setStyleSheet("QPushButton{background-color:transparent; outline:none; border:0px;} QPushButton:pressed{background-color:transparent; outline:none; border:0px;}");
    button_restart->setIcon(QIcon(":/icons/restart.png"));
    button_restart->setIconSize(QSize(48, 48));
    connect(button_restart, SIGNAL(released()), this, SLOT(pressed_restart()));

    //Init game rectangles and their positions
    gameMapRectanglesPosition = QVector<QPair<int,int>>(BOARD_SIZE);
    gameMapRectangles = QVector<QLabel*>(BOARD_SIZE);
    gameSnakeParts = QVector<QLabel*>(BOARD_SIZE);
    for(int y=0; y<BOARD_LENGTH; y++)
    {
        for(int x=0; x<BOARD_LENGTH; x++)
        {
            gameMapRectangles[y*BOARD_LENGTH+x] = new QLabel("", this->gameWidget);
            gameMapRectangles[y*BOARD_LENGTH+x]->setGeometry(QRect(QPoint(x*RECT_SIZE+XOffset, y*RECT_SIZE+YOffset),
                                                                                QSize(RECT_SIZE-1, RECT_SIZE-1)));
            gameSnakeParts[y*BOARD_LENGTH+x] = new QLabel("", this->gameWidget);
            gameSnakeParts[y*BOARD_LENGTH+x]->setGeometry(QRect(QPoint(x*RECT_SIZE+XOffset, y*RECT_SIZE+YOffset),
                                                                                QSize(RECT_SIZE, RECT_SIZE)));
            gameMapRectanglesPosition[y*BOARD_LENGTH+x].first = x*RECT_SIZE+XOffset;
            gameMapRectanglesPosition[y*BOARD_LENGTH+x].second = y*RECT_SIZE+YOffset;
            gameMapRectangles[y*BOARD_LENGTH+x]->setStyleSheet("border-style:solid;border:dashed #00f7ff;border-width: 2px 2px 2px 2px;");
        }
    }

    //Load planet images into vector
    planetURLs = QVector<QString>(NUM_PLANETS, "");
    for(int i=0; i<NUM_PLANETS;i++)
    {
        planetURLs[i] = ":/game/planet" + QString::number(i) + ".png";
    }


    //Init planet
    planetBody = new QLabel("", this->gameWidget);
    planetBody->setGeometry(QRect(QPoint(0, 0),
                                 QSize(PLANET_SIZE, PLANET_SIZE)));
    //Init bucket for quick logic
    gameLogicBucket = QVector<QVector<QString>>(BOARD_LENGTH * 2 + 1, QVector<QString>(BOARD_LENGTH * 2 + 1)); //Make sure bucket has enough space

    gameLogicBucket[MOVE_RIGHT + BOARD_LENGTH][MOVE_UP + BOARD_LENGTH] = SNAKE_TURN_RIGHT_UP;
    gameLogicBucket[MOVE_RIGHT + BOARD_LENGTH][MOVE_LEFT + BOARD_LENGTH] = SNAKE_BODY_HORIZONTAL;
    gameLogicBucket[MOVE_RIGHT + BOARD_LENGTH][MOVE_DOWN + BOARD_LENGTH] = SNAKE_TURN_RIGHT_DOWN;

    gameLogicBucket[MOVE_LEFT + BOARD_LENGTH][MOVE_UP + BOARD_LENGTH] = SNAKE_TURN_LEFT_UP;
    gameLogicBucket[MOVE_LEFT + BOARD_LENGTH][MOVE_RIGHT + BOARD_LENGTH] = SNAKE_BODY_HORIZONTAL;
    gameLogicBucket[MOVE_LEFT + BOARD_LENGTH][MOVE_DOWN + BOARD_LENGTH] = SNAKE_TURN_LEFT_DOWN;

    gameLogicBucket[MOVE_UP + BOARD_LENGTH][MOVE_RIGHT + BOARD_LENGTH] = SNAKE_TURN_RIGHT_UP;
    gameLogicBucket[MOVE_UP + BOARD_LENGTH][MOVE_LEFT + BOARD_LENGTH] = SNAKE_TURN_LEFT_UP;
    gameLogicBucket[MOVE_UP + BOARD_LENGTH][MOVE_DOWN + BOARD_LENGTH] = SNAKE_BODY_VERTICAL;

    gameLogicBucket[MOVE_DOWN + BOARD_LENGTH][MOVE_UP + BOARD_LENGTH] = SNAKE_BODY_VERTICAL;
    gameLogicBucket[MOVE_DOWN + BOARD_LENGTH][MOVE_LEFT + BOARD_LENGTH] = SNAKE_TURN_LEFT_DOWN;
    gameLogicBucket[MOVE_DOWN + BOARD_LENGTH][MOVE_RIGHT + BOARD_LENGTH] = SNAKE_TURN_RIGHT_DOWN;

}

void MainWindow::InitConsole()
{
    console = new QWidget();
    console->setWindowTitle("Console Output");
    consoleText = new QTextEdit(console);
    consoleText->setMinimumSize(QSize(450,450));
    console->show();

    logFile = new QFile(LOG_FILENAME);
    logFile->open(QIODevice::WriteOnly);

    if(!logFile->isOpen()){
        AppDebug("Error, unable to open log file:" DATA_FILENAME);
        exit(0);
    }

    logStream = new QTextStream(logFile);

    consoleTimer = new QTimer(console);
    connect(consoleTimer, &QTimer::timeout, console, [&]() {if(!stackedWidget->isVisible()) {logFile->close();exit(0);}}); //Make sure console closes if program closes via "X" button at the top.
    consoleTimer->start(350);


}

void MainWindow::UpdateOptions()
{
    button_changekeymenu->setText(QKeySequence(keyArray[KEY_MENU]).toString());
    button_changekeyup->setText(QKeySequence(keyArray[KEY_UP]).toString());
    button_changekeydown->setText(QKeySequence(keyArray[KEY_DOWN]).toString());
    button_changekeyleft->setText(QKeySequence(keyArray[KEY_LEFT]).toString());
    button_changekeyright->setText(QKeySequence(keyArray[KEY_RIGHT]).toString());
    SaveData();
    AppDebug("UpdateOptions() called");
}

void MainWindow::SaveData()
{
    QFile outputFile(DATA_FILENAME);
    outputFile.open(QIODevice::WriteOnly);

    if(!outputFile.isOpen()){
        AppDebug("Error, unable to open data file: " DATA_FILENAME);
        exit(0);
    }

    QTextStream outStream(&outputFile);
    for(int i=0; i<dataPointerVector.size(); i++)
    {
        outStream << *dataPointerVector[i] << "\n";
    }

    outputFile.close();

    AppDebug("SaveData() called");
}

int MainWindow::LoadData()
{
    QFile file(DATA_FILENAME);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return 0;
    }

    int index=0;

    while (!file.atEnd())
    {
        QString line = file.readLine();
        line.remove(line.size()-1, 1);
        if(index < dataPointerVector.size())
            *(dataPointerVector[index++]) = line.toInt();

    }
    file.close();

    AppDebug("LoadData() called");
    return 1;
}

void MainWindow::AppDebug(QString str)
{
    consoleText->append(str);
    *logStream << str << "\n";
}

void MainWindow::pressed_startgame()
{
    gameTimer->start();
    Game_Init();
    stackedWidget->setCurrentIndex(GAME_INDEX);
    AppDebug("pressed_startgame() called");
}
void MainWindow::pressed_options()
{
    stackedWidget->setCurrentIndex(OPTIONS_INDEX);
    AppDebug("pressed_options() called");
}

void MainWindow::pressed_exit()
{
   while(!console->close())
   {
   }
   delete(console);
   exit(0);
}

void MainWindow::pressed_musictoggle()
{
    if(music_on)
    {
        button_musictoggle->setIcon(QIcon(":/icons/music_off.png"));
        media_player->stop();
        music_on = false;
        AppDebug("pressed_musictroggle() called");
    }

    else
    {
        button_musictoggle->setIcon(QIcon(":/icons/music_on.png"));
        media_player->play();
        music_on = true;
        AppDebug("pressed_musictroggle() called");
    }
}

void MainWindow::pressed_terminaltoggle()
{
    if(terminal_on)
    {
        button_terminaltoggle->setIcon(QIcon(":/icons/terminal_off.png"));
        console->hide();
        terminal_on = false;
    }

    else
    {
        button_terminaltoggle->setIcon(QIcon(":/icons/terminal_on.png"));
        console->show();
        terminal_on = true;
    }

    AppDebug("pressed_terminaltroggle() called");
}

void MainWindow::pressed_key_menu()
{
   gameTimer->stop();
   stackedWidget->setCurrentIndex(MENU_INDEX);
   AppDebug("pressed_key_menu() called");
}

void MainWindow::pressed_key_up()
{
   if(snakeCurrentDirection != MOVE_DOWN)
         nextmove = MOVE_UP;
   AppDebug("NEXT MOVE IS: " + QString::number(nextmove));
   AppDebug("pressed_key_up() called");
}

void MainWindow::pressed_key_down()
{
   if(snakeCurrentDirection != MOVE_UP)
       nextmove = MOVE_DOWN;
   AppDebug("NEXT MOVE IS: " + QString::number(nextmove));
   AppDebug("pressed_key_down() called");
}

void MainWindow::pressed_key_left()
{
   if(snakeCurrentDirection != MOVE_RIGHT)
        nextmove = MOVE_LEFT;
   AppDebug("NEXT MOVE IS: " + QString::number(nextmove));
   AppDebug("pressed_key_left() called");
}

void MainWindow::pressed_key_right()
{
   if(snakeCurrentDirection != MOVE_LEFT)
        nextmove = MOVE_RIGHT;
   AppDebug("NEXT MOVE IS: " + QString::number(nextmove));
   AppDebug("pressed_key_right() called");
}

void MainWindow::pressed_changekeymenu()
{
    changekey = true;
    changeKeyIndex = KEY_MENU;
    button_changekeymenu->setText("...");
    AppDebug("pressed_changekeymenu() called");
}

void MainWindow::pressed_changekeyup()
{
    changekey = true;
    changeKeyIndex = KEY_UP;
    button_changekeyup->setText("...");
    AppDebug("pressed_changekeyup() called");
}

void MainWindow::pressed_changekeydown()
{
    changekey = true;
    changeKeyIndex = KEY_DOWN;
    button_changekeydown->setText("...");
    AppDebug("pressed_changekeydown() called");
}

void MainWindow::pressed_changekeyleft()
{
    changekey = true;
    changeKeyIndex = KEY_LEFT;
    button_changekeyleft->setText("...");
    AppDebug("pressed_changekeyleft() called");
}

void MainWindow::pressed_changekeyright()
{
    changekey = true;
    changeKeyIndex = KEY_RIGHT;
    button_changekeyright->setText("...");
    AppDebug("pressed_changekeyright() called");
}

void MainWindow::pressed_playpause()
{
    if(gameTimer->isActive())
    {
        gameTimer->stop();
        button_playpause->setIcon(QIcon(":/icons/play.png"));
        AppDebug("Game set to play");
    }

    else
    {
        gameTimer->start((int)20000/gamespeed);
        button_playpause->setIcon(QIcon(":/icons/pause.png"));
        AppDebug("Game set to pause");
    }

    AppDebug("pressed_playpause() called");
}

void MainWindow::pressed_restart()
{
    Game_Init();
    Game_Update();
}

void MainWindow::Game_Init()
{
    if(gameMap.size())
    {
        gameMap.clear();
    }

    if(snakeBody.size())
    {
        snakeBody.clear();
    }


    //Init gameMap, set defaults for some variables
    gameMap = QVector<int>(BOARD_SIZE, 0);
    nextmove = MOVE_RIGHT;


    //Init snake body's length and set it on the map
    snakeBody = QVector<int>(3) = {BOARD_SIZE/2 + BOARD_LENGTH/2, BOARD_SIZE/2 + BOARD_LENGTH/2 - 1, BOARD_SIZE/2 + BOARD_LENGTH/2 - 2};

    for(int i=0; i<snakeBody.size(); i++)
    {
        gameMap[snakeBody[i]] = SNAKE;
    }

    for(int i=0; i<gameSnakeParts.size(); i++)
    {
        gameSnakeParts[i]->close();
    }

    gameSnakeParts[snakeBody[0]]->setStyleSheet(SNAKE_HEAD_RIGHT);
    gameSnakeParts[snakeBody[0]]->show();
    gameSnakeParts[snakeBody[1]]->setStyleSheet(SNAKE_BODY_HORIZONTAL);
    gameSnakeParts[snakeBody[1]]->show();
    gameSnakeParts[snakeBody[2]]->setStyleSheet(SNAKE_TAIL_LEFT);
    gameSnakeParts[snakeBody[2]]->show();

    snakeCurrentDirection = MOVE_RIGHT;

    Game_SpawnPlanet();
    AppDebug("Game_Init() called");
}

void MainWindow::Game_SpawnPlanet()
{
    QVector<int> tempVector;
    QRandomGenerator prng1(QDateTime::currentMSecsSinceEpoch() / 1000);
    QString planetImageURL = planetURLs[prng1.generate() % NUM_PLANETS];

    for(int i=0; i<gameMap.size(); i++)
    {
        if(gameMap[i] == 0)
        {
            tempVector.push_back(i);
        }
    }

    if(!tempVector.size())
    {
        Game_Over(); //ggwp
    }

    int index = tempVector[prng1.generate() % tempVector.size()];
    prng1.seed((QDateTime::currentMSecsSinceEpoch() / 1000));
    gameMap[index] = PLANET;
    planetBody->move(gameMapRectanglesPosition[index].first, gameMapRectanglesPosition[index].second);
    planetBody->setStyleSheet("background-image: url(" + planetImageURL + ")");
    AppDebug("Game_SpawnPlanet() called");
}

void MainWindow::Game_Update()
{
    if(snakeBody[0] + nextmove < 0 || snakeBody[0] + nextmove >= BOARD_SIZE)
    {
        Game_Over();
        return;
    }

    if(snakeBody[0] % BOARD_LENGTH == 0 && nextmove == MOVE_LEFT)
    {
        Game_Over();
        return;
    }

    if(snakeBody[0] % BOARD_LENGTH == (BOARD_LENGTH - 1) && nextmove == MOVE_RIGHT)
    {
        Game_Over();
        return;
    }

    switch(gameMap[snakeBody[0] + nextmove])
    {
        case EMPTY:
            Game_MoveSnake();
            Game_SnakeRevisualize();
            break;

        case SNAKE:
            if(snakeBody[0] + nextmove != snakeBody[snakeBody.size() - 1])
            {
                Game_Over();
            }

            else
            {
                Game_MoveSnake();
                Game_SnakeRevisualize();
            }
            break;

        case PLANET:
            Game_GrowSnake();
            Game_SnakeRevisualize();
            break;
    }

    AppDebug("Game_Update() called, GAME TICK!");
}

void MainWindow::Game_MoveSnake()
{
    const int N = snakeBody.size();
    gameSnakeParts[ snakeBody[N - 1] ]->close(); //Hide last label of snake's body
    gameMap[snakeBody[N - 1]] = EMPTY;

    for(int i=N-1; i>=1; i--)
    {
        snakeBody[i] = snakeBody[i-1]; //move all parts of snake ahead to the ones in front of them, except the head
    }

    snakeBody[0] += nextmove; //move head towards next square
    gameMap[snakeBody[0]] = SNAKE;
    snakeCurrentDirection = nextmove;

    AppDebug("Game_MoveSnake() called, GAME TICK!");
}

void MainWindow::Game_GrowSnake()
{
    int tmp = snakeBody[snakeBody.size() - 1];
    Game_MoveSnake();
    snakeBody.push_back(tmp);
    gameSnakeParts[tmp]->show();
    gameMap[tmp] = SNAKE;

    if(snakeBody.size() > topscore)
    {
        topscore = snakeBody.size();
        SaveData();
        text_score->setText(QString::number(topscore));
    }

    Game_SpawnPlanet();

    AppDebug("Game_GrowSnake() called, GAME TICK!");
}

void MainWindow::Game_SnakeRevisualize()
{
    //Make sure all snake bodies have proper images
    const int N = snakeBody.size();

    switch(snakeBody[0] - snakeBody[1])
    {
        case MOVE_RIGHT:
            gameSnakeParts[snakeBody[0]]->setStyleSheet(SNAKE_HEAD_RIGHT);
            break;

        case MOVE_LEFT:
            gameSnakeParts[snakeBody[0]]->setStyleSheet(SNAKE_HEAD_LEFT);
            break;

        case MOVE_DOWN:
            gameSnakeParts[snakeBody[0]]->setStyleSheet(SNAKE_HEAD_DOWN);
            break;

        case MOVE_UP:
            gameSnakeParts[snakeBody[0]]->setStyleSheet(SNAKE_HEAD_UP);
            break;
    }

    gameSnakeParts[snakeBody[0]]->show();

    for(int i=1; i<(N - 1); i++)
    {
        gameSnakeParts[snakeBody[i]]->setStyleSheet(gameLogicBucket[ snakeBody[i-1] - snakeBody[i] + BOARD_LENGTH][ snakeBody[i+1] - snakeBody[i] + BOARD_LENGTH]);
    }

    switch(snakeBody[N-1] - snakeBody[N-2])
    {
        case MOVE_UP:
            gameSnakeParts[snakeBody[N-1]]->setStyleSheet(SNAKE_TAIL_UP);
            break;

        case MOVE_DOWN:
            gameSnakeParts[snakeBody[N-1]]->setStyleSheet(SNAKE_TAIL_DOWN);
            break;

        case MOVE_LEFT:
            gameSnakeParts[snakeBody[N-1]]->setStyleSheet(SNAKE_TAIL_LEFT);
            break;

        case MOVE_RIGHT:
            gameSnakeParts[snakeBody[N-1]]->setStyleSheet(SNAKE_TAIL_RIGHT);
            break;
    }

    AppDebug("Game_SnakeRevisualize() called, GAME TICK!");
}

void MainWindow::Game_Over()
{
    gameTimer->stop();
    stackedWidget->setCurrentIndex(MENU_INDEX);
    AppDebug("Game_Over() called");
}

void MainWindow::RandomizeSoundEffect()
{
    if (!media_player->loops())
    {
        QRandomGenerator prng1(QDateTime::currentMSecsSinceEpoch() / 1000);
        media_player->setSource(music_list[prng1.generate() % 20]);
        media_player->setLoops(1);
        if(music_on)
        {
            media_player->play();
        }
    }
}



