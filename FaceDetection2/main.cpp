#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <stdio.h>
#include <string.h>
#include <fstream>
#include <direct.h>

#include "Dir.h"


using namespace std;
using namespace cv;

/**-------- Function Headers --------**/
void FaceDetection( Mat frame );
void drawFace(cv::Mat image, vector<Rect> *faces, int flag);
void drawProfileFace(cv::Mat image, vector<Rect> *faces, int flag);
void ClippingAndDisplay(cv::Mat image, vector<Rect> *faces);
void CutExtention(const char* cfName);



/**-------- Global variables --------**/
String face_cascade_name = "C:\\opencv2.4.10\\sources\\data\\haarcascades\\haarcascade_frontalface_default.xml";
String profile_cascade_name = "C:\\opencv2.4.10\\sources\\data\\haarcascades\\haarcascade_profileface.xml";
String FileDirectoryPath;
string FileName;

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
	string fName;
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
	//string DirectoryName = "C:\\home\\komiya\\Head_pose_estimation\\data\\president\\2_George_W_Bush\\samplings\\sampling_1minutes\\spe_2004_0120_bush";
	string DirectoryName = "C:\\home\\komiya\\Head_pose_estimation\\data\\president\\processing";
	//string DirectoryName = "C:\\home\\komiya\\Head_pose_estimation\\data\\president\\1_Obama\\samplings\\sampling_1minutes\\spe_2009_0604_obama";
	//FileDirectoryPath = DirectoryName;

	// �������ʂ��i�[����f�B���N�g�������w��
	//string ResultDirectoryName = "C:\\home\\komiya\\Head_pose_estimation\\movie\\president\\2_George_W_Bush\\samplings\\sampling_1minutes\\spe_2004_0120_bush\\result";
	string ResultDirectoryName = "C:\\home\\komiya\\Head_pose_estimation\\data\\president\\processing\\ROI";
	//string ResultDirectoryName = "C:\\home\\komiya\\Head_pose_estimation\\data\\president\\1_Obama\\samplings\\sampling_1minutes\\spe_2009_0604_obama\\ROI\\test";
	FileDirectoryPath = ResultDirectoryName;

	// �t�@�C�������o�͂��邽�߂̃e�L�X�g�t�@�C�������w��
	//ofstream outputfile("C:\\home\\komiya\\Head_pose_estimation\\data\\president\\processing\\txt\\FileName.txt");
	
	
	// �f�B���N�g�����̃t�@�C���𕡐��w�肷��ꍇ�̓t���O1�A�t�@�C�������w�肷��ꍇ�̓t���O0
	#if 1
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
			int BeginNum = 18;
			// �I���ԍ�
			//int EndNum = 47;
			int EndNum = 61;
			// �t�@�C�����o��
			for(int i = BeginNum -1; i < EndNum; i++)
		#endif
			{
				stringstream FilePath;
				stringstream ResultFilePath;
				cout << backfilelist[i] << endl;
				FileName = backfilelist[i];
				//outputfile << backfilelist[i] << endl;
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
				FaceDetection(frame);
				imwrite(ResultFilePath.str(), frame);
				//imshow(WindowName, frame);
				//imshow(WindowName, frame);
				//waitKey(100);
			}
			//outputfile.close();
	#else
		fName = "spe_2009_0604_obama_00014.jpg";
		FileName = fName;
		//fileName = "out00996.jpg";
		cout << fName << endl;
		stringstream FilePath;
		stringstream ResultFilePath;
		FilePath << DirectoryName << "\\" <<  fName;
		ResultFilePath << ResultDirectoryName << "\\" << "result_" << fName;
		// �i1�j�Î~�摜�f�[�^���t�@�C������ǂݍ���
		frame = imread(FilePath.str(), IMREAD_COLOR);
		//  ���s������G���[�\��
		if(frame.empty()) {
			cout << "Cannot open." << endl;
			return -1;
		}
		// �i2�j�Î~�摜�f�[�^�̏������ʂ�\������
		// �珈��
		FaceDetection(frame);
		imwrite(ResultFilePath.str(), frame);
		imshow(WindowName, frame);
		//// �؂�o������
		//FaceDetection(frame);
		//imwrite(ResultFilePath.str(), frame);
		//imshow(WindowName, frame);
		//// �g�又��

		waitKey(0);
	#endif
		

#else
	// ���摜�ǂݍ���
	#if 1
		fName = "spe_2010_0127_obama_5.avi";
	#else
		fName = argv[1];
	#endif
		printf("movie success \n");
		cap.open(fName);
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
	// �א��ŕ`��
	//drawFace(frame, &faces, 1);

	// �����ŕ`�悷�����
	if(faces.size() > 0)
	{
		for(size_t i = 0; i < (int)faces.size() -1; i++)
		{
			if(faces[i].width < 0)
				continue;
			for(size_t j = i+1; j < (int)faces.size() ; j++)
			{
				if(faces[j].width < 0)
					continue;
				if(
					//i�
					//1�̃p�^�[��
					//x��
					faces[i].x <=  faces[j].x && faces[i].x <= (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) > (faces[j].x + faces[j].width - 1) &&
					//y��
					faces[i].y <=  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) > (faces[j].y + faces[j].height - 1) ||
					//2�̃p�^�[��
					//x��
					faces[i].x <=  faces[j].x && faces[i].x > (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) > (faces[j].x + faces[j].width - 1) &&
					//y��
					faces[i].y <=  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) > (faces[j].y + faces[j].height - 1) ||
					//3�̃p�^�[��
					//x��
					faces[i].x <=  faces[j].x && faces[i].x <= (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) <= (faces[j].x + faces[j].width - 1) &&
					//y��
					faces[i].y <=  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) > (faces[j].y + faces[j].height - 1) ||
					//4�̃p�^�[��
					//x��
					faces[i].x <=  faces[j].x && faces[i].x <= (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) > (faces[j].x + faces[j].width - 1) &&
					//y��
					faces[i].y >  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) > (faces[j].y + faces[j].height - 1) ||
					//5�̃p�^�[��
					//x��
					faces[i].x <=  faces[j].x && faces[i].x <= (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) > (faces[j].x + faces[j].width - 1) &&
					//y��
					faces[i].y <=  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) <= (faces[j].y + faces[j].height - 1) ||
					//6�̃p�^�[��
					//x��
					faces[i].x <=  faces[j].x && faces[i].x > (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) > (faces[j].x + faces[j].width - 1) &&
					//y��
					faces[i].y >  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) > (faces[j].y + faces[j].height - 1) ||
					//7�̃p�^�[��
					//x��
					faces[i].x <=  faces[j].x && faces[i].x <= (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) <= (faces[j].x + faces[j].width - 1) &&
					//y��
					faces[i].y <=  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) <= (faces[j].y + faces[j].height - 1) ||
					//8�̃p�^�[��
					//x��
					faces[i].x <=  faces[j].x && faces[i].x > (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) > (faces[j].x + faces[j].width - 1) &&
					//y��
					faces[i].y <=  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) <= (faces[j].y + faces[j].height - 1) ||
					//9�̃p�^�[��
					//x��
					faces[i].x <=  faces[j].x && faces[i].x <= (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) <= (faces[j].x + faces[j].width - 1) &&
					//y��
					faces[i].y >  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) > (faces[j].y + faces[j].height - 1) ||
					//10�̃p�^�[��
					//x��
					faces[i].x <=  faces[j].x && faces[i].x > (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) > (faces[j].x + faces[j].width - 1) &&
					//y��
					faces[i].y >  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) <= (faces[j].y + faces[j].height - 1) ||
					//11�̃p�^�[��
					//x��
					faces[i].x <=  faces[j].x && faces[i].x <= (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) <= (faces[j].x + faces[j].width - 1) &&
					//y��
					faces[i].y >  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) <= (faces[j].y + faces[j].height - 1) ||
					//12�̃p�^�[��
					//x��
					faces[i].x >  faces[j].x && faces[i].x <= (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) <= (faces[j].x + faces[j].width - 1) &&
					//y��
					faces[i].y >  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) > (faces[j].y + faces[j].height - 1) ||
					//13
					//x��
					faces[i].x >  faces[j].x && faces[i].x <= (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) <= (faces[j].x + faces[j].width - 1) &&
					//y��
					faces[i].y <=  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) <= (faces[j].y + faces[j].height - 1) ||
					//14
					//x��
					faces[i].x >  faces[j].x && faces[i].x <= (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) <= (faces[j].x + faces[j].width - 1) &&
					//y��
					faces[i].y >  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) <= (faces[j].y + faces[j].height - 1) ||

					//j�
					//1�̃p�^�[��
					//x��
					faces[j].x <=  faces[i].x && faces[j].x <= (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) > (faces[i].x + faces[i].width - 1) &&
					//y��
					faces[j].y <=  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) > (faces[i].y + faces[i].height - 1) ||
					//2�̃p�^�[��
					//x��
					faces[j].x <=  faces[i].x && faces[j].x > (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) > (faces[i].x + faces[i].width - 1) &&
					//y��
					faces[j].y <=  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) > (faces[i].y + faces[i].height - 1) ||
					//3�̃p�^�[��
					//x��
					faces[j].x <=  faces[i].x && faces[j].x <= (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) <= (faces[i].x + faces[i].width - 1) &&
					//y��
					faces[j].y <=  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) > (faces[i].y + faces[i].height - 1) ||
					//4�̃p�^�[��
					//x��
					faces[j].x <=  faces[i].x && faces[j].x <= (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) > (faces[i].x + faces[i].width - 1) &&
					//y��
					faces[j].y >  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) > (faces[i].y + faces[i].height - 1) ||
					//5�̃p�^�[��
					//x��
					faces[j].x <=  faces[i].x && faces[j].x <= (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) > (faces[i].x + faces[i].width - 1) &&
					//y��
					faces[j].y <=  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) <= (faces[i].y + faces[i].height - 1) ||
					//6�̃p�^�[��
					//x��
					faces[j].x <=  faces[i].x && faces[j].x > (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) > (faces[i].x + faces[i].width - 1) &&
					//y��
					faces[j].y >  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) > (faces[i].y + faces[i].height - 1) ||
					//7�̃p�^�[��
					//x��
					faces[j].x <=  faces[i].x && faces[j].x <= (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) <= (faces[i].x + faces[i].width - 1) &&
					//y��
					faces[j].y <=  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) <= (faces[i].y + faces[i].height - 1) ||
					//8�̃p�^�[��
					//x��
					faces[j].x <=  faces[i].x && faces[j].x > (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) > (faces[i].x + faces[i].width - 1) &&
					//y��
					faces[j].y <=  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) <= (faces[i].y + faces[i].height - 1) ||
					//9�̃p�^�[��
					//x��
					faces[j].x <=  faces[i].x && faces[j].x <= (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) <= (faces[i].x + faces[i].width - 1) &&
					//y��
					faces[j].y >  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) > (faces[i].y + faces[i].height - 1) ||
					//10�̃p�^�[��
					//x��
					faces[j].x <=  faces[i].x && faces[j].x > (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) > (faces[i].x + faces[i].width - 1) &&
					//y��
					faces[j].y >  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) <= (faces[i].y + faces[i].height - 1) ||
					//11�̃p�^�[��
					//x��
					faces[j].x <=  faces[i].x && faces[j].x <= (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) <= (faces[i].x + faces[i].width - 1) &&
					//y��
					faces[j].y >  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) <= (faces[i].y + faces[i].height - 1) ||
					//12�̃p�^�[��
					//x��
					faces[j].x >  faces[i].x && faces[j].x <= (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) <= (faces[i].x + faces[i].width - 1) &&
					//y��
					faces[j].y >  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) > (faces[i].y + faces[i].height - 1) ||
					//13
					//x��
					faces[j].x >  faces[i].x && faces[j].x <= (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) <= (faces[i].x + faces[i].width - 1) &&
					//y��
					faces[j].y <=  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) <= (faces[i].y + faces[i].height - 1) ||
					//14
					//x��
					faces[j].x >  faces[i].x && faces[j].x <= (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) <= (faces[i].x + faces[i].width - 1) &&
					//y��
					faces[j].y >  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) <= (faces[i].y + faces[i].height - 1) ||
					
					//���W��v�̏ꍇ
					faces[i].x ==  faces[j].x || faces[i].x == (faces[j].x + faces[j].width -1) || (faces[i].x + faces[i].width - 1) == faces[j].x || (faces[i].x + faces[i].width - 1) == (faces[j].x + faces[j].width - 1) ||
					faces[i].y ==  faces[j].y || faces[i].y == (faces[j].y + faces[j].height -1) || (faces[i].y + faces[i].height - 1) == faces[j].y || (faces[i].y+ faces[i].height - 1) == (faces[j].y + faces[j].height - 1) 
					)
						faces[j].width = -1;
				
			}	// for(j)
		}	// for(i)
	}
		//�؂�o�������E�g�又���y�я������ʕۑ�
		ClippingAndDisplay(frame, &faces);

		//�����ŕ`��
		drawFace(frame, &faces, 2);
	

	/* ���猟�o */
	//std::vector<Rect> profiles;
	//Mat profiles_gray = frame_gray;
	//profiles_cascade.detectMultiScale( profiles_gray, profiles, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(50, 50) );
	//if(faces.size() > 0)
	//{
	//	//�א��ŕ`��
	//	drawProfileFace(frame, &profiles, 1);
	//	//�����ŕ`��
	//	drawProfileFace(frame, &profiles, 2);
	//}
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

						// �ԍ��̕`��
						///putText(image, c_str, number, FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 255), 2, 8, 0);
						break;
			}	// swtich(flag)

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
void ClippingAndDisplay(Mat image, vector<Rect> *faces)
{
	for(size_t i=0; i<(int)faces->size(); i++ )
	{
		if(faces->at(i).width != -1)
		{
			// ���o������`�̍��W���i�[���ė̈��1.5�{�Ɋg������
			Rect rect1;

			rect1.x =(faces->at(i).x)-((((faces->at(i).width)*1.5)-(faces->at(i).width))/2);
			/*if(rect1.x < 0)
				break;*/
			//rect1.x =(faces->at(i).x);
			//rect1.y = faces->at(i).y;
			rect1.y = faces->at(i).y-((((faces->at(i).width)*1.5)-(faces->at(i).width))/2);
			/*if(rect1.y < 0)
				break;*/
			rect1.width = (faces->at(i).width)*1.5;
			rect1.height = (faces->at(i).height)*1.5;
			/*rect1.width = (faces->at(i).width);
			rect1.height = (faces->at(i).height);*/


			// ���ʂ��o�͂���p�X�ƃt�@�C�����i�[����̈�̊m��
			char c_fpath_num[256];
			char c_fpath[256];
			//char rclip[256];
			//char nearest[256];
			char linear[256];
			//char area[256];
			//char cubic[256];
			//char lanczos[256];

			String fPath = FileDirectoryPath;
			string fName = FileName;
			
			// �Î~�摜�t�@�C��������g���q�̎��o��
			char *name, *num;
			char c_fname[256];
			strcpy(c_fname, fName.c_str());
			name = strtok(c_fname, ".");

			//// �Î~�摜�t�@�C��������ԍ��̎��o��
			//char *mem, *num;
			//char c_fname[256];
			//strcpy(c_fname, fName.c_str());
			//mem = strtok(c_fname, "_");
			//for (int roop=0; roop < 3; roop++) 
			//{
   //             mem = strtok( NULL,"_" );
			//}
			//num =  strtok(NULL, "_");
			//mem = strtok(num, ".");

			// ���ʂ��o�͂���f�B���N�g���̍쐬
			//sprintf(c_fpath_num, "%s\\%s", fPath.c_str(), num);
			//if(_mkdir(c_fpath_num) == 0)
			//	cout << "Make directory_ImageNum" << endl;
			//sprintf(c_fpath, "%s\\%s\\%s%d", fPath.c_str(), num, "result_", i+1);
			//if(_mkdir(c_fpath) == 0)
			//	cout << "Make directory_resultNum" << endl;

			// ���ʂ̃t�@�C�����i�[
			//sprintf(rclip, "%s\\%s\\%s%d\\%s%s%d.jpg", fPath.c_str(), num, "result_1.5_", i+1, num, "_clipping_", i+1);
			//printf("%s\n", rclip);
			//sprintf(nearest, "%s\\%s\\%s%d%s.jpg", fPath.c_str(), "result", num, i+1, "_resize_NEAREST");
			//sprintf(linear, "%s\\%s\\%s%s%s%s%d%s.jpg", fPath.c_str(), "result", fName.c_str(), "_", num, "_", i+1, "_resize_LINEAR");
			sprintf(linear, "%s\\%s\\%s%s%d%s.jpg", fPath.c_str(), "result", name, "_", i+1, "_resize_LINEAR");
			//printf("%s\n", linear);
			//sprintf(area, "%s\\%s\\%s%d%s.jpg", fPath.c_str(), "result", num, i+1, "_result_AREA");
			//sprintf(cubic, "%s\\%s\\%s%d%s.jpg", fPath.c_str(), "result", num, i+1, "_result_CUBIC");
			//sprintf(lanczos, "%s\\%s\\%s%d%s.jpg", fPath.c_str(), "result", num, i+1, "_result_LANCZOS4");
			/*sprintf(nearest, "%s\\%s\\%s%d\\%s%s%d.jpg", fPath.c_str(), num, "result_", i+1, num, "_resize_NEAREST_", i+1);
			sprintf(linear, "%s\\%s\\%s%d\\%s%s%d.jpg", fPath.c_str(), num, "result_", i+1, num, "_resize_LINEAR_", i+1);
			sprintf(area, "%s\\%s\\%s%d\\%s%s%d.jpg", fPath.c_str(), num, "result_", i+1, num,"_result_AREA_", i+1);
			sprintf(cubic, "%s\\%s\\%s%d\\%s%s%d.jpg", fPath.c_str(), num, "result_", i+1, num, "_result_CUBIC_", i+1);
			sprintf(lanczos, "%s\\%s\\%s%d\\%s%s%d.jpg", fPath.c_str(), num, "result_", i+1,  num,"_result_LANCZOS4_", i+1);*/


			// ���o���̋�`�̎��o�������Ɗg�又���p��Mat����
			Mat ROI(image, rect1);
			Mat dst(Size(200, 200), CV_8U, Scalar::all(0));

			// ��`���o���������ʏ����o��
			//imwrite(rclip, ROI);
			//imshow(rclip, ROI);

			// �g�又���y�я������ʏ����o��
			//resize(ROI, dst, dst.size(), 0, 0, INTER_NEAREST);
			//imwrite(nearest, dst);
			resize(ROI, dst, dst.size(), 0, 0, INTER_LINEAR);
			imwrite(linear, dst);
			/*resize(ROI, dst, dst.size(), 0, 0, INTER_AREA);
			imwrite(area, dst);
			resize(ROI, dst, dst.size(), 0, 0, INTER_CUBIC);
			imwrite(cubic, dst);
			resize(ROI, dst, dst.size(), 0, 0, INTER_LANCZOS4);
			imwrite(lanczos, dst);*/
		}
	}
}
