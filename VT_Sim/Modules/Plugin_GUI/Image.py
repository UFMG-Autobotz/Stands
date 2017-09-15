#!/usr/bin/env python
# -*- coding: utf-8 -*-

from PyQt4 import QtCore
from PyQt4 import QtGui

class Image_Window(QtGui.QWidget):
	def __init__(self, parent = None):
		super(Image_Window, self).__init__(parent)
		self.setAttribute(QtCore.Qt.WA_DeleteOnClose)
		layout = QtGui.QHBoxLayout()
		label = QtGui.QLabel(u"Lugar onde serão exibidas imagens vindas da câmera ou Visão")
		label.setAlignment(QtCore.Qt.AlignCenter)
		layout.addWidget(label)
		self.setLayout(layout)