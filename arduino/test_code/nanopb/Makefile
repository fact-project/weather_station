all: weathervalues.pb.c weathervalues.pb.h weathervalues_pb2.py

weathervalues_pb2.py: weathervalues.proto
	protoc -I. --python_out=. weathervalues.proto

weathervalues.pb.c weathervalues.pb.h: weathervalues.pb
	/Users/jebuss/Documents/Arduino/libraries/nanopb/generator/nanopb_generator.py \
		weathervalues.pb

weathervalues.pb: weathervalues.proto
	protoc -o weathervalues.pb weathervalues.proto
