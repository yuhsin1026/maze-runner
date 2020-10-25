#include <QApplication>
#include <memory>

#include "config.h"
#include "startwindow.h"
#include "mainwindow.h"
#include "game.h"
#include "world.h"

using namespace std;
class Game;
class World;

int main(int argc, char *argv[])
{
    //int currentExitCode = 0;
    QApplication a(argc, argv);

    shared_ptr <startWindow> start = make_shared<startWindow>();
    start->show();

     //shared_ptr <MainWindow> mainwindow(new MainWindow);
     // mainwindow->init_setup(game);
     //mainwindow->show();
     //mainwindow->allconnect();

     //shared_ptr <Game> game(new Game);

     return a.exec();
}
