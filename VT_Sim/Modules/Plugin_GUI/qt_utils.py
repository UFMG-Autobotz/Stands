#!/usr/bin/env python
# -*- coding: utf-8 -*-

from PyQt4 import QtGui

def qt_Line(orient, tipo): # HLine, VLine / Plain, Raised, Sunken 
	linha = QtGui.QFrame()
	linha.setFrameShape( [QtGui.QFrame.HLine, QtGui.QFrame.VLine][orient] )
	linha.setFrameShadow( [QtGui.QFrame.Plain, QtGui.QFrame.Raised, QtGui.QFrame.Sunken][tipo] ) 
	return linha