tests = [{'description': 'PINA: 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF => PORTB: 0x07, state: s3',
	'steps': [{'inputs': [('PINA', 0xFF)], 'iterations': 1},
        {'inputs': [('PINA', 0xFE)], 'iterations': 1},
	{'inputs': [('PINA', 0xFF)], 'iterations': 1},
	{'inputs': [('PINA', 0xFE)], 'iterations': 1},
	{'inputs': [('PINA', 0xFF)], 'iterations': 1},
	{'inputs': [('PINA', 0xFE)], 'iterations': 1},
        {'inputs': [('PINA', 0xFF)], 'iterations': 1}],
	'expected': [('PORTB',0x07)],
	},]

watch = ['state']
