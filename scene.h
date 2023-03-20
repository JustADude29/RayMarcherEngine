//
// Created by pavan on 3/18/23.
//
#pragma once

#include <SFML/Graphics.hpp>
#include <Eigen/Dense>

class Scene{
public:
    Eigen::Vector3f GetCamPos() const{return cam_pos;}


private:
    Eigen::Vector3f cam_pos;
};


