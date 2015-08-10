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

/**-------- 検出器読み込み --------**/
if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
if( !profiles_cascade.load( profile_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };


/**-------- データ読み込み --------**/
/* 静止画像の読み込み時は フラグ1 */
/* 動画像　の読み込み時は フラグ0 */

#if 1
	// 静止画像読み込み
	// 静止画像を格納しているディレクトリ名を指定
	//string DirectoryName = "C:\\home\\komiya\\Head_pose_estimation\\data\\president\\2_George_W_Bush\\samplings\\sampling_1minutes\\spe_2004_0120_bush";
	string DirectoryName = "C:\\home\\komiya\\Head_pose_estimation\\data\\president\\processing";
	//string DirectoryName = "C:\\home\\komiya\\Head_pose_estimation\\data\\president\\1_Obama\\samplings\\sampling_1minutes\\spe_2009_0604_obama";
	//FileDirectoryPath = DirectoryName;

	// 処理結果を格納するディレクトリ名を指定
	//string ResultDirectoryName = "C:\\home\\komiya\\Head_pose_estimation\\movie\\president\\2_George_W_Bush\\samplings\\sampling_1minutes\\spe_2004_0120_bush\\result";
	string ResultDirectoryName = "C:\\home\\komiya\\Head_pose_estimation\\data\\president\\processing\\ROI";
	//string ResultDirectoryName = "C:\\home\\komiya\\Head_pose_estimation\\data\\president\\1_Obama\\samplings\\sampling_1minutes\\spe_2009_0604_obama\\ROI\\test";
	FileDirectoryPath = ResultDirectoryName;

	// ファイル名を出力するためのテキストファイル名を指定
	//ofstream outputfile("C:\\home\\komiya\\Head_pose_estimation\\data\\president\\processing\\txt\\FileName.txt");
	
	
	// ディレクトリ内のファイルを複数指定する場合はフラグ1、ファイル名を指定する場合はフラグ0
	#if 1
		// ディレクトリ名からファイルリストを作成
		vector<string> backfilelist = Dir::read(DirectoryName);
		// 画像範囲を指定しない場合（ディレクトリ内全ファイルの出力をする場合）はフラグ1とする
		#if 1
			// ファイル名出力
			for(int i = 0; i < backfilelist.size(); i++) 
		#else
			// 静止画像No範囲指定
			// 開始番号
			//int BeginNum = 47;
			int BeginNum = 18;
			// 終了番号
			//int EndNum = 47;
			int EndNum = 61;
			// ファイル名出力
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
				// （1）静止画像データをファイルから読み込む
				frame = imread(FilePath.str(), IMREAD_COLOR);
				//  失敗したらエラー表示
				if(frame.empty()) {
					cout << "Cannot open." << endl;
					return -1;
				}
				// （2）静止画像データの処理結果を表示する
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
		// （1）静止画像データをファイルから読み込む
		frame = imread(FilePath.str(), IMREAD_COLOR);
		//  失敗したらエラー表示
		if(frame.empty()) {
			cout << "Cannot open." << endl;
			return -1;
		}
		// （2）静止画像データの処理結果を表示する
		// 顔処理
		FaceDetection(frame);
		imwrite(ResultFilePath.str(), frame);
		imshow(WindowName, frame);
		//// 切り出し処理
		//FaceDetection(frame);
		//imwrite(ResultFilePath.str(), frame);
		//imshow(WindowName, frame);
		//// 拡大処理

		waitKey(0);
	#endif
		

#else
	// 動画像読み込み
	#if 1
		fName = "spe_2010_0127_obama_5.avi";
	#else
		fName = argv[1];
	#endif
		printf("movie success \n");
		cap.open(fName);
		// 動画像が読み込まれた場合の処理
		// 動画像が開けなければエラー表示
		if(cap.isOpened()) {
			for(;;) {
				// 動画像ファイルから1フレーム分の画像データを取得して、変数frameに格納する
				cap >> frame;

				// 画像データ取得に失敗したらループを抜ける
				if (!frame.empty()) {
					detectAndDisplay( frame );
					imshow(WindowName, frame);
				}
				else
					break;

				// 取得した画像データをウィンドウ表示する
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

	/**-------- 顔検出処理 --------**/
	/* 正面顔検出 */
	face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(50, 50) );
	// 細線で描画
	//drawFace(frame, &faces, 1);

	// 太線で描画する条件
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
					//i基準
					//1のパターン
					//x軸
					faces[i].x <=  faces[j].x && faces[i].x <= (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) > (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y <=  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) > (faces[j].y + faces[j].height - 1) ||
					//2のパターン
					//x軸
					faces[i].x <=  faces[j].x && faces[i].x > (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) > (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y <=  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) > (faces[j].y + faces[j].height - 1) ||
					//3のパターン
					//x軸
					faces[i].x <=  faces[j].x && faces[i].x <= (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) <= (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y <=  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) > (faces[j].y + faces[j].height - 1) ||
					//4のパターン
					//x軸
					faces[i].x <=  faces[j].x && faces[i].x <= (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) > (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y >  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) > (faces[j].y + faces[j].height - 1) ||
					//5のパターン
					//x軸
					faces[i].x <=  faces[j].x && faces[i].x <= (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) > (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y <=  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) <= (faces[j].y + faces[j].height - 1) ||
					//6のパターン
					//x軸
					faces[i].x <=  faces[j].x && faces[i].x > (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) > (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y >  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) > (faces[j].y + faces[j].height - 1) ||
					//7のパターン
					//x軸
					faces[i].x <=  faces[j].x && faces[i].x <= (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) <= (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y <=  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) <= (faces[j].y + faces[j].height - 1) ||
					//8のパターン
					//x軸
					faces[i].x <=  faces[j].x && faces[i].x > (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) > (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y <=  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) <= (faces[j].y + faces[j].height - 1) ||
					//9のパターン
					//x軸
					faces[i].x <=  faces[j].x && faces[i].x <= (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) <= (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y >  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) > (faces[j].y + faces[j].height - 1) ||
					//10のパターン
					//x軸
					faces[i].x <=  faces[j].x && faces[i].x > (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) > (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y >  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) <= (faces[j].y + faces[j].height - 1) ||
					//11のパターン
					//x軸
					faces[i].x <=  faces[j].x && faces[i].x <= (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) <= (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y >  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) <= (faces[j].y + faces[j].height - 1) ||
					//12のパターン
					//x軸
					faces[i].x >  faces[j].x && faces[i].x <= (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) <= (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y >  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) > (faces[j].y + faces[j].height - 1) ||
					//13
					//x軸
					faces[i].x >  faces[j].x && faces[i].x <= (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) <= (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y <=  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) <= (faces[j].y + faces[j].height - 1) ||
					//14
					//x軸
					faces[i].x >  faces[j].x && faces[i].x <= (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) <= (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y >  faces[j].y && faces[i].y <= (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) <= (faces[j].y + faces[j].height - 1) ||

					//j基準
					//1のパターン
					//x軸
					faces[j].x <=  faces[i].x && faces[j].x <= (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) > (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y <=  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) > (faces[i].y + faces[i].height - 1) ||
					//2のパターン
					//x軸
					faces[j].x <=  faces[i].x && faces[j].x > (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) > (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y <=  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) > (faces[i].y + faces[i].height - 1) ||
					//3のパターン
					//x軸
					faces[j].x <=  faces[i].x && faces[j].x <= (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) <= (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y <=  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) > (faces[i].y + faces[i].height - 1) ||
					//4のパターン
					//x軸
					faces[j].x <=  faces[i].x && faces[j].x <= (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) > (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y >  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) > (faces[i].y + faces[i].height - 1) ||
					//5のパターン
					//x軸
					faces[j].x <=  faces[i].x && faces[j].x <= (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) > (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y <=  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) <= (faces[i].y + faces[i].height - 1) ||
					//6のパターン
					//x軸
					faces[j].x <=  faces[i].x && faces[j].x > (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) > (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y >  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) > (faces[i].y + faces[i].height - 1) ||
					//7のパターン
					//x軸
					faces[j].x <=  faces[i].x && faces[j].x <= (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) <= (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y <=  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) <= (faces[i].y + faces[i].height - 1) ||
					//8のパターン
					//x軸
					faces[j].x <=  faces[i].x && faces[j].x > (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) > (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y <=  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) <= (faces[i].y + faces[i].height - 1) ||
					//9のパターン
					//x軸
					faces[j].x <=  faces[i].x && faces[j].x <= (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) <= (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y >  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) > (faces[i].y + faces[i].height - 1) ||
					//10のパターン
					//x軸
					faces[j].x <=  faces[i].x && faces[j].x > (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) > (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y >  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) <= (faces[i].y + faces[i].height - 1) ||
					//11のパターン
					//x軸
					faces[j].x <=  faces[i].x && faces[j].x <= (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) <= (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y >  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) <= (faces[i].y + faces[i].height - 1) ||
					//12のパターン
					//x軸
					faces[j].x >  faces[i].x && faces[j].x <= (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) <= (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y >  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) > (faces[i].y + faces[i].height - 1) ||
					//13
					//x軸
					faces[j].x >  faces[i].x && faces[j].x <= (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) <= (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y <=  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) <= (faces[i].y + faces[i].height - 1) ||
					//14
					//x軸
					faces[j].x >  faces[i].x && faces[j].x <= (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) <= (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y >  faces[i].y && faces[j].y <= (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) <= (faces[i].y + faces[i].height - 1) ||
					
					//座標一致の場合
					faces[i].x ==  faces[j].x || faces[i].x == (faces[j].x + faces[j].width -1) || (faces[i].x + faces[i].width - 1) == faces[j].x || (faces[i].x + faces[i].width - 1) == (faces[j].x + faces[j].width - 1) ||
					faces[i].y ==  faces[j].y || faces[i].y == (faces[j].y + faces[j].height -1) || (faces[i].y + faces[i].height - 1) == faces[j].y || (faces[i].y+ faces[i].height - 1) == (faces[j].y + faces[j].height - 1) 
					)
						faces[j].width = -1;
				
			}	// for(j)
		}	// for(i)
	}
		//切り出し処理・拡大処理及び処理結果保存
		ClippingAndDisplay(frame, &faces);

		//太線で描画
		drawFace(frame, &faces, 2);
	

	/* 横顔検出 */
	//std::vector<Rect> profiles;
	//Mat profiles_gray = frame_gray;
	//profiles_cascade.detectMultiScale( profiles_gray, profiles, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(50, 50) );
	//if(faces.size() > 0)
	//{
	//	//細線で描画
	//	drawProfileFace(frame, &profiles, 1);
	//	//太線で描画
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

			// 数値から文字列に変換
			char c_str[256];
			sprintf(c_str, "%d", i+1);

			//番号の描画する場所
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

						// 番号の描画
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

			// 数値から文字列に変換
			char c_str[256];

			sprintf(c_str, "%d", i+1);
 
			//番号の描画する場所
			Point number(faces->at(i).x + faces->at(i).width/2, faces->at(i).y + faces->at(i).height/2 + 60 );

			//楕円の描画
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
			// 番号の描画
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
			// 検出した矩形の座標を格納して領域を1.5倍に拡張する
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


			// 結果を出力するパスとファイル名格納する領域の確保
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
			
			// 静止画像ファイル名から拡張子の取り出し
			char *name, *num;
			char c_fname[256];
			strcpy(c_fname, fName.c_str());
			name = strtok(c_fname, ".");

			//// 静止画像ファイル名から番号の取り出し
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

			// 結果を出力するディレクトリの作成
			//sprintf(c_fpath_num, "%s\\%s", fPath.c_str(), num);
			//if(_mkdir(c_fpath_num) == 0)
			//	cout << "Make directory_ImageNum" << endl;
			//sprintf(c_fpath, "%s\\%s\\%s%d", fPath.c_str(), num, "result_", i+1);
			//if(_mkdir(c_fpath) == 0)
			//	cout << "Make directory_resultNum" << endl;

			// 結果のファイル名格納
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


			// 検出部の矩形の取り出し処理と拡大処理用のMat生成
			Mat ROI(image, rect1);
			Mat dst(Size(200, 200), CV_8U, Scalar::all(0));

			// 矩形取り出し処理結果書き出し
			//imwrite(rclip, ROI);
			//imshow(rclip, ROI);

			// 拡大処理及び処理結果書き出し
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
