#include "ikdtree.h"

#include <algorithm>
#include <cmath>

#include "util/log.h"


namespace epiphiyllum{

float calculateDistanceSquare(Point point1, Point point2){
  auto result = 
    (point1.x - point2.x)*(point1.x - point2.x) + 
    (point1.y - point2.y)*(point1.y - point2.y) + 
    (point1.z - point2.z)*(point1.z - point2.z);
  return result;
}

bool cmpPointByAxis(const Point& point1, const Point& point2, int spilt_axis){
  switch (spilt_axis){
    case 0: return point1.x <= point2.x;
    case 1: return point1.y <= point2.y;
    case 2: return point1.z <= point2.z;
    default:
      LOGE("axis {} invaild", spilt_axis);
      return false;
  }
}

void Ikdtree::build(std::vector<Point>& points, KDTREE_SPILT spilt_func){
  tree_ = buildTree(points, 0, 0, points.size() - 1, spilt_func);
}

IkdtreeNode* Ikdtree::buildTree(
  std::vector<Point>& points, int spilt_axis, int start_index, int end_index, KDTREE_SPILT spilt_func){
  if (start_index > end_index) return nullptr;
  auto cmp = [=](Point point1, Point point2){
    switch (spilt_func){
      case KDTREE_SPILT::AXIS: return cmpPointByAxis(point1, point2, spilt_axis);
      default: return false;
    }
  };
  std::sort(points.begin() + start_index, points.begin() + end_index, cmp);
  int mid = (end_index + start_index) / 2;
  IkdtreeNode* node = new IkdtreeNode(points[mid]);
  node->axis = spilt_axis;
  node->left = buildTree(points, (spilt_axis + 1) % 3, start_index, mid - 1, spilt_func);
  node->right = buildTree(points, (spilt_axis + 1)% 3, mid + 1, end_index, spilt_func);
  if(node->left != nullptr) node->left->parent = node;
  if(node->right != nullptr) node->right->parent = node;
  updateNode(node);
  return node;
}

#define getTreeNode(data, return_type, default_value) \
  return_type get_##data(IkdtreeNode* node){ \
    if(node == nullptr) return default_value; \
    else return node->data; \
  }

getTreeNode(tree_size, int, 0)
getTreeNode(invalid_num, int, 0)
getTreeNode(tree_deleted, bool, true)

void Ikdtree::updateNode(IkdtreeNode *node){
  std::array<Point, 2> temp_range =
    {Point{INFINITY, INFINITY, INFINITY}, Point{-INFINITY, -INFINITY, -INFINITY}};
  node->tree_size = get_tree_size(node->left) + get_tree_size(node->right) + 1;
  node->invalid_num = node->deleted ? 1: 0;
  node->invalid_num += get_invalid_num(node->left) + get_invalid_num(node->right);
  node->tree_deleted = node->deleted && get_tree_deleted(node->left) && get_tree_deleted(node->right);

  if (!get_tree_deleted(node->left)){
    temp_range[0].x = std::min(temp_range[0].x, node->left->range[0].x);
    temp_range[1].x = std::max(temp_range[1].x, node->left->range[1].x);
    temp_range[0].y = std::min(temp_range[0].y, node->left->range[0].y);
    temp_range[1].y = std::max(temp_range[1].y, node->left->range[1].y);
    temp_range[0].z = std::min(temp_range[0].z, node->left->range[0].z);
    temp_range[1].z = std::max(temp_range[1].z, node->left->range[1].z);
  }
  if (!get_tree_deleted(node->right)){
    temp_range[0].x = std::min(temp_range[0].x, node->right->range[0].x);
    temp_range[1].x = std::max(temp_range[1].x, node->right->range[1].x);
    temp_range[0].y = std::min(temp_range[0].y, node->right->range[0].y);
    temp_range[1].y = std::max(temp_range[1].y, node->right->range[1].y);
    temp_range[0].z = std::min(temp_range[0].z, node->right->range[0].z);
    temp_range[1].z = std::max(temp_range[1].z, node->right->range[1].z);      
  }
  if (!node->deleted){
    temp_range[0].x = std::min(temp_range[0].x, node->point.x);
    temp_range[1].x = std::max(temp_range[1].x, node->point.x);
    temp_range[0].y = std::min(temp_range[0].y, node->point.y);
    temp_range[1].y = std::max(temp_range[1].y, node->point.y);
    temp_range[0].z = std::min(temp_range[0].z, node->point.z);
    temp_range[1].z = std::max(temp_range[1].z, node->point.z);                    
  }

  memcpy(node->range.data(), temp_range.data(), sizeof(temp_range));
}

#define square(x) (x)*(x)

float Ikdtree::calc_box_dist3d(IkdtreeNode* node, Point point){  
  if (node == nullptr) return INFINITY;
  float min_dist = 0.0;
  if (point.x < node->range[0].x) min_dist += square(point.x - node->range[0].x);
  if (point.x > node->range[1].x) min_dist += square(point.x - node->range[0].x);
  if (point.y < node->range[0].y) min_dist += square(point.y - node->range[0].y);
  if (point.y > node->range[1].y) min_dist += square(point.y - node->range[0].y);
  if (point.z < node->range[0].z) min_dist += square(point.z - node->range[0].z);
  if (point.z > node->range[1].z) min_dist += square(point.z - node->range[0].z);
  return min_dist;
}

void Ikdtree::search(const int& k_nearest, const Point& point, DistanceHeap& heap, double max_dist){
  this->search(this->tree_, k_nearest, point, heap, max_dist);
}


void Ikdtree::search(IkdtreeNode *node, const int& k_nearest, const Point& point, DistanceHeap& heap, double max_dist){
  if(node == nullptr || node->tree_deleted) return;
  if(!node->deleted){
    float dist = calculateDistanceSquare(node->point, point);
    if((int)heap.size() < k_nearest || dist < heap.top().second){
      if((int)heap.size() >= k_nearest) heap.pop();
      heap.push(std::make_pair(node->point, dist));
    }
  }
  float left_min_dist = calc_box_dist3d(node->left, point);
  float right_min_dist = calc_box_dist3d(node->right, point);
  if((int)heap.size() < k_nearest || (left_min_dist < heap.top().second && right_min_dist < heap.top().second)){
    if(left_min_dist <= right_min_dist){
      this->search(node->left, k_nearest, point, heap, max_dist);
      if((int)heap.size() < k_nearest) {
        this->search(node->right, k_nearest, point, heap, max_dist);
      }
    }else{
      this->search(node->right, k_nearest, point, heap, max_dist);
      if((int)heap.size() < k_nearest) {
        this->search(node->left, k_nearest, point, heap, max_dist);
      }
    }
  }else if(left_min_dist < heap.top().second) {
    this->search(node->left, k_nearest, point, heap, max_dist);
    if((int)heap.size() < k_nearest) {
      this->search(node->right, k_nearest, point, heap, max_dist);
    }
  }else if(right_min_dist < heap.top().second){
    this->search(node->right, k_nearest, point, heap, max_dist);
    if((int)heap.size() < k_nearest) {
      this->search(node->left, k_nearest, point, heap, max_dist);
    }
  }
}

void Ikdtree::getSubTreePoint(IkdtreeNode *node, std::vector<Point>& points){
  if(node == nullptr)  return;
  points.emplace_back(node->point);
  getSubTreePoint(node->left, points);
  getSubTreePoint(node->right, points);
}


void Ikdtree::rebuildSubTree(IkdtreeNode *node){
  IkdtreeNode *new_node;
  std::vector<Point> points;
  points.reserve(node->tree_size);
  getSubTreePoint(node, points);
  new_node = buildTree(points, node->axis, 0, points.size(), KDTREE_SPILT::AXIS);
  new_node->parent = node->parent;
  node->parent = new_node;
  deleteNode(node);
}

void Ikdtree::deleteNode(IkdtreeNode *node){
  if(node == nullptr) return;
  deleteNode(node->left);
  deleteNode(node->right);
  node->parent = nullptr;
  delete node;
}


void Ikdtree::deleteNode(const Point& point){
  IkdtreeNode* node = findNode(tree_, point);
  if(node == nullptr) return;
  node->deleted = true;
  updateNode(node);
  if(node->tree_deleted) deleteNode(node);
}

IkdtreeNode* Ikdtree::findNode(IkdtreeNode *node, const Point& point){
  if(node == nullptr) return nullptr;
  if(node->point == point) return node;
  if (cmpPointByAxis(point, point, node->axis)){
    return findNode(node->left, point);
  }else{
    return findNode(node->right, point);
  }
}

void Ikdtree::insert(const Point& point){
  insertNode(tree_, point, 0);
}

void Ikdtree::insertNode(IkdtreeNode *node, const Point& point, int spilt_axis){
  if (cmpPointByAxis(point, node->point, spilt_axis)){
    if(node->left == nullptr) {
      node->left = new IkdtreeNode(point);
      node->axis = spilt_axis;
      node->left->parent = node;
      updateNode(node->left);
    } else{
      insertNode(node->left, point, (spilt_axis + 1) % 3);
    }
  }else{
    if(node->right == nullptr) {
      node->right = new IkdtreeNode(point);
      node->axis = spilt_axis;
      node->right->parent = node;
      updateNode(node->right);
    } else{
      insertNode(node->right, point, (spilt_axis + 1) % 3);
    }
  }
  updateNode(node);
}

DistanceHeap getDistanceHeap(){
  DistanceHeap dists(
    [](const std::pair<epiphiyllum::Point, float>& point1, const std::pair<epiphiyllum::Point, float>& point2)->bool{
      return point1.second < point2.second;
    });
  return dists;
}

} // epiphiyllum