#include "stdafx.h"
#include "common.h"
#include <opencv2/core/utils/logger.hpp>

using namespace std;

wchar_t* projectPath;

int* histogram(Mat src, int bins) {
	int* histo = (int*)calloc(bins, sizeof(int));
	int height = src.rows;
	int width = src.cols;
	int interval = 256 / bins;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			histo[src.at<uchar>(i, j) / interval]++;
		}
	}

	return histo;
}

uchar getTreshold(Mat src) {
	uchar min = 255;

	for (int i = 0; i < src.cols; i++) {
		for (int j = 0; j < src.rows; j++) {
			if (src.at<uchar>(i, j) < min)
				min = src.at<uchar>(i, j);
		}
	}

	return min + 10;
}


Mat binarise(Mat img)
{
	int treshold = getTreshold(img);
	int height = img.rows;
	int width = img.cols;
	Mat dst = Mat(height, width, CV_8UC1);

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (img.at<uchar>(i, j) < treshold) {
				dst.at<uchar>(i, j) = 0;
			}
			else {
				dst.at<uchar>(i, j) = 255;
			}
		}
	}
	return dst;
}


Mat rotateOriginal(Mat src, double angle)
{
	int height = src.rows;
	int width = src.cols;

	Point2f center(src.cols / 2.0, src.rows / 2.0);

	double sinAngle = sin(angle);
	double cosAngle = cos(angle);

	cout << "SIN = " << sinAngle << endl;
	cout << "COS = " << cosAngle << endl;

	Mat dst = Mat(height, width, CV_8UC1);
	dst.setTo(255);


	for (int y = 0; y < width; y++) {
		for (int x = 0; x < height; x++) {
			double x_new = (x - center.x) * cosAngle - (y - center.y) * sinAngle + center.x;
			double y_new = (x - center.x) * sinAngle + (y - center.y) * cosAngle + center.y;

			if (x_new - floor(x_new) < 0.5) {
				x_new = floor(x_new);
			}
			else {
				x_new = ceil(x_new);
			}

			if (y_new - floor(y_new) < 0.5) {
				y_new = floor(y_new);
			}
			else {
				y_new = ceil(y_new);
			}

			if (x_new >= 0 && x_new < dst.cols && y_new >= 0 && y_new < dst.rows) {
				dst.at<uchar>(y_new, x_new) = src.at<uchar>(y, x);
			}
		}
	}

	return dst;
}


Mat rotate(Mat src, double angle) {
	Mat rotatedImage;

	Scalar bgColor = cv::Scalar(255, 255, 255);

	Point2f center(src.cols / 2.0, src.rows / 2.0);

	Mat rotationMatrix = getRotationMatrix2D(center, angle, 1.0);

	warpAffine(src, rotatedImage, rotationMatrix, src.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT, bgColor);

	return rotatedImage;
}


Mat dilation(Mat src, int mat[3][3]) {
	int height = src.rows;
	int width = src.cols;

	Mat dst = Mat(height, width, CV_8UC1);
	dst.setTo(255);

	for (int i = 1; i < height - 1; i++) {
		for (int j = 1; j < width - 1; j++) {
			if (src.at<uchar>(i, j) == 0) {
				if (mat[0][0] == 255) dst.at<uchar>(i - 1, j - 1) = 0;
				if (mat[0][1] == 255) dst.at<uchar>(i - 1, j) = 0;
				if (mat[0][2] == 255) dst.at<uchar>(i - 1, j + 1) = 0;
				if (mat[1][0] == 255) dst.at<uchar>(i, j - 1) = 0;
				if (mat[1][1] == 255) dst.at<uchar>(i, j) = 0;
				if (mat[1][2] == 255) dst.at<uchar>(i, j + 1) = 0;
				if (mat[2][0] == 255) dst.at<uchar>(i + 1, j - 1) = 0;
				if (mat[2][1] == 255) dst.at<uchar>(i + 1, j) = 0;
				if (mat[2][2] == 255) dst.at<uchar>(i + 1, j + 1) = 0;
			}
		}
	}
	return dst;
}


Point getUpperBlackPoint(Mat src) {
	int height = src.rows;
	int width = src.cols;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (src.at<uchar>(i, j) == 0)
				return Point(i, j);
		}
	}
	return NULL;
}


Point getLeftMostBlackPoint(Mat src) {
	int height = src.rows;
	int width = src.cols;
	for (int j = 0; j < width; j++) {
		for (int i = 0; i < height; i++) {
			if (src.at<uchar>(i, j) == 0)
				return Point(i, j);
		}
	}
	return NULL;
}


double getAngle(Mat src) {
	Point upperPixel = getUpperBlackPoint(src);
	Point leftMostPixel = getLeftMostBlackPoint(src);

	if (upperPixel == leftMostPixel)
		return 0;

	double hypotenuse = sqrt(pow((upperPixel.x - leftMostPixel.x), 2) + pow((upperPixel.y - leftMostPixel.y), 2));
	double oposite_cathetus = abs(leftMostPixel.y - upperPixel.y);
	double angleSin = oposite_cathetus / hypotenuse;
	double angle = asin(angleSin);

	angle = angle * 180.0f / PI;

	return angle;
}


Point getUpperBlackPointForRotation(Mat src) {
	int height = src.rows;
	int width = src.cols;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (src.at<uchar>(i, j) == 0)
				return Point(i, j);
		}
	}
	return NULL;
}


Point getLeftMostBlackPointForRotation(Mat src) {
	int height = src.rows;
	int width = src.cols;
	for (int j = 0; j < width; j++) {
		for (int i = 0; i < height; i++) {
			if (src.at<uchar>(i, j) == 0)
				return Point(i, j);
		}
	}
	return NULL;
}


Point getFirstUpLeftBlackPoint(Mat src) {
	int height = src.rows;
	int width = src.cols;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (src.at<uchar>(i, j) == 0)
				return Point(i, j);
		}
	}
	return NULL;
}


Point getFirstUpRightBlackPoint(Mat src) {
	int height = src.rows;
	int width = src.cols;
	for (int i = 0; i < height; i++) {
		for (int j = width - 1; j > 0; j--) {
			if (src.at<uchar>(i, j) == 0)
				return Point(i, j);
		}
	}
	return NULL;
}


Point getFirstDownLeftBlackPoint(Mat src) {
	int height = src.rows;
	int width = src.cols;
	for (int i = height - 1; i > 0; i--) {
		for (int j = 0; j < width - 1; j++) {
			if (src.at<uchar>(i, j) == 0)
				return Point(i, j);
		}
	}
	return NULL;
}


int getSquareSideLength(Mat src) {
	int height = src.rows;
	int width = src.cols;
	Point startPoint = getFirstUpLeftBlackPoint(src);
	int i = startPoint.x + 2;
	int j = startPoint.y;
	while (src.at<uchar>(i, j) == 0) {
		j++;
	}
	return j - startPoint.y + 2;
}


int getUnit(Mat src) {
	Point leftPoint = getFirstUpLeftBlackPoint(src);
	Point rightPoint = getFirstUpRightBlackPoint(src);
	int sideLength = getSquareSideLength(src);
	return sideLength / 7;
}


int aprox(float value) {
	if (value - floor(value) < 0.5f) {
		return floor(value);
	}
	return ceil(value);
}


int getMatrixSize(Mat src, Point upLeft, Point upRight, int unit) {
	int standardSize = 21;
	int matrixSize = aprox((upRight.y - upLeft.y + 2) / (float)unit);
	int minDiff = 21;
	int size = 21;

	while (abs(standardSize - matrixSize) < minDiff) {
		size = standardSize;
		minDiff = abs(standardSize - matrixSize);
		standardSize += 4;
	}

	return size;
}


float getUnitError(Mat src, Point upLeft, Point upRight, int unit, int matrixSize) {
	float error = (float)((unit * matrixSize) - (upRight.y - upLeft.y + 2)) / (float)matrixSize;
	return error;
}


uchar getDominantColor(Mat src, Point upLeft, int x, int y, int unit, float unitError, int matrixSize) {
	int frecvWhite = 0;
	int frecvBlack = 0;

	for (int i = 0; i < unit - unitError; i++) {
		for (int j = 0; j < unit - unitError; j++) {
			int posX = upLeft.x + (int)(x * (float)(unit - unitError)) + i;
			int posY = upLeft.y + (int)(y * (float)(unit - unitError)) + j;

			if (src.at<uchar>(posX, posY) == 0) {
				frecvBlack++;
			}
			else
				frecvWhite++;
		}
	}

	if (frecvWhite > frecvBlack)
		return 255;
	return 0;
}


Mat getQRMatrix(Mat src) {
	Point upLeft = getFirstUpLeftBlackPoint(src);
	Point upRight = getFirstUpRightBlackPoint(src);
	Point downLeft = getFirstDownLeftBlackPoint(src);

	cout << "upLeft: " << upLeft << endl;
	cout << "upRight: " << upRight << endl;
	cout << "downLeft: " << downLeft << endl;

	int unit = getUnit(src);
	int matrixSize = getMatrixSize(src, upLeft, upRight, unit);
	float unitError = getUnitError(src, upLeft, upRight, unit, matrixSize);


	cout << "LENGTH " << upRight.y - upLeft.y << endl;
	cout << "Unit SIZE " << unit << endl;
	cout << "Matrix SIZE " << matrixSize << endl;
	cout << "Unit ERROR = " << unitError << endl;

	Mat matrix = Mat(matrixSize, matrixSize, CV_8UC1);

	for (int i = 0; i < matrixSize; i++) {
		for (int j = 0; j < matrixSize; j++) {
			uchar color = getDominantColor(src, upLeft, i, j, unit, unitError, matrixSize);
			matrix.at<uchar>(i, j) = color;
		}
	}

	return matrix;
}


int getMaskPattern(Mat src) {
	int m1 = src.at<uchar>(8, 2);
	int m2 = src.at<uchar>(8, 3);
	int m3 = src.at<uchar>(8, 4);

	int maskPattern = 4 * (m1 % 254) + 2 * (m2 % 254) + (m3 % 254);
	
	cout << "MASK PATTERN = " << maskPattern << endl;

	return maskPattern;
}


Mat getMask(Mat src, int maskPattern) {
	int height = src.rows;
	int width = src.cols;
	Mat dst = Mat(height, width, CV_8UC1);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (maskPattern == 0) {
				if (j % 3 == 0)
					dst.at<uchar>(i, j) = 0;
				else
					dst.at<uchar>(i, j) = 255;
			}

			else if (maskPattern == 1) {
				if ((i + j) % 3 == 0)
					dst.at<uchar>(i, j) = 0;
				else
					dst.at<uchar>(i, j) = 255;
			}

			else if (maskPattern == 2) {
				if ((i + j) % 2 == 0)
					dst.at<uchar>(i, j) = 0;
				else
					dst.at<uchar>(i, j) = 255;
			}

			else if (maskPattern == 3) {
				if (i % 2 == 0)
					dst.at<uchar>(i, j) = 0;
				else
					dst.at<uchar>(i, j) = 255;
			}

			else if (maskPattern == 4) {
				if (((i * j) % 3 + i * j) % 2 == 0)
					dst.at<uchar>(i, j) = 0;
				else
					dst.at<uchar>(i, j) = 255;
			}

			else if (maskPattern == 5) {
				if (((i * j) % 3 + i + j) % 2 == 0)
					dst.at<uchar>(i, j) = 0;
				else
					dst.at<uchar>(i, j) = 255;
			}

			else if (maskPattern == 6) {
				if ((i / 2 + j / 3) % 2 == 0)
					dst.at<uchar>(i, j) = 0;
				else
					dst.at<uchar>(i, j) = 255;
			}

			else {
				if ((i * j) % 2 + (i * j) % 3 == 0)
					dst.at<uchar>(i, j) = 0;
				else
					dst.at<uchar>(i, j) = 255;
			}
		}
	}

	return dst;
}


Mat applyMask(Mat src, Mat mask) {
	int height = src.rows;
	int width = src.cols;
	Mat dst = Mat(height, width, CV_8UC1);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (mask.at<uchar>(i, j) == 0) {
				if (src.at<uchar>(i, j) == 0)
					dst.at<uchar>(i, j) = 255;
				else
					dst.at<uchar>(i, j) = 0;
			}
			else {
				dst.at<uchar>(i, j) = src.at<uchar>(i, j);
			}
		}
	}

	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			dst.at<uchar>(i, j) = src.at<uchar>(i, j);
		}
	}

	for (int i = 0; i < 9; i++) {
		for (int j = width - 1; j > width - 9; j--) {
			dst.at<uchar>(i, j) = src.at<uchar>(i, j);
		}
	}

	for (int i = height - 1; i > height - 9; i--) {
		for (int j = 0; j < 9; j++) {
			dst.at<uchar>(i, j) = src.at<uchar>(i, j);
		}
	}

	for (int i = 7; i < height - 7; i++) {
		dst.at<uchar>(i, 6) = src.at<uchar>(i, 6);
	}

	for (int i = 7; i < width - 7; i++) {
		dst.at<uchar>(6, i) = src.at<uchar>(6, i);
	}

	if (height >= 25 && width >= 25) {
		for (int i = height - 9; i < height - 4; i++) {
			for (int j = width - 9; j < width - 4; j++) {
				dst.at<uchar>(i, j) = src.at<uchar>(i, j);
			}
		}
	}

	return dst;
}


int getEncodingType(Mat src) {
	int height = src.rows;
	int width = src.cols;

	int e1 = src.at<uchar>(height - 1, width - 1);
	int e2 = src.at<uchar>(height - 1, width - 2);
	int e3 = src.at<uchar>(height - 2, width - 1);
	int e4 = src.at<uchar>(height - 2, width - 2);
	int encoding = 8 * ((e1 + 1) % 256) + 4 * ((e2 + 1) % 256) + 2 * ((e3 + 1) % 256) + ((e4 + 1) % 256);

	cout << "ENCODING = " << encoding << endl;

	return encoding;
}


int getTextLength(Mat src) {
	int height = src.rows;
	int width = src.cols;

	int l1 = src.at<uchar>(height - 3, width - 1);
	int l2 = src.at<uchar>(height - 3, width - 2);
	int l3 = src.at<uchar>(height - 4, width - 1);
	int l4 = src.at<uchar>(height - 4, width - 2);
	int l5 = src.at<uchar>(height - 5, width - 1);
	int l6 = src.at<uchar>(height - 5, width - 2);
	int l7 = src.at<uchar>(height - 6, width - 1);
	int l8 = src.at<uchar>(height - 6, width - 2);
	int length = 128 * ((l1 + 1) % 256) + 64 * ((l2 + 1) % 256) + 32 * ((l3 + 1) % 256) + 16 * ((l4 + 1) % 256) +
		8 * ((l5 + 1) % 256) + 4 * ((l6 + 1) % 256) + 2 * ((l7 + 1) % 256) + ((l8 + 1) % 256);

	cout << "TEXT LENGTH = " << length << endl;

	return length;
}


int getDecimal(int binaryNumber[8]) {
	int two = 1;
	int decimalNumber = 0;
	for (int i = 7; i >= 0; i--) {
		decimalNumber += two * binaryNumber[i];
		two *= 2;
	}
	return decimalNumber;
}


int getValueFromPixel(Mat src, int x, int y) {
	if (src.at<uchar>(x, y) == 0)
		return 1;
	else
		return 0;
}


int getValueFromUpHorizontalRectangle(Mat src, int* x, int* y) {
	int binaryNumber[8];
	int index = 0;
	binaryNumber[index++] = getValueFromPixel(src, *x, *y);
	binaryNumber[index++] = getValueFromPixel(src, *x, *y - 1);
	binaryNumber[index++] = getValueFromPixel(src, *x - 1, *y);
	binaryNumber[index++] = getValueFromPixel(src, *x - 1, *y - 1);
	binaryNumber[index++] = getValueFromPixel(src, *x - 1, *y - 2);
	binaryNumber[index++] = getValueFromPixel(src, *x - 1, *y - 3);
	binaryNumber[index++] = getValueFromPixel(src, *x, *y - 2);
	binaryNumber[index] = getValueFromPixel(src, *x, *y - 3);

	//cout << "Up Horizontal Rectangle: " << getDecimal(binaryNumber) << endl;
	
	*x = *x + 1;
	*y = *y - 2;
	
	return getDecimal(binaryNumber);
}


int getValueFromBottomHorizontalRectangle(Mat src, int* x, int* y) {
	int index = 0;
	int binaryNumber[8];
	binaryNumber[index++] = getValueFromPixel(src, *x, *y);
	binaryNumber[index++] = getValueFromPixel(src, *x, *y - 1);
	binaryNumber[index++] = getValueFromPixel(src, *x + 1, *y);
	binaryNumber[index++] = getValueFromPixel(src, *x + 1, *y - 1);
	binaryNumber[index++] = getValueFromPixel(src, *x + 1, *y - 2);
	binaryNumber[index++] = getValueFromPixel(src, *x + 1, *y - 3);
	binaryNumber[index++] = getValueFromPixel(src, *x, *y - 2);
	binaryNumber[index] = getValueFromPixel(src, *x, *y - 3);
	
	//cout << "Bottom Horizontal Rectangle: " << getDecimal(binaryNumber) << endl;
	
	*x = *x - 1;
	*y = *y - 2;

	return getDecimal(binaryNumber);
}


int getValueFromVerticalUpBottomRectangle(Mat src, int* x, int y) {
	if (*x == 6)
		*x = *x + 1;

	int index = 0;
	int binaryNumber[8];
	for (int i = 0; i < 4; i++) {
		binaryNumber[index++] = getValueFromPixel(src, *x, y);
		binaryNumber[index++] = getValueFromPixel(src, *x, y - 1);
		*x = *x + 1;
	}

	//cout << "Up Vertical Rectangle: " << getDecimal(binaryNumber) << endl;

	return getDecimal(binaryNumber);
}

int getValueFromVerticalBottomUpRectangle(Mat src, int* x, int y) {
	if (*x == 6)
		*x = *x - 1;

	int index = 0;
	int binaryNumber[8];
	for (int i = 0; i < 4; i++) {
		binaryNumber[index++] = getValueFromPixel(src, *x, y);
		binaryNumber[index++] = getValueFromPixel(src, *x, y - 1);
		*x = *x - 1;
	}

	//cout << "Bottom Vertical Rectangle: " << getDecimal(binaryNumber) << endl;
	
	return getDecimal(binaryNumber);
}


vector<int> decodeQrCodeInAsci(Mat src) {
	int height = src.rows;
	int width = src.cols;

	int length = getTextLength(src);
	int encode = getEncodingType(src);

	if (encode != 4) {
		cout << "ENCODING TYPE NOT ACCEPTED" << endl;
		return std::vector<int>();
	}

	if (height >= 25) {
		int maxLength = 7 + (height - 25) / 2;
		if (length > maxLength) length = maxLength;
		cout << "FOR " << height << "x" << height << " QR CODES VALID MAXIMUM TEXT SIZE IS " << length << endl;
	}

	int count = 0;
	int minHeight = 14;
	int y = width - 1;
	int x = height - 7;
	vector<int> numbers;

	while (true) {
		//BOTTOM---UP
		while (x >= minHeight) {
			numbers.push_back(getValueFromVerticalBottomUpRectangle(src, &x, y));
			count++;
			if (count == length) {
				return numbers;
			}
		}

		//UP
		numbers.push_back(getValueFromUpHorizontalRectangle(src, &x, &y));
		count++;
		if (count == length) {
			return numbers;
		}

		//UP---BOTTOM
		while (x <= height - 6) {
			numbers.push_back(getValueFromVerticalUpBottomRectangle(src, &x, y));
			count++;
			if (count == length) {
				return numbers;
			}
		}

		//BOTTOM
		numbers.push_back(getValueFromBottomHorizontalRectangle(src, &x, &y));
		count++;
		if (count == length) {
			return numbers;
		}

		if (y < width - 8)
			minHeight = 3;
	}

	return numbers;
}


bool checkRotation(Mat mat) {
	int height = mat.rows;
	int width = mat.cols;

	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 7; j++) {
			if (mat.at<uchar>(i, j) != mat.at<uchar>(i, width - 7 + j)) {
				return false;
			}
		}
	}
	return true;
}


Mat matrixRotationCorrection(Mat src, Mat dil, double angle, int matrix[3][3]) {
	Mat dst = getQRMatrix(dil);
	bool isInCorrectPosition = checkRotation(dst);

	if (isInCorrectPosition == false) {
		Mat binAux = binarise(src);

		Mat rotAux = rotate(binAux, 270 + angle);

		Mat dilAux = dilation(rotAux, matrix);

		dst = getQRMatrix(rotAux);

		imshow("dilation", dil);
		imshow("dilationAux", rotAux);
	}

	return dst;
}


void testBinarisation() {
	char fname[MAX_PATH];

	if (openFileDlg(fname)) {
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		Mat srcColor = imread(fname);
		Mat dst = binarise(src);
		imshow("input image", srcColor);
		imshow("output image", dst);
		waitKey();
	}
}


void testRotation() {
	char fname[MAX_PATH];

	if (openFileDlg(fname)) {
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		double angle = getAngle(src);

		cout << "Angle = " << angle << endl;

		Mat dst = rotate(src, angle);

		imshow("input image", src);
		imshow("output image", dst);
		waitKey();
	}
}


void testGetQRMatrix() {
	char fname[MAX_PATH];

	if (openFileDlg(fname)) {
		Mat src = imread(fname, IMREAD_GRAYSCALE);

		int matrix[3][3] = { {255,255,255}, {255,255,255}, {255,255,255} };
		double angle = getAngle(src);
		Mat rot = rotate(src, angle);
		Mat bin = binarise(rot);
		Mat dil = dilation(rot, matrix);
		Mat mat = getQRMatrix(dil);

		cout << matrix << endl;

		imwrite("matrix.png", mat);
		imwrite("dilation.png", dil);

		imshow("input image", src);
		imshow("output image", mat);
		waitKey();
	}
}


void testGetMask() {
	char fname[MAX_PATH];

	if (openFileDlg(fname)) {
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		Mat mat = getQRMatrix(src);
		int maskPattern = getMaskPattern(mat);
		Mat mask = getMask(mat, maskPattern);
		Mat maskApplied = applyMask(mat, mask);
		int encoding = getEncodingType(maskApplied);
		int length = getTextLength(maskApplied);

		imwrite("matrix.png", mat);
		imwrite("mask.png", mask);
		imwrite("applied.png", maskApplied);

		imshow("original", src);
		imshow("mask", mask);
		imshow("mask applied", maskApplied);

		waitKey();
	}
}


void testDecode() {
	char fname[MAX_PATH];
	if (openFileDlg(fname)) {
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		Mat mat = getQRMatrix(src);
		int maskPattern = getMaskPattern(mat);
		Mat mask = getMask(mat, maskPattern);
		Mat maskApplied = applyMask(mat, mask);
		int encoding = getEncodingType(maskApplied);
		int length = getTextLength(maskApplied);
		vector<int> message = decodeQrCodeInAsci(maskApplied);

		cout << endl << "---------------------------------------------" << endl;

		for (int letter : message) {
			cout << (char)letter;
		}

		cout << endl << "---------------------------------------------" << endl;

		imwrite("matrix.png", mat);
		imwrite("mask.png", mask);
		imwrite("applied.png", maskApplied);

		imshow("original", src);
		imshow("mask", mask);
		imshow("mask applied", maskApplied);

		waitKey();
	}
}


void qrCodeScanner() {
	char fname[MAX_PATH];

	if (openFileDlg(fname)) {
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		Mat srcColor = imread(fname);
		int matrix[3][3] = { {255,255,255}, {255,255,255}, {255,255,255} };

		Mat bin = binarise(src);

		double angle = getAngle(bin);

		Mat rot = rotate(bin, angle);

		Mat dil = dilation(rot, matrix);

		Mat mat = matrixRotationCorrection(src, dil, angle, matrix);

		int maskPattern = getMaskPattern(mat);

		Mat mask = getMask(mat, maskPattern);

		Mat maskApplied = applyMask(mat, mask);

		int encoding = getEncodingType(maskApplied);

		int length = getTextLength(maskApplied);

		vector<int> message = decodeQrCodeInAsci(maskApplied);

		cout << endl << "---------------------------------------------" << endl;

		for (int letter : message) {
			cout << (char)letter;
		}

		cout << endl << "---------------------------------------------" << endl;

		imwrite("matrix.png", mat);
		imwrite("mask.png", mask);
		imwrite("applied.png", maskApplied);

		imshow("original", srcColor);
		//imshow("rotate", rot);
		//imshow("binarise", bin);
		//imshow("dilation", dil);
		//imshow("QR matrix", mat);
		//imshow("mask", mask);
		//imshow("applied", maskApplied);

		waitKey();
	}
}


int main()
{
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_FATAL);
	projectPath = _wgetcwd(0, 0);

	int op;
	do
	{
		system("cls");
		destroyAllWindows();
		printf("Menu:\n");
		printf(" 1 - QR Code Scanner\n");
		printf(" 2 - Test binarisation\n");
		printf(" 3 - Test rotation\n");
		printf(" 4 - Test get QR matrix\n");
		printf(" 5 - Test mask\n");
		printf(" 6 - Test decode\n");
		printf(" 0 - Exit\n\n");
		printf("Option: ");

		scanf("%d", &op);
		switch (op)
		{
		case 1:
			qrCodeScanner();
			break;

		case 2:
			testBinarisation();
			break;

		case 3:
			testRotation();
			break;

		case 4:
			testGetQRMatrix();
			break;

		case 5:
			testGetMask();
			break;

		case 6:
			testDecode();
			break;
		}
	} while (op != 0);
	return 0;
}