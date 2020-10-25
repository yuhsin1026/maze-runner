#include <QDebug>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QStackedWidget>
#include <QElapsedTimer>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "config.h"

//using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //this->setFocus();
    //gameVisibility(true);
}

void    MainWindow::game_reset(std::shared_ptr<Game> g)
{
    game = g;
    ui->graphicsView->setScene(g->view->g_scene.get());     //Launch graph view @start
    ui->TextView->setScene(g->view->t_scene.get());         //Disable text view @start
    ui->protView->setScene(g->view->prot_scene.get());
    ui->enemy_left->display(static_cast<int>(game->enemies.size()));
    ui->num_defeat->display(static_cast<int>(game->dead_enemies.size()));
    ui->hp_left->display(static_cast<int>(game->healthpacks.size()));
    game->setSpeed(500/ui->AnimationSpeed->value());
    game->pathfinder->setWeight(ui->weight->value());
    game->view->display_graph();
    game->heightMountain();      //Calculate mountains around /P\ and call view->display_text()
    ui->energy->setValue(100);
    ui->health->setValue(100);
    ui->PauseWindow->hide();
}
void    MainWindow::init_setup(std::shared_ptr<Game> g)
{
    game = g;
    qreal scale_x = NUM_HOR_TILES/static_cast<qreal>(game->world->getCols());
    qreal scale_y = NUM_VER_TILES/static_cast<qreal>(game->world->getRows());
    qreal scale=scale_x<=scale_y?scale_x:scale_y;
    // CREATE VIEWS/SCENES (Graphic, text, agonist)
    ui->graphicsView->setScene(g->view->g_scene.get());     //Launch graph view @start
    ui->graphicsView->scale(scale,scale);
    ui->TextView->setScene(g->view->t_scene.get());         //Disable text view @start
    ui->TextView->hide();
    ui->protView->setScene(g->view->prot_scene.get());
    ui->protView->hide();
    ui->protLabel->hide();
    ui->PauseWindow->hide();

    //ui->graphicsView->setSceneRect(0,0,(TILE_SIZE)*(game->world->getRows()),(TILE_SIZE)*(game->world->getCols()));


    // SETTINGS UI ELEMENTS
    QPalette p = ui->health->palette();
    p.setColor(QPalette::Highlight, Qt::red);
    ui->health->setPalette(p);
    ui->graph_Button->setEnabled(false);
    ui->manual->setEnabled(false);
    ui->energy->setValue(100);
    ui->health->setValue(100);
    ui->enemy_left->display(static_cast<int>(game->enemies.size()));
    ui->num_defeat->display(static_cast<int>(game->dead_enemies.size()));
    ui->hp_left->display(static_cast<int>(game->healthpacks.size()));
    ui->AnimationSpeed->setRange(1,10);
    ui->AnimationSpeed->setValue(5);
    ui->weight->setValue(70);
    ui->weight->setRange(0,100);


    game->view->display_graph();
    game->heightMountain();      //Calculate mountains around /P\ and call view->display_text()

    ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    ui->graphicsView->viewport()->installEventFilter(this);

}

void MainWindow::allconnect()
{
     /* CONNECT SIGNAL - SLOTS */
     connect(game.get(), SIGNAL(changePos(Direction)), game->my_protagonist.get(), SLOT(mov(Direction)));   //game::ChangePos           -> warriormodel::movInt
     connect(this, SIGNAL(keypress(Direction)), game.get(), SLOT(keyPress(Direction)));                     //mainWindow::keyPress      -> game::keyPress
     connect(game.get(),SIGNAL(changeEnergy(int)),this,SLOT(on_energy_valueChanged(int)));                  //game::changeEnergy        -> mainWindow::on_energvalueChanged
     connect(game.get(),SIGNAL(changeHealth(int)),this,SLOT(on_health_valueChanged(int)));                  //game::changeHealth       -> mainWindow::on_health_valueChanged
     connect(game.get(),SIGNAL(changeNum()),this,SLOT(num_changed()));                                      //game::changeNum           -> mainWindow::num_changed
     connect(game.get(),SIGNAL(statusUpdate(QString)),this,SLOT(receiveStatusUpdate(QString)));             //game::statusUpdate        -> mainWindow::receiveStatusUpdate
}
void MainWindow::disconnectall(std::shared_ptr<Game>&g)
{
    /* CONNECT SIGNAL - SLOTS */
    g->disconnect(SIGNAL(changePos(Direction)), g->my_protagonist.get(), SLOT(mov(Direction)));     //game::ChangePos           -> warriormodel::movInt
    g->disconnect(SIGNAL(changeEnergy(int)),this,SLOT(on_energy_valueChanged(int)));                //game::changeEnergy        -> mainWindow::on_energvalueChanged
    g->disconnect(SIGNAL(changeHealth(int)),this,SLOT(on_health_valueChanged(int)));                //game:: changeHealth       -> mainWindow:: on_health_valueChanged
    g->disconnect(SIGNAL(changeNum()),this,SLOT(num_changed()));                                    //game::changeNum           -> mainWindow::num_changed
    g->disconnect(SIGNAL(statusUpdate(QString)),this,SLOT(receiveStatusUpdate(QString)));           //game::statusUpdate        -> mainWindow::receiveStatusUpdate
    this->disconnect(SIGNAL(keypress(Direction)), g.get(), SLOT(keyPress(Direction)));              //mainWindow::keyPress      -> game::keyPress

}
// PROCESS MOUSE WHEEL ACTIVITY
void MainWindow::wheelEvent(QWheelEvent *event)
{
    if(ui->TextView->isHidden())
    {
    if(event->delta()>0)
        ui->graphicsView->scale(1.1,1.1);
    else
        ui->graphicsView->scale(0.9,0.9);
    }
}
bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->graphicsView->viewport() && event->type() == QEvent::Wheel) {
        event->ignore();
        return true;
    }
    if (object == ui->graphicsView->viewport() && event->type() == QEvent::MouseButtonRelease) {
        event->ignore();
        return true;
    }
    return false;
}


// PROCESS KEY PRESSES
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    //SWITCH VIEW
    if(event->key() == Qt::Key_T)
        on_text_Button_clicked();
    if(event->key() == Qt::Key_G)
        on_graph_Button_clicked();


    // PAUSE THE GAME
    if(game->getPause() == PAUSE)
    {
        game->setPause(RESUME);
        game->move_to_next();
        ui->PauseWindow->hide();
        ui->comboBox->setEnabled(true);
        return;
    }
    if(event->key()==Qt::Key_P)
    {
        game->setPause(PAUSE);
        ui->PauseWindow->show();
        ui->comboBox->setEnabled(false);
    }

    //MOVE
    if((ui->automation->isEnabled()))
    switch (event->key())
    {
    case Qt::Key_J:
        emit keypress(Direction::LEFT);
        break;

    case Qt::Key_L:
        emit keypress(Direction::RIGHT);
        break;

    case Qt::Key_I:
        emit keypress(Direction::UP);
        break;

    case Qt::Key_K:
        emit keypress(Direction::DOWN);
        break;
    default:
        break;
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}

// ----------------- SLOTS -----------------------//
// SHOW GRAPH VIEW
void MainWindow::on_graph_Button_clicked()
{
    ui->TextView->hide();
    ui->protView->hide();
    ui->protLabel->hide();
    ui->graphicsView->show();
    ui->graph_Button->setEnabled(false);
    ui->text_Button->setEnabled(true);
}

// SHOW TEXT VIEW
void MainWindow::on_text_Button_clicked()
{
    ui->graphicsView->hide();
    ui->TextView->show();
    ui->protLabel->show();
    ui->protView->show();
    ui->graph_Button->setEnabled(true);
    ui->text_Button->setEnabled(false);
}


void MainWindow::on_automation_clicked()
{
    if(game->getDisconnected())
        return;
    ui->manual->setEnabled(false);
    ui->automation->setEnabled(false);
    ui->MoveNext->setEnabled(false);
    ui->comboBox->setEnabled(false);
    game->setmode(AUTO_FINISH);
    game->auto_move();
}
void MainWindow::on_manual_clicked()
{
    ui->automation->setEnabled(true);
    ui->manual->setEnabled(false);
}
// MAKE PROT MOVE TO NEXT
void MainWindow::on_MoveNext_clicked()
{
    game->move_to_next();
}

// UPDATE #HP, # ENEMIES, # DEFEATED, HEALTH, ENERGY
void MainWindow::num_changed()
{
    ui->enemy_left->display(static_cast<int>(game->enemies.size()));
    if(static_cast<int>(game->enemies.size()) == 0)
    {
        receiveStatusUpdate(QString("You won! Click reset or restart to play another game."));
    }
    ui->num_defeat->display(static_cast<int>(game->dead_enemies.size()));

    int count=0;
    for(auto &temp:game->healthpacks)
        if(!temp->getUsed())
            count++;
    ui->hp_left->display(count);
}
void MainWindow::on_energy_valueChanged(int value)
{
    ui->energy->setValue(value);
}
void MainWindow::on_health_valueChanged(int  value)
{
    ui->health->setValue(value);
}


// FIND NEXT ENEMY / HP
void MainWindow::on_comboBox_activated(const QString &arg1)
{
    if(arg1=="Find Next Enemy")
    {
        QElapsedTimer timer;
        timer.start();
        if(game->go_nearest_enemy() != nullptr)
        game->draw_route();
        qDebug() << timer.elapsed();
    }
    if(arg1=="Find Next Healthpack")
    {
        QElapsedTimer hpTimer;
        hpTimer.start();
        if(game->go_nearest_hp() != nullptr)
        game->draw_route();
        qDebug() << hpTimer.elapsed();
    }
}

void MainWindow::gameVisibility(bool visualise)
{
    if(!visualise)
        ui->centralwidget->hide();
    else
        ui->centralwidget->show();
}


void MainWindow::on_replayBox_activated(const QString &arg1)
{
    if(arg1=="Reset")
    {
        game->route.clear();
        game->route_tile.clear();
        ui->MoveNext->setEnabled(true);
        ui->automation->setEnabled(true);
        ui->comboBox->setEnabled(true);
        ui->manual->setEnabled(false);
        ui->currentStatus->clear();
        disconnectall(game);
        game->setDisconnected(true);
        emit reset();
    }
    if(arg1=="Restart")
    {
        qDebug() << "Restarting application...";
        game->route.clear();
        game->route_tile.clear();
        emit reboot();
    }
}

void MainWindow::on_AnimationSpeed_sliderReleased()
{
    game->setSpeed(500/ui->AnimationSpeed->value());
}

void MainWindow::on_weight_sliderReleased()
{
    game->pathfinder->setWeight(ui->weight->value());

}

void MainWindow::receiveStatusUpdate(const QString & status)
{

    ui->currentStatus->setText(status);
    ui->currentStatus->setStyleSheet("color: red; font-size: 12px; background-color:white;");
}

void MainWindow::on_save_Button_clicked()
{
    if(std::find(game_list.begin(), game_list.end(), game)==game_list.end())
    {
        game_list.push_back(game);
        qDebug()<<"save game";
        QString printable = QStringLiteral("game %1").arg(i++);
        ui->load->addItem(printable);
    }
    else
        qDebug()<<"already saved";
}

void MainWindow::on_load_activated(const QString &arg1)
{
    for(unsigned long temp=0;temp<game_list.size();temp++)
        if(arg1==QStringLiteral("game %1").arg(temp))
        {
            if(game_list[temp]==game)
                qDebug()<<"cannot load the same game";
            else
            {
                disconnectall(game);
                game->setDisconnected(true);
                game->route.clear();
                game->route_tile.clear();
                ui->MoveNext->setEnabled(true);
                ui->automation->setEnabled(true);
                ui->comboBox->setEnabled(true);
                ui->manual->setEnabled(false);
                ui->currentStatus->clear();

                game_reset(game_list[temp]);
                allconnect();
                game->setDisconnected(false);

            }
        }
}

void MainWindow::on_findPath_clicked()
{
    QElapsedTimer timer;
    timer.start();
    game->goToXY(ui->xCoordinate->value(), ui->yCoordinate->value());
    game->draw_route();
    qDebug() << timer.elapsed();
}
