#include <cmath>
#include <QPointF>
#include <QList>
using namespace std;

#define EZERO   1e-14   // これより0に近い値は0とする

#define PW3(x)	((x)*(x)*(x))
#define PW2(x)	((x)*(x))

// 3次ベジェ曲線の係数を求める
void coefBezier(
  QList<QPointF> &cp,	// IN  ベジェ曲線 制御点
  double *a,			// OUT x係数
  double *b)			// OUT y係数
{
    a[0]=-cp[0].x()+3*cp[1].x()-3*cp[2].x()+cp[3].x();
    a[1]=3*cp[0].x()-6*cp[1].x()+3*cp[2].x();
    a[2]=-3*cp[0].x()+3*cp[1].x();
    a[3]=cp[0].x();
    b[0]=-cp[0].y()+3*cp[1].y()-3*cp[2].y()+cp[3].y();
    b[1]=3*cp[0].y()-6*cp[1].y()+3*cp[2].y();
    b[2]=-3*cp[0].y()+3*cp[1].y();
    b[3]=cp[0].y();
}

// 3次パラメトリック曲線の座標を求める
QPointF pointBezier(
  double *a,			// IN  x係数
  double *b,			// IN  y係数
  double t)				// IN  パラメータ
{
    return QPointF(a[0]*PW3(t)+a[1]*PW2(t)+a[2]*t+a[3],
                   b[0]*PW3(t)+b[1]*PW2(t)+b[2]*t+b[3]);
}

// 制御点の座標値を[0,1]に正規化
void normCP(
  QList<QPointF> &cp,  // IN  曲線の制御点
  QList<QPointF> &ncp) // OUT 曲線の制御点（正規化）
{
    double xmin=cp[0].x();
    double xmax=cp[0].x();
    double ymin=cp[0].y();
    double ymax=cp[0].y();
    foreach(QPointF p,cp.mid(1)){
        if(xmin>p.x()) xmin=p.x();
        if(xmax<p.x()) xmax=p.x();
        if(ymin>p.y()) ymin=p.y();
        if(ymax<p.y()) ymax=p.y();
    }
    double m=max(xmax-xmin,ymax-ymin);
    ncp.clear();
    foreach(QPointF p,cp)
        ncp<<QPointF((p.x()-xmin)/m,(p.y()-ymin)/m);
}

// 3次ベジェ曲線の特異点を求める
bool vBezierSingular(   // RETURN true:正常
  QList<QPointF> &cp,	// IN  3次ベジェ曲線 制御点 (4点)
  QList<QPointF> &opI,	// OUT 変曲点座標 (0～2点)
  QList<QPointF> &opC,	// OUT カスプ点座標 (0または1点)
  QList<QPointF> &opS)	// OUT 自己交差点座標 (0または1点)
{
    opI.clear();
    opC.clear();
    opS.clear();
    if(cp.size()!=4)
        return false;

    // ベジェ曲線の係数を求める
    double a[4],b[4];
    coefBezier(cp,a,b);

    // 制御点の正規化座標と係数を求める
    QList<QPointF> ncp;
    normCP(cp, ncp);
    double na[4],nb[4];
    coefBezier(ncp,na,nb);

    // 特異点を求める
    double v=na[0]*nb[1]-na[1]*nb[0];
    double w=na[0]*nb[2]-na[2]*nb[0];
    double u=na[1]*nb[2]-na[2]*nb[1];
    double D=4*u*v-3*PW2(w);
fprintf(stderr,"v=%lf w=%lf u=%lf D=%lf %lf\n",v,w,u,D,D/27);
    if(abs(v)>=EZERO){
        if(D<=-EZERO){
            // 変曲点
            double sqd=sqrt(-D/3);
            double t1=(-w+sqd)/2/v;
            double t2=(-w-sqd)/2/v;
fprintf(stderr,"D=%lf t1=%lf t2=%lf\n",D,t1,t2);
            if(t1>=0&&t1<=1)
                opI<<pointBezier(a,b,t1);
            if(t2>=0&&t2<=1)
                opI<<pointBezier(a,b,t2);
        }else if(D<EZERO){
            // カスプ
            double t=-w/2/v;
fprintf(stderr,"D=%lf t=%lf\n",D,t);
            if(t>=0&&t<=1)
                opC<<pointBezier(a,b,t);
        }else{
            // 自己交差
            double sqd=sqrt(D);
            double t1=(-w+sqd)/2/v;
            double t2=(-w-sqd)/2/v;
fprintf(stderr,"D=%lf t1=%lf t2=%lf\n",D,t1,t2);
            if(t1>=0&&t1<=1&&t2>=0&&t2<=1)
                opS<<pointBezier(a,b,t1);
        }
    }else{
        if(D<=-EZERO&&abs(w)>=EZERO){
            // 変曲点
            double t=-u/3/w;
fprintf(stderr,"D=%lf t=%lf\n",D,t);
            if(t>=0&&t<=1)
                opI<<pointBezier(a,b,t);
        }
    }
    return true;
}
