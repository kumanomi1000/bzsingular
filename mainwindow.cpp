#include "mainwindow.h"
#include "ui_mainwindow.h"

bool vBezierSingular(
    QList<QPointF> &cp,     // IN  3次ベジェ曲線 制御点 (4点)
    QList<QPointF> &opI,	// OUT 変曲点座標 (0～2点)
    QList<QPointF> &opC,	// OUT カスプ点座標 (0または1点)
    QList<QPointF> &opS);	// OUT 自己交差点座標 (0または1点)

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
    scene.setSceneRect(0,0,ui->graphicsView->width()-2,ui->graphicsView->height()-2);
    ui->graphicsView->setScene(&scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
#if 1
    // cusp
    cp<<QPointF(120,50);
    cp<<QPointF(120,150);
    cp<<QPointF(220,150);
    cp<<QPointF(20,50);
#elif 1
    // v=0
    cp<<QPointF(20,20);
    cp<<QPointF(20,120);
    cp<<QPointF(120,120);
    cp<<QPointF(120,220);
#else
    cp<<QPointF(0,0);
    cp<<QPointF(0,1);
    cp<<QPointF(1,1);
    cp<<QPointF(-0.5,0);
#endif
    for(int i=0;i<4;i++){
        rc[i]=scene.addEllipse(cp[i].x()-2.5,cp[i].y()-2.5,5,5,QPen(Qt::blue));
    }
    {
        QPainterPath curve(cp[0]);
        curve.cubicTo(cp[1],cp[2],cp[3]);
        ci=scene.addPath(curve,QPen(QBrush(Qt::blue),1));
    }
    QList<QPointF> opI,opC,opS;
    vBezierSingular(cp,opI,opC,opS);
    foreach (QPointF p,opI)
        ri<<scene.addEllipse(p.x()-2.5,p.y()-2.5,5,5,QPen(Qt::green),QBrush(Qt::green));
    foreach (QPointF p,opC)
        ri<<scene.addEllipse(p.x()-2.5,p.y()-2.5,5,5,QPen(Qt::cyan),QBrush(Qt::cyan));
    foreach (QPointF p,opS)
        ri<<scene.addEllipse(p.x()-2.5,p.y()-2.5,5,5,QPen(Qt::red),QBrush(Qt::red));
    scp=-1;

    connect(&scene,SIGNAL(mouseMove(QGraphicsSceneMouseEvent*)),this,SLOT(mouseMove(QGraphicsSceneMouseEvent*)));
    connect(&scene,SIGNAL(mousePress(QGraphicsSceneMouseEvent*)),this,SLOT(mousePress(QGraphicsSceneMouseEvent*)));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if((mouseEvent->buttons()&Qt::LeftButton)&&scp>=0){
        QPointF pos=mouseEvent->scenePos();
        cp[scp]=pos;
        rc[scp]->setRect(pos.x()-2.5,pos.y()-2.5,5,5);
        QPainterPath curve(cp[0]);
        curve.cubicTo(cp[1],cp[2],cp[3]);
        ci->setPath(curve);
        foreach(QGraphicsEllipseItem *gi,ri){
            scene.removeItem(gi);
            delete gi;
        }
        ri.clear();
        QList<QPointF> opI,opC,opS;
        vBezierSingular(cp,opI,opC,opS);
        foreach (QPointF p,opI)
            ri<<scene.addEllipse(p.x()-2.5,p.y()-2.5,5,5,QPen(Qt::green),QBrush(Qt::green));
        foreach (QPointF p,opC)
            ri<<scene.addEllipse(p.x()-2.5,p.y()-2.5,5,5,QPen(Qt::cyan),QBrush(Qt::cyan));
        foreach (QPointF p,opS)
            ri<<scene.addEllipse(p.x()-2.5,p.y()-2.5,5,5,QPen(Qt::red),QBrush(Qt::red));
    }
}

void MainWindow::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
	if(mouseEvent->buttons() & Qt::LeftButton){
        QPointF pos = mouseEvent->scenePos();
        scp=-1;
        for(int i=0;i<4;i++){
            if(QLineF(pos,rc[i]->rect().center()).length()<=5)
                scp=i;
        }
    }
}
