// OpencvTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <Windows.h>
#include <iomanip>
#include <math.h>

using namespace std;
using namespace cv;
	
int RGB2Int(Vec3b color);
Vec3b Int2RGB(int color);
uint* HilbertAxes(uint integer);
uint HilbertIndexTransposed(uint a, uint b, uint c);
int main()
{
	//HilbertAxes(14554);
	
	cout << HilbertAxes(14554)[0];
	cout << "\n";
	cout << HilbertAxes(14554)[1];
	cout << "\n";
	cout << HilbertAxes(14554)[2];
	cout << "\n";
	cout << HilbertIndexTransposed(HilbertAxes(14554)[0], HilbertAxes(14554)[1], HilbertAxes(14554)[2]);
	
	waitKey(0);
	Mat img = imread("rose.jpg");
	Mat largeMat = Mat(4096,4096,CV_8UC3);
	Mat condencedLargeMat = Mat(4096, 4096, CV_32FC1);
	resize(img, largeMat, largeMat.size(), 0, 0, CV_INTER_AREA);
	cvtColor(largeMat, largeMat, COLOR_BGR2RGB);

	for (int i = 0; i < 4095; i++) {
		for (int j = 0; j < 4095; j++) {
			condencedLargeMat.at<float>(Point(i,j)) = RGB2Int(largeMat.at<Vec3b>(Point(i, j)));
		}
	}
	
	
	Mat a2 = Mat(4096,4096,CV_32F);
	float _kernel2[3][3] =	{{0,0.2,0},
							{0.2,0.2,0.2},
							{0,0.2,0}};

	Mat kernel2 = Mat(Size(3, 3), CV_32F, _kernel2);
	cout << setprecision(3) << "M = " << endl << " " << kernel2 << endl << endl;
	filter2D(condencedLargeMat, a2, -1, kernel2);
	
	Mat a3;
	float _kernel3[3][3] = {{0.1111,0.1111,0.1111 },
							{ 0.1111,0.1111,0.1111 },
							{ 0.1111,0.1111,0.1111 }};
	Mat kernel3 = Mat(Size(3, 3), CV_32F, _kernel3);
	filter2D(condencedLargeMat, a3, -1, kernel3, Point(-1, -1));

	Mat a4;
	float _kernel4[5][5] = { { 0,0,1,0,0},
							 { 0,1,1,1,0},
							 { 1,1,1,1,1}, 
							 { 0,1,1,1,0}, 
							 { 0,0,1,0,0}};
	Mat kernel4 = Mat(Size(5, 5), CV_32F, _kernel4);
	multiply(kernel4, Scalar(0.0769), kernel4);
	filter2D(condencedLargeMat, a4, -1, kernel4, Point(-1, -1));

	Mat a5;
	float _kernel5[5][5] = { { 0,1,1,1,0 },
							 { 1,1,1,1,1 },
							 { 1,1,1,1,1 },
						   	 { 1,1,1,1,1 },
							 { 0,1,1,1,0 } };
	Mat kernel5 = Mat(Size(5, 5), CV_32F, _kernel5);
	multiply(kernel5, Scalar(0.0476), kernel5);
	filter2D(condencedLargeMat, a5, -1, kernel5, Point(-1, -1));
	Mat a6;
	float _kernel6[5][5] = { { 1,1,1,1,1 },
							{ 1,1,1,1,1 },
							{ 1,1,1,1,1 },
							{ 1,1,1,1,1 },
							{ 1,1,1,1,1 } };
	Mat kernel6 = Mat(Size(5, 5), CV_32F, _kernel6);
	multiply(kernel6, Scalar(0.04), kernel6);
	filter2D(condencedLargeMat, a6, -1, kernel6, Point(-1, -1));
	struct lexicographicSortWrapper
	{
		float Ktuples[6];
		Point p;
	
		lexicographicSortWrapper(float _a1, float _a2, float _a3, float _a4, float _a5, float _a6,Point pixel) : 
			Ktuples{ _a1,_a2,_a3,_a4,_a5,_a6 }, p(pixel)  {}
		bool operator < (const lexicographicSortWrapper& lexi) const
		{
			for (int i = 0; i < 6; i++) {
				if (Ktuples[i] < lexi.Ktuples[i]) return true;
				else if (Ktuples[i] > lexi.Ktuples[i]) return false;
			}
			return false;
		}

		bool operator > (const lexicographicSortWrapper& lexi) const
		{
			for (int i = 0; i < 6; i++) {
				if (Ktuples[i] < lexi.Ktuples[i]) return false;
				else if (Ktuples[i] > lexi.Ktuples[i]) return true;
			}
			return true;
		}
	};
	vector<lexicographicSortWrapper> vec;

	for (int i = 0; i < 4095; i++) {
		for (int j = 0; j < 4095; j++) {
			vec.push_back(lexicographicSortWrapper(
				condencedLargeMat.at<float>(Point(i, j)),
				a2.at<float>(Point(i, j)),
				a3.at<float>(Point(i, j)),
				a4.at<float>(Point(i, j)),
				a5.at<float>(Point(i, j)),
				a6.at<float>(Point(i, j)),
				Point(i, j)
			));
		}
	}
	
	sort(vec.begin(), vec.end());

	Mat equalized = Mat(4096, 4096, CV_32F);

	int k = 0;
	for (auto &i :vec) {
		equalized.at<float>(i.p) = k++;
	}
	Mat disp = Mat(4096,4096,CV_8SC3);
	for (int i = 0; i < 4095; i++) {
		for (int j = 0; j < 4095; j++) {
			Vec3b RGB = Int2RGB(equalized.at<float>(Point(i, j)));
			disp.at<Vec3b>(Point(i, j)) = RGB;
		}
	}
	
	namedWindow("image", WINDOW_NORMAL);
	imshow("image", disp);
	imwrite("C:/Users/NeilHazra/Desktop/allrgb.png", disp);
	waitKey(0);
	return(0);
}

int RGB2Int(Vec3b color)	{
	uchar r = color[0];
	uchar g = color[1];
	
	uchar b = color[2];
	return HilbertIndexTransposed(uint(r),uint(g),uint(b));
}
Vec3b Int2RGB(int color) {
	Vec3b rgb;
	uint* X = HilbertAxes(uint(color));
	rgb[0] = X[0];
	rgb[1] = X[1];
	rgb[2] = X[2];

	return rgb;
}

	/// <summary>
	/// Convert between Hilbert index and N-dimensional points.
	/// 
	/// The Hilbert index is expressed as an array of transposed bits. 
	/// 
	/// Example: 5 bits for each of n=3 coordinates.
	/// 15-bit Hilbert integer = A B C D E F G H I J K L M N O is stored
	/// as its Transpose                        ^
	/// X[0] = A D G J M                    X[2]|  7
	/// X[1] = B E H K N        <------->       | /X[1]
	/// X[2] = C F I L O                   axes |/
	///        high low                         0------> X[0]
	///        
	/// NOTE: This algorithm is derived from work done by John Skilling and published in "Programming the Hilbert curve".
	/// (c) 2004 American Institute of Physics.
	/// 
	/// </summary>
	
		/// <summary>
		/// Convert the Hilbert index into an N-dimensional point expressed as a vector of uints.
		///
		/// Note: In Skilling's paper, this function is named TransposetoAxes.
		/// </summary>
		/// <param name="transposedIndex">The Hilbert index stored in transposed form.</param>
		/// <param name="bits">Number of bits per coordinate.</param>
		/// <returns>Coordinate vector.</returns>
uint* HilbertAxes(uint integer)
{
	uint X[3] = {0,0,0};
	uint arr[8] = {0,0,0,0,0,0,0,0};
	int j = 7;
	while (integer > 0) {
		// storing remainder in binary array
		arr[j] = integer % 8;
		integer = integer / 8;
		j--;
	}
	/*
	for (int l = 0; l < 8; l++) {
		cout << arr[l];
		cout << " ";
	}
	cout << "\n";
	*/
	for (int k = 0; k < 8; k++) {
		int w = arr[k] % 2;
		arr[k] /= 2;
		int x = arr[k] % 2;
		arr[k] /= 2;
		int y = arr[k] % 2;
		X[0] += y << (7 - k);
		X[1] += x << (7 - k);
		X[2] += w << (7 - k);
	}
	/*
	cout << X[0];
	cout << "\n";
	cout << X[1];
	cout << "\n";
	cout << X[2];
	cout << "\n";
	*/
	int n = 3; // n: Number of dimensions
	uint N = 2U << (8 - 1), P, Q, t;
	int i;
	// Gray decode by H ^ (H/2)
	t = X[n - 1] >> 1;
	// Corrected error in Skilling's paper on the following line. The appendix had i >= 0 leading to negative array index.
	for (i = n - 1; i > 0; i--)
		X[i] ^= X[i - 1];
	X[0] ^= t;
	// Undo excess work
	for (Q = 2; Q != N; Q <<= 1)
	{
		P = Q - 1;
		for (i = n - 1; i >= 0; i--)
			if ((X[i] & Q) != 0U)
				X[0] ^= P; // invert
			else
			{
				t = (X[0] ^ X[i]) & P;
				X[0] ^= t;
				X[i] ^= t;
			}
	} // exchange
	return X;
}






		/// <summary>
		/// Given the axes (coordinates) of a point in N-Dimensional space, find the distance to that point along the Hilbert curve.
		/// That distance will be transposed; broken into pieces and distributed into an array.
		/// 
		/// The number of dimensions is the length of the hilbertAxes array.
		///
		/// Note: In Skilling's paper, this function is called AxestoTranspose.
		/// </summary>
		/// <param name="hilbertAxes">Point in N-space.</param>
		/// <param name="bits">Depth of the Hilbert curve. If bits is one, this is the top-level Hilbert curve.</param>
		/// <returns>The Hilbert distance (or index) as a transposed Hilbert index.</returns>
		uint HilbertIndexTransposed(uint a, uint b, uint c)
		{
			uint X[3] = { a,b,c };
			int n = 3; // n: Number of dimensions
			uint M = 1U << (8 - 1), P, Q, t;
			int i;
			// Inverse undo
			for (Q = M; Q > 1; Q >>= 1)
			{
				P = Q - 1;
				for (i = 0; i < n; i++)
					if ((X[i] & Q) != 0)
						X[0] ^= P; // invert
					else
					{
						t = (X[0] ^ X[i]) & P;
						X[0] ^= t;
						X[i] ^= t;
					}
			} // exchange
			  // Gray encode
			for (i = 1; i < n; i++)
				X[i] ^= X[i - 1];
			t = 0;
			for (Q = M; Q > 1; Q >>= 1)
				if ((X[n - 1] & Q) != 0)
					t ^= Q - 1;
			for (i = 0; i < n; i++)
				X[i] ^= t;


			uint index = 0;
			uint arr0[8] = { 0,0,0,0,0,0,0 };
			uint arr1[8] = { 0,0,0,0,0,0,0 };
			uint arr2[8] = { 0,0,0,0,0,0,0 };
			uint integer = X[0];
		
			int j = 7;
			while (integer > 0) {
				// storing remainder in binary array
				arr0[j] = integer % 2;
				integer = integer / 2;
				j--;
			}
			integer = X[1];
			j = 7;
			while (integer > 0) {
				// storing remainder in binary array
				arr1[j] = integer % 2;
				integer = integer / 2;
				j--;
			}
			integer = X[2];
			j = 7;
			while (integer > 0) {
				// storing remainder in binary array
				arr2[j] = integer % 2;
				integer = integer / 2;
				j--;
			}
			
			for (int k = 0; k < 8; k++) {
				int w = arr0[k];
				int x = arr1[k];
				int y = arr2[k];

				int z = (w << 2) + (x << 1) + y;

				index += z << 3*(7 - k);
			}
			return index;
		}



