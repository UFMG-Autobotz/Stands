#ifndef _VT_SIM_PLUGIN_HH_
#define _VT_SIM_PLUGIN_HH_

#include <iostream>
#include <vector>
#include <thread>
#include <string>
#include <algorithm>

#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>

#include "ros/ros.h"
#include "ros/callback_queue.h"
#include "ros/subscribe_options.h"
#include "std_msgs/Float32.h"

bool invalidChar (char c){
	return !((c>=47 && c <=57) || (c>=65 && c <=90) || (c>=97 && c <=122) );
}

void validate_str(std::string & str){
	std::replace_if( str.begin(), str.end(), invalidChar, '_' );
}

namespace gazebo{

	/// \brief A plugin to control a VT_sim.
	class VT_simPlugin : public ModelPlugin{

		/// \brief Constructor
		public: VT_simPlugin() {}

		/// \brief The load function is called by Gazebo when the plugin is
		/// inserted into simulation
		/// \param[in] _model A pointer to the model that this plugin is
		/// attached to.
		/// \param[in] _sdf A pointer to the plugin's SDF element.
		public: virtual void Load(physics::ModelPtr _model, sdf::ElementPtr _sdf){
			// Safety check

			this->n_joints = _model->GetJointCount();
			std::cerr << "\n";
			std::cerr << this->n_joints;
			std::cerr << " Joints found\n\n";

			if (this->n_joints == 0){
				std::cerr << "Invalid joint count, VT_sim plugin not loaded\n";
				return;
			}

			// Store the model pointer for convenience.
			this->model = _model;

			// Get the first joint. We are making an assumption about the model
			// having one joint that is the rotational joint.
			this->joints_vector = _model->GetJoints();

			for (int i = 0; i < this->n_joints; ++i){
				std::cerr << this->joints_vector[i]->GetScopedName();
				std::cerr << "\n";
				// Setup a P-controller, with a gain of 0.1.
				this->pid_vector.push_back(common::PID(0.1, 0, 0));
				// Apply the P-controller to the joint.
				this->model->GetJointController()->SetVelocityPID(this->joints_vector[i]->GetScopedName(), this->pid_vector.back());
			}
			std::cerr << "\n";

			// Default to zero velocity
			double velocity = 0;

			// Check that the velocity element exists, then read the value
			if (_sdf->HasElement("velocity"))
				velocity = _sdf->Get<double>("velocity");

			for (int i = 0; i < this->n_joints; ++i){
				this->SetVelocity(velocity, i);
			}

			// Initialize ros, if it has not already bee initialized.
			if (!ros::isInitialized()){
				int argc = 0;
				char **argv = NULL;
				ros::init(argc, argv, "gazebo_client", ros::init_options::NoSigintHandler);
			}

			// Create our ROS node. This acts in a similar manner to the Gazebo node
			this->rosNode.reset(new ros::NodeHandle("gazebo_client"));

			std::cerr << "Creating ROS topics:\n\n";

			for (int i = 0; i < this->n_joints; ++i){
				// Create a topic name
				std::string topicName = "/" + this->model->GetName() + "/joint_vel_" + this->joints_vector[i]->GetScopedName();
				validate_str(topicName);
				std::cerr << topicName << "\n";

				// Create a named topic, and subscribe to it.
				ros::SubscribeOptions so = ros::SubscribeOptions::create<std_msgs::Float32>( topicName, 1,
						boost::bind(&VT_simPlugin::OnRosMsg, this, _1, i), ros::VoidPtr(), &this->rosQueue);

				this->rosSub_vector.push_back(this->rosNode->subscribe(so));
			}
			std::cerr << "\n";

			// Spin up the queue helper thread.
			this->rosQueueThread = std::thread(std::bind(&VT_simPlugin::QueueThread, this));
		}

		/// \brief Set the velocity of the VT_sim
		/// \param[in] _vel New target velocity
		public: void SetVelocity(const double &_vel, int joint_ID){
			// Set the joint's target velocity.
			this->model->GetJointController()->SetVelocityTarget(
					this->joints_vector[joint_ID]->GetScopedName(), _vel);
		}

		/// \brief Handle an incoming message from ROS
		/// \param[in] _msg A float value that is used to set the velocity
		/// of the VT_sim.
		public: void OnRosMsg(const std_msgs::Float32ConstPtr &_msg, const int joint_ID){
			this->SetVelocity(_msg->data, joint_ID);
		}

		/// \brief ROS helper function that processes messages
		private: void QueueThread(){
			static const double timeout = 0.01;
			while (this->rosNode->ok()){
				this->rosQueue.callAvailable(ros::WallDuration(timeout));
			}
		}

		/// \brief Pointer to the model.
		private: physics::ModelPtr model;

		/// \brief Pointer to the joint.
		private: std::vector<physics::JointPtr> joints_vector;

		private: int n_joints;

		/// \brief A PID controller for the joint.
		private: std::vector<common::PID> pid_vector;

		/// \brief A node use for ROS transport
		private: std::unique_ptr<ros::NodeHandle> rosNode;

		/// \brief A ROS subscriber
		private: std::vector<ros::Subscriber> rosSub_vector;

		/// \brief A ROS callbackqueue that helps process messages
		private: ros::CallbackQueue rosQueue;

		/// \brief A thread the keeps running the rosQueue
		private: std::thread rosQueueThread;
	};

	// Tell Gazebo about this plugin, so that Gazebo can call Load on this plugin.
	GZ_REGISTER_MODEL_PLUGIN(VT_simPlugin)
}
#endif
