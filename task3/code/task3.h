#pragma once
#include <QtWidgets/QMainWindow>
#include<qaction.h>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<qmenubar.h>
#include<qfiledialog.h>
#include<string>
#include<qstring.h>
#include<qthread.h>
#include<qtimer.h>
#include<vector>
#include<qpushbutton.h>
#include"videoplayer.h"
#include"judge.h"
#include<QMetaType>
#define H_MAX 43
#define H_MIN 9
using namespace std;
using namespace cv;
class judge;
class task3 : public QMainWindow
{
	Q_OBJECT
public:
	task3(QWidget *parent = Q_NULLPTR);
	void on_learning_trigger();
	void on_updateModel_trigger(Mat MatJustBeShowed);
	void get_edges(Mat source);
	void on_stop();
    signals:
		void play(QString videoName);
		void inquiry(VideoCapture& learnVido);
		void updateModelMat(Mat MatJustBeShowed);
		void stopLearn();
		void startGrab();
private:
	QMenuBar* menu=new QMenuBar(this);
	QAction* preview = new QAction(tr("&preview"), this);
	QAction* learn = new QAction(tr("&learn"));
	QFileDialog* fDlg = new QFileDialog(this);
	VideoPlayer vp;          //����Ԥ����Ƶ�����߳�thd������
	judge* jg;                //���ڴ������ȡ��Ƶ�����ж��Ƿ������������th2������
	VideoCapture learnVido;
	workThread thd;
	workThread thd2;
	Mat curFlame;      
	Mat model;             //���ڱ��浱ǰ�û�������������maskͼ
	QTimer tm;
	QPushButton* stop = new QPushButton("STOP");
};
