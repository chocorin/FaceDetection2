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

/**-------- 検出器読み込み --------**/
if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
if( !profiles_cascade.load( profile_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };


/**-------- データ読み込み --------**/
/* 静止画像の読み込み時は フラグ1 */
/* 動画像　の読み込み時は フラグ0 */

#if 1
	// 静止画像読み込み
	// 静止画像を格納しているディレクトリ名を指定
	//string DirectoryName = "C:\\home\\komiya\\Head_pose_estimation\\movie\\president\\2_George_W_Bush\\samplings\\sampling_1minutes\\spe_2004_0120_bush";
	//string DirectoryName = "C:\\home\\komiya\\Head_pose_estimation\\movie\\president\\processing";
	string DirectoryName = "C:\\home\\komiya\\Head_pose_estimation\\movie\\president\\1_Obama\\samplings\\sampling_1minutes\\spe_2010_0127_obama";

	// 処理結果を格納するディレクトリ名を指定
	//string ResultDirectoryName = "C:\\home\\komiya\\Head_pose_estimation\\movie\\president\\2_George_W_Bush\\samplings\\sampling_1minutes\\spe_2004_0120_bush\\result";
	//string ResultDirectoryName = "C:\\home\\komiya\\Head_pose_estimation\\movie\\president\\processing\\result";
	string ResultDirectoryName = "C:\\home\\komiya\\Head_pose_estimation\\movie\\president\\1_Obama\\samplings\\sampling_1minutes\\spe_2010_0127_obama\\result_2";

	// ファイル名を出力するためのテキストファイル名を指定
	//ofstream outputfile("C:\\home\\komiya\\Head_pose_estimation\\movie\\president\\processing\\txt\\FileName.txt");
	
	
	// ディレクトリ内のファイルを複数指定する場合はフラグ1、ファイル名を指定する場合はフラグ0
	#if 0
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
			int BeginNum = 726;
			// 終了番号
			//int EndNum = 47;
			int EndNum = 727;
			// ファイル名出力
			for(int i = BeginNum -1; i < EndNum; i++)
		#endif
			{
				stringstream FilePath;
				stringstream ResultFilePath;
				cout << backfilelist[i] << endl;
				outputfile << backfilelist[i] << endl;
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
		imshow(WindowName, frame);
		waitKey(0);
	#endif
		

#else
	// 動画像読み込み
	#if 1
		fileName = "spe_2010_0127_obama_5.avi";
	#else
		fileName = argv[1];
	#endif
		printf("movie success \n");
		cap.open(fileName);
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
	if(faces.size() > 0)
	{
		//細線で描画
		//drawFace(frame, &faces, 1);
		//太線で描画
		drawFace(frame, &faces, 2);
	}


	/* 横顔検出 */
	std::vector<Rect> profiles;
	Mat profiles_gray = frame_gray;
	profiles_cascade.detectMultiScale( profiles_gray, profiles, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(50, 50) );
	if(faces.size() > 0)
	{
		//細線で描画
		//drawProfileFace(frame, &profiles, 1);
		//太線で描画
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

			//ROI処理および保存
			Rect rect1;
			rect1.x =faces->at(i).x;
			rect1.y = faces->at(i).y;
			rect1.width = faces->at(i).width;
			rect1.height = faces->at(i).height;
			ClippingAndDisplay(image, rect1);

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
						break;
			}	// swtich(flag)
			// 番号の描画
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
void ClippingAndDisplay(Mat image, Rect rect)
{
		//Mat ROI(image, Rect(200,200,100,100));
		Mat ROI(image, rect);
		imwrite("clipped.jpg", ROI);
		imshow("result_clipped", ROI);
}