#!/usr/bin/env python
# -*- coding: utf-8 -*-

import rospy
from std_msgs.msg import Float32
from PyQt4 import QtGui
from Main_Window import Main_Window
import sys

class Controller_App():
	def __init__(self, args, node_name = "joint_plugin_controller"):
		rospy.init_node(node_name, anonymous=True)
		self.app = QtGui.QApplication(args)
		self.app.setStyle("Plastique")
		self.main_window = Main_Window()

	def run(self):
		self.main_window.show()
		sys.exit(self.app.exec_())

if __name__ == '__main__':
	c = Controller_App(sys.argv)
	c.run()