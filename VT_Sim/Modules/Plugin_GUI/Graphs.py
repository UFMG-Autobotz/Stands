#!/usr/bin/env python
# -*- coding: utf-8 -*-

import rospy
from std_msgs.msg import Float32

from PyQt4 import QtCore
from PyQt4 import QtGui

class Graphs_Window(QtGui.QWidget):
	def __init__(self, parent = None):
		super(Graphs_Window, self).__init__(parent)
		self.setAttribute(QtCore.Qt.WA_DeleteOnClose)
		self.update_topics()
		self.create_window()

	def update_topics(self):
		self.active_topics = sorted(rospy.get_published_topics(), key=lambda x: x[0])

	def highlighted_update(self,i):
		old_topics = self.active_topics
		self.update_topics()
		if old_topics != self.active_topics:
			for i, cb_group in enumerate(self.cb):
				for j, cb in enumerate(cb_group):
					self.cb[i][j].clear()
					for topic in self.active_topics:
						self.cb[i][j].addItem( topic[0] )

	def selectionchange(self,i):
		current_choice = str(self.graph_type_choice.currentText())
		if current_choice == 'Y vs t':
			self.stacked_windows.setCurrentIndex(0)
		elif current_choice == 'X vs Y':
			self.stacked_windows.setCurrentIndex(1)
		elif current_choice == 'X vs Y vs Z':
			self.stacked_windows.setCurrentIndex(2)

	def new_screen(self,n_cb):
		self.cb.append([])
		new_widget = QtGui.QWidget()
		layout = QtGui.QFormLayout()
		label_choice = [['Y'], ['X', 'Y'], ['X', 'Y', 'Z']][n_cb-1]
		for i in xrange(n_cb):
			new_cb = QtGui.QComboBox()
			for topic in self.active_topics:
				new_cb.addItem( topic[0] )
			new_cb.highlighted.connect(self.highlighted_update)
			self.cb[-1].append(new_cb)
			layout.addRow(label_choice[i],self.cb[-1][-1])
		new_widget.setLayout(layout)
		return new_widget

	def create_window(self):
		layout = QtGui.QHBoxLayout()
		label = QtGui.QLabel(u"Lugar onde serão exibidos gráficos plotados a partir de tópicos do ROS")
		label.setAlignment(QtCore.Qt.AlignCenter)

		self.selection_widget = QtGui.QWidget()
		choice_layout = QtGui.QVBoxLayout()

		self.cb = []

		self.graph_type_choice = QtGui.QComboBox()
		self.graph_type_choice.addItem('Y vs t')
		self.graph_type_choice.addItem('X vs Y')
		self.graph_type_choice.addItem('X vs Y vs Z')

		initial_choice = str(self.graph_type_choice.currentText())
		self.graph_type_choice.currentIndexChanged.connect(self.selectionchange)

		self.stacked_windows = QtGui.QStackedWidget(self)
		self.op_1 = self.new_screen(1)
		self.op_2 = self.new_screen(2)
		self.op_3 = self.new_screen(3)
		self.stacked_windows.addWidget (self.op_1)
		self.stacked_windows.addWidget (self.op_2)
		self.stacked_windows.addWidget (self.op_3)

		choice_layout.addWidget(self.graph_type_choice)
		choice_layout.addWidget(self.stacked_windows)
		self.selection_widget.setLayout(choice_layout)

		self.splitter = QtGui.QSplitter(QtCore.Qt.Horizontal)
		self.splitter.addWidget(self.selection_widget)
		self.splitter.addWidget(label)

		layout.addWidget(self.splitter)
		self.setLayout(layout)