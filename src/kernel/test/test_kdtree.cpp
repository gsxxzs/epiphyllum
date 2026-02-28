#include <opencv2/flann.hpp>
#include <opencv2/core.hpp>
#include "util/log.h"
#include "../opencl_kernel/ikdtree.h"


#define __timeStart__(tag)  auto start##tag = std::chrono::high_resolution_clock::now();
#define __timeEnd__(tag)  \
  auto end##tag = std::chrono::high_resolution_clock::now(); \
  LOGI("{} cost {} ns.", #tag, std::chrono::duration_cast<std::chrono::nanoseconds>(end##tag - start##tag).count());

void test_opencv(int knn){
  // 示例数据：二维数据点集合
  cv::Mat data = (cv::Mat_<float>(5, 3) << 2.0f, 2.0f, 1.0f,
                                            2.0f, 3.0f, 2.0f,
                                            3.0f, 4.0f, 3.0f,
                                            4.0f, 5.0f, 4.0f,
                                            5.0f, 6.0f, 5.0f);
  __timeStart__(kdtree)
  // 设置 KDTree 索引的参数
  cv::flann::KDTreeIndexParams indexParams(4); // 使用 4 棵树

  // 创建 FLANN 索引
  cv::flann::Index kdtree(data, indexParams);
  // 查询一个数据点
  cv::Mat query = (cv::Mat_<float>(1, 3) << 2.1f, 2.4f, 3.5f);
  std::vector<int> indices(2);  // 最邻近的 2 个索引
  std::vector<float> dists(2);  // 最邻近的 2 个距离

  // 查找最近邻
  kdtree.knnSearch(query, indices, dists, knn, cv::flann::SearchParams(32));
  __timeEnd__(kdtree)

  // 打印结果
  LOGI("nearest neighbors: ");
  for(int i = 0; i < knn; ++i){
    LOGI("point {}: ({}, {}, {}) distance : {:.2f}", 
        indices[i] + 1 , data.at<float>(indices[i], 0), data.at<float>(indices[i], 1), data.at<float>(indices[i], 2), dists[i]);
   }
}

void test_ikdtree(int knn){
  std::vector<epiphiyllum::Point> data {{2.0f, 2.0f, 1.0f},
                                          {2.0f, 3.0f, 2.0f},
                                          {3.0f, 4.0f, 3.0f},
                                          {4.0f, 5.0f, 4.0f},
                                          {5.0f, 6.0f, 5.0f}};
  __timeStart__(ikdtree)

  epiphiyllum::Point qurey {2.1f, 2.4f, 3.5f};

  epiphiyllum::Ikdtree kdtree;
  kdtree.build(data);
  epiphiyllum::DistanceHeap dists = epiphiyllum::getDistanceHeap();
  kdtree.insert(epiphiyllum::Point {3.0f, 4.0f, 3.1f});
  kdtree.search(knn, qurey, dists, INFINITY);
  __timeEnd__(ikdtree)

  // 打印结果
  LOGI("nearest neighbors: ");
  for(int i = 0; i < knn; ++i){
    auto [point, dist] = dists.top();
    dists.pop();
    LOGI("point {}: ({}, {}, {}) distance : {:.2f}", i, point.x, point.y, point.z, dist);
  }
}


int main() {
  test_ikdtree(4);
  test_opencv(4);
  return 0;
}