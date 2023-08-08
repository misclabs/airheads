#include "cluster.h"

#include <vector>
#include <queue>

namespace Airheads::Cluster {
	//constexpr int MAX_CLUSTER_ITERATIONS = 4096;

	Cluster::Cluster(std::vector<int>& X, std::vector<int>& Y, std::vector<uchar>& W, int _N) {
		N = _N;

		if (N <= 0) {
			//Ymin = -1;
			//Ymax = -1;
			//Xmin = -1;
			//Xmax = -1;
			////Wmin = -1;
			////Wmax = -1;
			//Yavg = -1;
			//Xavg = -1;
			////Wavg = -1;
			////Ystdev = -1;
			////Xstdev = -1;
			////Wstdev = -1;
			////YWavg = -1;
			////XWavg = -1;
			////YWstdev = -1;
			////XWstdev = -1;

			return;
		}

		//	#note trouble with ubyte overflow from W
		//	self.Ymin = float(min(Y))
		//	self.Ymax = float(max(Y))
		//	Ysum = float(sum(Y))
		//	Y2sum = float(np.sum(np.square(Y))) #fastest over list comprehension, generators, and np.dot
		float Ysum, Y2sum;
		{
			int Ymin_int = std::numeric_limits<int>::max();
			int Ymax_int = std::numeric_limits<int>::min();
			int Ysum_int = 0;
			int Y2sum_int = 0;
			for (const auto& y : Y) {
				if (y < Ymin_int)
					Ymin_int = y;

				if (y > Ymax_int)
					Ymax_int = y;

				Ysum_int += y;
				Y2sum_int += y * y;
			}
			Ymin = (float)Ymin_int;
			Ymax = (float)Ymax_int;
			Ysum = (float)Ysum_int;
			Y2sum = (float)Y2sum_int;
		}


		//	self.Xmin = float(min(X))
		//	self.Xmax = float(max(X))
		//	Xsum = float(sum(X))
		//	X2sum = float(np.sum(np.square(X)))
		float Xsum, X2sum;
		{
			int Xmin_int = std::numeric_limits<int>::max();
			int Xmax_int = std::numeric_limits<int>::min();
			int Xsum_int = 0;
			int X2sum_int = 0;
			for (const auto& x : X) {
				if (x < Xmin_int)
					Xmin_int = x;

				if (x > Xmax_int)
					Xmax_int = x;

				Xsum_int += x;
				X2sum_int += x * x;
			}
			Xmin = (float)Xmin_int;
			Xmax = (float)Xmax_int;
			Xsum = (float)Xsum_int;
			X2sum = (float)X2sum_int;
		}

		//	self.Wmin = float(min(W))
		//	self.Wmax = float(max(W))
		//	Wnp = np.array(W, dtype = np.uint32)
		//	Wsum = float(np.sum(Wnp))
		//	W2sum = float(np.sum(np.square(Wnp)))
		//	WXsum = float(np.dot(X, W))
		//	WYsum = float(np.dot(Y, W))

		//	####

		//	Nfloat = float(N)
		float Nfloat = (float)N;

		//	self.Yavg = Ysum / Nfloat
		//	self.Xavg = Xsum / Nfloat
		Yavg = Ysum / Nfloat;
		Xavg = Xsum / Nfloat;

		//	self.Wavg = Wsum / Nfloat
		//	self.Ystdev = sqrt((Y2sum / Nfloat) - self.Yavg * self.Yavg)
		//	self.Xstdev = sqrt((X2sum / Nfloat) - self.Xavg * self.Xavg)
		//	self.Wstdev = sqrt((W2sum / Nfloat) - self.Wavg * self.Wavg)

		//	Wdiff = float(Wsum - N * self.Wmin)
		//	stdev_factor_deonominator = Wdiff * Wdiff
		//	if stdev_factor_deonominator > 0:
		//self.YWavg = float(WYsum - self.Wmin * Ysum) / Wdiff
		//	self.XWavg = float(WXsum - self.Wmin * Xsum) / Wdiff
		//	stdev_factor_radical = 1.0 - (float(W2sum - self.Wmin * Wsum * 2 + N * (self.Wmin * self.Wmin)) / stdev_factor_deonominator)
		//	#print(f"N {N}, stdev_factor_radical {stdev_factor_radical }, Wmin {self.Wmin}, Wmax {self.Wmax}, Wsum {Wsum} W2sum {W2sum}, deominator {stdev_factor_deonominator}")
		//	stdev_factor = 1
		//	if stdev_factor_radical > 0:
		//stdev_factor = sqrt(stdev_factor_radical)
		//	self.XWstdev = self.Xstdev * stdev_factor
		//	self.YWstdev = self.Ystdev * stdev_factor
		//	#self.validW = True
		//	else: #This happens when all weights are identical.
		//		self.YWavg = self.Yavg
		//		self.XWavg = self.Xavg
		//		self.XWstdev = self.Xstdev
		//		self.YWstdev = self.Ystdev
		//		#self.validW = False

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

	Cluster GrowCluster(cv::Mat& img, cv::Point seed, uchar minval, int clusterColor, int maxSizePx) {
		int imgW = img.cols;
		int imgH = img.rows;
		//#accepted pixels must be light colored, with value >= minval
		//N = 0
		int N = 0;
		//int y = seed.y;
		//int x = seed.x;
		cv::Point pt = seed;
		std::vector<int> Y;
		std::vector<int> X;
		std::vector<uchar> W;

		if (img.at<uchar>(seed) < minval) {
			return Cluster(X, Y, W, N); // TODO(jw): 
		}

		//cluster_color = max(0, min(cluster_color, minval - 1))  #new color of clustered pixels, must be less than minval(~162)
		clusterColor = std::clamp(clusterColor, 0, minval - 1);

		//todo_list = [seed] # a fifo
		std::queue<cv::Point> todo_list;
		todo_list.push(seed);

		//img[y, x] = cluster_color
		img.at<uchar>(pt) = (uchar)clusterColor;

		while (todo_list.size() != 0) {
			//N += 1
			//y, x = todo_list[0] #get front item
			//Y.append(y)
			//X.append(x)
			//W.append(img[y, x])
			N += 1;
			pt = todo_list.front();
			Y.push_back(pt.y);
			X.push_back(pt.x);
			W.push_back(img.at<uchar>(pt));

			if (N >= maxSizePx) {
				return Cluster(X, Y, W, N - 1);
			}

			// ### do look_at_neighbors_swiss
			//yp = y + 1
			//if yp < imgH and img[yp, x] >= minval:
				//todo_list.append((yp, x))
				//img[yp, x] = cluster_color
			int yp = pt.y + 1;
			if (yp < imgH && img.at<uchar>(yp, pt.x)) {
				auto south_pt = cv::Point(pt.x, yp);
				todo_list.push(south_pt);
				img.at<uchar>(south_pt) = (uchar)clusterColor;
			}

			//ym = y - 1
			//if y > 0 and img[ym, x] >= minval:
				//todo_list.append((ym, x))
				//img[ym, x] = cluster_color
			int ym = pt.y - 1;
			if (ym > 0 && img.at<uchar>(ym, pt.x) >= minval) {
				auto north_pt = cv::Point(pt.x, ym);
				todo_list.push(north_pt);
				img.at<uchar>(north_pt) = (uchar)clusterColor;
			}

			//xp = x + 1
			//if xp < imgW and img[y, xp] >= minval :
				//todo_list.append((y, xp))
				//img[y, xp] = cluster_color
			int xp = pt.x + 1;
			if (xp < imgW && img.at<uchar>(pt.y, xp) >= minval) {
				auto east_pt = cv::Point(xp, pt.y);
				todo_list.push(east_pt);
				img.at<uchar>(east_pt) = (uchar)clusterColor;
			}

			//xm = x - 1
			//if x > 0 and img[y, xm] >= minval:
				//todo_list.append((y, xm))
				//img[y, xm] = cluster_color
			int xm = pt.x - 1;
			if (xm > 0 && img.at<uchar>(pt.y, xm) >= minval) {
				auto west_pt = cv::Point(xm, pt.y);
				todo_list.push(west_pt);
				img.at<uchar>(west_pt) = (uchar)clusterColor;
			}

			todo_list.pop();
		}
		return Cluster(X, Y, W, N);
	}

}