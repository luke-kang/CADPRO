
#include <QDebug>
#include <QComboBox>

#include "itemproperties.h"

ItemProperties::ItemProperties():
    shapeid(0),
    length(0)
{
}
void ItemProperties::initdialog(){

    mainLayout = new QVBoxLayout;

    tableWidget =new QTableWidget();

    tableWidget->setColumnCount(2);
    tableWidget->setFrameShape(QFrame::Box);//设置边框
    tableWidget->setShowGrid(true); //设置显示格子线
    //table_widget->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    //表头内容
    QStringList m_Header;
    m_Header<<tr("属性")<<tr("值");
    tableWidget->setHorizontalHeaderLabels(m_Header);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //设置表头字体加粗
    QFont font;
    font.setBold(true);
    tableWidget->horizontalHeader()->setFont(font);
    //设置表格内容

    //显示ShapeId
    tableWidget->insertRow(0);
    QTableWidgetItem *tableitem= new QTableWidgetItem(tr("ID"));
    tableWidget->setItem(0,0,tableitem);
    tableitem->setTextAlignment(Qt::AlignCenter);
    //tableitem->setFlags(false);//设置为不可编辑

    QString string = QString::number(shapeid, 10);//将int变成QString类
    tableitem = new QTableWidgetItem(string);
    tableWidget->setItem(0,1,tableitem);
    tableitem->setTextAlignment(Qt::AlignCenter);
    //类型
    tableWidget->insertRow(1);
    tableitem = new QTableWidgetItem(tr("类型"));
    tableWidget->setItem(1,0,tableitem);
    tableitem->setTextAlignment(Qt::AlignCenter);
    tableitem = new QTableWidgetItem(tr("线"));
    tableWidget->setItem(1,1,tableitem);
    tableitem->setTextAlignment(Qt::AlignCenter);
    //显示线段长度
    tableWidget->insertRow(2);
    tableitem = new QTableWidgetItem(tr("长度"));
    tableWidget->setItem(2,0,tableitem);
    tableitem->setTextAlignment(Qt::AlignCenter);
    string = QString::number(length,'g',6);
    tableitem = new QTableWidgetItem(string);
    tableWidget->setItem(2,1,tableitem);
    tableitem->setTextAlignment(Qt::AlignCenter);
    //QObject::connect(tableitem,signal())

    //显示线形
    tableWidget->insertRow(3);
    tableitem = new QTableWidgetItem(tr("线形"));
    tableWidget->setItem(3,0,tableitem);
    tableitem->setTextAlignment(Qt::AlignCenter);
    linestyle =new QComboBox;
    linestyle->addItem(tr("SolidLine"));
    linestyle->addItem(tr("DashLine"));
    linestyle->addItem(tr("DotLine"));
    linestyle->addItem(tr("DashDotLine"));
    linestyle->addItem(tr("DashDotDotLine"));
    linestyle->addItem(tr("CustomDashLine"));
    tableWidget->setCellWidget(3,1,linestyle);
    QObject::connect(linestyle,SIGNAL(activated(QString)),this,SLOT(linestylechange()));

    //显示颜色
    tableWidget->insertRow(4);
    tableitem = new QTableWidgetItem(tr("颜色"));
    tableWidget->setItem(4,0,tableitem);
    tableitem->setTextAlignment(Qt::AlignCenter);
    linecolor = new QComboBox;
    linecolor->addItem(tr("红"));
    linecolor->addItem(tr("黄"));
    linecolor->addItem(tr("蓝"));
    linecolor->addItem(tr("绿"));
    tableWidget->setCellWidget(4,1,linecolor);
    QObject::connect(linecolor,SIGNAL(activated(QString)),this,SLOT(linecolorchange()));
    //属性
    tableWidget->insertRow(5);
    tableitem = new QTableWidgetItem(tr("属性"));
    tableWidget->setItem(5,0,tableitem);
    tableitem->setTextAlignment(Qt::AlignCenter);
    QComboBox *line_type_choose =new QComboBox;
    line_type_choose->addItem(tr("通用"));
    line_type_choose->addItem(tr("标记"));
    line_type_choose->addItem(tr("周长"));
    line_type_choose->addItem(tr("切割"));
    line_type_choose->addItem(tr("缝线"));
    tableWidget->setCellWidget(5,1,line_type_choose);

    mainLayout->addWidget(tableWidget);
    setLayout(mainLayout);
}

int ItemProperties::getShapeid() const
{
    return shapeid;
}

void ItemProperties::setShapeid(int value)
{
    shapeid = value;
}

double ItemProperties::getLength() const
{
    return length;
}

void ItemProperties::setLength(double value)
{
    length = value;
}

bool ItemProperties::getOk() const
{
    return ok;
}

void ItemProperties::setOk(bool value)
{
    ok = value;
    this->initdialog();
}

QPen ItemProperties::getPen() const
{
    return pen;
}

void ItemProperties::setPen(const QPen &value)
{
    pen = value;
}

void ItemProperties::typechanged()
{
    emit this->PropertiesChanged();
    //    qDebug()<<"发出信号";
}

void ItemProperties::linestylechange()
{
    switch (linestyle->currentIndex())
    {
        case 0:
            pen.setStyle(Qt::SolidLine);
            break;
        case 1:
            pen.setStyle(Qt::DashLine);
            break;
        case 2:
            pen.setStyle(Qt::DotLine);
            break;
        case 3:
            pen.setStyle(Qt::DashDotLine);
            break;
        case 4:
            pen.setStyle(Qt::DashDotDotLine);
            break;
        case 5:
            pen.setStyle(Qt::CustomDashLine);
            break;
    }
    typechanged();
}

void ItemProperties::linecolorchange()
{
    switch (linecolor->currentIndex())
    {
        case 0:
            pen.setColor(Qt::red);
            break;
        case 1:
            pen.setColor(Qt::yellow);
            break;
        case 2:
            pen.setColor(Qt::blue);
            break;
        case 3:
            pen.setColor(Qt::green);
            break;
    }
    typechanged();
//    qDebug()<<"换色";
}
