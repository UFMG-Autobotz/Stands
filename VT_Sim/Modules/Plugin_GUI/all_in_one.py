#!/usr/bin/env python
# -*- coding: utf-8 -*-

import rospy
from std_msgs.msg import Float32

import sys
import numpy as np

from PyQt4 import QtCore
from PyQt4 import QtGui
from general_utils import get_yaml_dict

def qt_Line(orient, tipo): # HLine, VLine / Plain, Raised, Sunken
	linha = QtGui.QFrame()
	linha.setFrameShape( [QtGui.QFrame.HLine, QtGui.QFrame.VLine][orient] )
	linha.setFrameShadow( [QtGui.QFrame.Plain, QtGui.QFrame.Raised, QtGui.QFrame.Sunken][tipo] )
	return linha

class Controller_App():
	def __init__(self, args, node_name = "joint_plugin_controller"):
		rospy.init_node(node_name, anonymous=True)
		self.app = QtGui.QApplication(args)
		self.app.setStyle("Plastique")
		self.main_window = Main_Window()

	def run(self):
		self.main_window.show()
		sys.exit(self.app.exec_())

class Main_Window(QtGui.QMainWindow):
	def __init__(self, parent = None):
		super(Main_Window, self).__init__(parent)
		self.setAttribute(QtCore.Qt.WA_DeleteOnClose)

		self.setTabPosition( QtCore.Qt.TopDockWidgetArea , QtGui.QTabWidget.North )
		self.setDockOptions( QtGui.QMainWindow.AllowNestedDocks )
		self.setDockOptions( QtGui.QMainWindow.AllowTabbedDocks )
		# self.setDockOptions( QtGui.QMainWindow.ForceTabbedDocks )

		self.slider_groups_file = 'topics_vt.yaml'

		self.my_menu = self.menuBar()

		# self.file_menu = self.my_menu.addMenu("File")
		# self.exit_action = QtGui.QAction('Exit', self)
		# self.exit_action.triggered.connect(exit)
		# self.file_menu.addAction(exit_action)

		self.sliders_window = self.my_menu.addMenu("Controller")
		self.sliders_window.addAction("Nova aba")
		self.sliders_window.triggered[QtGui.QAction].connect(self.controller_action)

		self.graphs_window = self.my_menu.addMenu("Graph Plotter")
		self.graphs_window.addAction("Nova aba")
		self.graphs_window.triggered[QtGui.QAction].connect(self.graph_Plotter_action)

		self.image_window = self.my_menu.addMenu("Image Viewer")
		self.image_window.addAction("Nova aba")
		self.image_window.triggered[QtGui.QAction].connect(self.image_Viewer_action)

		self.dockList = []
		self.dockTitleList = []

		w = 900; h = 600
		self.resize(w, h)

		self.setWindowTitle("Main Window")

	def new_Dock(self, name):
		dock = QtGui.QDockWidget(name, self)
		dock.setAllowedAreas(QtCore.Qt.TopDockWidgetArea)
		dock.setAttribute(QtCore.Qt.WA_DeleteOnClose)
		dock_title = DockTitleBar(dock)
		self.addDockWidget(QtCore.Qt.TopDockWidgetArea, dock)
		self.dockList.append( dock )

		# if len(self.dockList) > 1:
		# 	self.tabifyDockWidget(self.dockList[-2],self.dockList[-1])

	def controller_action(self,q):
		if q.text() == 'Nova aba':
			self.new_Dock('Slider')
			self.dockList[-1].setWidget(Sliders_Window(self.slider_groups_file, self.dockList[-1]))

	def graph_Plotter_action(self,q):
		if q.text() == 'Nova aba':
			self.new_Dock('Graph')
			self.dockList[-1].setWidget(Graphs_Window(self.dockList[-1]))

	def image_Viewer_action(self,q):
		if q.text() == 'Nova aba':
			self.new_Dock('Image')
			self.dockList[-1].setWidget(Image_Window(self.dockList[-1]))

class DockTitleBar(QtGui.QWidget):
	def __init__(self, dockWidget):
		super(DockTitleBar, self).__init__(dockWidget)
		self.setAttribute(QtCore.Qt.WA_DeleteOnClose)

		boxLayout = QtGui.QHBoxLayout(self)
		boxLayout.setSpacing(1)
		boxLayout.setMargin(1)

		self.titleEdit = QtGui.QLineEdit(self)
		self.titleEdit.hide()
		self.titleEdit.editingFinished.connect(self.finishEdit)

		boxLayout.addSpacing(2)
		boxLayout.addWidget(self.titleEdit)
		boxLayout.addSpacing(5)
		self.titleEdit.setText(dockWidget.windowTitle())

		dockWidget.installEventFilter(self)

	def eventFilter(self, source, event):
		if event.type() == QtCore.QEvent.WindowTitleChange:
			self.titleEdit.setText(source.windowTitle())
		return super(DockTitleBar, self).eventFilter(source, event)

	def startEdit(self):
		self.titleEdit.show()
		self.titleEdit.setFocus()

	def finishEdit(self):
		self.titleEdit.hide()
		self.parent().setWindowTitle(self.titleEdit.text())

	def mouseDoubleClickEvent(self, event):
		if event.pos().x() <= self.titleEdit.width():
			self.startEdit()
		else:
			super(DockTitleBar, self).mouseDoubleClickEvent(event)

	def mouseReleaseEvent(self, event):
		event.ignore()

	def mousePressEvent(self, event):
		event.ignore()

	def mouseMoveEvent(self, event):
		event.ignore()

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

class Image_Window(QtGui.QWidget):
	def __init__(self, parent = None):
		super(Image_Window, self).__init__(parent)
		self.setAttribute(QtCore.Qt.WA_DeleteOnClose)
		layout = QtGui.QHBoxLayout()
		label = QtGui.QLabel(u"Lugar onde serão exibidas imagens vindas da câmera ou Visão")
		label.setAlignment(QtCore.Qt.AlignCenter)
		layout.addWidget(label)
		self.setLayout(layout)

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

if __name__ == '__main__':
	c = Controller_App(sys.argv)
	c.run()
