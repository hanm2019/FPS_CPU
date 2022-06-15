//
// Created by 韩萌 on 2022/6/14.
//

#include "KDNode.h"
KDNode::KDNode() : left(NULL), right(NULL), max_point() {
    points.clear();
    delaypoints.clear();
}

KDNode::KDNode(const KDNode &a) {
    points = a.points;
    delaypoints = a.delaypoints;
    max_point = a.max_point;
    left = a.left;
    right = a.right;
}

void KDNode::init(const Point &ref) {
    while (!waitpoints.empty()) waitpoints.pop();
    delaypoints.clear();
    if (this->left && this->right) {
        this->left->init(ref);
        this->right->init(ref);
        updateMaxPoint(this->left->max_point, this->right->max_point);
    } else {
        double dis;
        double maxdis(0);
        for (auto &it: points) {
            dis = it.updatedistance(ref);
            if (dis > maxdis) {
                maxdis = dis;
                max_point = it;
            }
        }
    }
}

void KDNode::updateMaxPoint(const Point &lpoint, const Point &rpoint) {
    if (lpoint.dis > rpoint.dis) this->max_point = lpoint;
    else this->max_point = rpoint;
}

double KDNode::bound_distance(const Point &ref_point) {
    double bound_dis(0);
    double dim_distance;
    for (int cur_dim = 0; cur_dim < 3; cur_dim++) {
        dim_distance = 0;
        if (ref_point[cur_dim] > bbox[cur_dim].high) dim_distance = ref_point[cur_dim] - bbox[cur_dim].high;
        if (ref_point[cur_dim] < bbox[cur_dim].low) dim_distance = bbox[cur_dim].low - ref_point[cur_dim];
        bound_dis += pow(dim_distance, 2);
    }
    return bound_dis;
}

void KDNode::send_delay_point(const Point &point) {
    this->waitpoints.push(point);
}

void KDNode::update_distance(int &memory_ops, int &mult_ops) {
    while (!this->waitpoints.empty()) {

        Point ref_point = this->waitpoints.front();
        this->waitpoints.pop();

        double lastmax_distance = this->max_point.dis;
        double cur_distance = this->max_point.distance(ref_point);
        mult_ops++;
        // cur_distance > lastmax_distance意味着当前Node的max_point不会进行更新
        if (cur_distance > lastmax_distance) {
            double boundary_distance = bound_distance(ref_point);
            mult_ops++;
            if (boundary_distance < lastmax_distance) {
                this->delaypoints.push_back(ref_point);
            }
        } else {
            if (this->right && this->left) {
                for (const auto &delay_point: delaypoints) {
                    this->left->send_delay_point(delay_point);
                    this->right->send_delay_point(delay_point);
                }
                if (!delaypoints.empty()) delaypoints.clear();
                this->left->send_delay_point(ref_point);
                this->right->send_delay_point(ref_point);
                this->right->update_distance(memory_ops, mult_ops);
                this->left->update_distance(memory_ops, mult_ops);
                updateMaxPoint(this->left->max_point, this->right->max_point);
            } else {
                double dis;
                double maxdis;
                this->delaypoints.push_back(ref_point);
                for (const auto &delay_point: delaypoints) {
                    maxdis = 0;
                    for (auto &it: points) {
                        dis = it.updatedistance(delay_point);
                        if (dis > maxdis) {
                            maxdis = dis;
                            max_point = it;
                        }
                    }
                }
                mult_ops += delaypoints.size() * points.size();
                memory_ops += points.size();
                this->delaypoints.clear();
            }
        }
    }
}

void KDNode::reset() {
    for(auto p:points){
        p.reset();
    }
    while(!this->waitpoints.empty()) this->waitpoints.pop();
    this->delaypoints.clear();
    this->max_point.reset();
    if(this->left && this->right){
        this->left->reset();
        this->right->reset();
    }
}