#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QDialog>
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QMainWindow>
#include <vector>
#include <memory>
#include "world.h"
#include "config.h"
#include "game.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class World;
class Game;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void    init_setup(std::shared_ptr<Game>);
    void    game_reset(std::shared_ptr<Game>);
    void    allconnect();
    void    disconnectall(std::shared_ptr<Game>&);
    void    keyPressEvent(QKeyEvent *event);
    void    wheelEvent(QWheelEvent *event);
    bool    eventFilter(QObject *object, QEvent *event);
    void    gameVisibility(bool visualise);

signals:
    void keypress(Direction direction);
    void reboot();
    void reset();
    void passXY(int x, int y);

private slots:
    void on_energy_valueChanged(int value);
    void on_health_valueChanged(int value);
    void on_graph_Button_clicked();
    void on_text_Button_clicked();
    void on_MoveNext_clicked();
    void on_manual_clicked();
    void on_automation_clicked();
    void num_changed();
    void receiveStatusUpdate(const QString & status);
    //void receiveGameSettings(int nrOfEnemies, int nrOfHealthPacks);

    void on_comboBox_activated(const QString &arg1);
    void on_replayBox_activated(const QString &arg1);
    void on_AnimationSpeed_sliderReleased();
    void on_weight_sliderReleased();
    void on_save_Button_clicked();
    void on_load_activated(const QString &arg1);
    void on_findPath_clicked();

private:
    int i=0;
    std::shared_ptr<Game> game;
    std::vector<std::shared_ptr<Game>> game_list;
    Ui::MainWindow * ui;

};
#endif // MAINWINDOW_H
