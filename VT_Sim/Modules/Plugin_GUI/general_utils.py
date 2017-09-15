import json, os, yaml
from pprint import pprint as pp
from datetime import datetime
import itertools

def get_date_str():
	return str(datetime.now()).replace(' ', '_').replace(':', '.')[:-10]

def print_error_msg(msg):
	print '\n'*3
	print '-'*30
	print msg
	print '-'*30
	print '\n'*3

def reverse_enumerate(iterable):
	return itertools.izip(reversed(xrange(len(iterable))), reversed(iterable))


def get_dict_from_file(filename, handler):
	# print 'Using file', filename
	if os.path.isfile(filename):
		with open(filename) as f:
			dic = handler.load(f)
			f.close()
		return dic
	else:
		print_error_msg('File dos not exist!')
		return {}

def get_yaml_dict(filename):
	return get_dict_from_file(filename, yaml)

def get_json_dict(filename):
	return get_dict_from_file(filename, json)

def set_new_var(var_dict):
	var = lambda: None
	for param in var_dict:
		if type(var_dict[param]) == type({}):
			var.__dict__[param] = set_new_var(var_dict[param])
		else:
			var.__dict__[param] = var_dict[param]
	return var

def pprint_var(var, name=None, tab=0):
	if name is not None:
		print '\t'*max(tab-1,0), name + ':'
	for param in var.__dict__:
		if type(var.__dict__[param]) == type(lambda: None):
			pprint_var(var.__dict__[param], param, tab+1)
		else:
			print '\t'*tab, param + ':',
			pp(var.__dict__[param])
	if name is None:
		print '\n\n'

def fillEmptyArgs(var, configMap):
	for param in configMap:
		if not(hasattr(args, param)):
			var.__dict__[param] = configMap[param] 
		elif var.__dict__[param] is None:
			var.__dict__[param] = configMap[param]

def str2bool(v):
  #susendberg's function
  return v.lower() in ("yes", "true", "t", "1")

def save_json_dict(dic, filename):
	print 'Saving file', filename
	if os.path.isfile(filename):
		dt = get_json_dict(filename)
		dic.update(dt)
	with open(filename, 'w') as f:
		json.dump(dic, f)
		f.close()

def save_yaml_dict(dic, filename):
	print 'Saving file', filename
	if os.path.isfile(filename):
		dt = get_yaml_dict(filename)
		dic.update(dt)
	with open(filename, 'w') as f:
		yaml.dump(dic, f)
		f.close()
