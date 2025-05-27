import struct
# Forza.py - A simple parser for Forza config files

type_map = {
    "s32": ("<i", 4),
    "u32": ("<I", 4),
    "f32": ("<f", 4),
    "u16": ("<H", 2),
    "u8":  ("<B", 1),
    "s8":  ("<b", 1),
	"hzn":("<f", 12),  # Assuming 'hzn' is a float, adjust as needed
}

class Forza:
	def __init__(self, configFile):
		self.configFile = configFile
		self.config = self.load_config()

	
	def load_config(self):
		config = {}
		try:
			offset = 0
			count = 0
			with open(self.configFile, 'r') as file:
				for line in file:
					line = line.strip()
					count += 1
					if '//' in line:
						line = line.split('//')[0].strip()
					line = line.strip(';').strip()
					parts = line.split()
					typ, name = parts[0], parts[1]
					# if typ not in type_map:
						# for unknowns like hzn, just skip or assign 4 bytes f32
						# typ = "f32"
					fmt, size = type_map[typ]
					config[name.lower()] = {
						'format': fmt,
						'size': size,
						'offset': offset
					}
					print(f"Loaded field: {name} with format {fmt}, size {size}, offset {offset}")
					offset += size
			# print(f"Loaded {count} fields from config file.")
		except FileNotFoundError:
			print(f"Config file {self.configFile} not found.")
		return config
	
	def get_value(self, data, name):
		if name.lower() not in self.config:
			print(f"Field {name} not found in config.")
			return None
		field = self.config[name.lower()]
		fmt = field['format']
		offset = field['offset']
		size = field['size']
		
		if len(data) < offset + size:
			print(f"Data too short for field {name}. Expected at least {offset + size} bytes, got {len(data)} bytes.")
			return None
		
		value = struct.unpack_from(fmt, data, offset)[0]
		return value