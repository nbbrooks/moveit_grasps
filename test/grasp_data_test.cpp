/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2018, PickNik LLC
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of PickNik LLC nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

/* Author: Dave Coleman

   Test loading the Panda config
*/

// C++
#include <string>

// ROS
#include <ros/ros.h>

// Testing
#include <gtest/gtest.h>

// Grasp generation
#include <moveit_grasps/grasp_data.h>
#include <moveit_visual_tools/moveit_visual_tools.h>

namespace moveit_grasps
{
class GraspDataTest : public ::testing::Test
{
public:
  GraspDataTest() : nh_("~"), ee_group_name_("hand"), visual_tools_(new moveit_visual_tools::MoveItVisualTools("base"))
  {
  }

protected:
  ros::NodeHandle nh_;
  std::string ee_group_name_;
  moveit_visual_tools::MoveItVisualToolsPtr visual_tools_;
};  // class GraspGenerator

TEST_F(GraspDataTest, ConstructDestruct)
{
  GraspData grasp_data(nh_, ee_group_name_, visual_tools_->getRobotModel());
}

TEST_F(GraspDataTest, CheckConfigValues)
{
  GraspData grasp_data(nh_, ee_group_name_, visual_tools_->getRobotModel());

  // Grasp Pose To EEF Pose
  EXPECT_EQ(grasp_data.grasp_pose_to_eef_pose_.translation().x(), 0);
  EXPECT_EQ(grasp_data.grasp_pose_to_eef_pose_.translation().y(), 0);
  EXPECT_EQ(grasp_data.grasp_pose_to_eef_pose_.translation().z(), -0.13);

  // Pre Grasp Posture
  EXPECT_EQ(grasp_data.pre_grasp_posture_.header.frame_id, "world");
  EXPECT_GT(grasp_data.pre_grasp_posture_.header.stamp.toSec(), 0);
  EXPECT_EQ(grasp_data.pre_grasp_posture_.points.size(), 1);
  EXPECT_GT(grasp_data.pre_grasp_posture_.points[0].positions.size(), 0);

  // Grasp Posture
  EXPECT_EQ(grasp_data.grasp_posture_.header.frame_id, "world");
  EXPECT_GT(grasp_data.grasp_posture_.header.stamp.toSec(), 0);
  EXPECT_EQ(grasp_data.grasp_posture_.points.size(), 1);
  EXPECT_GT(grasp_data.grasp_posture_.points[0].positions.size(), 0);

  // Semantics
  EXPECT_EQ(grasp_data.base_link_, "world");
  EXPECT_EQ(grasp_data.ee_jmg_->getName(), "hand");
  EXPECT_EQ(grasp_data.arm_jmg_->getName(), "panda_arm_hand");
  EXPECT_EQ(grasp_data.parent_link_->getName(), "panda_hand");
  EXPECT_EQ(grasp_data.robot_model_->getName(), "panda");

  // Geometry doubles
  EXPECT_GT(grasp_data.grasp_depth_, 0);
  EXPECT_GT(grasp_data.angle_resolution_, 0);
  EXPECT_GT(grasp_data.grasp_max_depth_, 0);
  EXPECT_GT(grasp_data.grasp_resolution_, 0);
  EXPECT_GT(grasp_data.grasp_depth_resolution_, 0);
  EXPECT_GT(grasp_data.grasp_min_depth_, 0);
  EXPECT_GT(grasp_data.gripper_finger_width_, 0);
  EXPECT_GT(grasp_data.max_grasp_width_, 0);
  EXPECT_GT(grasp_data.approach_distance_desired_, 0);
  EXPECT_GT(grasp_data.retreat_distance_desired_, 0);
  EXPECT_GT(grasp_data.lift_distance_desired_, 0);
  EXPECT_GT(grasp_data.grasp_padding_on_approach_, 0);
  EXPECT_GT(grasp_data.max_finger_width_, 0);
  EXPECT_GT(grasp_data.min_finger_width_, 0);
}

TEST_F(GraspDataTest, SetRobotState)
{
  GraspData grasp_data(nh_, ee_group_name_, visual_tools_->getRobotModel());
  moveit::core::RobotStatePtr robot_state = visual_tools_->getSharedRobotState();

  // Pre Grasp
  grasp_data.setRobotStatePreGrasp(robot_state);
  EXPECT_EQ(grasp_data.pre_grasp_posture_.points[0].positions[0],
            robot_state->getJointPositions("panda_finger_joint1")[0]);
  EXPECT_EQ(grasp_data.pre_grasp_posture_.points[0].positions[1],
            robot_state->getJointPositions("panda_finger_joint2")[0]);

  // Grasp
  grasp_data.setRobotStateGrasp(robot_state);
  EXPECT_EQ(grasp_data.grasp_posture_.points[0].positions[0], robot_state->getJointPositions("panda_finger_joint1")[0]);
  EXPECT_EQ(grasp_data.grasp_posture_.points[0].positions[1], robot_state->getJointPositions("panda_finger_joint2")[0]);
}

// TODO(davetcoleman): write test for remainder of this class

}  // namespace moveit_grasps

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  ros::init(argc, argv, "grasp_data_test");

  ros::AsyncSpinner spinner(1);
  spinner.start();

  int result = RUN_ALL_TESTS();

  spinner.stop();
  ros::shutdown();
  return result;
}
