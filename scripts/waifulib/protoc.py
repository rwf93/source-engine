from waflib.Task import Task
from waflib.TaskGen import extension 

"""
A simple tool to integrate protocol buffers into your build system.

    def configure(conf):
        conf.load('compiler_cxx cxx protoc')

    def build(bld):
        bld(
                features = 'cxx cxxprogram'
                source   = 'main.cpp file1.proto proto/file2.proto', 
                include  = '. proto',
                target   = 'executable') 

"""

class protoc(Task):
	run_str = '${PROTOC} --proto_path=${SRC[0].parent.abspath()} --cpp_out=${SRC[0].parent.relpath()} ${PROTOC_FLAGS} ${PROTOC_ST:INCPATHS} ${SRC[0].abspath()}'
	color   = 'BLUE'
	ext_out = ['.h', 'pb.cc']

@extension('.proto')
def process_protoc(self, node):
	cpp_node = node.change_ext('.pb.cc')
	hpp_node = node.change_ext('.pb.h')
	self.create_task('protoc', node, [cpp_node, hpp_node])
	self.source.append(cpp_node)

	use = getattr(self, 'use', '')
	if not 'PROTOBUF' in use:
		self.use = self.to_list(use) + ['PROTOBUF']

def configure(conf):
	conf.check_cfg(package="protobuf", uselib_store="PROTOBUF", args=['--cflags', '--libs'])
	conf.find_program('protoc', var='PROTOC')
	conf.env.PROTOC_ST = '-I%s'

