/*


   Copyright (c) 2020, Tharaka Ratnayake, email: tharakasachintha.17@cse.mrt.ac.lk
   All rights reserved. https://github.com/tharaka27/ImageStitcherFAST

   Some algorithms used in this code referred to:
   1. OpenCV: http://opencv.org/



   Revision history:
	  April 26th, 2020: initial version.
*/


#include "ORBMatcher.h"



ORBMatcher::ORBMatcher()
{
}


/*
 @Compute the number of Inliers using symmetric transfer error.

 @Param num Number of points in the array.
 @Param H Pointer to the homography matrix.
 @Param p1 std::vector to the array of cv::Point2f obj.
 @Param p2 std::vector to the array of cv::Point2f scene.
 @Param inlier_mask Pointer to the inlier mask matrix.
 @Param dist_std Pointer to the std of the distance among all inliers.

 Compute number of inliers by computing distance under a perticular H
		distance = d(Hx, x') + d(invH x', x)

 return: number of inliers
 */
int ORBMatcher::ComputeOrbDistance(cv::Mat vector1, cv::Mat vector2)
{
	int numBit = 0;

	for (int i = 0; i < 32; ++i)
	{
		unsigned char c = vector1.at<uchar>(i) ^ vector2.at<uchar>(i);
		c = c - ((c >> 1) & 0125);               // 01010101
		c = (unsigned char)(c & 063) + (unsigned char)((c >> 2) & 063);   // 00110011
		c = (unsigned char)(c * 021) >> 4;                                  // 11110000
		numBit += c;
	}

	return numBit;
}


/*
 @Compute the number of Inliers using symmetric transfer error.

 @Param num Number of points in the array.
 @Param H Pointer to the homography matrix.
 @Param p1 std::vector to the array of cv::Point2f obj.
 @Param p2 std::vector to the array of cv::Point2f scene.
 @Param inlier_mask Pointer to the inlier mask matrix.
 @Param dist_std Pointer to the std of the distance among all inliers.

 Compute number of inliers by computing distance under a perticular H
		distance = d(Hx, x') + d(invH x', x)

 return: number of inliers
 */
std::vector<int> ORBMatcher::MatchDescriptors(cv::Mat descriptors1, cv::Mat descriptors2, std::vector<cv::DMatch> *matches)
{
	int numDes1 = descriptors1.rows;
	int numDes2 = descriptors2.rows;
	std::vector<int> result(numDes1, -1);

	std::vector<int> OrbDistances;
	for (int i = 0; i < numDes1; ++i)
	{
		cv::Mat descriptor1 = descriptors1.row(i);
		OrbDistances.clear();
		OrbDistances.resize(numDes2);
		for (int j = 0; j < numDes2; ++j)
		{
			OrbDistances[j] = ComputeOrbDistance(descriptor1, descriptors2.row(j));
		}

		std::vector<int>::iterator iter = std::min_element(OrbDistances.begin(), OrbDistances.end());

		if (*iter < TH_HAMMING_DIST)
		{
			int minId = std::distance(OrbDistances.begin(), iter);
			result[i] = minId;

			cv::DMatch match;
			match.distance = OrbDistances[minId];
			match.queryIdx = i;
			match.trainIdx = minId;

			matches->push_back(match);
		}
	}

	return result;
}
