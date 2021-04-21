tests = [{'description': 'PINA: 0xFC, 0xFE, 0xFF, 0xFE, 0xFF, 0xFD => PORTC: 0x01, state: wait_change',
	'steps': [{'inputs': [('PINA', 0xFC)], 'iterations': 1},
        {'inputs': [('PINA', 0xFE)], 'iterations': 1},
	{'inputs': [('PINA', 0xFF)], 'iterations': 1},
	{'inputs': [('PINA', 0xFE)], 'iterations': 1},
	{'inputs': [('PINA', 0xFF)], 'iterations': 1},
	{'inputs': [('PINA', 0xFD)], 'iterations': 1}],
	'expected': [('PORTC',0x01)],
	},]

watch = ['state']
