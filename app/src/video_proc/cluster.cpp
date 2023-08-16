#include "cluster.h"

#include <vector>
#include <queue>
#include <limits>

namespace Airheads::Cluster {

ClusterMetrics ClusterMetrics::FromWeightedPixels(std::vector<ClusterPixel> &accepted) {
  if (accepted.empty()) {
    return {};
  }

  ClusterMetrics metrics;
  metrics.size_px = (int) accepted.size();

  int x_sum = 0;
  int x_weighted_sum = 0;

  int y_sum = 0;
  int y_weighted_sum = 0;

  int value_sum = 0;
  int value_sum_of_sqrs = 0;
  int value_min = std::numeric_limits<int>::max();

  for (const auto &pixel : accepted) {
    x_sum += pixel.loc.x;
    x_weighted_sum += pixel.loc.x * pixel.value;

    y_sum += pixel.loc.y;
    y_weighted_sum += pixel.loc.y * pixel.value;

    value_sum += pixel.value;
    value_sum_of_sqrs += value_sum * value_sum;
    if (value_min > pixel.value)
      value_min = pixel.value;
  }

  const float size_float = (float) metrics.size_px;
  const float value_sum_diff = (float) (value_sum - metrics.size_px * value_min);
  const float stdevFactorDenominator = value_sum_diff * value_sum_diff;
  if (stdevFactorDenominator > 0) {
    metrics.center = {
        (int) round((float)(x_weighted_sum - value_min * x_sum) / value_sum_diff),
        (int) round((float)(y_weighted_sum - value_min * y_sum) / value_sum_diff)
    };
  } else {
    // #This happens when all weights are identical.
    metrics.center = {
        (int) round((float)x_sum / size_float),
        (int) round((float)y_sum / size_float)
    };
  }

  return metrics;
}

ClusterMetrics ClusterMetrics::FromPixels(std::vector<ClusterPixel> &accepted) {
  if (accepted.empty()) {
    return {};
  }

  ClusterMetrics metrics;
  metrics.size_px = (int) accepted.size();

  int x_sum = 0;
  int y_sum = 0;

  for (const auto &pixel : accepted) {
    x_sum += pixel.loc.x;
    y_sum += pixel.loc.y;
  }

  const float size_float = (float) metrics.size_px;
  metrics.center = {
      (int) round((float)x_sum / size_float),
      (int) round((float)y_sum / size_float)
  };

  return metrics;
}

[[nodiscard]] std::optional<cv::Point> FindSeed(cv::Mat &img,
                                                cv::Point seed_guess,
                                                int min_val,
                                                int max_radius_px,
                                                int step_px = 2) {

  for (int R = step_px; R <= max_radius_px; R += step_px) {
    //#limit values of the squares for this Radius R
    int y0 = std::max(0, seed_guess.y - R);
    int x0 = std::max(0, seed_guess.x - R);
    int y1 = std::min(img.rows - 1, seed_guess.y + R);
    int x1 = std::min(img.cols - 1, seed_guess.x + R);

    //#Look North
    int y = y0;
    for (int x = x0; x <= x1; x += step_px) {
      if (img.at<uchar>(y, x) >= min_val)
        return cv::Point{x, y};
    }

    //#Look South
    cv::Point pt;
    pt.y = y1;
    for (pt.x = x0; pt.x <= x1; pt.x += step_px) {
      if (img.at<uchar>(pt) >= min_val)
        return pt;
    }

    //#Look East
    pt.x = x0;
    for (pt.y = y0; pt.y <= y1; pt.y += step_px) {
      if (img.at<uchar>(pt) >= min_val)
        return pt;
    }

    //#Look West
    pt.x = x1;
    for (pt.y = y0; pt.y <= y1; pt.y += step_px) {
      if (img.at<uchar>(pt) >= min_val)
        return pt;
    }
  }

  return {};
}

void ClusterFill(cv::Mat &img,
                 cv::Point seed,
                 uchar min_val,
                 int cluster_color,
                 int max_size_px,
                 std::vector<ClusterPixel> &accepted) {
  accepted.clear();

  if (img.at<uchar>(seed) < min_val)
    return;

  int img_width = img.cols;
  int img_height = img.rows;

  //#accepted pixels must be light colored, with value >= min_val
  cv::Point pt = seed;
  cluster_color = std::clamp(cluster_color, 0, min_val - 1);

  std::queue<cv::Point> todo_list;
  todo_list.push(seed);

  img.at<uchar>(pt) = (uchar) cluster_color;

  while (!todo_list.empty()) {
    if (accepted.size() >= max_size_px)
      return;

    pt = todo_list.front();
    accepted.emplace_back(pt, img.at<uchar>(pt));

    // ### do look_at_neighbors_swiss
    int yp = pt.y + 1;
    if (yp < img_height && img.at<uchar>(yp, pt.x)) {
      auto south_pt = cv::Point(pt.x, yp);
      todo_list.push(south_pt);
      img.at<uchar>(south_pt) = (uchar) cluster_color;
    }

    int ym = pt.y - 1;
    if (ym > 0 && img.at<uchar>(ym, pt.x) >= min_val) {
      auto north_pt = cv::Point(pt.x, ym);
      todo_list.push(north_pt);
      img.at<uchar>(north_pt) = (uchar) cluster_color;
    }

    int xp = pt.x + 1;
    if (xp < img_width && img.at<uchar>(pt.y, xp) >= min_val) {
      auto east_pt = cv::Point(xp, pt.y);
      todo_list.push(east_pt);
      img.at<uchar>(east_pt) = (uchar) cluster_color;
    }

    int xm = pt.x - 1;
    if (xm > 0 && img.at<uchar>(pt.y, xm) >= min_val) {
      auto west_pt = cv::Point(xm, pt.y);
      todo_list.push(west_pt);
      img.at<uchar>(west_pt) = (uchar) cluster_color;
    }

    todo_list.pop();
  }

}

}