//
// Created by 韩萌 on 2022/6/14.
//

#ifndef KD_TREE_BASED_FARTHEST_POINT_SAMPLING_KDTREE_H
#define KD_TREE_BASED_FARTHEST_POINT_SAMPLING_KDTREE_H

#include "Point.h"
#include "vector"
#include "KDNode.h"


class KDTreeBase {
public:


    typedef std::vector <Point> Points;
    typedef std::vector <Point> *PointPtr;
    typedef KDNode *NodePtr;

    int memory_ops;
    int mult_ops;
    Points sample_points;
    NodePtr root_;
    Points points_;

public:
    KDTreeBase(Points &data);

    ~KDTreeBase() ;

    void deleteNode(NodePtr node_p) ;

    void buildKDtree() ;

    NodePtr get_root();

    int verify();

    NodePtr divideTree(int left, int right, std::vector <Interval> &bbox_ptr, int curr_high);

    void planeSplit(int left, int right, int split_dim,
                    float split_val, int &lim1, int &lim2) ;


    void qSelectMedian(int dim , int left, int right , float &median_value) ;

    static void findSplitDim(int &best_dim, std::vector <Interval> &bbox_ptr);

    inline void computeBoundingBox(int left, int right, std::vector <Interval> &bbox) ;

    inline void computeBoundingBox(int left, int right, std::vector <Interval> &bbox, int dim) ;

    inline void computeMinMax(int left, int right, int dim, Interval &bound) ;

    void init(const Point &ref) ;

    void cout_sample();

    virtual void addNode(NodePtr p) = 0;

    virtual bool leftNode(int high, int count) = 0;

    virtual Point max_point() = 0;

    virtual void update_distance(const Point &ref_point) = 0;

    virtual void sample(const int sample_num) = 0;
};


#endif //KD_TREE_BASED_FARTHEST_POINT_SAMPLING_KDTREE_H
