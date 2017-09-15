#!/usr/bin/env python
# -*- coding: utf-8 -*-
import pyqtgraph as pg
import numpy as np
import random, time, math, collections
from pyqtgraph.Qt import QtGui, QtCore

def exp_smooth(x_new, x_old, alpha=0.85):
	return alpha * x_old + x_new * (1 - alpha)


class DynamicPlotter():
	def __init__(self, sampleinterval=0.01, timewindow=10., size=(600,350), title=''):
		# Data stuff
		self._interval = int(sampleinterval*1000)
		self._bufsize = int(timewindow/sampleinterval)
		self.databuffer = collections.deque([0.0]*self._bufsize, self._bufsize)
		self.givedata_buffer = None
		self.x = np.linspace(-timewindow, 0.0, self._bufsize)
		self.y = np.zeros(self._bufsize, dtype=np.float)

		# PyQtGraph stuff
		self.app = QtGui.QApplication([])
		self.title = 'Dynamic Plotting with PyQtGraph' if title == '' else title
		self.win = pg.GraphicsWindow()
		self.plt = self.win.addPlot(title=self.title)
		self.plt.resize(*size)
		self.plt.showGrid(x=True, y=True)
		self.plt.setLabel('left', 'amplitude', 'V')
		self.plt.setLabel('bottom', 'time', 's')
		self.curve = self.plt.plot(self.x, self.y, pen=(255,0,0))

		# QTimer
		self.timer = QtCore.QTimer()
		self.timer.timeout.connect(self.updateplot)
		self.timer.start(self._interval)

	def getdata(self):
		frequency = 0.5
		noise = random.normalvariate(0., 1.)
		new = 10.*math.sin(time.time()*frequency*2*math.pi) + noise
		return new

	def givedata(self, data):
		self.givedata_buffer = data

	def updateplot(self):
		# If we haven't received data to givedata_buffer
		if self.givedata_buffer == None:

			# Get your data from self.getdata()
			self.databuffer.append( self.getdata() )
		else:
			self.databuffer.append( self.givedata_buffer )

		self.y[:] = self.databuffer
		self.curve.setData(self.x, self.y)
		self.app.processEvents()

	def run(self):
		self.app.exec_()

	def data_wrapper(self, func, *args, **kwargs):
		def wrapped(*args, **kwargs):
			while True:
				res = func(*args, **kwargs)
				time.sleep(float(self._interval) / 1000)
				self.givedata(res)

		return wrapped

class DynamicPlotterNumpy(DynamicPlotter):
	def __init__(self, sampleinterval=0.01, timewindow=2, title="Phone data plotting"):
		DynamicPlotter.__init__(self, sampleinterval=sampleinterval, timewindow=timewindow, title=title)

		# 3 acceleration buffers for phone
		self.databuffer_A = np.zeros((3, self._bufsize))
		self.databuffer_G = np.zeros((3, self._bufsize))

		# Plotting buffers
		self.n_curves = 15
		self.plotData = np.zeros((self.n_curves, self._bufsize), dtype=np.float)

		self.curve_colors = [(255, 255, 255), (0, 255, 0), (0, 0, 255), (255, 255, 255), (0, 255, 0),
							 (0, 0, 255), (0, 255, 255), (0, 255, 255), (255, 0, 155), (100, 255, 0), (100, 100, 100),
							 (0, 255, 255), (255, 0, 155), (100, 255, 0), (100, 100, 100)]

		self.plot_lines = [self.plt.plot(x=self.x, y=self.plotData[i, :], pen=self.curve_colors[i % len(self.curve_colors)]) for i in range(self.n_curves)]

		# Plot settings
		self.plt.setYRange(-4, 4, padding=0.05)

		# Convenience
		self.frames_since_start = 0

		# For smoothing data coming in
		self.last_data = np.zeros(3)

		self.givedata_buffer = None

	def updateplot(self):
		if self.givedata_buffer is not None:
			data_vec  = self.givedata_buffer	# Get the data from buffer variable
			# Pad with zeros if you have more lines than we were given data
			data_vec = \
				np.array([data_vec[i] if i < len(data_vec) else 0 \
						  for i in range(self.n_curves)])
			self.plotData = np.hstack((self.plotData, np.array(data_vec).reshape((-1, 1))))
			self.plotData = self.plotData[:, -self._bufsize:]
		else:
			pass
		for i in range(self.n_curves):
			self.plot_lines[i].setData(self.x, self.plotData[i, -self._bufsize:])

		self.frames_since_start += 1

		self.app.processEvents()