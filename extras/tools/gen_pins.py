#!/usr/bin/env python
from __future__ import print_function

from glob import glob
from os.path import basename, dirname, exists, join
import os
import re
import sys

def usage(msg):
	if msg:
		print(msg)
		print('')

	print('%s: PATH' % sys.argv[0])
	sys.exit(1)


errors = 0

def error(msg):
	print('Error:', msg, file=sys.stderr)
	global errors
	errors += 1

sam_pin_declaration_start = 'const PinDescription g_APinDescription'
sam_pin_declaration_end = '}'


def read_sam_variant(variant_file):
	print('reading %s' % variant_file)
	reading = False
	pins = []
	pin_num = 0
	pin_re = re.compile('{(.*)}')
	data_re = re.compile('\s*(?:PORT|PIO)([A-L]),\s*(?:PIO_P[A-F])?([0-9]+)')

	with open(variant_file) as f:
		for line in f.readlines():
			if line.strip().startswith('//'):
				continue
			if sam_pin_declaration_start in line:
				reading = True
			elif line.startswith(sam_pin_declaration_end):
				reading = False
			elif reading:
				# first look for the brackets, which indicate pin data
				# then look for the port/pin info contained within.
				# Some SAMD variants have empty brackets which indicate a
				# pin number that cannot be used.
				m = pin_re.search(line)
				if m:
					data = m.group(1)
					m = data_re.match(data)
					if m:
						pins.append((pin_num, m.groups()))
					pin_num += 1

	if not pins:
		error('no pins found in %s' % variant_file)
	return pins


avr_port_declaration_start = 'const uint8_t PROGMEM digital_pin_to_port_PGM['
avr_pin_declaration_start = 'const uint8_t PROGMEM digital_pin_to_bit_mask_PGM['
avr_declaration_end = '};'


def read_avr_variant(variant_file):
	print('reading %s' % variant_file)
	reading_ports = False
	reading_pins = False
	found_include = False
	ports = []
	pins = []
	pin_num = 0
	port_re = re.compile('\s*P(?:ORT_)?([A-L])')
	pin_re = re.compile('\s*_BV\(\s*([0-9]+)\s*\)')
	include_re = re.compile('#include "(\.\./[^/]+/pins_arduino.h)"')

	if variant_file.endswith('pins_arduino.c'):
		print('Warning: check generated pins for %s; multiple MCU variants may be combined' % variant_file)

	with open(variant_file) as f:
		for line_num, line in enumerate(f.readlines()):
			if reading_ports:
				if line.startswith(avr_declaration_end):
					reading_ports = False
				
				m = port_re.match(line)
				if m:
					ports.append(m.group(1))
			elif reading_pins:
				if line.startswith(avr_declaration_end):
					reading_pins = False
				
				m = pin_re.match(line)
				if m:
					pins.append(m.group(1))

			elif line.startswith(avr_port_declaration_start):
				reading_ports = True

			elif line.startswith(avr_pin_declaration_start):
				reading_pins = True
			else:
				m = include_re.match(line)
				if m:
					found_include = True
					target = m.group(1)
					print('%s -> %s' % (variant_file, target))
					return read_avr_variant(join(dirname(variant_file), target))

		if len(ports) != len(pins):
			error('ports/pins mismatch in %s (%d ports, %d pins)' % (variant_file, len(ports), len(pins)))
			return []

		if not pins and not found_include:
			error('no pins or include reference found in %s' % variant_file)
			return []

	return list(enumerate(zip(ports, pins)))


def generate_header(variant_name, variant_type, pins):
	filename = join('include', 'boards', variant_type, variant_name + '.h')
	try:
		os.makedirs(dirname(filename))
	except OSError:
		pass

	if pins:
		with open(filename, 'w') as f:
			print('// DirectIO support for %s\n' % variant_name, file=f)

			for pin_num, (port, port_pin) in pins:
				print('_define_pin(%d, PORT_%s, %s);' % (pin_num, port, port_pin), file=f)
		print('generated %s (%d pins)' % (filename, len(pins)))


def main(argv):
	base_path = argv[1]	
	if not exists(base_path):
		usage("Can't find base path: %s" % base_path)

	hardware_path = join(base_path, 'packages', 'arduino', 'hardware')
	if not exists(hardware_path):
		usage("Can't find hardware at: %s" % hardware_path)

	# iterate over all hardware_type/version
	# SAM variants use definitions in variants.cpp
	variants = {}

	pattern = join(base_path, 'packages', '*', 'hardware', 'sam*', '*', 'variants', '*', 'variant.cpp')
	variants.update({v: read_sam_variant(v) for v in glob(pattern)})

	pattern = join(base_path, 'packages', '*', 'hardware', 'avr', '*', 'variants', '*', 'pins_arduino.*')
	variants.update({v: read_avr_variant(v) for v in glob(pattern)})

	for path, pins in variants.items():
		variant_name = basename(dirname(path))
		variant_type = basename(dirname(dirname(dirname(dirname(path)))))
		generate_header(variant_name, variant_type, pins)

if __name__ == '__main__':
	main(sys.argv)
	sys.exit(errors)
