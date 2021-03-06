﻿#include "collisiondectect.h"
#include "common.h"
#include <qmath.h>

#include <QDebug>

CollisionDectect::CollisionDectect(QVector<QPointF> pList1, QVector<QPointF> pList2, bool isCircle1, bool isCircle2, short precision)
{
    // 判断该多边形是否首位相连（即第一点和最后一点重复），如果是的话，要把最后一点删去
    if(!isCircle1 && pList1[0] == pList1[pList1.length()-1]){
        pList1.removeLast();
    }
    if(!isCircle2 && pList2[0] == pList2[pList2.length()-1]){
        pList2.removeLast();
    }

    this->pList1 = pList1;
    this->pList2 = pList2;
    this->isCircle1 = isCircle1;
    this->isCircle2 = isCircle2;
    this->precision = precision;
}

/**
 * @brief CollisionDectect::getBoundingRect
 * 包围盒碰撞检测.顺序:左上右下四个值→Xmin,Ymin,Xmax,Ymax
 * @param pList
 * @return
 */
QVector<qreal> CollisionDectect::getBoundingRect(QVector<QPointF> pList)
{
    int len = pList.length();
    if(len < 1){
        return QVector<qreal>(4);
    }
    qreal minX = pList[0].rx();
    qreal minY = pList[0].ry();
    qreal maxX = pList[0].rx();
    qreal maxY = pList[0].ry();
    for(int i=1; i<len; i++){
        if(pList[i].rx() < minX){
            minX = pList[i].rx();
        }
        if(pList[i].ry() < minY){
            minY = pList[i].ry();
        }
        if(pList[i].rx() > maxX){
            maxX = pList[i].rx();
        }
        if(pList[i].ry() > maxY){
            maxY = pList[i].ry();
        }
    }

    QVector<qreal> ret;
    ret.append(minX);
    ret.append(minY);
    ret.append(maxX);
    ret.append(maxY);

    return ret;
}

CollisionDectect::CircleInfo CollisionDectect::getBoundingCircle(QVector<QPointF> pList)
{
    int len = pList.length();
    if(len != 2){
        return CircleInfo();
    }
    return CircleInfo(pList[0], pList[1].rx());
}

bool CollisionDectect::circleHit(QVector<QPointF> pList1, QVector<QPointF> pList2)
{
    if(pList1.length() != 2 || pList2.length() != 2){
        return false;
    }
    qreal dis2 = qPow(pList1[0].rx()-pList2[0].rx(), 2)
            + qPow(pList1[0].ry()-pList2[0].ry(), 2);
    qreal rd = qPow(pList1[1].rx()+pList2[1].rx(), 2);
    return dis2 < rd;
}

bool CollisionDectect::boundHit(QVector<qreal> bound1, QVector<qreal> bound2)
{
    if(bound1.length() != 4 || bound2.length() != 4){
        return false;
    }
    if(!overlap(QPointF(bound1[0],bound1[2]),QPointF(bound2[0], bound2[2]))) {
        return false;
    }
    if(!overlap(QPointF(bound1[1],bound1[3]),QPointF(bound2[1], bound2[3]))) {
        return false;
    }
    return true;
}

qreal CollisionDectect::dotProduct(QPointF v1, QPointF v2)
{
    qreal ret = v1.rx() * v2.rx() + v1.ry() * v2.ry();
    return qrealPrecision(ret, precision);
}

QPointF CollisionDectect::normalize(QPointF v)
{
    qreal n = qSqrt(v.rx() * v.rx() + v.ry() * v.ry());
    if(n == 0){
        return v;
    }
    qreal x = qrealPrecision(v.rx() / n, precision);
    qreal y = qrealPrecision(v.ry() / n, precision);
    v.setX(x);
    v.setY(y);
    return v;
}

QPointF CollisionDectect::perpendicular(QPointF v)
{
    qreal tmp = v.rx();
    v.setX(v.ry());
    v.setY(-tmp);
    return v;
}

QVector<QPointF> CollisionDectect::getSeparatingAxis(QVector<QPointF> pList, QVector<QPointF> separatingAxis)
{
    int len = pList.length();
    QPointF nor, segment;
    for(int i=0; i<len; i++){
        segment.setX(pList[(i+1)%len].rx() - pList[i].rx());
        segment.setY(pList[(i+1)%len].ry() - pList[i].ry());
        nor = perpendicular(normalize(segment));
        if(nor.rx() == 0 && nor.ry() == 0){  // 防止前后两点重复导致错误
             continue;
        }
        bool flag = false;  // 重复标志
        // 判断该分离轴是否重复，可用QMap<qreal, QVector<qreal>> 优化
        for(int j=0; j<separatingAxis.length(); j++){
            if((separatingAxis[j].rx() == nor.rx()
                    && separatingAxis[j].ry() == nor.ry()) ||
                    (separatingAxis[j].rx() == -nor.rx()
                    && separatingAxis[j].ry() == -nor.ry())){
                flag = true;
                break;
            }
        }
        if(!flag){  // 当不重复时将向量加入分离轴数组
            separatingAxis.append(nor);
        }
    }
    return separatingAxis;
}

bool CollisionDectect::overlap(QPointF s1, QPointF s2)
{
    qreal sx1 = qrealPrecision(s1.rx(), precision);
    qreal sy1 = qrealPrecision(s1.ry(), precision);
    qreal sx2 = qrealPrecision(s2.rx(), precision);
    qreal sy2 = qrealPrecision(s2.ry(), precision);
    if(sx1 >= sy2 || sy1 <= sx2){
        return false;
    }
    return true;
}

QPointF CollisionDectect::getPolygonProjection(QVector<QPointF> pList, QPointF axis)
{
    QPointF ret;
    qreal min = dotProduct(pList[0], axis);
    qreal max = min;
    for(int i=1; i<pList.length(); i++){
        qreal n = dotProduct(pList[i], axis);
        if(n < min){
            min = n;
        }
        if(n > max){
            max = n;
        }
    }
    ret.setX(min);
    ret.setY(max);
    return ret;
}

QPointF CollisionDectect::getCircleProjection(QVector<QPointF> pList, QPointF axis)
{
    QPointF ret;
    ret.setX(dotProduct(pList[0], axis) - pList[1].rx());
    ret.setY(ret.rx() + 2 * pList[1].rx());
    return ret;
}

bool CollisionDectect::collision()
{
    if(pList1.length() < 1 && pList2.length() < 1){
        return false;
    }

    //qDebug() << "collision: ";
    // 判断两图形的类型
    ConcavePolygon concavePoly1(pList1);
    ConcavePolygon concavePoly2(pList2);

    bool isConcavePoly1 = concavePoly1.isConcavePolygon(pList1);
    bool isConcavePoly2 = concavePoly2.isConcavePolygon(pList2);

    //qDebug() << endl << "new test";
    // 如果两多边形都不为凹多边形
    if(!isConcavePoly1 && !isConcavePoly2){
        //qDebug() << "all not concavePoly" ;
        return convexPolygonCollision(pList1, pList2, isCircle1, isCircle2);
    }
    // 如果两多边形都为凹多边形
    if(isConcavePoly1 && isConcavePoly2){
        //qDebug() << "all concavePoly" ;
        QMap<int, QVector<QPointF>> splitRes1 = concavePoly1.onSeparateConcavePoly(pList1);
        QMap<int, QVector<QPointF>> splitRes2 = concavePoly2.onSeparateConcavePoly(pList2);
        for(int i=0; i<splitRes1.size(); i++){
            for(int j=0; j<splitRes2.size(); j++){
                if(convexPolygonCollision(splitRes1[i], splitRes2[j])){
                    return true;
                }
            }
        }
        return false;
    }
    // 如果两多边形不全为凹多边形
    if(isConcavePoly1){
        //qDebug() << "first is concavePoly" ;
        //qDebug() << "";
        QMap<int, QVector<QPointF>> splitRes1 = concavePoly1.onSeparateConcavePoly(pList1);
        for(int i=0; i<splitRes1.size(); i++){
            if(convexPolygonCollision(splitRes1[i], pList2)){
                return true;
            }
        }
        return false;
    }
    if(isConcavePoly2){
        //qDebug() << "second is concavePoly" ;
        QMap<int, QVector<QPointF>> splitRes2 = concavePoly2.onSeparateConcavePoly(pList2);
        for(int i=0; i<splitRes2.size(); i++){
            if(convexPolygonCollision(pList1, splitRes2[i])){
                return true;
            }
        }
        return false;
    }
    return false;
}

bool CollisionDectect::convexPolygonCollision(QVector<QPointF> pList1, QVector<QPointF> pList2, bool isCircle1, bool isCircle2)
{
    if(pList1.length() < 1 && pList2.length() < 1){
        return false;
    }

    // 如果两个图形都是圆，则直接进行圆的碰撞检测
    if(isCircle1 && isCircle2){
        //qDebug() << "all circles";
        return circleHit(pList1, pList2);
    }

    // 如果至少有一个不是圆，则需要利用分离轴进行碰撞检测
    QVector<QPointF> separatingAxis;
    if(!isCircle1){
        //qDebug() << "first is not circle";
        separatingAxis = getSeparatingAxis(pList1, separatingAxis);
    }
    if(!isCircle2){
        //qDebug() << "second is not circle";
        separatingAxis = getSeparatingAxis(pList2, separatingAxis);
    }
    //qDebug() << "separating Axis: "  << separatingAxis.length();
    //for(int i=0; i<separatingAxis.length(); i++){
    //    //qDebug() << separatingAxis[i];
    //}
    //qDebug() << "";
    QPointF extreme1, extreme2;
    for(int i=0; i<separatingAxis.length(); i++){
        //qDebug() << "separating Axis is #" << i << "：" << separatingAxis[i];
        if(isCircle1){
            extreme1 = getCircleProjection(pList1, separatingAxis[i]);
        } else{
            extreme1 = getPolygonProjection(pList1, separatingAxis[i]);
        }
        //qDebug() << "第一个图形的投影极值为："
                 // << extreme1.rx() << "  " << extreme1.ry();
        if(isCircle2){
            extreme2 = getCircleProjection(pList2, separatingAxis[i]);
        } else{
            extreme2 = getPolygonProjection(pList2, separatingAxis[i]);
        }
        //qDebug() << "第一个图形的投影极值为："
                 // << extreme2.rx() << "  " << extreme2.ry();
        if(!overlap(extreme1, extreme2)){
            //qDebug() << "在投影轴为#" << i << "：" << separatingAxis[i]
                        // << "时分离";
            return false;
        }
    }
    return true;
}

ConcavePolygon::ConcavePolygon(QVector<QPointF> &list) :
    coordinateSystem(LeftHandRuleCS)
{
    //qDebug() << "构造ConcavePolygon, 默认是左手定则";
    // 默认为逆时针，否则反转多边形
    if(!isAntiClockDir(list)){
        //qDebug() << list.length() << " - 顺时针";
        conversPoly(list);
    } else {
        //qDebug() << list.length() << " - 逆时针";
    }
    pList = list;
}

ConcavePolygon::ConcavePolygon(QVector<QPointF> &list, CoordinateSystem cs)
{
    //qDebug() << "构造ConcavePolygon, 默认是左手定则";
    // 默认为逆时针，否则反转多边形
    if(!isAntiClockDir(list)){
        //qDebug() << list.length() << " - 顺时针";
        conversPoly(list);
    } else {
        //qDebug() << list.length() << " - 逆时针";
    }
    pList = list;
    coordinateSystem = cs;
    // 如果使用了平面直角坐标系统，对坐标系要进行转换
    if(coordinateSystem == RightHandRuleCS){
        //qDebug() << "使用了平面直角坐标系";
        for(int i=0; i<list.length(); i++){
            pList[i].setY(-pList[i].ry());
        }
    }
}

bool ConcavePolygon::isConcavePolygon(QVector<QPointF> pList)
{
    if(pList.length() <= 3){
        return false;
    }
    QVector<int> res = getAllConcaveIndex(pList, 0);
    for(int i=0; i<res.length(); i++){
        //qDebug() << "凹点: #" << res[i] << "  " << pList[res[i]];
    }
    bool ret = getNextConcaveIndex(pList, 0) >=0;
    //qDebug() << "检测该多边形是否为凹多边形: "  << ret;
    return ret;
}

QMap<int, QVector<QPointF>> ConcavePolygon::onSeparateConcavePoly(QVector<QPointF> pList)
{
    QMap<int, QVector<QPointF>> ret;
    _separateConcavePoly(pList, ret, 0);
    return ret;
}

int ConcavePolygon::getNextConcaveIndex(QVector<QPointF> pList, int startIndex)
{
    int len = pList.length();
    if(len <= 3){
        return -1;
    }
    int curDir = 0;
    int nMax = len + startIndex;
    for(int i=startIndex; i<nMax; i++){
        curDir = getMutiPtClockDir(pList[(i + len) % len],
                pList[(i - 1 + len) % len],
                pList[(i + 1 + len) % len]);
        if(curDir == Anticlockwise){
            return i % len;
        }
    }
    return -1;
}

QVector<int> ConcavePolygon::getAllConcaveIndex(QVector<QPointF> pList, int startIndex)
{
    int len = pList.length();
    if(len <= 3){
        return QVector<int>();
    }
    QVector<int> ret;
    int curDir = 0;
    int nMax = len + startIndex;
    for(int i=startIndex; i<nMax; i++){
        curDir = getMutiPtClockDir(pList[(i + len) % len],
                pList[(i - 1 + len) % len],
                pList[(i + 1 + len) % len]);
        if(curDir == Anticlockwise){
            ret.append(i % len);
        }
    }
    return ret;
}

void ConcavePolygon::_separateConcavePoly(QVector<QPointF> pList, QMap<int, QVector<QPointF>> &map, int startIndex)
{
    int len = pList.length();
    if(len <= 3){
        map.insert(map.size(), pList);
        return;
    }

    int nextConcaveIndex = getNextConcaveIndex(pList, startIndex);
    //qDebug() << "nextConcaveIndex = " << nextConcaveIndex;

#if 1
    if(nextConcaveIndex > 0 && nextConcaveIndex < len){
        //qDebug() << "凹点为：" << pList[nextConcaveIndex];
    }
#endif

    if(nextConcaveIndex < 0){
        map.insert(map.size(), pList);
        return;
    }
    startIndex = nextConcaveIndex + 1;
    IntersectionPoint intersectionPoint;
    if(getSplitPointByVertexRegion(pList, nextConcaveIndex, intersectionPoint)){
        //qDebug() << "交点" << intersectionPoint.index << "  " << intersectionPoint.intersection;
        QVector<QPointF> pLeftList;
        QVector<QPointF> pRightList;
        splitPolyByIntersection(pList, nextConcaveIndex, intersectionPoint, pLeftList, pRightList);
        //qDebug() << "pLeftList: " << pLeftList.length();
        //qDebug() << "pRightList: " << pRightList.length();
        _separateConcavePoly(pLeftList, map, startIndex);
        _separateConcavePoly(pRightList, map, startIndex);
        return;
    }
    map.insert(map.size(), pList);
}

bool ConcavePolygon::splitPolyByIntersection(QVector<QPointF> pList, int concaveIndex, ConcavePolygon::IntersectionPoint intersectionPoint, QVector<QPointF> &pLeftList, QVector<QPointF> &pRightList)
{
    int len = pList.length();
    if(concaveIndex < 0 || concaveIndex >= len){
        return false;
    }
    int intersectionPointIndex = intersectionPoint.index;
    if(intersectionPointIndex < 0 || intersectionPointIndex >= len){
        return false;
    }
    int i = 0;
    // 左边矩形
    if(concaveIndex <= intersectionPointIndex + 1){
        for(i = intersectionPointIndex; i <= concaveIndex + len; i++){
            QPointF pn1 = pList[i % len];
            pLeftList.append(pn1);
        }
    } else{
        for(i = intersectionPointIndex; i <= concaveIndex; i++){
            QPointF pn2 = pList[i];
            pLeftList.append(pn2);
        }
    }
    // 右边矩形
    if(concaveIndex <= intersectionPointIndex){
        for(i = concaveIndex; i <= intersectionPointIndex; i++){
            QPointF pn3 = pList[i];
            pRightList.append(pn3);
        }
    } else{
        for(i = concaveIndex; i <= intersectionPointIndex + len; i++){
            QPointF pn4 = pList[i % len];
            pRightList.append(pn4);
        }
    }

    if(pList[intersectionPointIndex].rx() != intersectionPoint.intersection.rx()
            && pList[intersectionPointIndex].ry() != intersectionPoint.intersection.ry()){
        pLeftList.append(intersectionPoint.intersection);
        pRightList.append(intersectionPoint.intersection);
    }
    return true;
}

bool ConcavePolygon::getSplitPointByVertexRegion(QVector<QPointF> pList, int concaveIndex, ConcavePolygon::IntersectionPoint &intersectionPoint)
{
    int len = pList.length();
    if(len <= 3){
        return false;
    }
     //qDebug() << "----getSplitPointByVertexRegion-----";
    int preIndex = concaveIndex > 0 ? concaveIndex - 1 : len - 1;
    int nextIndex = (concaveIndex < (len - 1)) ? concaveIndex + 1 : 0;
    int ret1 = 0;
    int ret2 = 0;
    // 分区计算
    QVector<int> A;
    QVector<int> B;
    QVector<int> C;
    QVector<int> D;
    // 可见点区域
    QVector<int> A1;
    QVector<int> B1;
    QVector<int> C1;
    QVector<int> D1;
    int nMax = (nextIndex <= preIndex) ? preIndex : preIndex + len;

    //qDebug() << "preIndex: " << preIndex;
    //qDebug() << "concaveIndex: " << concaveIndex;
    //qDebug() << "nextIndex: " << nextIndex;
    //qDebug() << "nMax: " << nMax;

    for(int i=nextIndex; i<=nMax; i++){
        int nCur = i % len;
        //qDebug() << "nCur: " << nCur;
        ret1 = getMutiPtClockDir(pList[concaveIndex], pList[preIndex], pList[nCur]);
        ret2 = getMutiPtClockDir(pList[concaveIndex], pList[nextIndex], pList[nCur]);
        // 计算所在区域
        if(ret1 <= 0 && ret2 >= 0){
            A.append(nCur);
        } else if(ret1 > 0 && ret2 >= 0){
            B.append(nCur);
        } else if(ret1 <= 0 && ret2 < 0){
            C.append(nCur);
        } else if(ret1 > 0 && ret2 < 0){
            D.append(nCur);
        }
    }
    //qDebug() << "A:  ";
    for(int i=0; i<A.length(); i++){
        //qDebug() << i << "  " << A[i];
    }
    //qDebug() << "B:  ";
    for(int i=0; i<B.length(); i++){
        //qDebug() << i << "  " << B[i];
    }
    //qDebug() << "C:  ";
    for(int i=0; i<C.length(); i++){
        //qDebug() << i << "  " << C[i];
    }
    //qDebug() << "D:  ";
    for(int i=0; i<D.length(); i++){
        //qDebug() << i << "  " << D[i];
    }
    // 取可见点分区
    //qDebug() << "A1可见区";
    visibleRegionPtSet(pList, concaveIndex, A, A1);
    for(int i=0; i<A1.length(); i++){
        //qDebug() << i << "   " << A1[i];
    }
    B1.append(B);
    //qDebug() << "B1可见区:  ";
    for(int i=0; i<B1.length(); i++){
        //qDebug() << i << "  " << B1[i];
    }

    //qDebug() << "C1可见区";
    visibleRegionPtSet(pList, concaveIndex, C, C1);
    for(int i=0; i<C1.length(); i++){
        //qDebug() << i << "   " << C1[i];
    }
    D1.append(D);
    //qDebug() << "D1可见区:  ";
    for(int i=0; i<D1.length(); i++){
        //qDebug() << i << "  " << D1[i];
    }

    if(A1.length() > 0){
        QVector<int> setA, setB;
        setSplitByRegion(pList, A1, setA, setB);
        if(setB.length() > 0){
            intersectionPoint.index = getBestIntersectionPt(pList, concaveIndex, setB);
        } else{
            intersectionPoint.index = getBestIntersectionPt(pList, concaveIndex, setA);
        }
        if(intersectionPoint.index < 0 ||
                intersectionPoint.index >= len){
            return false;
        }
        intersectionPoint.intersection = pList[intersectionPoint.index];
        //qDebug() << "A1 不为空， 交点： #" << intersectionPoint.index << "  " << intersectionPoint.intersection;
        return true;
    }
    // 如果A为空，BC必不为空
    //qDebug() << "A1 为空";
    if(B1.length() < 1 || C1.length() < 1){
        //qDebug() << "BC分区为空错误";
        return false;
    }
    //int mid = concaveIndex;
    int left = B1[B1.length() - 1];
    int right = C1[0];
    // BC区域的首位点必在一条直线上
    QPointF d1(pList[right].rx() - pList[left].rx(),
               pList[right].ry() - pList[left].ry());
    //qDebug() << "d1: " << d1;
    QPointF d00(pList[concaveIndex].rx() - pList[preIndex].rx(),
                pList[concaveIndex].ry() - pList[preIndex].ry());
    //qDebug() << "d00: " << d00;
    QPointF d01(pList[concaveIndex].rx() - pList[nextIndex].rx(),
                pList[concaveIndex].ry() - pList[nextIndex].ry());
    //qDebug() << "d01: " << d01;
    // A区域与交点区域的角平分线
    QPointF d0((d00.rx() + d01.rx()) / 2,
               (d00.ry() + d01.ry()) / 2);
    //qDebug() << "d0: " << d0;
    QPointF crossPt(0, 0);

    QLineF line1(pList[concaveIndex], d0);
    QLineF line2(pList[left], d1);
    QPointF point;
    QLineF::IntersectType type = line1.intersect(line2,&point);
    if(type != QLineF::NoIntersection){
        //qDebug() << "line's intersect: " << point;
    } else{
        //qDebug() << "无交点";
    }

    if(!getCrossByRadialAndSegment(pList[concaveIndex], d0, pList[left], d1, crossPt)){
        return false;
    }

    intersectionPoint.index = left;
    intersectionPoint.intersection = crossPt;
    //qDebug() << "**************";
    //qDebug() << "intersectionPoint.index = " << left;
    //qDebug() << "intersectionPoint.intersection = " << crossPt;
    //qDebug() << "**************";
    return true;
}

ConcavePolygon::PolyDirection ConcavePolygon::getMutiPtClockDir(QPointF p1, QPointF p2, QPointF p3)
{
    qreal ret = (p1.rx() - p2.rx()) * (p3.ry() - p1.ry())
            - (p3.rx() - p1.rx()) * (p1.ry() - p2.ry());
    return ret > 0 ? Anticlockwise : (ret < 0 ? Clockwise : InALine);
}

ConcavePolygon::PolyDirection ConcavePolygon::getMutiPtClockDirByIndex(QVector<QPointF> pList, int index)
{
    int len = pList.length();
    if(len <= 2){
        return InALine;
    }
    return getMutiPtClockDir(pList[index % len], pList[(index+len-1)%len], pList[(index+1)%len]);
}

bool ConcavePolygon::isAntiClockDir(QVector<QPointF> pList)
{
    int len = pList.length();
    int count = 0;
    for(int i=0; i<len; i++){
        int nextIndex1 = (i + 1) % len;
        int nextIndex2 = (i + 2) % len;
        int n = (pList[nextIndex1].rx() - pList[i].rx()) * (pList[nextIndex2].ry() - pList[nextIndex1].ry());
        n -= (pList[nextIndex1].ry() - pList[i].ry()) * (pList[nextIndex2].rx() - pList[nextIndex1].rx());
        if(n < 0){
            count--;
        } else if(n > 0){
            count++;
        }
    }
    return count <= 0;
}

void ConcavePolygon::conversPoly(QVector<QPointF> &pList)
{
    int len = pList.length();
    int i = -1;
    while(++i < len / 2){
        QPointF pn1, pn2;
        pn1 = pList[i];
        pn2 = pList[len - i - 1];
        pList[i] = pn2;
        pList[len - i - 1] = pn1;
    }
}

bool ConcavePolygon::isVectorInsection(QPointF p1, QPointF d1, QPointF p2, QPointF d2)
{
    //qDebug() << p1 << ", " << d1 << ", " << p2 << ", " << d2;
    QLineF l1(p1, d1);
    QLineF l2(p2, d2);
    QPointF intersection;
    return getLineIntersection(l1, l2, intersection);
#if 0
    QPointF ePoint(p2.rx()-p1.rx(), p2.ry()-p1.ry());
    qreal cross = d1.rx() * d2.ry() - d1.ry() * d2.rx();
    qreal sqrtCross = cross * cross;
    qreal sqrtLen1 = d1.rx() * d1.rx() + d1.ry() * d1.ry();
    qreal sqrtLen2 = d2.rx() * d2.rx() + d2.ry() * d2.ry();
    qreal sqrtEpsilon = 0.01f;
    //qDebug() << "ePoint: " << ePoint;
    //qDebug() << "cross:" << cross;
    //qDebug() << "sqrtCross: " << sqrtCross;
    //qDebug() << "sqrtLen1: " << sqrtLen1;
    //qDebug() << "sqrtLen2: " << sqrtLen2;
    //qDebug() << "sqrtEpsilon * sqrtLen1 * sqrtLen2: " << sqrtEpsilon * sqrtLen1 * sqrtLen2;
    //qDebug() << endl;
    if(sqrtCross > (sqrtEpsilon * sqrtLen1 * sqrtLen2)){
        // lines of the segments are not parallel
        qreal s = (ePoint.rx() * d2.ry() - ePoint.ry() * d2.rx()) / cross;
        if(s < 0 || s > 1){
            //qDebug() << "s " << s;
            //intersection of lines is not a point on segment P0 + s * D0
            return false;
        }
        qreal t = (ePoint.rx() * d1.ry() - ePoint.ry() * d1.rx()) / cross;
        if(t < 0 || t > 1){
            //qDebug() << "t: " << t;
            return false;
        }
        //qDebug() << "return true";
        return true;
    }
    return false;
#endif
}

bool ConcavePolygon::isVisiblePtToConcave(QVector<QPointF> pList, int index1, int index2)
{
    int len = pList.length();
    for(int i=0; i<len; i++){
        int nextIndex = (i+1)%len;
        if(i == index1
                || i == index2
                || nextIndex == index1
                || nextIndex == index2){
            continue;
        }
        //qDebug() << index1 << ", " << index2 << ", " << i << ", " << nextIndex;
        if(isVectorInsection(pList[index1], pList[index2], pList[i], pList[nextIndex])){
            //qDebug() << "Vector Insection";
            return false;
        }
    }
    return true;
}

void ConcavePolygon::visibleRegionPtSet(QVector<QPointF> pList, int concaveIndex, QVector<int> region, QVector<int> &region1)
{
    int i = -1;
    while(++i < region.length()){
        if(isVisiblePtToConcave(pList, concaveIndex, region[i])){
            //qDebug() << concaveIndex << " 对 " << region[i] << "可见";
            region1.append(region[i]);
        }
    }
}

void ConcavePolygon::setSplitByRegion(QVector<QPointF> pList, QVector<int> region, QVector<int> &setA, QVector<int> &setB)
{
    int i = -1;
    while(++i < region.length()){
        if(getMutiPtClockDirByIndex(pList, region[i])){
            setA.append(region[i]);
        } else{
            setB.append(region[i]);
        }
    }
}

void ConcavePolygon::normalizeVector(QPointF &v)
{
    int n = qSqrt(v.rx() * v.rx() + v.ry() * v.ry());
    if(n <= 0){
        return;
    }
    v.setX(v.rx() / n);
    v.setY(v.ry() / n);
}

qreal ConcavePolygon::dotVector(QPointF v1, QPointF v2)
{
    return v1.rx() * v2.rx() + v1.ry() * v2.ry();
}

int ConcavePolygon::getBestIntersectionPt(QVector<QPointF> pList, int concaveIndex, QVector<int> ptSets)
{
    if(ptSets.length() < 1){
        return -1;
    }
    int len = pList.length();
    int bestIndex = -1;
    int preIndex = (len + concaveIndex - 1) % len;
    int nextIndex = (len + concaveIndex + 1) % len;
    QPointF d00(pList[concaveIndex].rx() - pList[preIndex].rx(),
                pList[concaveIndex].ry() - pList[preIndex].ry());
    QPointF d01(pList[concaveIndex].rx() - pList[nextIndex].rx(),
                pList[concaveIndex].ry() - pList[nextIndex].ry());
    normalizeVector(d00);
    normalizeVector(d01);
    int fLen = -1;
    int i = -1;
    while(++i < ptSets.length()){
        QPointF tp = pList[ptSets[i]];
        QPointF dp(tp.rx() - pList[concaveIndex].rx(),
                   tp.ry() - pList[concaveIndex].ry());
        normalizeVector(dp);
        qreal nTemLen = qAbs(dotVector(d00, dp));
        if(nTemLen > fLen){
            bestIndex = ptSets[i];
        }
    }
    return bestIndex;
}

bool ConcavePolygon::getCrossByRadialAndSegment(QPointF p1, QPointF d1, QPointF p2, QPointF d2, QPointF &crossPoint)
{
    QPointF ePoint(p2.rx()-p1.rx(), p2.ry()-p1.ry());
    qreal cross = d1.rx() * d2.ry() - d1.ry() * d2.rx();
    qreal sqrtCross = cross * cross;
    qreal sqrtLen1 = d1.rx() * d1.rx() + d1.ry() * d1.ry();
    qreal sqrtLen2 = d2.rx() * d2.rx() + d2.ry() * d2.ry();
    qreal sqrtEpsilon = 0.01;

    if(sqrtCross > sqrtEpsilon * sqrtLen1 * sqrtLen2){
        // lines of the segments are not parallel
        qreal s = (ePoint.rx() * d2.ry() - ePoint.ry() * d2.rx()) / cross;
        //qDebug() << "s: " << s;
        if(s < 0 || s > 1){
            //qDebug() << "s<0";
            return false;
        }
        qreal t = (ePoint.rx() * d1.ry() - ePoint.ry() * d1.rx()) / cross;
        if(t < 0 || t > 1){
            //intersection of lines is not a point on segment P1 + t * D1
            //qDebug() << "t < 0 || t > 1";
            return false;
        }
        QPointF pt(d1.rx() * s, d1.ry() * s);
        crossPoint.setX(p1.rx() + pt.rx());
        crossPoint.setY(p1.ry() + pt.ry());
        return true;
    }
    return false;
}

ConcavePolygon::PointRelationToLine ConcavePolygon::GetPosRelationToLine(QPointF sPoint, QPointF ePoint, QPointF p)
{
    qreal ret = p.rx() * (ePoint.ry() - sPoint.ry())
            + p.ry() * (sPoint.rx() - ePoint.rx())
            + sPoint.ry() * ePoint.rx()
            - sPoint.rx() * ePoint.ry();
    return ret > 0 ? RightLine : (ret < 0 ? LeftLine : InLine) ;
}



