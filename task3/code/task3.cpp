#include "task3.h"
task3* tmp;
task3::task3(QWidget *parent)
	: QMainWindow(parent)
{
	qRegisterMetaType< Mat >("Mat");   //�Ǽ�Mat�����ں���������ӵ��źŲ���
	tmp = this;
	jg = new judge(&model);
	jg->moveToThread(&thd2);
	vp.moveToThread(&thd);
	menu->addAction(preview);
	menu->addAction(learn);
	connect(preview, &QAction::triggered, [this]() {
		QString filename = fDlg->getOpenFileName();
		if (filename == NULL) return;                  //���ļ����������̣߳�֮��������ӽ������߳���Ӧ
		emit play(filename);
	});                     
	connect(this, &task3::play, &vp, &VideoPlayer::on_play_trigger);
	connect(learn, &QAction::triggered, this, &task3::on_learning_trigger);
	connect(this, &task3::inquiry, &vp, &VideoPlayer::on_inquiry_trigger, Qt::BlockingQueuedConnection);  
	//���ڴ����߳��л�ȡlearnVido��ѯ���Ƿ��Ѿ���Ԥ����Ƶ���ڲ��ţ����ǣ���ֱ�ӴӲ��ŵ���Ƶ��ʼѧϰ
	connect(this, &task3::updateModelMat, this, &task3::on_updateModel_trigger, Qt::QueuedConnection);  
	//�Զ������ӣ���֤model�ĸ��²���imshow��������ͼƬ���ָ��û�֮ǰ
	connect(stop, &QPushButton::clicked, [this]() {
		emit stopLearn();
	});
	//ͬ�ϣ���֤�ر��¼������뵽�¼�������� ,��֮ǰ���¼�ִ�к���ִ�С�
	connect(this, &task3::stopLearn, this, &task3::on_stop, Qt::QueuedConnection);
	//֪ͨλ��thd2��judge��ʼGrab
	connect(this, &task3::startGrab, jg, &judge::on_grab);
	thd.start();
	thd2.start();
	emit startGrab();
}
void task3::on_learning_trigger() {
    tm.stop();
	emit inquiry(learnVido);
	QString filename;
	if (!learnVido.isOpened()) {           //ͨ��learnVido�Ƿ�Ϊ��״̬�ж��Ƿ��Ԥ���л������Ƶ��������Σ�ԭ��Ƶ���ᱻ�ر�
		filename = fDlg->getOpenFileName();
		if (filename == NULL) {
			emit stopLearn();       //���ǵ��ȡ����ֱ�ӹر�ѧϰ��
			return;             
		}
		learnVido.open(filename.toStdString());
	}
	learnVido >> curFlame;
	get_edges(curFlame); 
	
	tm.start(10000);
	connect(&tm, &QTimer::timeout, [this]() {
		learnVido.set(CV_CAP_PROP_POS_FRAMES, (int)learnVido.get(CV_CAP_PROP_POS_FRAMES) + 300);//10����֡
		learnVido >> curFlame;
		if (curFlame.empty())
			; //tm.stop()  ���������Զ��������
		else 
			get_edges(curFlame);
	});
	stop -> show();                                                              
}
void task3::get_edges(Mat source) {
	
	cvtColor(source, source, cv::COLOR_BGR2HSV);
	//GaussianBlur(source, source,Size(5,5),0,0);
	Mat mask,tmp, resultMat = Mat(source.rows, source.cols, CV_8UC1, Scalar(255));
	inRange(source, Scalar(35, 43, 46), Scalar(77, 255, 255), mask);
	mask.copyTo(tmp);     //findcontours���ƻ�mask
	vector<vector<Point>> contours;
	vector<Vec4i> topo;
	findContours(tmp, contours, topo,CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	/*int i =  topo[0][2];
	while (i != -1) {
		drawContours(resultMat, contours, i, 0, 2);
		i = topo[i][0];
	}*/
	for (int i = 0; i < contours.size(); i++) {
		if(contours[i].size()>30)drawContours(resultMat, contours, i, 0, 2);
	}
	cv::imshow("result",resultMat);  // ��֤��Ҫģ�µ�ͼƬ�ĸ��±�Ȼ����imshow�¼���������漴ִ��
	emit updateModelMat(mask);  //��ֹ�������Ѿ���ģ��ͼƬ���£����û���ȴ��Ϊimshow�¼����ͺ�ִ�ж��޷������µ�ģ��ͼƬ
}
void task3::on_updateModel_trigger(Mat matJustBeShowed) {
	qDebug() << "upmodl";
	model = matJustBeShowed;
}
void task3::on_stop() {
	tm.stop();
	model.release();
	destroyWindow("result");
	destroyWindow("imitation");
	stop->close();
}