
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/xfeatures2d/nonfree.hpp"

#include "Header.h"

#include <iostream>
#include <vld.h>

#include <windows.h>
#include <Shlwapi.h>
#include <stdio.h>
#include <tchar.h>
#include <conio.h>
#include <Shlobj.h>
#include <string.h>
#include <map>

#pragma comment (lib, "Shell32.lib")
#pragma comment (lib, "Shlwapi.lib")

#include "opencv2/features2d.hpp" //Thanks to Alessandro

using namespace cv;
using namespace std;

bool saveKeyPointToFile(string fileName, const KeyPoint & key);
bool saveKeyPointToFile(tstring fileName, const vector<KeyPoint> & keys);
bool saveMatToFile(string fileName, const Mat & matMetadata);

bool DetectKeypointsAndDescriptors(tstring tsFileImg, tstring  tsFileSaveKeypoints, tstring  tsFileSaveDescriptors);
bool DetectFeaturesImageToFile(tstring sPathFolderImage, tstring sPathFolderDataDescriptors);
void Save(tstring sFileName, Mat & mat);
Mat Load(tstring sFileName);
int ScanAndProcessDir(LPCTSTR szPath, LPCTSTR szPathFolderDataOut);
int ProcessFile(LPCTSTR szPath, LPCTSTR szPathFolderDataOut);

//------------------------------------------------------------------------------
// Tim kiem anh trong thu muc - sd SIFT trich chon cac dac trung va tinh khoang
// cach tu anh toi cac anh trong csdl
//------------------------------------------------------------------------------
map<double, tstring> GetNearestImage(
	tstring tsPathImgQuery,
	tstring tsPathFolderImg,
	tstring tsPathFolderDataDes,
	int nCountImage = 10

	);

int FindNearestImageInDir(
	tstring tsFileImgQuery,
	LPCTSTR szPathFolderImage,
	LPCTSTR szPathFolderDataOut,
	map<double, tstring> & lsttsPathImageFound
	);

//------------------------------------------------------------------------------
// sd SIFT trich chon cac dac trung va tinh khoang
// cach tu anh toi cac anh trong csdl
//------------------------------------------------------------------------------
void ComputeDistance(
	tstring tsFileImgQuery,
	LPCTSTR szFileName,
	LPCTSTR szPathFolderDataOut,
	map<double, tstring> & lsttsPathImageFound);

//------------------------------------------------------------------------------
// Lay duoi file 
//------------------------------------------------------------------------------
int GetIdxOfTypeFile(LPCTSTR szPath);

//------------------------------------------------------------------------------
// Lay duong dan thu muc cha
//------------------------------------------------------------------------------
tstring GetParentFolder(LPCTSTR tszPath);

//------------------------------------------------------------------------------
// Cau truc du lieu luu tru ra file
//------------------------------------------------------------------------------
struct MatMetadata
{
	int nRows;
	int nCols;
	int nType;
	int nSizeData;
};

//------------------------------------------------------------------------------
// Bien luu tru kieu du lieu anh
//------------------------------------------------------------------------------
vector <tstring> g_lstImageType =
{
	_T(".png"),
	_T(".bmp"),
	_T(".jpeg"),
	_T(".jpg")
};

vector<float> g_lstDistances;
int g_nCountImageFound;
cv::Mat g_descriptorsQuery;


void drawText(Mat & image);

int HelloWorld()
{
	Mat img = imread("vietnam.jpg", CV_LOAD_IMAGE_COLOR);
	namedWindow("Viet Nam", CV_WINDOW_AUTOSIZE);
	imshow("Viet Nam", img);
	waitKey(0);
	return 0;
}

void ChuyenKhongGianMau()
{
	Mat src = imread("LucBinh.jpg", CV_LOAD_IMAGE_COLOR);
	Mat gray, hsv, ycrcb;
	cvtColor(src, gray, CV_BGR2GRAY);
	cvtColor(src, hsv, CV_BGR2HSV);
	cvtColor(src, ycrcb, CV_BGR2YCrCb);
	imshow("src", src);
	imshow("gray", gray);
	imshow("hsv", hsv);
	imshow("ycrcb", ycrcb);
	waitKey(0);
}

int DoSangVaTuongPhan()
{
	cout << "Chuong trinh dieu chinh do sang va tuong phan" << endl;
	Mat src = imread("hoa_huong_duong.jpg", CV_LOAD_IMAGE_COLOR);
	Mat dst = src.clone();
	double alpha =3.0;
	int beta = 10;
	for (int i = 0; i < src.rows; i++)
		for (int j = 0; j < src.cols; j++)
			for (int k = 0; k < 3; k++)
				dst.at<Vec3b>(i, j)[k] = saturate_cast<uchar>(alpha*(src.at<Vec3b>(i, j)[k]) + beta);
	imshow("anh goc", src);
	imshow("anh co sau khi chinh do tuong phan va do sang", dst);
	waitKey(0);
	return 0;
}

int NhiPhanHoaAnh()
{
	cout << "Nhi phan anh voi nguong dong" << endl;
	Mat src = imread("Thap_But.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	Mat dst;
	//adaptiveThreshold(src, dst, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 35, 5);
	threshold(src, dst, 150, 255, CV_THRESH_BINARY);
	imshow("Anh xam goc", src);
	imshow("Anh nhi phan voi nguong dong", dst);
	waitKey(0);
	return 1;
}

int TinhToanHistogram()
{
	
	std::cout << "Tim histogram anh mau" << std::endl;

	Mat src = imread("buoi.jpg");
	vector<Mat> img_rgb;
	Mat img_r, img_g, img_b;
	int w = 400, h = 400;
	int size_hist = 255;
	float range[] = { 0, 255 };
	const float* hist_range = { range };

	split(src, img_rgb);
// 	imshow(" img Red chennel", img_rgb[0]);
// 	imshow(" img blue chennel", img_rgb[1]);
// 	imshow(" img green chennel", img_rgb[2]);

	calcHist(&img_rgb[0], 1, 0, Mat(), img_b, 1, &size_hist, &hist_range, true, false);
	calcHist(&img_rgb[1], 1, 0, Mat(), img_g, 1, &size_hist, &hist_range, true, false);
	calcHist(&img_rgb[2], 1, 0, Mat(), img_r, 1, &size_hist, &hist_range, true, false);

	int bin = cvRound((double)w / size_hist);

	Mat disp_r(w, h, CV_8UC3, Scalar(255, 255, 255));
	Mat disp_g = disp_r.clone();
	Mat disp_b = disp_r.clone();
	/// Normalize the result to [ 0, histImage.rows ]
	normalize(img_b, img_b, 0, disp_b.rows, NORM_MINMAX, -1, Mat());
	normalize(img_g, img_g, 0, disp_g.rows, NORM_MINMAX, -1, Mat());
	normalize(img_r, img_r, 0, disp_r.rows, NORM_MINMAX, -1, Mat());

	for (int i = 1; i < 255; i++)
	{
		line(disp_b, Point(bin*(i), h), Point(bin*(i), h - cvRound(img_b.at<float>(i))),
			Scalar(255, 0, 0), 2, 8, 0);
		line(disp_g, Point(bin*(i), h), Point(bin*(i), h - cvRound(img_g.at<float>(i))),
			Scalar(0, 255, 0), 2, 8, 0);
		line(disp_r, Point(bin*(i), h), Point(bin*(i), h - cvRound(img_r.at<float>(i))),
			Scalar(0, 0, 255), 2, 8, 0);
	}
	namedWindow("src", 0);
	imshow("src", src);
	imshow("Histogram of Red chennel", disp_r);
	imshow("Histogram of Green chennel", disp_g);
	imshow("Histogram of Blue chennel", disp_b);
	cv::waitKey(0);
	return 1;
}

void CanbangHistogramGraph()
{
	cout << "Chuong trinh can bang histogram" << endl;
	Mat src = imread("Cho_Ben_Thanh.jpg", CV_LOAD_IMAGE_COLOR);

	// Can bang histogram anh xam
	Mat gray;
	cvtColor(src, gray, CV_BGR2GRAY);
	imshow("Anh Xam goc", gray);
	equalizeHist(gray, gray);
	imshow("Anh xam sau khi can bang histogram", gray);

	// Can bang histogram anh mau

	Mat hsv, disp;
	cvtColor(src, hsv, CV_BGR2HSV);
	vector<Mat> hsv_channels;
	// Tach hsv thanh 3 kenh mau
	split(hsv, hsv_channels);
	// Can bang histogram kenh mau v (value)
	equalizeHist(hsv_channels[2], hsv_channels[2]);
	// Tron anh
	merge(hsv_channels, hsv);
	// Chuyen doi hsv sang rgb de hien thi
	cvtColor(hsv, disp, CV_HSV2BGR);
	imshow("anh mau goc", src);
	imshow("anh mau sau khi can bang histogram", disp);
	waitKey(0);

}

int siftTest(){
	const cv::Mat input = cv::imread("vietnam.jpg", 0); //Load as grayscale

	cv::Feature2D detector;
	std::vector<cv::KeyPoint> keypoints;
	imshow("vietnam src", input);
	detector.detect(input, keypoints);

	// Add results to image and save.
 	cv::Mat output;
 	cv::drawKeypoints(input, keypoints, output);
 	cv::imwrite("sift_result.jpg", output);
	cv::waitKey(0);

	return 0;

// 	
// 	// now, you can no more create an instance on the 'stack', like in the tutorial
// 	// (yea, noticed for a fix/pr).
// 	// you will have to use cv::Ptr all the way down:
// 	//
// 	cv::Ptr<Feature2D> f2d = Feature2D::create();
// 	//cv::Ptr<Feature2D> f2d = xfeatures2d::SURF::create();
// 	//cv::Ptr<Feature2D> f2d = ORB::create();
// 	// you get the picture, i hope..
// 	
// 	//-- Step 1: Detect the keypoints:
// 	std::vector<KeyPoint> keypoints_1, keypoints_2;
// 	f2d->detect(img_1, keypoints_1);
// 	f2d->detect(img_2, keypoints_2);
// 	
// 	//-- Step 2: Calculate descriptors (feature vectors)    
// 	Mat descriptors_1, descriptors_2;
// 	f2d->compute(img_1, keypoints_1, descriptors_1);
// 	f2d->compute(img_2, keypoints_2, descriptors_2);
// 	
// 	//-- Step 3: Matching descriptor vectors using BFMatcher :
// 	BFMatcher matcher;
// 	std::vector< DMatch > matches;
// 	matcher.match(descriptors_1, descriptors_2, matches);
}

bool saveKeyPointToFile(string fileName, const KeyPoint & keypoint)
{
	bool bRet = false;
	int nSizeObj = sizeof(KeyPoint);
	const KeyPoint* pMem = &keypoint;
	FILE * fSaveObject = NULL;

	if (fileName.size() == 0) goto _EXIT_FUNCTION;

	fSaveObject = fopen(fileName.c_str(), "ab+");

	if (fSaveObject == NULL) goto _EXIT_FUNCTION;

	fwrite(pMem, nSizeObj, 1, fSaveObject);

	bRet = true;

_EXIT_FUNCTION:
	if (fSaveObject) fclose(fSaveObject);

	return bRet;
}

bool saveKeyPointToFile(tstring tsFileName, const vector<KeyPoint> & keys)
{
	bool bRet = false;
	int nSizeObj = sizeof(KeyPoint);
	int nCountObj = keys.size();
	const KeyPoint* pMem = &keys.at(0);
	string sFileName(tsFileName.begin(), tsFileName.end());
	FILE * fSaveObject = NULL;

	if (sFileName.size() == 0 || nCountObj == 0) goto _EXIT_FUNCTION;

	fSaveObject = fopen(sFileName.c_str(), "ab+");

	if (fSaveObject == NULL) goto _EXIT_FUNCTION;

	fwrite(pMem, nSizeObj, nCountObj, fSaveObject);

	bRet = true;

_EXIT_FUNCTION:
	if (fSaveObject) fclose(fSaveObject);

	return bRet;
}

bool loadKeyPointFromFile(string fileName, vector<KeyPoint> & vtResult)
{
	bool bRet = false;
	int nSizeObj = sizeof(KeyPoint);
	KeyPoint*	pMem = NULL;
	size_t		nSizeFile = 0;
	FILE * fSaveObject = NULL;

	if (fileName.size() == 0) goto _EXIT_FUNCTION;

	// Mo file doc du lieu
	fSaveObject = fopen(fileName.c_str(), "rb");
	if (fSaveObject == NULL) goto _EXIT_FUNCTION;

	// Lay kich thuoc file
	fseek(fSaveObject, 0, SEEK_END);   // non-portable
	nSizeFile = ftell(fSaveObject);
	if (nSizeFile == 0 || nSizeFile%nSizeObj != 0)
		goto _EXIT_FUNCTION;
	fseek(fSaveObject, 0, SEEK_SET);   // non-portable

	// Doc noi dung File
	pMem = new KeyPoint[nSizeFile / nSizeObj];
	if (pMem == NULL)
		goto _EXIT_FUNCTION;
	fread(pMem, nSizeObj, nSizeFile / nSizeObj, fSaveObject);

	// Gan noi dung vao vector
	vtResult = std::vector<KeyPoint>(pMem, pMem + nSizeFile / nSizeObj);
	bRet = true;

_EXIT_FUNCTION:
	if (fSaveObject) fclose(fSaveObject);
	if (pMem) delete[]pMem;

	return bRet;

}


bool saveMatToFile(string fileName, const Mat & matMetadata)
{
 	bool bRet = false;
	int nSizeHead = sizeof(MatMetadata);
	MatMetadata headData;
	FILE * fSaveObject = NULL;
	MatStep tt;

	if (fileName.size() == 0) goto _EXIT_FUNCTION;

	fSaveObject = fopen(fileName.c_str(), "wb");

	if (fSaveObject == NULL) goto _EXIT_FUNCTION;

	headData.nCols = matMetadata.cols;
	headData.nRows = matMetadata.rows;
	headData.nType = matMetadata.type();
	headData.nSizeData = matMetadata.dataend - matMetadata.datastart;

	fwrite(&headData, nSizeHead, 1, fSaveObject);
	fwrite(matMetadata.data, headData.nSizeData, 1, fSaveObject);

	bRet = true;

_EXIT_FUNCTION:
	if (fSaveObject) fclose(fSaveObject);

	return bRet;
}

bool loadMatFromFile(string fileName, Mat & matMetadata)
{
	bool bRet = false;
	int nSizeHead = sizeof(MatMetadata);
	MatMetadata headData;
	FILE * fSaveObject = NULL;
	unsigned char* pMem = NULL;
	MatStep tt;

	if (fileName.size() == 0) goto _EXIT_FUNCTION;

	fSaveObject = fopen(fileName.c_str(), "rb");

	if (fSaveObject == NULL) goto _EXIT_FUNCTION;

	fread(&headData, nSizeHead, 1, fSaveObject);

	if (headData.nSizeData == 0)
		goto _EXIT_FUNCTION;

	pMem = (uchar*)new char[headData.nSizeData];
	fread(pMem, headData.nSizeData, 1, fSaveObject);

	matMetadata = Mat(headData.nRows, headData.nCols, headData.nType, pMem);

	bRet = true;

_EXIT_FUNCTION:
	if (fSaveObject) fclose(fSaveObject);

	return bRet;
}

void Save(tstring tsFileName, Mat & mat)
{
	string sFileName(tsFileName.begin(), tsFileName.end());
	FileStorage fs(sFileName, FileStorage::WRITE);
	fs << "dataMat" << mat;
	fs.release();
}

Mat Load(tstring tsFileName)
{
	Mat m;
	string sFileName(tsFileName.begin(), tsFileName.end());
	FileStorage fs(sFileName, FileStorage::READ);
	fs["dataMat"] >> m;
	fs.release();
	return m;
}

bool DetectFeaturesImageToFile(tstring sPathFolderImage, tstring sPathFolderDataDescriptors)
{
	bool bRet = false;
	
	SHCreateDirectoryEx( NULL, sPathFolderDataDescriptors.c_str(), NULL);

	if (!PathIsDirectory(sPathFolderDataDescriptors.c_str()))
		goto _EXIT_FUNCTION;

	ScanAndProcessDir(sPathFolderImage.c_str(), sPathFolderDataDescriptors.c_str());

	bRet = true;

_EXIT_FUNCTION:
	return bRet;
}

//------------------------------------------------------------------------------
// Quet 1 folder
//------------------------------------------------------------------------------
int ScanAndProcessDir(LPCTSTR szPathFolderImage, LPCTSTR szPathFolderDataOut)
{

	WIN32_FIND_DATA	w32Data;
	TCHAR szFileName[MAX_PATH + 1];
	HANDLE hFind = INVALID_HANDLE_VALUE;

	if (szPathFolderImage == NULL) return 0;
	if (!PathIsDirectory(szPathFolderImage)) return 0;
	if (PathIsDirectoryEmpty(szPathFolderImage))
		return 0;

	_stprintf_s(szFileName, MAX_PATH, _T("%s\\*.*"), szPathFolderImage);
	__try
	{
		hFind = FindFirstFile(szFileName, &w32Data);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (!_tcscmp(w32Data.cFileName, _T(".")) || !_tcscmp(w32Data.cFileName, _T("..")))
					continue;
				_stprintf_s(szFileName, MAX_PATH, _T("%s\\%s"), szPathFolderImage, w32Data.cFileName);

				// Neu la folder >> duyet de qui thu muc
				if ((w32Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
				{
					//ScanAndProcessDir(szFileName);
					continue;
				}
				else{
					// xu ly file
					ProcessFile(szFileName, szPathFolderDataOut);
				}


			} while (FindNextFile(hFind, &w32Data));
		}
	}
	__finally
	{
		if (hFind != INVALID_HANDLE_VALUE){
			FindClose(hFind);
		}
		RemoveDirectory(szPathFolderImage);
	}
	return 1;
}

//------------------------------------------------------------------------------
// Lay duoi file 
//------------------------------------------------------------------------------
int GetIdxOfTypeFile(LPCTSTR szPath)
{
	int i;
	int nLeng = _tcsnlen(szPath, MAX_PATH);
	char nCharCheck = 0;
	for (i = nLeng - 1; i >= 0; i--)
	{
		nCharCheck = szPath[i];
		if (nCharCheck == '\\' || nCharCheck == '.' || nCharCheck == 0)
			break;
	}
	return i;
}

//------------------------------------------------------------------------------
// Lay ten file 
//------------------------------------------------------------------------------
tstring GetNameFile(LPCTSTR szPath)
{
	int nIdx = GetIdxOfTypeFile(szPath);
	int idxName;
	char nCharCheck;
	tstring tsPath = szPath;
	tstring tsRet;
	
	for (idxName = nIdx - 1; idxName >= 0; idxName--)
	{
		nCharCheck = szPath[idxName];
		if (nCharCheck == '\\')
		{
			tsRet = tsPath.substr(idxName+1, nIdx - idxName - 1);
			break;
		}
	}

	return tsRet;
}

//------------------------------------------------------------------------------
// Lay duong dan thu muc cha
//------------------------------------------------------------------------------
tstring GetParentFolder(LPCTSTR tszPath)
{
	tstring tsFolder = tszPath;
	tstring tsRet = tsFolder;
	int nSizeSzFolder = tsFolder.size();

	if (nSizeSzFolder == 0)
		goto _EXIT_FUNCTION;

	// Kiem tra vi tri '\' co o vi tri cuoi cung hay ko
	if (tsFolder.at(nSizeSzFolder - 1) == '\\')
		nSizeSzFolder--;

	// Tim vi tri '\' cuoi cung 
	for (int i = nSizeSzFolder - 1; i >= 0; i--)
	{
		if (tsFolder.at(i) == '\\')
		{
			if (i > 0 )
			{
				tsRet = tsFolder.substr(0, i);
			}
			break;
		}
	}

_EXIT_FUNCTION:
	return tsRet;
}

//------------------------------------------------------------------------------
// Lay dia chi file chua cac dac trung da duoc tinh toan trong csdl
//------------------------------------------------------------------------------
tstring GetPathDataFileKeyPointOfImg(tstring tsFileImageInDB, tstring tsPathFolderDataDes)
{
	tstring tsPathDataFileOfImg;

	tstring tsNameFile = GetNameFile(tsFileImageInDB.c_str());
	tsPathDataFileOfImg = tsPathFolderDataDes + _T("\\") + tsNameFile + _T(".keypoints");

	return tsPathDataFileOfImg;
}

//------------------------------------------------------------------------------
// Lay dia chi file chua cac dac trung da duoc tinh toan trong csdl
//------------------------------------------------------------------------------
tstring GetPathDataFileDescriptorsOfImg(tstring tsFileImageInDB, tstring tsPathFolderDataDes)
{
	tstring tsPathDataFileOfImg;

	tstring tsNameFile = GetNameFile(tsFileImageInDB.c_str());
	tsPathDataFileOfImg = tsPathFolderDataDes + _T("\\") + tsNameFile + _T(".descriptors");

	return tsPathDataFileOfImg;
}

//------------------------------------------------------------------------------
// Quet 1 file 
//------------------------------------------------------------------------------
BOOL ProcessFile(LPCTSTR tszPathFile, LPCTSTR szPathFolderDataOut)
{
	BOOL bRet = FALSE;
	BOOL bCheckType = FALSE;
	
	int nSizeList = g_lstImageType.size();
	int idx = GetIdxOfTypeFile(tszPathFile);
	tstring tsFileSaveKeypoints;
	tstring tsFileSaveDescriptors;
	tstring tsFileSave;
	tstring tsTypeCheck;
	tstring tsFileImg;

	if (tszPathFile == NULL) goto _EXIT_FUNCTION;

	tsFileImg = tszPathFile;

	for (int i = 0; i < nSizeList; i++)
	{
		tsTypeCheck = g_lstImageType.at(i);
		if (_tcsicmp(tszPathFile + idx, tsTypeCheck.c_str()) == 0)
		{
			bCheckType = TRUE;
			break;
		}
	}
	
	if (bCheckType)
	{
		tsFileSave = tstring(szPathFolderDataOut) + _T("\\") + GetNameFile(tszPathFile);
		tsFileSaveKeypoints = tsFileSave + _T(".keypoints");
		tsFileSaveDescriptors = tsFileSave + _T(".descriptors");
		OutputDebugString(_T("\n"));
		OutputDebugString(tsFileImg.c_str());
		DetectKeypointsAndDescriptors(tsFileImg, tsFileSaveKeypoints, tsFileSaveDescriptors);
	}

	bRet = TRUE;

_EXIT_FUNCTION:
	return bRet;
}

//------------------------------------------------------------------------------
// Tim kiem cac diem - cac dac trung cua anh
// Luu tru dac diem - dac trung tim duoc ra file
//------------------------------------------------------------------------------
bool DetectKeypointsAndDescriptors(
	tstring tsPathFileImg,
	tstring tsPathFileKeypoints, 
	tstring tsPathFileDescriptors)
{
	string sPathFileImg(tsPathFileImg.begin(), tsPathFileImg.end());
	cv::Ptr<Feature2D> f2d = cv::xfeatures2d::SIFT::create();
	Mat descriptors;
	std::vector<KeyPoint> keypoints;
	const cv::Mat img = cv::imread(sPathFileImg, CV_LOAD_IMAGE_GRAYSCALE); //Load as grayscale
	bool bRet = false;

	//-- Step 1: Detect the keypoints:
	f2d->detect(img, keypoints);
	if (saveKeyPointToFile(tsPathFileKeypoints, keypoints) == false)
	{
		goto _EXIT_FUNCTION;
	}

	//-- Step 2: Calculate descriptors (feature vectors)    
	f2d->compute(img, keypoints, descriptors);
	Save(tsPathFileDescriptors, descriptors);

	bRet= true;

_EXIT_FUNCTION:
	return bRet;
}

//------------------------------------------------------------------------------
// Tim kiem anh trong thu muc - sd SIFT trich chon cac dac trung va tinh khoang
// cach tu anh toi cac anh trong csdl
//------------------------------------------------------------------------------
map<double, tstring> GetNearestImage(
	tstring tsPathImgQuery, 
	tstring tsPathFolderImg,
	tstring tsPathFolderDataDes,
	int nCountImage
	)
{
	bool bRet = false;
	map<double, tstring> lstRet;
	string sPathFileImg(tsPathImgQuery.begin(), tsPathImgQuery.end());
	cv::Mat imgQuery = cv::imread(sPathFileImg, CV_LOAD_IMAGE_GRAYSCALE); //Load as grayscale

	cv::Ptr<Feature2D> f2d = cv::xfeatures2d::SIFT::create();
	std::vector<KeyPoint> keypointsQuery;

	g_nCountImageFound = nCountImage;

	if (imgQuery.data == NULL) goto _EXIT_FUNCTION;

	//Tim kiem keypoints:
	f2d->detect(imgQuery, keypointsQuery);

	//Tinh toan descriptors (feature vectors)    
	f2d->compute(imgQuery, keypointsQuery, g_descriptorsQuery);

	//Tim duoc 1 anh thi anh xa sang thu muc du lieu anh doc thong tin cac trich 
	//chon neu co anh ma khong co file thong tin dac trung cua anh thi se tao ra 
	//1 file luu tru thong tin
	FindNearestImageInDir(tsPathImgQuery, tsPathFolderImg.c_str(), 
		tsPathFolderDataDes.c_str(), lstRet);

_EXIT_FUNCTION:
	return lstRet;
}

void ComputeDistance(
	tstring tsFileImgQuery,
	LPCTSTR szFileName,
	LPCTSTR szPathFolderDataOut,
	map<double, tstring> & maptsPathImageFound)
{
	bool bRet = false;
	tstring tsFileNameInDB = szFileName;
	string sFileNameInDB(tsFileNameInDB.begin(), tsFileNameInDB.end());

	tstring tsPathFileKeypoints;
	tstring tsPathFileDescriptors;
	string sPathFileKeypoints;
	string sPathFileDescriptors;

	FILE *fileCheckExist = NULL;

	cv::Mat descriptorsDB;
	cv::Mat imgInDB;

	BFMatcher matcher;
	std::vector< DMatch > matches;
	std::vector< DMatch > good_matches;
	map<double, tstring> ::iterator it;// = maptsPathImageFound.end();


	//Tim kiem tung anh trong tsPathFolderImg - tinh toan khoang cach

	//Tim duoc 1 anh thi anh xa sang thu muc du lieu anh doc thong tin cac trich 
	//chon neu co anh ma khong co file thong tin dac trung cua anh thi se tao ra 
	//1 file luu tru thong tin
	tsPathFileKeypoints = GetPathDataFileKeyPointOfImg(tsFileNameInDB, szPathFolderDataOut);
	tsPathFileDescriptors = GetPathDataFileDescriptorsOfImg(tsFileNameInDB, szPathFolderDataOut);

	// Kiem tra su ton tai cua file
	sPathFileDescriptors = string(tsPathFileDescriptors.begin(), tsPathFileDescriptors.end());
	fileCheckExist = fopen(sPathFileDescriptors.c_str(), "r");
	if (fileCheckExist == NULL)
	{
		// File khong ton tai - trich chon dac trung cua anh hien tai va luu 
		// ra file cho lan sd sau
		goto _EXIT_FUNCTION;
	}
	else{
		fclose(fileCheckExist);

		// File ton tai - doc cac dac trung cua file
		descriptorsDB = Load(tsPathFileDescriptors);
		matcher.match(g_descriptorsQuery, descriptorsDB, matches);
	}

	// Chon nhung dac trung tot nhat 
	double max_dist = 0, min_dist = 1000, fSumDistance = 0, fAVGDistance;

	//-- Quick calculation of max and min distances between keypointsQuery
	for (int i = 0; i < g_descriptorsQuery.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	if (maptsPathImageFound.size() > 0)
	{
		it = maptsPathImageFound.end();
		it--;
		if (min_dist > it->first)
			goto _EXIT_FUNCTION;
	}

// 	std::cout << "\nMax dist :" << max_dist;
// 	std::cout << "\nMin dist :" << min_dist;

	//-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
	//-- or a small arbitary value ( 0.02 ) in the event that min_dist is very
	//-- small)
	//-- PS.- radiusMatch can also be used here.
	for (int i = 0; i < g_descriptorsQuery.rows; i++)
	{
		if (matches[i].distance <= max(1.3 * min_dist, 0.02))
		{
			good_matches.push_back(matches[i]);
			fSumDistance += matches[i].distance;
		}
	}

	// tinh trung binh cong khoang cach
	fAVGDistance = fSumDistance / good_matches.size();
	std::cout << "\n[" << sPathFileDescriptors << "] avg dist :" << fAVGDistance;
	OutputDebugString(_T("\n"));
	OutputDebugString(tsFileNameInDB.c_str());

	if (maptsPathImageFound.size() < g_nCountImageFound)
	{
		maptsPathImageFound.insert({ fAVGDistance, tsFileNameInDB });
	}
	else {
		it = maptsPathImageFound.end();
		it--;
		if (it->first > fAVGDistance)
		{
			// Them vao danh sach
			maptsPathImageFound.insert({ fAVGDistance, tsFileNameInDB });

			it = maptsPathImageFound.end();
			it--;
			if (maptsPathImageFound.size() > g_nCountImageFound)
				maptsPathImageFound.erase(it);
		}
	}
_EXIT_FUNCTION:
	return;
}

//------------------------------------------------------------------------------
// Quet 1 folder
//------------------------------------------------------------------------------
int FindNearestImageInDir(
	tstring tsFileImgQuery,
	LPCTSTR szPathFolderImage,
	LPCTSTR szPathFolderDataOut,
	map<double, tstring> & lsttsPathImageFound
	)
{
	WIN32_FIND_DATA	w32Data;
	TCHAR szFileName[MAX_PATH + 1];
	HANDLE hFind = INVALID_HANDLE_VALUE;

	if (szPathFolderImage == NULL) return 0;
	if (!PathIsDirectory(szPathFolderImage)) return 0;
	if (PathIsDirectoryEmpty(szPathFolderImage))
		return 0;

	_stprintf_s(szFileName, MAX_PATH, _T("%s\\*.*"), szPathFolderImage);
	try
	{
		hFind = FindFirstFile(szFileName, &w32Data);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (!_tcscmp(w32Data.cFileName, _T(".")) || !_tcscmp(w32Data.cFileName, _T("..")))
					continue;
				_stprintf_s(szFileName, MAX_PATH, _T("%s\\%s"), szPathFolderImage, w32Data.cFileName);

				// Neu la folder >> duyet de qui thu muc
				if ((w32Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
				{
					//ScanAndProcessDir(szFileName);
					continue;
				}
				else{
					// xu ly file
					ComputeDistance(tsFileImgQuery, szFileName, szPathFolderDataOut, lsttsPathImageFound);
				}


			} while (FindNextFile(hFind, &w32Data));
		}
	}
	catch (...)
	{
		if (hFind != INVALID_HANDLE_VALUE){
			FindClose(hFind);
		}
		RemoveDirectory(szPathFolderImage);
	}
	return 1;
}

int siftTest2()
{
	// now, you can no more create an instance on the 'stack', like in the tutorial
	// (yea, noticed for a fix/pr).
	// you will have to use cv::Ptr all the way down:
		//
	const cv::Mat img_1 = cv::imread("C:\\Users\\SONY\\Pictures\\sifttest\\2.PNG", 0); //Load as grayscale
	const cv::Mat img_2 = cv::imread("C:\\Users\\SONY\\Pictures\\sifttest\\0.PNG", 0); //Load as grayscale

	imshow("src1", img_1);
	imshow("src2", img_2);

	//cv::Ptr<Feature2D> f2d = cv::xfeatures2d::SIFT::create(0, 3, 0.15);
	//cv::Ptr<Feature2D> f2d = cv::xfeatures2d::SIFT::create(0);
	
	cv::Ptr<Feature2D> f2d = cv::xfeatures2d::SIFT::create();

	//xfeatures2d::SiftDescriptorExtractor extractor(3.0);

	//cv::Ptr<Feature2D> f2d = xfeatures2d::SURF::create();
	//cv::Ptr<Feature2D> f2d = ORB::create();
	// you get the picture, i hope..

	//-- Step 1: Detect the keypointsQuery:
	std::vector<KeyPoint> keypoints_1, keypoints_2;
	f2d->detect(img_1, keypoints_1);
	f2d->detect(img_2, keypoints_2);

	//bool bRet = saveKeyPointToFile("hi2.dat", keypoints_2);
	//bool bRet = saveKeyPointToFile("hi.dat", te2);
	//vector<KeyPoint> keypoints_test;
	//loadKeyPointFromFile("hi2.dat", keypoints_test);

	//-- Step 2: Calculate descriptors (feature vectors)    
	Mat descriptors_1, descriptors_2;
	f2d->compute(img_1, keypoints_1, descriptors_1);
	f2d->compute(img_2, keypoints_2, descriptors_2);

#ifdef _DRAW_KEYPOINT_IMG
	Mat output1, output2; 
	drawKeypoints(img_1, keypoints_1, output1);
	drawKeypoints(img_2, keypoints_2, output2);
	imshow("img with keypoint1", output1);
	imshow("img with keypoint2", output2);
#endif
	//imwrite("ngon.jpg", descriptors_1);
	
	//Save(_T("tesss.descr"), descriptors_1);
	//Mat test;
	//bool br = loadMatFromFile("mat.data", test);
	//test = Load(_T("tesss.descr"));

		
	//-- Step 3: Matching descriptor vectors using BFMatcher :
	BFMatcher matcher;
	std::vector< DMatch > matches;
	matcher.match(descriptors_1, descriptors_2, matches);


	double max_dist = 0; double min_dist = 1000;


	//-- Quick calculation of max and min distances between keypointsQuery
	for (int i = 0; i < descriptors_1.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	std::cout<<"\nMax dist :"<< max_dist ;
	std::cout<<"\nMin dist :"<< min_dist ;

	//-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
	//-- or a small arbitary value ( 0.02 ) in the event that min_dist is very
	//-- small)
	//-- PS.- radiusMatch can also be used here.
	std::vector< DMatch > good_matches;
	
	for (int i = 0; i < descriptors_1.rows; i++)
	{
		if (matches[i].distance <= max(2 * min_dist, 0.02))
		{
			good_matches.push_back(matches[i]);
		}
	}

	Mat matchImg;
	drawMatches(img_1, keypoints_1, img_2, keypoints_2, good_matches, matchImg
		, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	imshow("img with match", matchImg);


	cv::waitKey(0);
	return 1;
}


int main()
{
	//siftTest2();
	tstring tszPathFolderImg = _T("E:\\DH\\NAM_4\\Ki 2\\MMDB\\DataImage\\Img");
	tstring tszPathFolderDataDes = _T("E:\\DH\\NAM_4\\Ki 2\\MMDB\\DataImage\\DataDescriptors");
	tstring tsFileImg = _T("C:\\Users\\SONY\\Pictures\\sifttest\\te.jpg");
	map<double, tstring> mapRet;
	mapRet = GetNearestImage(tsFileImg, tszPathFolderImg, tszPathFolderDataDes);
	//ScanAndProcessDir(tszPathFolderImg);

	
// 	cout << "Built with OpenCV " << CV_VERSION << endl;
// 	Mat image;
// 	VideoCapture capture;
// 	capture.open(0);
// 	if (capture.isOpened())
// 	{
// 		cout << "Capture is opened" << endl;
// 		for (;;)
// 		{
// 			capture >> image;
// 			if (image.empty())
// 				break;
// 			drawText(image);
// 			imshow("Sample", image);
// 			if (waitKey(10) >= 0)
// 				break;
// 		}
// 	}
// 	else
// 	{
// 		cout << "No capture" << endl;
// 		image = Mat::zeros(480, 640, CV_8UC1);
// 		drawText(image);
// 		imshow("Sample", image);
// 		waitKey(0);
// 	}
 	return 0;
}

void drawText(Mat & image)
{
	putText(image, "Hello OpenCV",
		Point(20, 50),
		FONT_HERSHEY_COMPLEX, 1, // font face and scale
		Scalar(255, 255, 255), // white
		1, LINE_AA); // line thickness and type
}
