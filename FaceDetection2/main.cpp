#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <stdio.h>
#include <iostream>


using namespace std;
using namespace cv;

/**-------- Function Headers --------**/
void FaceDetection( Mat frame );
void drawFace(cv::Mat image, vector<Rect> *faces, int flag);
void drawProfileFace(cv::Mat image, vector<Rect> *faces, int flag);
//void ClippingAndDisplay(cv::Mat image, Rect);
void ClippingAndDisplay(cv::Mat image, Rect);


/**-------- Global variables --------**/
String face_cascade_name = "C:\\opencv2.4.10\\sources\\data\\haarcascades\\haarcascade_frontalface_default.xml";
String profile_cascade_name = "C:\\opencv2.4.10\\sources\\data\\haarcascades\\haarcascade_profileface.xml";

CascadeClassifier face_cascade;
CascadeClassifier profiles_cascade;

RNG rng(12345);



/**
 * @function main
 */
int main( int argc, char* argv[] )
{
	VideoCapture cap;
	Mat frame;
	Mat ROI;
	int ImageMax = 48;
	string fileName;
	string WindowName = "Window - Face detection";

/**-------- ���o��ǂݍ��� --------**/
if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
if( !profiles_cascade.load( profile_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };


/**-------- �f�[�^�ǂݍ��� --------**/
/* �Î~�摜�̓ǂݍ��ݎ��� �t���O1 */
/* ���摜�@�̓ǂݍ��ݎ��� �t���O0 */

#if 1
	// �Î~�摜�ǂݍ���
	// �Î~�摜���i�[���Ă���f�B���N�g�������w��
	//string DirectoryName = "C:\\home\\komiya\\Head_pose_estimation\\movie\\president\\2_George_W_Bush\\samplings\\sampling_1minutes\\spe_2004_0120_bush";
	//string DirectoryName = "C:\\home\\komiya\\Head_pose_estimation\\movie\\president\\processing";
	string DirectoryName = "C:\\home\\komiya\\Head_pose_estimation\\movie\\president\\1_Obama\\samplings\\sampling_1minutes\\spe_2010_0127_obama";

	// �������ʂ��i�[����f�B���N�g�������w��
	//string ResultDirectoryName = "C:\\home\\komiya\\Head_pose_estimation\\movie\\president\\2_George_W_Bush\\samplings\\sampling_1minutes\\spe_2004_0120_bush\\result";
	//string ResultDirectoryName = "C:\\home\\komiya\\Head_pose_estimation\\movie\\president\\processing\\result";
	string ResultDirectoryName = "C:\\home\\komiya\\Head_pose_estimation\\movie\\president\\1_Obama\\samplings\\sampling_1minutes\\spe_2010_0127_obama\\result_2";

	// �t�@�C�������o�͂��邽�߂̃e�L�X�g�t�@�C�������w��
	//ofstream outputfile("C:\\home\\komiya\\Head_pose_estimation\\movie\\president\\processing\\txt\\FileName.txt");
	
	
	// �f�B���N�g�����̃t�@�C���𕡐��w�肷��ꍇ�̓t���O1�A�t�@�C�������w�肷��ꍇ�̓t���O0
	#if 0
		// �f�B���N�g��������t�@�C�����X�g���쐬
		vector<string> backfilelist = Dir::read(DirectoryName);
		// �摜�͈͂��w�肵�Ȃ��ꍇ�i�f�B���N�g�����S�t�@�C���̏o�͂�����ꍇ�j�̓t���O1�Ƃ���
		#if 1
			// �t�@�C�����o��
			for(int i = 0; i < backfilelist.size(); i++) 
		#else
			// �Î~�摜No�͈͎w��
			// �J�n�ԍ�
			//int BeginNum = 47;
			int BeginNum = 726;
			// �I���ԍ�
			//int EndNum = 47;
			int EndNum = 727;
			// �t�@�C�����o��
			for(int i = BeginNum -1; i < EndNum; i++)
		#endif
			{
				stringstream FilePath;
				stringstream ResultFilePath;
				cout << backfilelist[i] << endl;
				outputfile << backfilelist[i] << endl;
				FilePath << DirectoryName << "\\" << backfilelist[i];
				ResultFilePath << ResultDirectoryName << "\\" << "result_" << backfilelist[i];
				// �i1�j�Î~�摜�f�[�^���t�@�C������ǂݍ���
				frame = imread(FilePath.str(), IMREAD_COLOR);
				//  ���s������G���[�\��
				if(frame.empty()) {
					cout << "Cannot open." << endl;
					return -1;
				}
				// �i2�j�Î~�摜�f�[�^�̏������ʂ�\������
				detectAndDisplay(frame);
				imwrite(ResultFilePath.str(), frame);
				//imshow(WindowName, frame);
				//waitKey(100);
			}
			outputfile.close();
	#else
		fileName = "spe_2010_0127_obama_00001.jpg";
		//fileName = "out00996.jpg";
		cout << fileName << endl;
		stringstream FilePath;
		stringstream ResultFilePath;
		FilePath << DirectoryName << "\\" <<  fileName;
		ResultFilePath << ResultDirectoryName << "\\" << "result_" << fileName;
		// �i1�j�Î~�摜�f�[�^���t�@�C������ǂݍ���
		frame = imread(FilePath.str(), IMREAD_COLOR);
		//  ���s������G���[�\��
		if(frame.empty()) {
			cout << "Cannot open." << endl;
			return -1;
		}
		// �i2�j�Î~�摜�f�[�^�̏������ʂ�\������
		FaceDetection(frame);
		imwrite(ResultFilePath.str(), frame);
		imshow(WindowName, frame);
		waitKey(0);
	#endif
		

#else
	// ���摜�ǂݍ���
	#if 1
		fileName = "spe_2010_0127_obama_5.avi";
	#else
		fileName = argv[1];
	#endif
		printf("movie success \n");
		cap.open(fileName);
		// ���摜���ǂݍ��܂ꂽ�ꍇ�̏���
		// ���摜���J���Ȃ���΃G���[�\��
		if(cap.isOpened()) {
			for(;;) {
				// ���摜�t�@�C������1�t���[�����̉摜�f�[�^���擾���āA�ϐ�frame�Ɋi�[����
				cap >> frame;

				// �摜�f�[�^�擾�Ɏ��s�����烋�[�v�𔲂���
				if (!frame.empty()) {
					detectAndDisplay( frame );
					imshow(WindowName, frame);
				}
				else
					break;

				// �擾�����摜�f�[�^���E�B���h�E�\������
				if(waitKey(100) >= 0) break;
			}
		}

#endif
	return 0;
}




/**
* @function FaceDetection
*/
void FaceDetection(Mat frame)
{
	std::vector<Rect> faces;
	Mat frame_gray;
	cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
	equalizeHist( frame_gray, frame_gray );

	/**-------- �猟�o���� --------**/
	/* ���ʊ猟�o */
	face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(50, 50) );
	if(faces.size() > 0)
	{
		//�א��ŕ`��
		//drawFace(frame, &faces, 1);
		//�����ŕ`��
		drawFace(frame, &faces, 2);
	}


	/* ���猟�o */
	std::vector<Rect> profiles;
	Mat profiles_gray = frame_gray;
	profiles_cascade.detectMultiScale( profiles_gray, profiles, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(50, 50) );
	if(faces.size() > 0)
	{
		//�א��ŕ`��
		//drawProfileFace(frame, &profiles, 1);
		//�����ŕ`��
		//drawProfileFace(frame, &profiles, 2);
	}
}




/**
* @function DrawFace
*/
void drawFace(cv::Mat image, vector<Rect> *faces, int flag)
{
	for(size_t i=0; i<(int)faces->size(); i++ )
	{
		if(faces->at(i).width != -1)
		{
			Point pt1;
			Point pt2;

			pt1.x = faces->at(i).x;
			pt1.y = faces->at(i).y;
			pt2.x = faces->at(i).x + faces->at(i).width - 1;
			pt2.y = faces->at(i).y + faces->at(i).height - 1;

			//ROI��������ѕۑ�
			Rect rect1;
			rect1.x =faces->at(i).x;
			rect1.y = faces->at(i).y;
			rect1.width = faces->at(i).width;
			rect1.height = faces->at(i).height;
			ClippingAndDisplay(image, rect1);

			// ���l���當����ɕϊ�
			char c_str[256];

			sprintf(c_str, "%d", i+1);
 
			//�ԍ��̕`�悷��ꏊ
			Point number(faces->at(i).x + faces->at(i).width/2, faces->at(i).y + faces->at(i).height/2 - 40 );

			switch (flag)
			{
				case 1:
						rectangle(image, pt1, pt2, CV_RGB(255, 0, 255), 1, 8, 0 );
						//rectangle( image, Point(190, 93), Point(280, 190), Scalar(255, 0, 0), 1, 8, 0);
						break;

				case 2:
						rectangle(image, pt1, pt2, CV_RGB(255, 0, 255), 3, 8, 0 );
						rectangle(image, pt1, pt2, CV_RGB(255, 255, 255), 2, 8, 0 );
						rectangle(image, pt1, pt2, CV_RGB(255, 0, 255), 1, 8, 0 );
						break;
			}	// swtich(flag)
			// �ԍ��̕`��
			putText(image, c_str, number, FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 255), 2, 8, 0);
		}
	}	// for(i)
}



/**
* @function DrawProfileFace
*/
void drawProfileFace(cv::Mat image, vector<Rect> *faces, int flag)
{
	for( size_t i=0; i<(int)faces->size(); i++ )
	{
			Point pt_center;
			Size sz_circle;

			pt_center.x   = faces->at(i).x + faces->at(i).width / 2;
			pt_center.y   = faces->at(i).y + faces->at(i).height / 2;
			sz_circle.width  = faces->at(i).width / 2;
			sz_circle.height = faces->at(i).height / 2;

			// ���l���當����ɕϊ�
			char c_str[256];

			sprintf(c_str, "%d", i+1);
 
			//�ԍ��̕`�悷��ꏊ
			Point number(faces->at(i).x + faces->at(i).width/2, faces->at(i).y + faces->at(i).height/2 + 60 );

			//�ȉ~�̕`��
			switch (flag)
			{
				case 1:
					ellipse(image, pt_center, sz_circle, 0, 0, 360, Scalar( 255, 0, 0 ), 1, 8, 0 );
					break;

				case 2:
					ellipse(image, pt_center, sz_circle, 0, 0, 360, Scalar( 255, 0, 0 ), 3, 8, 0 );
					ellipse(image, pt_center, sz_circle, 0, 0, 360, Scalar( 255, 255, 255 ), 2, 8, 0 );
					ellipse(image, pt_center, sz_circle, 0, 0, 360, Scalar( 255, 0, 0 ), 1, 8, 0 );
					break;
			}
			// �ԍ��̕`��
			putText(image, c_str, number, FONT_HERSHEY_SIMPLEX, 0.8, Scalar(255, 0, 0), 2, 8, 0);
	}
}




/**
* @function ClippingAndDisplay
*/
void ClippingAndDisplay(Mat image, Rect rect)
{
		//Mat ROI(image, Rect(200,200,100,100));
		Mat ROI(image, rect);
		imwrite("clipped.jpg", ROI);
		imshow("result_clipped", ROI);
}