#include "cluster.h"

#include <vector>
#include <queue>
#include <limits>

namespace Airheads::Cluster {

	ClusterMetrics ClusterMetrics::FromWeightedPixels(std::vector<ClusterPixel>& accepted) {
		if (accepted.size() == 0) {
			return ClusterMetrics();
		}

		ClusterMetrics metrics;
		metrics.sizePx = (int)accepted.size();

		int xSum = 0;
		int xWeightedSum = 0;

		int ySum = 0;
		int yWeightedSum = 0;

		int valueSum = 0;
		int valueSumOfSqrs = 0;
		int valueMin = std::numeric_limits<int>::max();

		for (const auto& pixel : accepted) {
			xSum += pixel.loc.x;
			xWeightedSum += pixel.loc.x * pixel.value;

			ySum += pixel.loc.y;
			yWeightedSum += pixel.loc.y * pixel.value;

			valueSum += pixel.value;
			valueSumOfSqrs += valueSum * valueSum;
			if (valueMin > pixel.value)
				valueMin = pixel.value;
		}

		const float sizeFloat = (float)metrics.sizePx;
		const float valueSumDiff = (float)(valueSum - metrics.sizePx * valueMin);
		const float stdevFactorDenominator = valueSumDiff * valueSumDiff;
		if (stdevFactorDenominator > 0) {
			metrics.center = {
				(int)round((xWeightedSum - valueMin * xSum) / valueSumDiff),
				(int)round((yWeightedSum - valueMin * ySum) / valueSumDiff)
			};
		} else {
			// #This happens when all weights are identical.
			metrics.center = {
				(int)round(xSum / sizeFloat),
				(int)round(ySum / sizeFloat)
			};
		}

		return metrics;
	}

	ClusterMetrics ClusterMetrics::FromPixels(std::vector<ClusterPixel>& accepted) {
		if (accepted.size() == 0) {
			return ClusterMetrics();
		}

		ClusterMetrics metrics;
		metrics.sizePx = (int)accepted.size();

		int xSum = 0;
		int ySum = 0;

		for (const auto& pixel : accepted) {
			xSum += pixel.loc.x;
			ySum += pixel.loc.y;
		}

		float sizeFloat = (float)metrics.sizePx;
		metrics.center = { 
			(int)round(xSum / sizeFloat), 
			(int)round(ySum / sizeFloat) 
		};

		return metrics;
	}

	bool FindSeed(cv::Mat& img, cv::Point seed_guess, int minval, int max_radius, cv::Point& out_point) {

		if (img.at<uchar>(seed_guess) >= minval) {
			out_point = seed_guess;
			return true;
		}

		const int step = 5;
		//for R in range(step, max_radius + 1, step) :
		for (int R = step; R < max_radius + 1; R += step) {
			//#limit values of the squares for this Radius R
			//y0 = max(0, seed_guess[0] - R)
			//x0 = max(0, seed_guess[1] - R)
			//y1 = min(imgH, seed_guess[0] + R + 1)
			//x1 = min(imgW, seed_guess[1] + R + 1)
			int y0 = std::max(0, seed_guess.y - R);
			int x0 = std::max(0, seed_guess.x - R);
			//int y1 = std::min(imgH, seed_guess.y + R + 1);
			//int x1 = std::min(imgW, seed_guess.x + R + 1);
			int y1 = std::min(img.rows, seed_guess.y + R + 1);
			int x1 = std::min(img.cols, seed_guess.x + R + 1);

			//#Look North
			//y = y0
			//for x in range(x0, x1, step) :
			int y = y0;
			for (int x = x0; x < x1; x += step) {
				//#if is_cluster(img[y][x], minval):
				//if img[y][x] >= minval:
				//    return True, (y,x)
				if (img.at<uchar>(y, x) >= minval) {
					out_point = { x, y };
					return true;
				}
			}

			//#Look South    
			//y = y1-1
			//for x in range(x0, x1, step):
			cv::Point pt;
			pt.y = y1 - 1;
			for (pt.x = x0; pt.x < x1; pt.x += step) {
				//#if is_cluster(img[y][x], minval):
				//if img[y][x] >= minval:
				//    return True, (y,x)
				if (img.at<uchar>(pt) >= minval) {
					out_point = pt;
					return true;
				}
			}

			//#Look East
			//x = x0
			//for y in range(y0, y1, step) :
			pt.x = x0;
			for (pt.y = y0; pt.y < y1; pt.y += step) {
				//#if is_cluster(img[y][x], minval):
				//if img[y][x] >= minval:
				//	return True, (y,x)
				if (img.at<uchar>(pt) >= minval) {
					out_point = pt;
					return true;
				}
			}

			//#Look West
			//x = x1-1
			//for y in range(y0, y1, step):
			pt.x = x1 - 1;
			for (pt.y = y0; pt.y < y1; pt.y += step) {
				//#if is_cluster(img[y][x], minval):
				//if img[y][x] >= minval:
				//	return True, (y,x)
				if (img.at<uchar>(pt) >= minval) {
					out_point = pt;
					return true;
				}
			}
		}

		out_point = { -1, -1 };
		return false;
	}

	ClusterMetrics ClusterFill(cv::Mat& img, cv::Point seed, uchar minval, int clusterColor, int maxSizePx) {
		if (img.at<uchar>(seed) < minval) {
			return ClusterMetrics();
		}

		int imgW = img.cols;
		int imgH = img.rows;
		//#accepted pixels must be light colored, with value >= minval
		//N = 0
		int N = 0;
		//int y = seed.y;
		//int x = seed.x;
		cv::Point pt = seed;
		//std::vector<int> Y;
		//std::vector<int> X;
		//std::vector<uchar> W;
		std::vector<ClusterPixel> accepted;

		//cluster_color = max(0, min(cluster_color, minval - 1))  #new color of clustered pixels, must be less than minval(~162)
		clusterColor = std::clamp(clusterColor, 0, minval - 1);

		//todo_list = [seed] # a fifo
		std::queue<cv::Point> todo_list;
		todo_list.push(seed);

		//img[y, x] = cluster_color
		img.at<uchar>(pt) = (uchar)clusterColor;

		while (todo_list.size() != 0) {
			if (accepted.size() >= maxSizePx) {
				//return ClusterMetrics::FromPixels(accepted);
				return ClusterMetrics::FromWeightedPixels(accepted);
			}

			//N += 1
			//y, x = todo_list[0] #get front item
			//Y.append(y)
			//X.append(x)
			//W.append(img[y, x])
			//N += 1;
			pt = todo_list.front();
			//Y.push_back(pt.y);
			//X.push_back(pt.x);
			//W.push_back(img.at<uchar>(pt));
			accepted.emplace_back(pt, img.at<uchar>(pt));


			// ### do look_at_neighbors_swiss
			int yp = pt.y + 1;
			if (yp < imgH && img.at<uchar>(yp, pt.x)) {
				auto south_pt = cv::Point(pt.x, yp);
				todo_list.push(south_pt);
				img.at<uchar>(south_pt) = (uchar)clusterColor;
			}

			int ym = pt.y - 1;
			if (ym > 0 && img.at<uchar>(ym, pt.x) >= minval) {
				auto north_pt = cv::Point(pt.x, ym);
				todo_list.push(north_pt);
				img.at<uchar>(north_pt) = (uchar)clusterColor;
			}

			int xp = pt.x + 1;
			if (xp < imgW && img.at<uchar>(pt.y, xp) >= minval) {
				auto east_pt = cv::Point(xp, pt.y);
				todo_list.push(east_pt);
				img.at<uchar>(east_pt) = (uchar)clusterColor;
			}

			int xm = pt.x - 1;
			if (xm > 0 && img.at<uchar>(pt.y, xm) >= minval) {
				auto west_pt = cv::Point(xm, pt.y);
				todo_list.push(west_pt);
				img.at<uchar>(west_pt) = (uchar)clusterColor;
			}

			todo_list.pop();
		}

		//return ClusterMetrics::FromPixels(accepted);
		return ClusterMetrics::FromWeightedPixels(accepted);
	}

}