#include "dot_diff.h"
#include "cluster.h"

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "imgui.h"

#include <vector>
#include <queue>
#include <limits>
#include <cmath>

namespace Airheads {

	int DistanceBetween(cv::Point a, cv::Point b) {
		auto dx = b.x - a.x;
		auto dy = b.y - a.y;
		return (int)sqrt(dx * dx + dy * dy);
	}

	struct Bounds {
		cv::Point topLeft;
		cv::Point bottomRight;

		void Clamp(cv::Point& pt) {
			if (pt.x < topLeft.x)
				pt.x = topLeft.x;
			else if (pt.x > bottomRight.x)
				pt.x = bottomRight.x;

			if (pt.y < topLeft.y)
				pt.y = topLeft.y;
			else if (pt.y > bottomRight.y)
				pt.y = bottomRight.y;
		}
	};

	static const std::string g_dotDiffName {"Dot Diff"};

	DotDiffUniquePtr DotDiff::Create() {
		return std::make_unique<DotDiff>();
	}

	const std::string& DotDiff::Name() const {
		return g_dotDiffName;
	}

	void DotDiff::StartCapture(int frameWidth, int frameHeight) {
		m_frameWidth = frameWidth;
		m_frameHeight = frameHeight;
		Reset();
	}

	void DotDiff::Reset() {
		m_upperClusterLastCoords = { m_frameWidth / 2, (int)(m_frameHeight * 0.3) };
		m_lowerClusterLastCoords = { m_frameWidth / 2, (int)(m_frameWidth * 0.6) };
		m_interclusterDistPx = m_frameWidth;
		m_minDist = m_frameWidth;
		m_maxDist = 0;
	}

	void DotDiff::ProcessFrame(ProcessingContext& context) {

		double inv_thresh = (double)(255 - m_valueThreshold);
		if (inv_thresh < 1.0)
			inv_thresh = 1.0;

		cv::cvtColor(context.frame, hsv, cv::COLOR_BGR2HSV);

		context.saturation_map.create(hsv.rows, hsv.cols, CV_8UC1);
		context.value_map.create(hsv.rows, hsv.cols, CV_8UC1);
		{
			cv::Mat sv_out[] = { context.saturation_map, context.value_map };
			int from_to[] = { 1,0, 2,1 };
			cv::mixChannels(&hsv, 1, sv_out, 2, from_to, 2);
		}
		
		//_, saturation_map = cv2.threshold(saturation_map, saturation_threshold, 255, cv2.THRESH_BINARY_INV) #now saturation_map is really a mask
		cv::threshold(context.saturation_map, context.saturation_map, m_saturationThreshold, 255, cv::THRESH_BINARY_INV);

		//_, value_map = cv2.threshold(~value_map, inv_thresh, 255, cv2.THRESH_TOZERO)
		context.value_map.forEach<uchar>([](uchar& pixel, const int * position) -> void {
			pixel = ~pixel;
		});
		cv::threshold(context.value_map, context.value_map, inv_thresh, 255, cv::THRESH_TOZERO);

		//#first return value of cv2.threshold is the threshold
		//#manual: https://docs.opencv.org/3.4/d7/d4d/tutorial_py_thresholding.html
		//value_map = cv2.bitwise_and(value_map, saturation_map)
		//cv::bitwise_and(context.value_map, context.saturation_map, context.value_map);
		context.cluster_map.create(context.value_map.rows, context.value_map.cols, CV_8UC1);
		cv::bitwise_and(context.value_map, context.saturation_map, context.cluster_map);

		int max_seek_radius = (int)std::max(100.0, 0.65 * m_interclusterDistPx);

		cv::Point upper_cluster_seed;
		bool foundU = Cluster::FindSeed(context.cluster_map,
			m_upperClusterLastCoords, 
			(int)inv_thresh, max_seek_radius,
			upper_cluster_seed);
		
		cv::Point lower_cluster_seed;
		bool foundL = Cluster::FindSeed(context.cluster_map,
			m_lowerClusterLastCoords,
			(int)inv_thresh, max_seek_radius,
			lower_cluster_seed
		);

		Bounds frameBounds = { {0, 0}, {context.frame.cols, context.frame.rows} };
		Cluster::Cluster cluster_upper;
		if (foundU) {
			cluster_upper = Cluster::GrowCluster(context.cluster_map, upper_cluster_seed, (uchar)inv_thresh, m_clusterColor, m_maxClusterSizePx);
			m_upperClusterLastCoords = cluster_upper.get_center();
			frameBounds.Clamp(m_upperClusterLastCoords);
		}

		Cluster::Cluster cluster_lower;
		if (foundL) {
			cluster_lower = Cluster::GrowCluster(context.cluster_map, lower_cluster_seed, (uchar)inv_thresh, m_clusterColor, m_maxClusterSizePx);
			m_lowerClusterLastCoords = cluster_lower.get_center();
			frameBounds.Clamp(m_lowerClusterLastCoords);
		}

		//#### DECORATIONS ####
		if (foundU && cluster_upper.N > 2) {
			int dotRadius = (int)(2 + 16.0 * cluster_upper.N / m_maxClusterSizePx);
			cv::circle(context.frame, m_upperClusterLastCoords, dotRadius, cv::Scalar(0, 255, 0), cv::FILLED, cv::LINE_8);
		}

		if (foundL && cluster_lower.N > 2) {
			int dotRadius = (int)(2 + 16.0 * cluster_lower.N / m_maxClusterSizePx);
			cv::circle(context.frame, m_lowerClusterLastCoords, dotRadius, cv::Scalar(0, 0, 255), cv::FILLED, cv::LINE_8);
		}

		if (foundU && foundL && cluster_upper.N > 2 && cluster_lower.N > 2) {
			m_interclusterDistPx = DistanceBetween(m_upperClusterLastCoords, m_lowerClusterLastCoords);

			m_minDist = std::min(m_minDist, m_interclusterDistPx);
			m_maxDist = std::max(m_maxDist, m_interclusterDistPx);
		
			int overlay_line_width_pixels = 2;
			cv::line(context.frame, m_upperClusterLastCoords, m_lowerClusterLastCoords, cv::Scalar(0, 255, 0), overlay_line_width_pixels);
		}
	}

	void DotDiff::UpdateConfigControls() {
		ImGui::SliderInt("Saturation Threshold", &m_saturationThreshold, 0, 255);
		ImGui::SliderInt("Value Threshold", &m_valueThreshold, 0, 255);
		
		if (ImGui::Button("Reset Cluster Guess")) {
			Reset();
		}
	}

	void DotDiff::UpdateStatsControls() {
		//overlay_color = (0, 255, 0) #BGR color.a 4th and final param is possible here.If it's alpha, it doesn't work.
		//nope_color = (255, 255, 255)

		ImGui::Text("Intercluster Distance: %dpx", m_interclusterDistPx);

		ImGui::Text("Min distance: %dpx", m_minDist);
		ImGui::Text("Max distance: %dpx", m_maxDist);
		//if dmdm > dmdm_thresh:
		//	dm_color = overlay_color
		//else:
		//	dm_color = nope_color
		double dmdm = m_maxDist / (double)m_minDist;
		//if (dmdm > dmdm_thresh) {

		//}

		//cv2.putText(out_frame, f"Current Dmax/Dmin: {dmdm:.3f}",
		//	text_position4, cv2.FONT_HERSHEY_SIMPLEX, 2, dm_color, overlay_line_width_pixels)
		ImGui::Text("Dmax/Dmin: %.3f", dmdm);

		//#TODO make printing this optional
		//#    print(f"   cluster_upper N={cluster_upper.N }, cluster_lower N={cluster_lower.N}")
		//#    print(f"   cu = {upper_cluster_last_coords[0] },{upper_cluster_last_coords[1] }, cl = {lower_cluster_last_coords[0] },{lower_cluster_last_coords[1] }, dist = {intercluster_distance_pixels:.1f} ")
	}

}