#!/usr/bin/env python
# -*- coding: utf-8 -*-

import rospy
from std_msgs.msg import Float32

import numpy as np

from PyQt4 import QtCore
from PyQt4 import QtGui

from general_utils import get_yaml_dict
from qt_utils import qt_Line

class Sliders_Window(QtGui.QWidget):
	def __init__(self, groups_file, parent = None):
		super(Sliders_Window, self).__init__(parent)
		self.setAttribute(QtCore.Qt.WA_DeleteOnClose)
		specs = get_yaml_dict(groups_file)
		self.init_groups(specs)
		self.create_window()

	def init_groups(self, specs):
		self.groups = []
		self.pubs = []
		for key in sorted(specs.keys()):
			self.groups.append(specs[key])
			self.pubs.append([])
			for topic_group in self.groups[-1]['topicos']:
				self.pubs[-1].append([])
				for topic_name in topic_group:
					self.pubs[-1][-1].append(rospy.Publisher(topic_name, Float32, queue_size=1))

	def change_value(self, topic, slider, tipo, info):
		_min, _max, ticks = info
		slider_value = float(self.sliders[ topic ][ slider ].value())

		if tipo == 'int':
			if ticks > _max-_min:
				value = int(_min+slider_value)
			else:
				value = int(_min + (_max-_min)*slider_value/ticks)
		else:
			value = round(_min + (_max-_min)*slider_value/ticks, 3)

		self.current_values[ topic, slider ] = value
		self.display_values[ topic ][ slider ].setText(str(value))

		if self.radio_buttons[topic][slider][1].isChecked() and self.keep_publish[topic][slider] == True:
			self.publish(topic, slider)
		else:
			self.keep_publish[topic][slider] = False

	def enterPress(self, topic, slider, tipo, info):
		text_value = self.display_values[ topic ][ slider ].text()
		_min, _max, ticks = info

		try:
			value = round(float(text_value), 3)
			value = min(_max, value)
			value = max(_min, value)

			if tipo == 'int':
				value = int(value)

			self.sliders[ topic ][ slider ].setValue( int((value-_min)*ticks/(_max-_min)) )
			self.current_values[ topic, slider ] = value
			self.display_values[ topic ][ slider ].setText(str(value))

			if self.radio_buttons[topic][slider][1].isChecked() == False and self.keep_publish[topic][slider] == True:
				self.keep_publish[topic][slider] = False
		except:
			pass

	def publish(self, topic, slider):
		if self.radio_buttons[topic][slider][1].isChecked():
			self.keep_publish[topic][slider] = True
		for pub in self.pubs[ topic ][ slider ]:
			pub.publish( self.current_values[ topic, slider ] )

	def create_window(self):
		layout = QtGui.QHBoxLayout()
		self.current_values = np.zeros((len(self.groups), 1))
		self.display_values = []
		self.sliders = []
		self.pub_buttons = []
		self.radio_buttons = []
		self.radio_buttons_groups = []
		self.keep_publish = []

		for k, group in enumerate(self.groups):
			group_vbox = QtGui.QVBoxLayout()

			label = QtGui.QLabel(group['nome'])
			label.setAlignment(QtCore.Qt.AlignCenter)

			n_sliders = len(group['sliders'])
			diff = n_sliders - self.current_values.shape[1]
			if diff > 0:
				self.current_values = np.column_stack( ( self.current_values, np.zeros( ( len(self.groups), diff ) ) ) )

			self.display_values.append([])
			self.sliders.append([])
			self.pub_buttons.append([])
			self.radio_buttons.append([])
			self.radio_buttons_groups.append([])
			self.keep_publish.append([])

			sliders_hbox = QtGui.QHBoxLayout()
			for i in xrange(n_sliders):
				value_vbox = QtGui.QVBoxLayout()

				slider_label = QtGui.QLabel(group['sliders'][i])
				slider_label.setAlignment(QtCore.Qt.AlignCenter)

				self.sliders[-1].append(QtGui.QSlider(QtCore.Qt.Vertical))
				_min = float(group['range'][i][0])
				_max = float(group['range'][i][1])
				ticks = float(group['ticks'])
				slider_info = [_min, _max, ticks]

				if group['tipo'][i] == 'int' and ticks > _max-_min:
					self.sliders[-1][-1].setMaximum(_max-_min)
				else:
					self.sliders[-1][-1].setMaximum(ticks)

				self.sliders[-1][-1].setMinimum(0)
				self.sliders[-1][-1].setValue(0)
				self.sliders[-1][-1].setTickPosition(QtGui.QSlider.TicksBelow)
				self.sliders[-1][-1].setTickInterval(1)
				self.sliders[-1][-1].valueChanged.connect( lambda _, topic=k, slider=i, tipo=group['tipo'][i], info=slider_info: self.change_value(topic, slider, tipo, info) )

				self.display_values[-1].append(QtGui.QLineEdit())
				self.display_values[-1][-1].setText(str(0))
				# self.display_values[-1][-1].setReadOnly(True)
				self.display_values[-1][-1].editingFinished.connect(lambda topic=k, slider=i, tipo=group['tipo'][i], info=slider_info: self.enterPress(topic, slider, tipo, info) )

				self.pub_buttons[-1].append(QtGui.QPushButton('Publish'))
				self.pub_buttons[-1][-1].clicked.connect( lambda _, topic=k, slider=i: self.publish(topic, slider) )

				self.keep_publish[-1].append(False)

				radio_hbox = QtGui.QHBoxLayout()
				self.radio_buttons_groups[-1].append(QtGui.QButtonGroup())

				self.radio_buttons[-1].append([QtGui.QRadioButton("Once"), QtGui.QRadioButton("Stream")])
				self.radio_buttons[-1][-1][0].setChecked(True)
				
				self.radio_buttons_groups[-1][-1].addButton(self.radio_buttons[-1][-1][0])
				self.radio_buttons_groups[-1][-1].addButton(self.radio_buttons[-1][-1][1])
				radio_hbox.addWidget(self.radio_buttons[-1][-1][0])
				radio_hbox.addWidget(self.radio_buttons[-1][-1][1])

				value_vbox.addWidget(slider_label)
				value_vbox.addWidget(self.display_values[-1][-1])
				value_vbox.addWidget(self.pub_buttons[-1][-1])
				value_vbox.addLayout(radio_hbox)
				value_vbox.addWidget(self.sliders[-1][-1])
				sliders_hbox.addLayout(value_vbox)

			group_vbox.addWidget(label)
			group_vbox.addLayout(sliders_hbox)
			layout.addLayout(group_vbox)
			if k+1 < len(self.groups):
				layout.addWidget( qt_Line(1,2) )

		self.setLayout(layout)