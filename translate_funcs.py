#!/usr/bin/env python3
"""
Batch translate HPL function HTML documentation files with bilingual (EN/CN) layout.
"""

import os
import re
import glob

www_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "www")

section_translations = {
    "Name": "名称",
    "Synopsis": "概要",
    "Description": "描述",
    "Arguments": "参数",
    "See Also": "另见",
}

arg_type_translations = {
    "local input": "本地输入",
    "global input": "全局输入",
    "local input/output": "本地输入/输出",
    "global input/output": "全局输入/输出",
    "local output": "本地输出",
    "global output": "全局输出",
    "local workspace": "本地工作空间",
    "global workspace": "全局工作空间",
    "input": "输入",
    "output": "输出",
    "input/output": "输入/输出",
    "workspace": "工作空间",
}

desc_translations = {
    "On entry,  GRID  points  to the data structure containing the\n        process grid information.":
        "进入时，GRID 指向包含进程网格信息的数据结构。",
    "On entry,  ALGO  points to  the data structure containing the\n        algorithmic parameters.":
        "进入时，ALGO 指向包含算法参数的数据结构。",
    "On entry, A points to the data structure containing the local\n        array information.":
        "进入时，A 指向包含本地数组信息的数据结构。",
    "On entry,  PANEL  points to the data structure containing the\n        panel information.":
        "进入时，PANEL 指向包含面板信息的数据结构。",
    "On entry,  M specifies the local number of rows of sub(A).":
        "进入时，M 指定 sub(A) 的本地行数。",
    "On entry,  N specifies the local number of columns of sub(A).":
        "进入时，N 指定 sub(A) 的本列数。",
    "On entry, ICOFF specifies the row and column offset of sub(A)\n        in A.":
        "进入时，ICOFF 指定 sub(A) 在 A 中的行和列偏移量。",
    "On entry, WORK  is a workarray of size at least 2*(4+2*N0).":
        "进入时，WORK 是大小至少为 2*(4+2*N0) 的工作数组。",
}

func_name_translations = {
    "Solve A x = b.": "求解 A x = b。",
    "Right-looking panel factorization.": "右视面板分解。",
    "Left-looking panel factorization.": "左视面板分解。",
    "Crout panel factorization.": "Crout 面板分解。",
    "Right-looking recursive panel factorization.": "右视递归面板分解。",
    "Left-looking recursive panel factorization.": "左视递归面板分解。",
    "Crout recursive panel factorization.": "Crout 递归面板分解。",
    "Broadcast the panel L.": "广播面板 L。",
    "Perform the row swap.": "执行行交换。",
    "Perform the row swap and broadcast.": "执行行交换和广播。",
    "Generate a random matrix.": "生成随机矩阵。",
    "Perform the update.": "执行更新。",
    "Backward substitution.": "回代。",
    "Perform the panel broadcast.": "执行面板广播。",
    "Factor the panel.": "分解面板。",
    "Grid initialization.": "网格初始化。",
    "Grid exit.": "网格退出。",
    "Grid information.": "网格信息。",
    "Local index conversion.": "本地索引转换。",
    "Global index conversion.": "全局索引转换。",
    "Timer functions.": "计时器函数。",
    "Send / receive.": "发送 / 接收。",
    "Broadcast.": "广播。",
    "Reduce.": "归约。",
    "Barrier.": "屏障。",
    "Panel initialization.": "面板初始化。",
    "Panel free.": "面板释放。",
    "Panel broadcast.": "面板广播。",
    "Panel factorization.": "面板分解。",
    "Row swap.": "行交换。",
    "Matrix generation.": "矩阵生成。",
    "Timing.": "计时。",
    "Print timing.": "打印计时。",
    "Print test.": "打印测试。",
    "Read parameters.": "读取参数。",
    "Run the test.": "运行测试。",
    "Main driver.": "主驱动程序。",
}


def translate_func_html(filepath):
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()

    # Skip if already translated
    if '名称' in content:
        return False

    # Translate section headers
    for en, cn in section_translations.items():
        content = content.replace(f'<H1>{en}</H1>', f'<H1>{en} / {cn}</H1>')

    # Translate argument types in PRE blocks
    for en, cn in arg_type_translations.items():
        pattern = f'({en})'
        content = re.sub(
            r'\((local|global)\s+(input|output|input/output|workspace)\)',
            lambda m: f'({m.group(0)[1:-1]} / {arg_type_translations.get(m.group(0)[1:-1], m.group(0)[1:-1])})',
            content
        )

    # Translate known description patterns
    for en, cn in desc_translations.items():
        content = content.replace(en, f'{en}<BR>{cn}')

    # Translate function name descriptions
    for en, cn in func_name_translations.items():
        content = content.replace(f'</B> {en}', f'</B> {en} / {cn}')

    # Add bilingual footer note
    if '</BODY>' in content:
        content = content.replace('</BODY>', '<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>\n</BODY>')

    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(content)

    return True


if __name__ == "__main__":
    # Get all HPL_*.html files
    func_files = sorted(glob.glob(os.path.join(www_dir, "HPL_*.html")))
    translated = 0
    for filepath in func_files:
        if translate_func_html(filepath):
            translated += 1
            print(f"Translated: {os.path.basename(filepath)}")
        else:
            print(f"Skipped (already translated): {os.path.basename(filepath)}")
    print(f"\nTranslated {translated} of {len(func_files)} function documentation files")
