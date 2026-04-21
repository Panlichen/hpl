#!/usr/bin/env python3
import os
import json
import glob

# 项目根目录
project_root = os.path.abspath('.')
# 包含目录
inc_dir = os.path.join(project_root, 'include')
# 假设使用默认架构
arch = 'UNKNOWN'

# 编译命令模板
compile_command = 'cc -I{inc_dir} -I{inc_arch_dir} -c {{file}} -o {{file}}.o'.format(
    inc_dir=inc_dir,
    inc_arch_dir=os.path.join(inc_dir, arch)
)

# 收集所有C源文件
c_files = []
for root, dirs, files in os.walk(project_root):
    # 跳过.git目录
    if '.git' in root:
        continue
    for file in files:
        if file.endswith('.c'):
            c_files.append(os.path.join(root, file))

# 生成compile_commands.json内容
compile_commands = []
for file in c_files:
    # 计算相对路径
    rel_path = os.path.relpath(file, project_root)
    # 构建编译命令
    cmd = compile_command.format(file=rel_path)
    # 添加到列表
    compile_commands.append({
        'directory': project_root,
        'command': cmd,
        'file': file
    })

# 写入文件
with open('compile_commands.json', 'w') as f:
    json.dump(compile_commands, f, indent=2)

print(f'Generated compile_commands.json for {len(compile_commands)} files')
