#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Scene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit Scene(QObject *parent = nullptr){ Q_UNUSED(parent) }

signals:
    void mouseMove(QGraphicsSceneMouseEvent *mouseEvent);
    void mousePress(QGraphicsSceneMouseEvent *mouseEvent);

protected:
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override
    { emit mouseMove(mouseEvent); }
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override
    { emit mousePress(mouseEvent); }
};

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void mouseMove(QGraphicsSceneMouseEvent *mouseEvent);
	void mousePress(QGraphicsSceneMouseEvent *mouseEvent);

private:
	Scene scene;
    QGraphicsEllipseItem *rc[4];
    QGraphicsPathItem *ci;
    QList<QGraphicsEllipseItem *> ri;
    QList<QPointF> cp;
    int scp;

private:
	Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
