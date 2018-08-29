#include"judge.h"
extern int g_interrupt;
void judge::on_grab() {
	qDebug() << "startgrab";
	
	while (!g_interrupt) {
		if (model->empty()) continue;
		if (camera.isOpened()) qDebug() << ">>";
		qDebug() << "grab";
		model->copyTo(tmp);
		camera >> *imitate;
		resize(*imitate,*imitate,model->size());
		imshow("imitation", *imitate);
		if (isCorrespond(*imitate, tmp))
			emit show();
		waitKey(60);
	}
}
bool judge::isCorrespond(Mat camera, Mat model) {
	int deviation=0;
	double modelSize = 0;
	cvtColor(camera, camera, CV_BGR2HSV);
	Mat cameraMask(camera.size(),CV_8UC1);
	for (int i = 0; i < camera.rows; i++) {
		for (int j = 0; j < camera.cols; j++) {
			if (H_MIN < camera.at<Vec3b>(i, j)[0] && camera.at<Vec3b>(i, j)[0]< H_MAX) {
				cameraMask.at<uchar>(i, j) = 1;
			}
			else {
				cameraMask.at<uchar>(i, j) = 0;
			}
			   
		}
	}
	for (int i = 0; i < camera.rows; i++) {         //�ж������ͼ����modelͼ�������֮�䡰��϶���Ĵ�С����deviation����
		for (int j = 0; j < camera.cols; j++) {     //֮ǰmodel��ǰ������Ϊ0������Ϊ1������ǰ��Ϊ1������Ϊ0��ֻҪ��ֵ��ͬ���־���Ϊ�ǡ���϶��
			deviation += (cameraMask.at<uchar>(i, j) && model.at<uchar>(i, j))|| (!cameraMask.at<uchar>(i, j) && !model.at<uchar>(i, j));
			if (model.at<uchar>(i, j) == 0) modelSize++;
		}
	}     //����model������������á���϶��/����ó���ֵ���ݴ��ʱȽ�
	qDebug() << deviation;
	qDebug() << modelSize;
	if (deviation / modelSize < 0.1) return 1;
	else return 0;
}