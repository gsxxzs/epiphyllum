#pragma once
#include <vector>
#include <array>
#include <queue>
#include <functional>


namespace epiphiyllum {
template<typename T>
struct Point3{
  T x, y, z;
  bool operator==(const Point3& point) const {
    return (x == point.x && y == point.y && z == point.z);
  }
};
using Ponit3f = epiphiyllum::Point3<float>;
using point3i = epiphiyllum::Point3<int>;
} //epiphiyllum


namespace epiphiyllum {
using Point = epiphiyllum::Ponit3f;

enum KDTREE_SPILT{
  AXIS = 0,
};

struct IkdtreeNode{
  Point point;          // 点的数值
  IkdtreeNode *left;    // 左子树
  IkdtreeNode *right;   // 右子树
  IkdtreeNode *parent;  // 父节点
  int axis;             // 划分轴
  int tree_size;        // 子树的大小
  int invalid_num;      // 子树中标记为deleted的结点数
  std::array<Point, 2> range; // 子树中每个维度的最大值与最小值
  bool deleted;         // 节点是否删除
  bool tree_deleted;    // 子树是否全部删除
  bool pushdown;    

  IkdtreeNode() = default;
  IkdtreeNode(Point point): 
    point(point), left(nullptr), right (nullptr), axis(0), tree_size(1), 
    invalid_num(0), deleted(false), tree_deleted(false), pushdown(false){
  }
};


using DistanceHeap = std::priority_queue<
    std::pair<Point, float>, 
    std::vector<std::pair<Point, float>>, 
    std::function<bool(const std::pair<Point, float>&, const std::pair<Point, float>&)>>;

DistanceHeap getDistanceHeap();

class Ikdtree {
 public:
  Ikdtree() = default;
  ~Ikdtree(){
    deleteNode(tree_);
  }
  void build(std::vector<Point>& points, KDTREE_SPILT spilt_func = KDTREE_SPILT::AXIS);
  void search(const int& k_nearest, const Point& point, DistanceHeap& heap, double max_dist);
  void insert(const Point& point);
  void deleteNode(const Point& point);

 private:
  IkdtreeNode* buildTree(std::vector<Point>& points, int spilt_axis, int start_index, int end_index, KDTREE_SPILT spilt_func);
  void search(IkdtreeNode *node, const int& k_nearest, const Point& point, DistanceHeap& heap, double max_dist);
  void updateSubTree();
  void rebuildSubTree(IkdtreeNode *node);
  void getSubTreePoint(IkdtreeNode *node, std::vector<Point>& points);
  void insertNode(IkdtreeNode *node, const Point& point, int spilt_axis);
  void deleteNode(IkdtreeNode *node);
  void updateNode(IkdtreeNode *node);
  IkdtreeNode* findNode(IkdtreeNode *node, const Point& point);
  IkdtreeNode* findRootNode(IkdtreeNode *node);

  float calc_box_dist3d(IkdtreeNode* node, Point point);

  IkdtreeNode* tree_;
};
} //epiphiyllum