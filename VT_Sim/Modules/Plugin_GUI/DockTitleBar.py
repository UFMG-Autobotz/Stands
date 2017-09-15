#!/usr/bin/env python
# -*- coding: utf-8 -*-

from PyQt4 import QtCore
from PyQt4 import QtGui

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