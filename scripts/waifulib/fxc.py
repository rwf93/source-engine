# This code is really stinky at times. You are warned.

import re
import os

from waflib.Task import Task
from waflib.TaskGen import extension
from waflib.Configure import conf

re_ps2x = re.compile(r'_ps2x')
re_vsxx = re.compile(r'_vsxx')
re_shader = re.compile(r'_(ps11|ps14|ps2x|ps20b|ps20|ps30|ps40|vs11|vs14|vsxx|vs20b|vs20|vs30|vs40)')
re_static_match = re.compile(r'\s*STATIC\s*\:\s*\"(.*)\"\s+\"(\d+)\.\.(\d+)\"')
re_dynamic_match = re.compile(r'\s*DYNAMIC\s*\:\s*\"(.*)\"\s+\"(\d+)\.\.(\d+)\"')
re_model_match = re.compile(r'\[([ps11|ps20b|ps20|ps30|ps40|vs11|vs20|vs20b|vs30|vs40]+)\]')
re_assignment_match = re.compile(r'\[\=(.*?)\]')
re_platform_match = re.compile(r'\[(XBOX|PC)\]')

class ShaderDefine:
    def __init__(self, name: str, min_value: int, max_value: int, assignment: str | None):
        self.name = name
        self.min_value = min_value
        self.max_value = max_value
        self.assignment = assignment

    name: str
    min_value: int
    max_value: int
    assignment: str | None

class ShaderParser:
    def __init__(self, node, shader_model, shader_platform):
        self.model = shader_model

        self.static_define_map = []
        self.dynamic_define_map = []

        for line in node.read().replace('\r', '').split('\n'):
            line = re.sub(r'^\s*//\s*', '', line) # Strip C style comments
            line = re.sub(r'^\s*#\s*', '', line) # Strip HLSL directives
            line = re.sub(r'^\s*;\s*', '', line) # Strip HLSL comments

            static_match = re_static_match.match(line)
            dynamic_match = re_dynamic_match.match(line)

            if static_match:
                name, min_val, max_val = static_match.groups()
                model = re_model_match.findall(line)
                platform = re_platform_match.search(line).group(1) if re_platform_match.search(line) is not None else shader_platform

                if not shader_model in model and len(model) > 0:
                    continue

                if platform != shader_platform:
                    continue

                self.static_define_map.append(
                    ShaderDefine(
                        name,
                        int(min_val),
                        int(max_val),
                        re_assignment_match.search(line)
                    )
                )

            if dynamic_match:
                name, min_val, max_val = dynamic_match.groups()
                model = re_model_match.findall(line)
                platform = re_platform_match.search(line).group(1) if re_platform_match.search(line) is not None else shader_platform

                if not shader_model in model and len(model) > 0:
                    continue

                if platform != shader_platform:
                    continue

                self.dynamic_define_map.append(
                    ShaderDefine(
                        name,
                        int(min_val),
                        int(max_val),
                        re_assignment_match.search(line)
                    )
                )

    model: str

    static_define_map: list[ShaderDefine]
    dynamic_define_map: list[ShaderDefine]

def emit_members_code(define_map: ShaderDefine) -> str:
    source = ''
    for define in define_map:
        source += f'int m_n{define.name};\n'
        source += f'void Set{define.name}( int i ) {{ Assert( i >= {define.min_value} && i <= {define.max_value} ); m_n{define.name} = i; }}\n'
    return source

def emit_assignment_code(define_map: ShaderDefine) -> str:
    source = ''
    for define in define_map:
        source += f'm_n{define.name} = {define.assignment.group(1).lstrip(" ") if define.assignment is not None else "0"};\n'
    return source

def emit_index_code(define_map: ShaderDefine, scale: int) -> str:
    source = 'int GetIndex() { return '
    for define in define_map:
        source += f'( {scale} * m_n{define.name} ) + '
        scale *= define.max_value - define.min_value + 1
    source += "0; }\n"
    return source

def emit_cpp_code(parser: ShaderParser, name: str):
    shader_type = parser.model[0:2] + 'h'
    source = '#include "shaderlib/cshader.h"\n'

    # Static index class
    source += f'class {name.lower()}_Static_Index {{\n'
    source += 'public:\n'

    source += emit_members_code(parser.static_define_map)

    source += f'{name.lower()}_Static_Index() {{\n'
    source += emit_assignment_code(parser.static_define_map)
    source += "}\n"

    static_scale = 1
    # Static scale is offset by the dynamic scaling
    for dynamic in parser.dynamic_define_map:
        static_scale *= dynamic.max_value - dynamic.min_value + 1

    source += emit_index_code(parser.static_define_map, static_scale)

    source += '};\n'

    source += f"#define shaderStaticTest_{name.lower()} "
    for static in parser.static_define_map:
         if static.assignment is None:
            source += f"{shader_type}_forgot_to_set_static_{static.name} + "
    source += '0\n'

    # Dynamic index class
    source += f'class {name.lower()}_Dynamic_Index {{\n'
    source += 'public:\n'

    source += emit_members_code(parser.dynamic_define_map)

    source += f'{name.lower()}_Dynamic_Index() {{\n'
    source += emit_assignment_code(parser.dynamic_define_map)
    source += "}\n"

    dynamic_scale = 1
    source += emit_index_code(parser.dynamic_define_map, dynamic_scale)
    source += '};\n'

    source += f"#define shaderDynamicTest_{name.lower()} "
    for dynamic in parser.dynamic_define_map:
         if dynamic.assignment is None:
            source += f"{shader_type}_forgot_to_set_dynamic_{dynamic.name} + "
    source += '0\n'

    return source

COMPILER_MAPPING = {
    '.fxc': os.path.abspath('./dx9sdk/utilities/fxc.exe'),
    '.vsh': 'vsa.exe',
    '.psh': 'psa.exe'
}

SHADER_TYPE = {
    'vs11': 'vs_1_1',
    'vs14': 'vs_1_4',
    'vs20': 'vs_2_0',
    'vs20b': 'vs_2_0',
    'vs30': 'vs_3_0',

    'ps11': 'ps_1_1',
    'ps14': 'ps_1_4',
    'ps20': 'ps_2_0',
    'ps20b': 'ps_2_b',
    'ps30': 'ps_3_0',
}

MODEL_MAPPING = {
    # Fuck you valve.
    'ps2x': ['ps20', 'ps20b'],
    'vsxx': ['vs11', 'vs20'],

    'vs11': ['vs11'],
    'vs20b': ['vs20b'],
    'vs20': ['vs20'],
    'vs30': ['vs30'],
    'vs40': ['vs40'],

    'ps11': ['ps11'],
    'ps20b': ['ps20b'],
    'ps20': ['ps20'],
    'ps30': ['ps30'],
    'ps40': ['ps40'],
}

def get_models_from_name(filename, override: int):
    if not re_shader.search(filename):
        return {}

    model = re_shader.search(filename).group(1)

    if override is not None:
        if 'vs' in model:
            return {model: [f'vs{override}']}
        if 'ps' in model:
            return {model: [f'ps{override}']}

    for key, models in MODEL_MAPPING.items():
        if key in model:
            return {key: models}

    return {}

def calculate_combo_count(combo_list: list[ShaderDefine]):
    count = 1
    for define in combo_list:
        count *= define.max_value - define.min_value + 1

    return count

class fxc(Task):
    before = ['c', 'cxx']

    def run(self):
        parser = ShaderParser(self.inputs[0], self.shader_model, 'PC')
        self.outputs[0].write(emit_cpp_code(parser, self.outputs[0].name.rsplit('.', maxsplit=1)[0])) # Write .inc

        executable = COMPILER_MAPPING[self.inputs[0].suffix()]
        if self.inputs[0].suffix() == '.fxc':
            arguments = []
            for static in parser.static_define_map:
                arguments.append(f'/D{static.name}')

            for dynamic in parser.dynamic_define_map:
                arguments.append(f'/D{dynamic.name}')

            #arguments += [
            #    f'/DTOTALSHADERCOMBOS={calculate_combo_count(parser.dynamic_define_map) + calculate_combo_count(parser.static_define_map)}', 
            #    f'/DCENTROIDMASK=0',
            #    f'/DNUMDYNAMICCOMBOS={calculate_combo_count(parser.dynamic_define_map)}',
            #    f'/DFLAGS=0x0',
            #    f'/Dmain=main',
            #    f'/Emain',
            #    f'/T{SHADER_TYPE[self.shader_model]}',
            #    f'/DSHADER_MODEL_{SHADER_TYPE[self.shader_model].upper()}=1',
            #    f'/nologo',
            #    f'/Fo{self.outputs[1]}',
            #    f'{self.inputs[0]}'
            #]
            #self.exec_command(f'{executable} {" ".join(arguments)}')
        else:
            pass

        self.outputs[1].write('TODO') # Write .inc

    color = 'GREEN'
    ext_out = ['.inc', '.vsc']

@extension('.fxc')
def process_fxc(self, node):
    fxctmp9_node = node.parent.make_node('fxctmp9').make_node(node.name)

    override = None
    try:
        # TODO(rwf93): Make this be setable through BuildContext
        if self.FORCE_MODEL:
            override = self.FORCE_MODEL
    except:
        pass

    for key, models in get_models_from_name(fxctmp9_node.name, override).items():
        for model in models:
            inc_node = fxctmp9_node.parent.make_node(node.name.replace(key, model)).change_ext('.inc')
            vsc_node = fxctmp9_node.parent.make_node(node.name.replace(key, model)).change_ext('.vsc')
            self.create_task('fxc', node, [inc_node, vsc_node]).shader_model = model

@extension('.vsh')
def process_vsh(self, node):
    vshtmp9_node = node.parent.make_node('vshtmp9').make_node(node.name)

    shader_model = ''
    if re_shader.search(node.name):
        shader_model = re_shader.search(node.name).group(1)

    inc_node = vshtmp9_node.change_ext('.inc')
    vsc_node = vshtmp9_node.change_ext('.vsc')
    self.create_task('fxc', node, [inc_node, vsc_node]).shader_model = shader_model

@extension('.psh')
def process_psh(self, node):
    pshtmp9_node = node.parent.make_node('pshtmp9').make_node(node.name)

    shader_model = ''
    if re_shader.search(node.name):
        shader_model = re_shader.search(node.name).group(1)

    inc_node = pshtmp9_node.change_ext('.inc')
    vsc_node = pshtmp9_node.change_ext('.vsc')
    self.create_task('fxc', node, [inc_node, vsc_node]).shader_model = shader_model

def configure(conf):
    return